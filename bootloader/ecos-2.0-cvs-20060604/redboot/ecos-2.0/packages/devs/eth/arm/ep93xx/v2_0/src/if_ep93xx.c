//==========================================================================
//
//      dev/if_ep93xx.c
//
//      Ethernet device driver for Cirrus Logic EP93xx
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    
// Contributors: 
// Date:         
// Purpose:      
// Description:  hardware driver for EP9312 MAC
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Ethernet device driver for Cirrus Logic EP93xx

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_arm_ep93xx.h>
#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#include <cyg/kernel/kapi.h>
#endif
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>
#include <cyg/hal/ep93xx.h>
#include "ep93xx_eth.h"
                
// ------------------------------------------------------------------------
#ifndef CYGSEM_ARM_EP93XX_SET_ESA
#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <redboot.h>
#include <flash_config.h>
RedBoot_config_option("Set eth0 network hardware address [MAC]",
                      ep93xx_esa,
                      ALWAYS_ENABLED,
                      true,
                      CONFIG_BOOL,
                      false
    );
RedBoot_config_option("eth0 network hardware address [MAC]",
                      ep93xx_esa_data,
                      "ep93xx_esa", 
                      true,
                      CONFIG_ESA, 
                      0
    );
#endif
#endif
#endif

// ------------------------------------------------------------------------
extern void *memcpy( void *, const void *, unsigned long );

// ------------------------------------------------------------------------

//#define ETHDEBUG
#ifdef ETHDEBUG
#define dprintf diag_printf
#else 
#define dprintf(x...)
#endif // ETHDEBUG

// ------------------------------------------------------------------------

#define ETHER_ADDR_LEN 6

#define ETHERNET_INTERRUPT (CYGNUM_HAL_INTERRUPT_MAC) // in case it changes

struct ep93xx_priv_data {
    // These are fixed, set-up-once, pointers to uncachable
    // addresses for the tables the device writes to:
    RxHdr        *rxhdrs;
    RxStat       *rxstat;
    TxHdr        *txhdrs;
    TxStat       *txstat;
    // These are dynamic status pointers.
    RxStat       *rxsp;
    TxStat       *txsp;
#ifdef CYGPKG_NET
    cyg_interrupt interrupt;
    cyg_handle_t  interrupt_handle;
#endif // CYGPKG_NET
    unsigned char enaddr[ETHER_ADDR_LEN];
    cyg_uint8     hardwired_esa;
    int           txbusy;  // Number of packets already queued
    int           txnext;  // Index to next hdr to use          
    unsigned long txkey[CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM];   // Used to ack when packet sent
    unsigned long rxmode;

    // From here downwards, never access these directly, use
    // the uncachable pointers above:
    RxHdr         _rxhdrs[CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM];
    RxStat        _rxstat[CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM];
    TxHdr         _txhdrs[CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM];
    TxStat        _txstat[CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM];
    // The actual data buffers
    unsigned char _rxbufs[CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM]
                        [CYGNUM_DEVS_ETH_ARM_EP93XX_BUFSIZE];
    unsigned char _txbufs[CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM]
                        [CYGNUM_DEVS_ETH_ARM_EP93XX_BUFSIZE];
} _ep93xx_priv_data = {

#ifdef CYGSEM_ARM_EP93XX_SET_ESA
    hardwired_esa: 1,
    enaddr: CYGDAT_ARM_EP93XX_ESA,
#else
    hardwired_esa: 0,
#endif

};


ETH_DRV_SC(ep93xx_sc,
           &_ep93xx_priv_data, // Driver specific data
           "eth0",             // Name for this interface
           ep93xx_start,
           ep93xx_stop,
           ep93xx_control,
           ep93xx_can_send,
           ep93xx_send,
           ep93xx_recv,
           ep93xx_deliver,     // "pseudoDSR" called from fast net thread
           ep93xx_poll,        // poll function, encapsulates ISR and DSR
           ep93xx_int_vector);

NETDEVTAB_ENTRY(ep93xx_netdev, 
                "ep93xx", 
                ep93xx_init, 
                &ep93xx_sc);

static void ep93xx_poll(struct eth_drv_sc *sc);

#ifdef CYGPKG_NET
// This ISR is called when the ethernet interrupt occurs
static int
ep93xx_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
// in case functionality moves into the ISR so that poll() uses it.
#ifdef CYGPKG_NET
    cyg_drv_interrupt_mask(ETHERNET_INTERRUPT);
#endif // CYGPKG_NET
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}
#endif // CYGPKG_NET

