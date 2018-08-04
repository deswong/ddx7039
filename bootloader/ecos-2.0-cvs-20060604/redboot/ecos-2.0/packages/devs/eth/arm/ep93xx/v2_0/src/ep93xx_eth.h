#ifndef _CYGONCE_EP93XX_ETH_H_
#define _CYGONCE_EP93XX_ETH_H_
//==========================================================================
//
//      ep93xx_eth.h
//
//      Cirrus Logic Ethernet for EP93xx
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-05-15
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Cirrus Logic EP93xx Ethernet

typedef struct TxHdr {
    void         *bufaddr;
    unsigned long len:12,  // Buffer length
                  :3,      // Reserved
                  af:1,    // Abort frame
                  bi:15,   // Buffer Index
                  eof:1;   // End of Frame
} TxHdr;

typedef struct TxStat {
    unsigned long bi:15,   // Buffer index
                  :1,      // Reserved
                  coll:5,  // Number of collisions
                  :3,      // Reserved
                  tmc:1,   // Too many collisions
                  txu:1,   // Tx underrun
                  ow:1,    // Out of window (collision)
                  :1,      // Reserved
                  lcrs:1,  // Loss of CRS
                  fa:1,    // Frame abort
                  twe:1,   // Processed without error
                  fp:1;    // Frame processed

} TxStat;

typedef struct RxHdr {
    void         *bufaddr;
    unsigned long len:16,  // Buffer length
                  bi:15,   // Buffer index
                  nsof:1;  // Set if continued frame   
} RxHdr;

typedef struct RxStat {
    unsigned long :8,      // Reserved
                  hti:6,   // Hash table index
                  :1,      // Reserved
                  crci:1,  // CRC included
                  crce:1,  // CRC error
                  edata:1, // Extra data
                  runt:1,  // Frame < 64 bytes
                  fe:1,    // Framing error
                  oe:1,    // Overrun
                  rxe:1,   // Receive error (RX_ERR)
                  am:2,    // Address match
                  :4,      // reserved
                  eob:1,   // End of Buffer
                  eof:1,   // End of Frame
                  rwe:1,   // Received without error
                  fp:1;    // Frame processed
    unsigned long 
                  len:16,  // Frame length
                  bi:15,   // Buffer Index
                  rfp:1;   // Frame processed
} RxStat;

// Register layout
#define MAC_RxCtl               (EP9312_MAC+0x0000)
#define MAC_RxCtl_PauseA        (1<<20)
#define MAC_RxCtl_RxFCE         (3<<18)
#define MAC_RxCtl_RxFCE_None    (0<<18)
#define MAC_RxCtl_RxFCE_Ctl     (3<<18)
#define MAC_RxCtl_RxFCE_NoCtl   (2<<18)
#define MAC_RxCtl_BCRC          (1<<17)
#define MAC_RxCtl_SRxON         (1<<16)
#define MAC_RxCtl_RCRC          (1<<13)
#define MAC_RxCtl_RA            (1<<12)
#define MAC_RxCtl_RCRCA         (1<<13)
#define MAC_RxCtl_Prom          (1<<11)
#define MAC_RxCtl_Broadcast     (1<<10)
#define MAC_RxCtl_Multicast     (1<<9)
#define MAC_RxCtl_IAH           (1<<8)
#define MAC_RxCtl_IA3           (1<<3)
#define MAC_RxCtl_IA2           (1<<2)
#define MAC_RxCtl_IA1           (1<<1)
#define MAC_RxCtl_IA0           (1<<0)

#define MAC_TxCtl           (EP9312_MAC+0x0004)
#define MAC_TxCtl_DefDis    (1<<7)
#define MAC_TxCtl_ModBE     (1<<6)
#define MAC_TxCtl_NoCRC     (1<<5)
#define MAC_TxCtl_NoPAD     (1<<4)
#define MAC_TxCtl_NoColl    (1<<3)
#define MAC_TxCtl_Pause     (1<<2)
#define MAC_TxCtl_Pausing   (1<<1)
#define MAC_TxCtl_TxON      (1<<0)