// The deliver function (ex-DSR)  handles the ethernet [logical] processing
static void
ep93xx_deliver(struct eth_drv_sc *sc)
{
#ifdef CYGPKG_NET
    ep93xx_poll(sc);
    // Allow interrupts to happen again
    cyg_drv_interrupt_acknowledge(ETHERNET_INTERRUPT);
    cyg_drv_interrupt_unmask(ETHERNET_INTERRUPT);
#endif // CYGPKG_NET
}

static int
ep93xx_int_vector(struct eth_drv_sc *sc)
{
    return (ETHERNET_INTERRUPT);
}

#define ERR_PHY_OK        (0)
#define ERR_PHY_UNKNOWN   (-1)
#define ERR_PHY_BUSY      (-2)
#define ERR_PHY_TIMEOUT   (-3)
#define PHY_TIMEOUT       10000

static int
readPHY(int phy, int reg, unsigned long *val)
{
    unsigned long stat;
    int timeout;
#ifdef ETHDEBUG
    //dprintf("readPHY()\n");
#endif // ETHDEBUG
    
    timeout = PHY_TIMEOUT;
    do {
        if (--timeout == 0) 
        {
        #ifdef ETHDEBUG
            dprintf("readPHY() Phy Busy\n");
        #endif // ETHDEBUG
            return ERR_PHY_BUSY;
        }
        HAL_READ_UINT32(MAC_MiiStat, stat);
    } while (stat & MAC_MiiStat_Busy);
    HAL_WRITE_UINT32(MAC_MiiCmd, MiiCmdRead|(phy<<5)|reg);
    timeout = PHY_TIMEOUT;
    do {
        if (--timeout == 0) 
        {
        #ifdef ETHDEBUG
            dprintf("readPHY() Phy timeout\n");
        #endif // ETHDEBUG
            return ERR_PHY_TIMEOUT;
        }
        HAL_READ_UINT32(MAC_MiiStat, stat);
    } while (stat & MAC_MiiStat_Busy);
    HAL_READ_UINT32(MAC_MiiData, stat);
    *val = stat;
    return ERR_PHY_OK;
}

static void
writePHY(int phy, int reg, unsigned long val)
{
    unsigned long stat;
#ifdef ETHDEBUG
    //dprintf("writePHY()\n");
#endif // ETHDEBUG
    
    do {
        HAL_READ_UINT32(MAC_MiiStat, stat);
    } while (stat & MAC_MiiStat_Busy);
    HAL_WRITE_UINT32(MAC_MiiData, val);
    HAL_WRITE_UINT32(MAC_MiiCmd, MiiCmdWrite|(phy<<5)|reg);
    do {
        HAL_READ_UINT32(MAC_MiiStat, stat);
    } while (stat & MAC_MiiStat_Busy);
}

void
doPHY(void)
{
    unsigned long stat1, stat2, oldSelfCtlVal, testCtlVal;
    int indx, err;
    cyg_uint32 phys_found = 0; // bitset of good PHYs found
    cyg_uint32 phys_good  = 0; // bitset of PHYs found with good link
    
    HAL_READ_UINT32(MAC_SelfCtl, oldSelfCtlVal);
    //Set MDC clock to be divided by 8, and disable PreambleSurpress
    //so that MAC can read/write PHY registers.
    HAL_WRITE_UINT32(MAC_SelfCtl, 0x4e00);

    // First scan for existant PHYs on the MII bus
#ifdef ETHDEBUG
    dprintf("... Scan for PHY units\n");
#endif // ETHDEBUG
    // PHY ID 0 is for broadcasting to all PHYs.  Don't scan it.
    for (indx = 1;  indx < 32;  indx++) {
        err = readPHY(indx, PHY_ID_ONE, &stat1);
        if (err != ERR_PHY_OK)
            continue;
        if (stat1 == 0x0000FFFF) // Most likely, this device isn't there
            continue;
        err = readPHY(indx, PHY_ID_TWO, &stat2);
        if (err != ERR_PHY_OK)
            continue;
#ifdef ETHDEBUG
        dprintf("PHY ID[%x] = %x/%x", indx, stat1, stat2);
#endif // ETHDEBUG
        // Some of these bits latch and only reflect "the truth" on a 2nd reading.
        // So read and discard.
        readPHY(indx, PHY_CONTROL_REG, &stat2);
        readPHY(indx, PHY_STATUS_REG , &stat1);
        readPHY(indx, PHY_CONTROL_REG, &stat2);
        readPHY(indx, PHY_STATUS_REG,  &stat1);
#ifdef ETHDEBUG
        dprintf(", stat = %x, control = %x\n", stat1, stat2);
#endif // ETHDEBUG
        phys_found |= (1<<indx);
        // Command the PHY to renegotiate the link and advertise!
        readPHY(indx, PHY_AUTONEG_ADVERT , &stat1);
        stat1 |= (PHY_AUTONEG_100BASETX_FDX +
                  PHY_AUTONEG_100BASETX_HDX + 
                  PHY_AUTONEG_10BASET_FDX + 
                  PHY_AUTONEG_10BASET_HDX);
        writePHY(indx, PHY_AUTONEG_ADVERT , stat1);
        stat2 |= PHY_CONTROL_AUTONEG_EN | PHY_CONTROL_AUTONEG_RST;
        stat2 &=~PHY_CONTROL_POWERDOWN;
        writePHY(indx, PHY_CONTROL_REG, stat2 );
        hal_delay_us( 1000 );
    }

    for (indx = 1;  indx < 32;  indx++) {
        unsigned long i, j;
        if ( 0 == (phys_found & (1<<indx)) )
            continue;
        i = 3000; // 3 seconds should be enough
        do {
            hal_delay_us( 1000 );
            err = readPHY(indx, PHY_STATUS_REG, &stat1 );
            if (err != ERR_PHY_OK || 0 == i-- )
                break;
        } while ( 0 == (PHY_STATUS_AUTONEG_ACK & stat1) );
        // Then autonegotiation is complete (or timed out)
        readPHY(indx, PHY_STATUS_REG, &stat1 );

        if ( (PHY_STATUS_LINK_OK & stat1) ) {
            readPHY(indx, PHY_AUTONEG_ADVERT, &i );
            readPHY(indx, PHY_AUTONEG_REMOTE, &j );
#ifdef ETHDEBUG
            dprintf( "MII %d: capabilities are %04x, %04x; common %04x\n",
                     indx,                        i, j, i & j );
#endif // ETHDEBUG
            j &= i;
            if ( j & (PHY_AUTONEG_100BASETX_FDX + PHY_AUTONEG_100BASETX_HDX + 
                      PHY_AUTONEG_10BASET_FDX + PHY_AUTONEG_10BASET_HDX) ) {
                // Then there is commonality of capabilities, we are happy!
                phys_good |= (1<<indx);

                // OPTIONAL: having found one good interface, that's
                // enough: saves wasting time scanning all - when the
                // network device seems unable to use other PHYs.
                break;
            }
            if ( j & (PHY_AUTONEG_100BASETX_FDX + PHY_AUTONEG_10BASET_FDX)) {
               // If PHY is full duplex, set MAC to full duplex too.
               HAL_READ_UINT32(MAC_TestCtl, testCtlVal);
               testCtlVal |= MAC_TestCtl_FDX;
               HAL_WRITE_UINT32(MAC_TestCtl, testCtlVal);
            }
        }
    }

    // so there is at least one good link there
    if ( 0 == phys_good )
        phys_good = phys_found & ~(phys_found-1);

#ifdef ETHDEBUG
    dprintf( "After scan, phys_found %08x, phys_good %08x\n",
             phys_found, phys_good );
#endif // ETHDEBUG

    // select the first one we find
    phys_good = phys_good & ~(phys_good-1);

    for (indx = 1;  indx < 32;  indx++) {
        if ( (1<<indx) & phys_found ) {
            if ( 0 == ((1<<indx) & phys_good) ) {
#ifdef ETHDEBUG
                dprintf("Disabling PHY unit %d\n", indx );
#endif // ETHDEBUG
                // Disable second PHY; Power down device
                writePHY(indx, PHY_CONTROL_REG, PHY_CONTROL_POWERDOWN );
            }
        }
    }

    // restore the old vaule of SelfCtl register.
    HAL_WRITE_UINT32(MAC_SelfCtl, oldSelfCtlVal);
}