#define MAC_TestCtl         (EP9312_MAC+0x0008)
#define MAC_TestCtl_FDX     (1<<6)

#define MAC_MiiCmd          (EP9312_MAC+0x0010)
/*
*       OpReg_MIICmd    0x0010   2-RW MII(Media Independent Intf) Command
*/
#define MAC_MiiCmd_RegAd_Mask               (0x001F)        /*  */
#define MAC_MiiCmd_PhyAd_Mask               (0x03E0)        /*  */
#define MAC_MiiCmd_Opcode_Mask              (0xC000)        /*  */


/** QQQQQQQ */
#define MAC_MiiCmd_PhyAd_8950               (0x0000)        /* Address the 8950 Phy */
#define MAC_MiiCmd_Opcode_Read              (0x8000)        /* Read register */
#define MAC_MiiCmd_Opcode_Write             (0x4000)        /* Write register */


#define MAC_MiiData         (EP9312_MAC+0x0014)

#define MAC_MiiStat         (EP9312_MAC+0x0018)
#define MAC_MiiStat_Busy    (1<<0)

#define MAC_SelfCtl         (EP9312_MAC+0x0020)
#define MAC_SelfCtl_MiiLB   (1<<2)
#define MAC_SelfCtl_Reset   (1<<0)

#define MAC_IntEnab         (EP9312_MAC+0x0024)
#define MAC_IntEnab_RxSQIE  (1<<27)
#define MAC_IntEnab_TxECI   (1<<25)
#define MAC_IntEnab_TxSQ    (1<<3)

// These are for individual rx queue events
// We do not enable the events, but we unmask the interrupts to workaround
// a problem (Item 22. MAC "RXSQ interrupt bit does not assert")
#define MAC_IntEnab_REOFIE  (1<<2)
#define MAC_IntEnab_REOBIE  (1<<1)
#define MAC_IntEnab_RHRDIE  (1<<0)

#define MAC_IntEnab_Rx \
  (MAC_IntEnab_RxSQIE | MAC_IntEnab_REOFIE | MAC_IntEnab_REOBIE | MAC_IntEnab_RHRDIE)

#define MAC_IntEnab_Tx \
  (MAC_IntEnab_TxSQ | MAC_IntEnab_TxECI)

#define MAC_IntStat         (EP9312_MAC+0x0028)  // Write-1-to-clear status register
#define MAC_IntClr          (EP9312_MAC+0x002C)  // Read-clear copy

#define MAC_IntStat_RWI     (1<<30)     // Rx Wakeup
#define MAC_IntStat_RxMI    (1<<29)     // Rx Underrun
#define MAC_IntStat_RxBI    (1<<28)     // Rx last buffdesc read in
#define MAC_IntStat_RxSQI   (1<<27)     // Rx status Q empty
#define MAC_IntStat_TxLEI   (1<<26)     // Tx length error
#define MAC_IntStat_TxECI   (1<<25)     // Tx end of chain read
#define MAC_IntStat_TxUHI   (1<<24)     // Rx underrun
#define MAC_IntStat_MOI     (1<<18)     // Rx miss/underrun
#define MAC_IntStat_TxCOL   (1<<17)     // Tx collision count overflow
#define MAC_IntStat_RxROI   (1<<16)     // Rx runt count overflow
#define MAC_IntStat_MII     (1<<12)     // MII op complete
#define MAC_IntStat_PHY     (1<<11)     // MII interrupt request
#define MAC_IntStat_TO      (1<<10)     // Timer timed out
#define MAC_IntStat_AHBE    (1<<9)      // Bus error
#define MAC_IntStat_SWI     (1<<8)      // SWI bit
#define MAC_IntStat_OTHER   (1<<4)      // other than next 2
#define MAC_IntStat_TxSQ    (1<<3)      // Tx status Q
#define MAC_IntStat_RxSQ    (1<<2)      // Rx status Q

#define MAC_IntStat_Rx \
  (MAC_IntStat_RxSQ | MAC_IntStat_RxSQI | MAC_IntStat_RxBI)