// ------------------------------------------------------------------------
//
// Reset the receiver and rebuild queues, etc.
//
static void
ep93xx_RxReset(struct ep93xx_priv_data *cpd)
{
    cyg_uint32 stat;
    cyg_uint32 phys;
    int i;

#ifdef ETHDEBUG
    dprintf("ep93xx_RxReset()\n");
#endif // ETHDEBUG
    // Reset Rx engine
    HAL_WRITE_UINT32(MAC_BMCtl, MAC_BMCtl_RxReset);
    do {
        HAL_READ_UINT32(MAC_BMCtl, stat);
    } while (stat & MAC_BMCtl_RxReset);
    HAL_WRITE_UINT32(MAC_RxCtl, 0);
    // Set up queues
    
    HAL_VIRT_TO_PHYS_ADDRESS( (cyg_uint32)cpd->rxhdrs, phys );
    HAL_WRITE_UINT32(MAC_RxDBA, phys );
    HAL_WRITE_UINT32(MAC_RxDBL, CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM * sizeof(RxHdr));
    HAL_WRITE_UINT32(MAC_RxDCA, phys );
    HAL_VIRT_TO_PHYS_ADDRESS( (cyg_uint32)cpd->rxstat, phys );
    HAL_WRITE_UINT32(MAC_RxSBA, phys );
    HAL_WRITE_UINT32(MAC_RxSBL, CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM * sizeof(RxStat));
    HAL_WRITE_UINT32(MAC_RxSCA, phys );
    HAL_WRITE_UINT32(MAC_RxDTH, 0x00040002);
    HAL_WRITE_UINT32(MAC_RxSTH, 0x00040002);
    HAL_WRITE_UINT32(MAC_RxBTH, 0x00800040);
    cpd->rxsp = cpd->rxstat;

    for (i = 0;  i < CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM;  i++) 
    {
        HAL_VIRT_TO_PHYS_ADDRESS( (cyg_uint32)&cpd->_rxbufs[i][0], phys );
        cpd->rxhdrs[i].bufaddr = (void *)phys;
        cpd->rxhdrs[i].bi = i;
        cpd->rxhdrs[i].len = CYGNUM_DEVS_ETH_ARM_EP93XX_BUFSIZE;
    }

    // Wait for Receiver to get started
    HAL_WRITE_UINT32(MAC_BMCtl, MAC_BMCtl_RxEnable);
    do {
        HAL_READ_UINT32(MAC_BMStat, stat);
    } while ((stat & MAC_BMStat_RxAct) == 0) ;
    HAL_WRITE_UINT32(MAC_RxDEQ, CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM);
    HAL_WRITE_UINT32(MAC_RxSEQ, CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM);
    // Configure and enable receiver
    HAL_WRITE_UINT32(MAC_MaxFL, (CYGNUM_DEVS_ETH_ARM_EP93XX_BUFSIZE<<16));
}

// ------------------------------------------------------------------------

static void
ep93xx_TxReset(struct ep93xx_priv_data *cpd)
{
    cyg_uint32 stat;
    cyg_uint32 phys;

#ifdef ETHDEBUG
    dprintf("ep93xx_TxReset()\n");
#endif // ETHDEBUG

    // Clear all test options (was: set FD => ignore CSMA!)
    HAL_WRITE_UINT32(MAC_TestCtl, 0);

    HAL_VIRT_TO_PHYS_ADDRESS( (unsigned long)cpd->txhdrs, phys );
    HAL_WRITE_UINT32(MAC_TxDBA, phys );
    HAL_WRITE_UINT32(MAC_TxDBL, CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM * sizeof(TxHdr));
    HAL_WRITE_UINT32(MAC_TxDCA, phys );
    HAL_VIRT_TO_PHYS_ADDRESS( (unsigned long)cpd->txstat, phys );
    HAL_WRITE_UINT32(MAC_TxSBA, phys );
    HAL_WRITE_UINT32(MAC_TxSBL, CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM * sizeof(TxStat));
    HAL_WRITE_UINT32(MAC_TxSCA, phys );
    HAL_WRITE_UINT32(MAC_TxDTH, 0x00040002);
    HAL_WRITE_UINT32(MAC_TxSTH, 0x00040002);
    HAL_WRITE_UINT32(MAC_TxDTH, 0x00200010);
    cpd->txsp = cpd->txstat;

    // Configure and enable transmitter
    HAL_WRITE_UINT32(MAC_BMCtl, MAC_BMCtl_TxEnable);
    do {
        HAL_READ_UINT32(MAC_BMStat, stat);
    } while ((stat & MAC_BMStat_TxAct) == 0) ;
    HAL_WRITE_UINT32(MAC_TxCtl, MAC_TxCtl_TxON);

}

// ------------------------------------------------------------------------