#define MAC_IntStat_Tx \
  (MAC_IntStat_TxSQ | MAC_IntStat_TxECI)

#define MAC_GenTmr          (EP9312_MAC+0x0040)

#define MAC_FlowTmr         (EP9312_MAC+0x0044)

#define MAC_FlowFmt         (EP9312_MAC+0x0048)

#define MAC_AFP             (EP9312_MAC+0x004C)
#define MAC_AFP_IAPrimary       (0x00)  /* primary IA for Wakeup, Tx pause and Rx pause frames */
#define MAC_AFP_IASecondary1    (0x01)  /* secondary IA for Rx pause frames */
#define MAC_AFP_IASecondary2    (0x02)  /* secondary IA for qualifying Rx frames */
#define MAC_AFP_IASecondary3    (0x03)  /* secondary IA for qualifying Rx frames */
#define MAC_AFP_Tx              (0x06)  /* destination address for Tx */
#define MAC_AFP_Hash            (0x07)  /* hash table */


#define MAC_IAD             (EP9312_MAC+0x0050)

#define MAC_HashTbl         (EP9312_MAC+0x0050)

#define MAC_IntCtl          (EP9312_MAC+0x0064)  // ***FIXME: NOT DOCUMENTED AT ALL
#define MAC_IntCtl_Enable   (1<<15)

#define MAC_ColCnt          (EP9312_MAC+0x0070)

#define MAC_MissCnt         (EP9312_MAC+0x0074)

#define MAC_RuntCnt         (EP9312_MAC+0x0078)

#define MAC_BMCtl           (EP9312_MAC+0x0080)
#define MAC_BMCtl_TxReset   (1<<10)
#define MAC_BMCtl_TxDisable (1<<9)
#define MAC_BMCtl_TxEnable  (1<<8)
// RSVD                         7
// EH2 - enable header 2        6
// EH1 - enable header 1        5
// EEOB - .. end of buffer      4
// RSVD                         3
#define MAC_BMCtl_RxReset   (1<<2)
#define MAC_BMCtl_RxDisable (1<<1)
#define MAC_BMCtl_RxEnable  (1<<0)

#define MAC_BMStat              (EP9312_MAC+0x0084)
#define MAC_BMStat_TxAct        (1<<7)
#define MAC_BMStat_TP           (1<<4)
#define MAC_BMStat_RxAct        (1<<3)
#define MAC_BMStat_QID_MASK     (7<<0)
#define MAC_BMStat_QID_RxData   (0x00)          /*  */
#define MAC_BMStat_QID_TxData   (0x01)          /*  */
#define MAC_BMStat_QID_RxSts    (0x02)          /*  */
#define MAC_BMStat_QID_TxSts    (0x03)          /*  */
#define MAC_BMStat_QID_RxDesc   (0x04)          /*  */
#define MAC_BMStat_QID_TxDesc   (0x05)          /*  */

#define MAC_RxBCA       (EP9312_MAC+0x0088)
#define MAC_TxBCA       (EP9312_MAC+0x008C)
#define MAC_RxDBA       (EP9312_MAC+0x0090)
#define MAC_RxDBL       (EP9312_MAC+0x0094)
#define MAC_RxDCL       (EP9312_MAC+0x0096)
#define MAC_RxDCA       (EP9312_MAC+0x0098)
#define MAC_RxDEQ       (EP9312_MAC+0x009C)
#define MAC_RxSBA       (EP9312_MAC+0x00A0)
#define MAC_RxSBL       (EP9312_MAC+0x00A4)
#define MAC_RxSCL       (EP9312_MAC+0x00A6)
#define MAC_RxSCA       (EP9312_MAC+0x00A8)

#define MAC_RxSEQ       (EP9312_MAC+0x00AC)
#define MAC_RxSEQ_Count (MAC_RxSEQ+2)