static unsigned long
ep93xx_read_eeprom(unsigned long ulOffset)
{
    unsigned long ulRet, ulTemp;

    HAL_WRITE_UINT32(EP9312_SSP_DR, 0x03);
    HAL_WRITE_UINT32(EP9312_SSP_DR, (ulOffset >> 16) & 255);
    HAL_WRITE_UINT32(EP9312_SSP_DR, (ulOffset >> 8) & 255);
    HAL_WRITE_UINT32(EP9312_SSP_DR, ulOffset & 255);
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    HAL_WRITE_UINT32(EP9312_SSP_DR, 0);
    HAL_WRITE_UINT32(EP9312_SSP_DR, 0);
    HAL_WRITE_UINT32(EP9312_SSP_DR, 0);
    HAL_WRITE_UINT32(EP9312_SSP_DR, 0);
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulRet);
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    ulRet |= ulTemp << 8;
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    ulRet |= ulTemp << 16;
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x04));
    HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    ulRet |= ulTemp << 24;
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
    } while (!(ulTemp & 0x01));
    do {
        HAL_READ_UINT32(EP9312_SSP_SR, ulTemp);
        if (ulTemp & 0x04)
            HAL_READ_UINT32(EP9312_SSP_DR, ulTemp);
    } while (ulTemp & 0x04);
    return ulRet;
}

static bool 
ep93xx_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    struct ep93xx_priv_data *cpd = (struct ep93xx_priv_data *)sc->driver_private;
    cyg_uint32 stat;
    int i, set_esa, esa_configured = false;
    cyg_uint32 uncached;
    unsigned long temp;

#ifdef ETHDEBUG
    dprintf("ep93xx_init\n");
#endif // ETHDEBUG

    // Set up uncached addresses for our data structures
    HAL_VIRT_TO_UNCACHED_ADDRESS( &cpd->_rxhdrs[0], uncached );
    cpd->rxhdrs = (RxHdr *)uncached;
    HAL_VIRT_TO_UNCACHED_ADDRESS( &cpd->_rxstat[0], uncached );
    cpd->rxstat = (RxStat *)uncached;
    HAL_VIRT_TO_UNCACHED_ADDRESS( &cpd->_txhdrs[0], uncached );
    cpd->txhdrs = (TxHdr *)uncached;
    HAL_VIRT_TO_UNCACHED_ADDRESS( &cpd->_txstat[0], uncached );
    cpd->txstat = (TxStat *)uncached;

#ifdef CYGPKG_NET
    // Initialize environment, setup interrupt handler
    cyg_drv_interrupt_create(ETHERNET_INTERRUPT,
                             99, // Priority - what goes here?
                             (cyg_addrword_t)sc, //  Data item passed to interrupt handler
                             (cyg_ISR_t *)ep93xx_isr,
                             (cyg_DSR_t *)eth_drv_dsr, // The logical driver DSR
                             &cpd->interrupt_handle,
                             &cpd->interrupt);
    cyg_drv_interrupt_attach(cpd->interrupt_handle);
    cyg_drv_interrupt_acknowledge(ETHERNET_INTERRUPT);
    cyg_drv_interrupt_unmask(ETHERNET_INTERRUPT);
#endif // CYGPKG_NET

    // Configure and enable PHY (wire) interfaces
    doPHY();

    // Reset the device
    HAL_WRITE_UINT32(MAC_SelfCtl, MAC_SelfCtl_Reset);
    do 
    {
        HAL_READ_UINT32(MAC_SelfCtl, stat);
    } while (stat & MAC_SelfCtl_Reset);

    //Set MDC clock to be divided by 8, and enable PreambleSurpress
    // for Tx and Rx frames.
    HAL_WRITE_UINT32(MAC_SelfCtl, 0x4f00);

    // Set the hardware address
    if (flash_get_config("ep93xx_esa", &set_esa, CONFIG_BOOL) == true) {
        if (set_esa) {
            if (flash_get_config("ep93xx_esa_data", &cpd->enaddr[0], CONFIG_ESA) == true) {
                esa_configured = true;
            }
        }
    }
    if (!esa_configured && cpd->hardwired_esa) {
        esa_configured = true;
    }
    if (!esa_configured) {
        HAL_WRITE_UINT32(EP9312_GPIO_PBDDR, 0x50);
        HAL_WRITE_UINT32(EP9312_GPIO_PBDR, 0x50);
        HAL_WRITE_UINT32(EP9312_GPIO_PADDR, 0x80);
        HAL_WRITE_UINT32(EP9312_GPIO_PADR, 0x00);
        HAL_WRITE_UINT32(EP9312_SSP_CR0, 0x6c7);
        HAL_WRITE_UINT32(EP9312_SSP_CPSR, 2);
        HAL_WRITE_UINT32(EP9312_SSP_CR1, 0x10);
        if (ep93xx_read_eeprom(0x1000) == 0x43414d45)
        {
            temp = ep93xx_read_eeprom(0x1004);
            cpd->enaddr[0] = temp & 255;
            cpd->enaddr[1] = (temp >> 8) & 255;
            cpd->enaddr[2] = (temp >> 16) & 255;
            cpd->enaddr[3] = temp >> 24;
            temp = ep93xx_read_eeprom(0x1008);
            cpd->enaddr[4] = temp & 255;
            cpd->enaddr[5] = (temp >> 8) & 255;
            esa_configured = true;
        }
    }
    if (!esa_configured) {
        diag_printf("EP93xx - no EEPROM, static ESA, or RedBoot config option.\n");
        return false;
    }

    HAL_WRITE_UINT32(MAC_AFP, 0);  // Individual address slot #0
    for (i = 0;  i < ETHER_ADDR_LEN;  i++) {
        HAL_WRITE_UINT8(MAC_IAD+i, cpd->enaddr[i]);
    }

    // Reset Rx
    ep93xx_RxReset(cpd);
    cpd->rxmode = MAC_RxCtl_Broadcast | MAC_RxCtl_IA0 | MAC_RxCtl_SRxON | 
        MAC_RxCtl_RCRCA;
    // ***FIXME: PROMISCUOUS MODE: add MAC_RxCtl_Prom
    // And move a load of this into start().
    HAL_WRITE_UINT32(MAC_RxCtl, cpd->rxmode);

    // Reset Tx
    ep93xx_TxReset(cpd);

    cpd->txbusy = 0;
    cpd->txnext = 0;

    HAL_WRITE_UINT32(MAC_IntStat, 0xFFFFFFFF);    // Clear any pending interrupts
    // Set up interrupts
    HAL_WRITE_UINT32(MAC_IntEnab, MAC_IntEnab_Rx | MAC_IntEnab_Tx );
    HAL_WRITE_UINT32(MAC_IntStat, 0xFFFFFFFF);    // Clear again

// ***FIXME: this register is NOT DOCUMENTED AT ALL ***
//    HAL_WRITE_UINT32(MAC_IntCtl, MAC_IntCtl_Enable);

    // Initialize upper level driver
    (sc->funs->eth_drv->init)(sc, &cpd->enaddr[0]);

#ifdef ETHDEBUG
    dprintf("ep93xx_init exit\n");
#endif // ETHDEBUG
    return true;
}

//
// This function is used to shutdown an interface.
//
static void
ep93xx_stop(struct eth_drv_sc *sc)
{
    cyg_uint32 stat;

#ifdef ETHDEBUG
    dprintf("ep93xx_stop()\n");
#endif // ETHDEBUG

    // Reset the device
    HAL_WRITE_UINT32(MAC_SelfCtl, MAC_SelfCtl_Reset);
    do 
    {
        HAL_READ_UINT32(MAC_SelfCtl, stat);
    } while (stat & MAC_SelfCtl_Reset);
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
ep93xx_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
#ifdef ETHDEBUG
    dprintf("ep93xx_start()\n");
#endif // ETHDEBUG
}

//
// This routine is called to perform special "control" opertions
//
static int
ep93xx_control(struct eth_drv_sc *sc, unsigned long key,
               void *data, int data_length)
{
#ifdef ETHDEBUG
    dprintf("ep93xx_control()\n");
#endif // ETHDEBUG
    
    switch (key) {
    case ETH_DRV_SET_MAC_ADDRESS:
        return 0;
        break;
    default:
        return 1;
        break;
    }
}

//
// This routine is called to see if it is possible to send another packet.
// It will return non-zero if a transmit is possible, zero otherwise.
//
static int
ep93xx_can_send(struct eth_drv_sc *sc)
{
    struct ep93xx_priv_data *cpd = (struct ep93xx_priv_data *)sc->driver_private;
#ifdef ETHDEBUG
    dprintf("ep93xx_can_send()\n");
#endif // ETHDEBUG
#ifdef NOTYET
    unsigned short stat;


    stat = get_reg(PP_LineStat);
    if ((stat & PP_LineStat_LinkOK) == 0) {
        return false;  // Link not connected
    }
#endif // NOTYET
    return (cpd->txbusy < CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM);
}