#define MAC_TxDBA       (EP9312_MAC+0x00B0)
#define MAC_TxDBL       (EP9312_MAC+0x00B4)
#define MAC_TxDCL       (EP9312_MAC+0x00B6)
#define MAC_TxDCA       (EP9312_MAC+0x00B8)
#define MAC_TxDEQ       (EP9312_MAC+0x00BC)
#define MAC_TxSBA       (EP9312_MAC+0x00C0)
#define MAC_TxSBL       (EP9312_MAC+0x00C4)
#define MAC_TxSCL       (EP9312_MAC+0x00C6)
#define MAC_TxSCA       (EP9312_MAC+0x00C8)
#define MAC_RxBTH       (EP9312_MAC+0x00D0)
#define MAC_TxBTH       (EP9312_MAC+0x00D4)
#define MAC_RxSTH       (EP9312_MAC+0x00D8)
#define MAC_TxSTH       (EP9312_MAC+0x00DC)
#define MAC_RxDTH       (EP9312_MAC+0x00E0)
#define MAC_TxDTH       (EP9312_MAC+0x00E4)
#define MAC_MaxFL       (EP9312_MAC+0x00E8)
#define MAC_RxHL        (EP9312_MAC+0x00EC)
#define MAC_Cfg0        (EP9312_MAC+0x0100)
#define MAC_Cfg1        (EP9312_MAC+0x0104)
#define MAC_Cfg2        (EP9312_MAC+0x0108)
#define MAC_Cfg3        (EP9312_MAC+0x010C)
#define MAC_Fifo        (EP9312_MAC+0x4000)

// MII/PHY interface control
#define MiiCmdRead   0x8000
#define MiiCmdWrite  0x4000

// PHY registers
#define PHY_CONTROL_REG     (0)
#define PHY_STATUS_REG      (1)
#define PHY_ID_ONE          (2)
#define PHY_ID_TWO          (3)
#define PHY_AUTONEG_ADVERT  (4)
#define PHY_AUTONEG_REMOTE  (5)

#define PHY_CONTROL_RESET           (1<<15)
#define PHY_CONTROL_LOOPBACK        (1<<14)
#define PHY_CONTROL_SPEED100        (1<<13)
#define PHY_CONTROL_AUTONEG_EN      (1<<12)
#define PHY_CONTROL_POWERDOWN       (1<<11)
#define PHY_CONTROL_MII_DIS         (1<<10)
#define PHY_CONTROL_AUTONEG_RST     (1<< 9)
#define PHY_CONTROL_DPLX_FULL       (1<< 8)
#define PHY_CONTROL_COLLTEST        (1<< 7)
  
#define PHY_STATUS_CAP_T4           (1<<15)
#define PHY_STATUS_CAP_100TXF       (1<<14)
#define PHY_STATUS_CAP_100TXH       (1<<13)
#define PHY_STATUS_CAP_10TF         (1<<12)
#define PHY_STATUS_CAP_10TH         (1<<11)
#define PHY_STATUS_CAP_SUPR         (1<< 6)
#define PHY_STATUS_AUTONEG_ACK      (1<< 5)
#define PHY_STATUS_REMOTEFAULT      (1<< 4)
#define PHY_STATUS_CAP_AUTONEG      (1<< 3)
#define PHY_STATUS_LINK_OK          (1<< 2)
#define PHY_STATUS_JABBER           (1<< 1)
#define PHY_STATUS_EXTREGS          (1<< 0)

// These are the same for both AUTONEG registers
#define PHY_AUTONEG_NEXT            (1<<15)
#define PHY_AUTONEG_ACK             (1<<14)
#define PHY_AUTONEG_REMOTEFAULT     (1<<13)
#define PHY_AUTONEG_100BASET4       (1<< 9)
#define PHY_AUTONEG_100BASETX_FDX   (1<< 8)
#define PHY_AUTONEG_100BASETX_HDX   (1<< 7)
#define PHY_AUTONEG_10BASET_FDX     (1<< 6)
#define PHY_AUTONEG_10BASET_HDX     (1<< 5)
#define PHY_AUTONEG_CSMA_802_3      (1<< 0)

#endif // _CYGONCE_EP93XX_ETH_H_

// EOF ep93xx_eth.h