//
// This routine is called to send data to the hardware.
static void 
ep93xx_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, 
            int total_len, unsigned long key)
{
    struct ep93xx_priv_data *cpd = (struct ep93xx_priv_data *)sc->driver_private;
    TxHdr * volatile hdr;
    int i;
    int len;
    unsigned char * volatile data, * volatile bp, * volatile base;
    cyg_uint32 phys;

#ifdef ETHDEBUG
    dprintf("ep93xx_send()\n");
#endif // ETHDEBUG
    
    if(cpd == 0)
    {
#ifdef ETHDEBUG
        dprintf("ep93xx_send cpd is NULL!");
#endif // ETHDEBUG
        while (1);
    }
    if (cpd->txbusy >= CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM)
    {
        diag_printf("ep93xx_send you are hosed!");
        while (1);
        return;
    }

    if(cpd->txkey[cpd->txnext] != 0)
    {
        diag_printf("ep93xx_send outta places you are hosed!");
        while (1);
    }
    // Mark xmitter busy
    cpd->txkey[cpd->txnext] = key;
    cpd->txbusy++;

    // Fill in the header
    hdr = &cpd->txhdrs[cpd->txnext];
    HAL_VIRT_TO_PHYS_ADDRESS((cyg_uint32)&cpd->_txbufs[cpd->txnext][0], phys);
    HAL_VIRT_TO_UNCACHED_ADDRESS( &cpd->_txbufs[cpd->txnext][0], base );
    hdr->bufaddr = (void *)phys;
    hdr->eof = 1;  // Complete frame
    hdr->bi = cpd->txnext;
    hdr->len = total_len;
    if (++cpd->txnext >= CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM)
    {
        cpd->txnext = 0;
    }

    // Put data into buffer
    bp = base;
    for (i = 0;  i < sg_len;  i++)
    {
        data = (unsigned char *)sg_list[i].buf;
        len = sg_list[i].len;
        if(bp == 0)
        {
            diag_printf("ep93xx_send() bp was 0\n");
            while (1);
        }
        if(data == 0)
        {
            diag_printf("ep93xx_send() data was 0\n");
            while (1);
        }
        memcpy(bp, data, len);
        bp += len;
        if(bp > ((unsigned char *)base + CYGNUM_DEVS_ETH_ARM_EP93XX_BUFSIZE))
        {
            diag_printf("ep93xx_send() out of bounds\n");
            while (1);
        }
    }
    
    // Tell the device it's ready
    HAL_WRITE_UINT32(MAC_TxDEQ, 1);  // Increments number of "valid" descriptors

#ifdef ETHDEBUG
    dprintf("ep93xx_send() exit\n");
#endif // ETHDEBUG
}

//
// This function is called when a packet has been sent
static void
ep93xx_TxEvent(struct eth_drv_sc *sc, unsigned long stat)
{
    struct ep93xx_priv_data *cpd = (struct ep93xx_priv_data *)sc->driver_private;
    int i, bi;
    TxStat *sp;

#ifdef ETHDEBUG
    dprintf("ep93xx_TxEvent()\n");
#endif // ETHDEBUG

    if(cpd == 0)
    {
#ifdef ETHDEBUG
        dprintf("ep93xx_TxEvent cpd is NULL!");
#endif // ETHDEBUG
    }
    
    sp = cpd->txstat;
    for (i = 0;  i < CYGNUM_DEVS_ETH_ARM_EP93XX_TxNUM;  i++, sp++)
    {
        if (sp->fp)
        {
            // This status is valid
            bi = sp->bi;
            cpd->txbusy--;
            sp->fp = 0;
            (sc->funs->eth_drv->tx_done)(sc, cpd->txkey[bi], 0);
            cpd->txkey[bi] = 0;
        }
    }
#ifdef ETHDEBUG
    dprintf("ep93xx_TxEventExit()\n");
#endif // ETHDEBUG
}

// ------------------------------------------------------------------------
// This functio n is called when a packet has been received.  Its job is
// to prepare to unload the packet from the hardware.  Once the length of
// the packet is known, the upper layer of the driver can be told.  When
// the upper layer is ready to unload the packet, the internal function
// 'ep93xx_recv' will be called to actually fetch it from the hardware.
//
static void
ep93xx_RxEvent(struct eth_drv_sc *sc)
{
    struct ep93xx_priv_data *cpd = (struct ep93xx_priv_data *)sc->driver_private;
    static int events=0;

#ifdef ETHDEBUG
    dprintf("ep93xx_RxEvent()\n");
#endif // ETHDEBUG

    if(cpd == 0)
    {
#ifdef ETHDEBUG
        dprintf("ep93xx_RxEvent cpd is NULL!\n");
#endif // ETHDEBUG
        while (1);
    }

    if(sc == 0)
    {
#ifdef ETHDEBUG
        dprintf("ep93xx_RxEvent sc is NULL!\n");
#endif // ETHDEBUG        
        while (1);
    }
    
    while (cpd->rxsp->fp)
    {
#ifdef ETHDEBUG
        dprintf("recv fp = %08x sc=%08x events %08x len=%08x.\n",cpd->rxsp->fp,sc, events, cpd->rxsp->len);
#endif // ETHDEBUG
        if((events++ < 40) || (cpd->rxsp->len != 0x3a))
        {
            (sc->funs->eth_drv->recv)(sc, cpd->rxsp->len);
        }
 
        cpd->rxsp->fp = 0;
        cpd->rxsp->rfp = 0;
        cpd->rxsp->len = 0;
        cpd->rxsp++;

        if (cpd->rxsp >= &cpd->rxstat[CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM])
        {
            cpd->rxsp = cpd->rxstat;
        }
        // Tell device we took the data
        HAL_WRITE_UINT32(MAC_RxDEQ, 1);
        HAL_WRITE_UINT32(MAC_RxSEQ, 1);
    }
#ifdef ETHDEBUG
    dprintf("ep93xx_RxEvent() Exit\n");
#endif // ETHDEBUG
}

//
// This function is called as a result of the "eth_drv_recv()" call above.
// It's job is to actually fetch data for a packet from the hardware once
// memory buffers have been allocated for the packet.  Note that the buffers
// may come in pieces, using a scatter-gather list.  This allows for more
// efficient processing in the upper layers of the stack.
//
static void
ep93xx_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    struct ep93xx_priv_data *cpd = (struct ep93xx_priv_data *)sc->driver_private;
    int i, mlen;
    cyg_uint8 *data, *bp;
    int tot_len = 0;
    cyg_uint32 pp, uu;

#ifdef ETHDEBUG
    dprintf("ep93xx_recv()\n");
#endif // ETHDEBUG

    if(cpd == 0)
    {
        diag_printf("ep93xx_recv cpd is NULL!");
        while(1);
    }

    if (cpd->rxsp->bi >= CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM)
    {
        dprintf("ep93xx_recv bi is > %03x!",CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM);
        while(1);
    }

    HAL_PHYS_TO_VIRT_ADDRESS(cpd->rxhdrs[cpd->rxsp->bi].bufaddr, pp);
    HAL_VIRT_TO_UNCACHED_ADDRESS(pp, uu);
    bp = (cyg_uint8 *)uu; // uncached access to the data

    for (i = 0;  i < sg_len;  i++)
    {
        data = (cyg_uint8 *)sg_list[i].buf;
        mlen = sg_list[i].len;

        if(data == 0)
        {
            dprintf("ep93xx_recv() data was NULL i=%08x len=%08x sg_list=%08x\n",i,mlen,sg_list);
            continue;
        }
        if(bp == 0)
        {
            dprintf("ep93xx_recv() bp was NULL\n");
        }

//        diag_printf("ep93xx_recv() data=%08x bp=%08x len=%08x\n",data, bp, mlen);
        memcpy(data, bp, mlen);
        bp += mlen;
        tot_len += mlen;
    }

#ifdef ETHDEBUG
    dprintf("ep93xx_recv()\n");
#endif // ETHDEBUG
}

// ------------------------------------------------------------------------
//
// ------------------------------------------------------------------------
static void
ep93xx_poll(struct eth_drv_sc *sc)
{
    unsigned long status;
    //, orig_status;
//    unsigned short seq;

    HAL_READ_UINT32(MAC_IntStat, status);

//    orig_status = status;
//#if 1
//    // The chip does not seem to present the RxSQ interrupt
//    HAL_READ_UINT16(MAC_RxSEQ_Count, seq);
//    if (seq != CYGNUM_DEVS_ETH_ARM_EP93XX_RxNUM)
//    
//        status |= (MAC_IntStat_RxSQ);
//    }
//#endif
    if (MAC_IntStat_Tx & status)
    {
        ep93xx_TxEvent(sc, status);
    }
    if (MAC_IntStat_Rx & status)
    {
        ep93xx_RxEvent(sc);
    }
    
    //
    // Clear the interrupts
    //
    HAL_WRITE_UINT32(MAC_IntStat, status);
}
  

// ------------------------------------------------------------------------
// EOF if_ep93xx.c
