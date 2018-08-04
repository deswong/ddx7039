//==========================================================================
//
//      ep93xx_misc.c
//
//      HAL misc board support code for ARM9/EP93XX
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
// Contributors: hmt, Travis C. Furrer <furrer@mit.edu>, jskov
// Date:         2001-04-23
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/
#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt names
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/ep93xx.h>             // Platform specifics
#include <cyg/hal/regs_VIC.h>

#include <cyg/infra/diag.h>             // diag_printf

unsigned long ConfigInfo[5];
unsigned long SDRAMAddressMap[8];
unsigned long SDRAMSize;

#define CONFIG_SDRAM_BANK_SIZE          0
#define CONFIG_SDRAM_ADDRESS_MASK       1
#define CONFIG_SDRAM_BANK_COUNT         2
#define CONFIG_SDRAM_BASE               3
#define CONFIG_FLASH_BASE               4

//=============================================================================
// hal_virt_to_phys_address
//=============================================================================
// Returns a physical address given a virtual address.
cyg_uint32 hal_virt_to_phys_address(cyg_uint32 vaddr)
{
    int i;

    /*
     * If the virtual address is in SDRAM, then find its physical address.
     */
    if(((vaddr >= 0x00000000) && (vaddr < 0x10000000)) ||
       ((vaddr >= 0xc0000000) && (vaddr < 0xd0000000)))
    {
        vaddr &= 0x0fffffff;

        for(i = 0; i < ConfigInfo[CONFIG_SDRAM_BANK_COUNT]; i++)
        {
            if(vaddr < ConfigInfo[CONFIG_SDRAM_BANK_SIZE])
            {
                break;
            }

            vaddr -= ConfigInfo[CONFIG_SDRAM_BANK_SIZE];
        }

        if(i == ConfigInfo[CONFIG_SDRAM_BANK_COUNT])
        {
            vaddr = 0xBAD0ADD8;
        }
        else
        {
            vaddr = ConfigInfo[CONFIG_SDRAM_BASE] + SDRAMAddressMap[i] + vaddr;
        }
    }

    /*
     * If the virtual is in FLASH, then find its physical address.
     */
    else if(((vaddr >= 0x60000000) && (vaddr < 0x70000000)) |
            ((vaddr >= 0xe0000000) && (vaddr < 0xf0000000)))
    {
        vaddr &= 0x0fffffff;

        vaddr |= ConfigInfo[CONFIG_FLASH_BASE];
    }

    return(vaddr);
}

cyg_uint32 hal_virt_to_uncached_address(cyg_uint32 vaddr)
{
    if(vaddr < 0x10000000)
    {
        vaddr += 0xc0000000;
    }
    else
    {
        vaddr = 0xBAD0ADD8u; /* nothing there; uncached unavailable */
    }

    return (vaddr);
}

cyg_uint32 hal_phys_to_virt_address(cyg_uint32 paddr)
{
    int i;

    if((paddr >= ConfigInfo[CONFIG_SDRAM_BASE]) &&
       (paddr <= (ConfigInfo[CONFIG_SDRAM_BASE] + 0x0fffffff)))
    {
        paddr &= 0x0fffffff;

        for(i = 0; i < ConfigInfo[CONFIG_SDRAM_BANK_COUNT]; i++)
        {
            if((paddr >= SDRAMAddressMap[i]) &&
               (paddr <
                (SDRAMAddressMap[i] + ConfigInfo[CONFIG_SDRAM_BANK_SIZE])))
            {
                break;
            }
        }

        if(i != ConfigInfo[CONFIG_SDRAM_BANK_COUNT])
        {
            paddr = ((ConfigInfo[CONFIG_SDRAM_BANK_SIZE] * i) +
                     (paddr & (ConfigInfo[CONFIG_SDRAM_BANK_SIZE] - 1)));
        }
        else
        {
            paddr = 0xBAD0ADD8;
        }
    }
    else if((paddr >= ConfigInfo[CONFIG_FLASH_BASE]) &&
            (paddr <= (ConfigInfo[CONFIG_FLASH_BASE] + 0x0fffffff)))
    {
        paddr &= 0x0fffffff;
        paddr |= 0x60000000;
    }

    return (paddr);
}

//=============================================================================
//=============================================================================
// Write a locked syscon Register.
static __inline__ void
sysconWriteReg(int offset, unsigned long val)
{
    HAL_WRITE_UINT32(EP9312_SYSCON+EP9312_SYSCON_LOCK, 0xAA);
    HAL_WRITE_UINT32(EP9312_SYSCON+offset, val);
}


//
// for kenwood target
// 
/* ep93xx.h
// System control registers
#define EP9312_SYSCON                   0x80930000
#define EP9312_SYSCON_LOCK              0x00c0

// Clock control registers
#define EP9312_CLKSET1                  (EP9312_SYSCON + 0x0020)
#define EP9312_CLKSET1_NBYP             0x00800000
#define EP9312_CLKSET1_HCLKDIV_MASK     0x00700000
#define EP9312_CLKSET1_HCLKDIV_SHIFT    20
#define EP9312_CLKSET1_FCLKDIV_MASK     0x0e000000
#define EP9312_CLKSET1_FCLKDIV_SHIFT    25
#define EP9312_CLKSET1_PCLKDIV_MASK     0x000c0000    
#define EP9312_CLKSET1_PCLKDIV_SHIFT    18

#define EP9312_CLKSET2                  (EP9312_SYSCON + 0x0024)
#define EP9312_CLKSET2_PLL2_EN          0x00000001
#define EP9312_CLKSET2_PLL2EXCLKSEL     0x00000002
#define EP9312_CLKSET2_PLL2_P_MASK      0x0000007C
#define EP9312_CLKSET2_PLL2_P_SHIFT     2
#define EP9312_CLKSET2_PLL2_M2_MASK     0x00000F80
#define EP9312_CLKSET2_PLL2_M2_SHIFT    7
#define EP9312_CLKSET2_PLL2_M1_MASK     0x0001F000
#define EP9312_CLKSET2_PLL2_M1          12
#define EP9312_CLKSET2_PLL2_PS_MASK     0x000C0000
#define EP9312_CLKSET2_PLL2_PS_SHIFT    18
#define EP9312_CLKSET2_USBDIV_MASK      0xF0000000
#define EP9312_CLKSET2_USBDIV_SHIFT     28

#define EP9312_PWRCNT                   (EP9312_SYSCON + 0x0004)
#define EP9312_PWRCNT_UARTBAUD          0x20000000

#define EP9312_DEVCFG                   (EP9312_SYSCON + 0x0080)
#define EP9312_DEVCFG_U1EN              0x00040000
#define EP9312_DEVCFG_U2EN              0x00100000
#define EP9312_DEVCFG_U3EN              0x01000000
#define EP9312_DEVCFG_SWRST             0x80000000  // 1->0 in this bit resets board

// WATCHDOG
#define EP9312_WATCHDOG                 0x80940000

// UARTs
#define EP9312_UART_DATA        0x0000  // Data/FIFO register

#define EP9312_UART_SR          0x0004  // Status register
#define EP9312_UART_SR_FE       0x0001  // Framing error
#define EP9312_UART_SR_PE       0x0002  // Parity error
#define EP9312_UART_SR_BE       0x0004  // Break error
#define EP9312_UART_SR_OE       0x0008  // Overrun

#define EP9312_UART_LCR_H       0x0008  // Control register High
#define EP9312_UART_LCR_H_BRK   0x0001 // Send break
#define EP9312_UART_LCR_H_PEN   0x0002 // Enable parity
#define EP9312_UART_LCR_H_EPS   0x0004 // Odd/Even parity
#define EP9312_UART_LCR_H_STP2  0x0008 // One/Two stop bits
#define EP9312_UART_LCR_H_FE    0x0010 // Enable FIFO
#define EP9312_UART_LCR_H_WLEN5 0x0000 // Word length - 5 bits
#define EP9312_UART_LCR_H_WLEN6 0x0020 // Word length - 6 bits
#define EP9312_UART_LCR_H_WLEN7 0x0040 // Word length - 7 bits
#define EP9312_UART_LCR_H_WLEN8 0x0060 // Word length - 8 bits

#define EP9312_UART_LCR_M       0x000C  // Baud rate 8..15
#define EP9312_UART_LCR_L       0x0010  // Baud rate 0..7

#define EP9312_UART_CR          0x0014  // Control register
#define EP9312_UART_CR_UARTE    0x0001 // Enable uart
#define EP9312_UART_CR_RIE      0x0010 // Enable Rx interrupt
#define EP9312_UART_CR_TIE      0x0020 // Enable Tx interrupt
#define EP9312_UART_CR_RTIE     0x0040 // Enable Rx timeout interrupt
#define EP9312_UART_CR_LBE      0x0080 // Loopback mode

#define EP9312_UART_FR          0x0018  // Flags register
#define EP9312_UART_FR_CTS      0x0001 // Clear-to-send status
#define EP9312_UART_FR_DSR      0x0002 // Data-set-ready status
#define EP9312_UART_FR_DCD      0x0004 // Data-carrier-detect status
#define EP9312_UART_FR_BUSY     0x0008 // Transmitter busy
#define EP9312_UART_FR_RXFE     0x0010 // Receive FIFO empty
#define EP9312_UART_FR_TXFF     0x0020 // Transmit FIFO full
#define EP9312_UART_FR_RXFF     0x0040 // Receive FIFO full
#define EP9312_UART_FR_TXFE     0x0080 // Transmit FIFO empty

#define EP9312_UARTIIR          0x001C  // Interrupt status
#define EP9312_UARTIIR_MS       0x0001 // Modem status interrupt
#define EP9312_UARTIIR_RIS      0x0002 // Rx interrupt
#define EP9312_UARTIIR_TIS      0x0004 // Tx interrupt
#define EP9312_UARTIIR_RTIS     0x0008 // Rx timeout interrupt

#define EP9312_UART_MCR         0x0100  // Modem control

#define EP9312_UART1            0x808C0000
#define EP9312_UART2            0x808D0000
#define EP9312_UART3            0x808E0000

// LED interface - LED0 is green, LED1 is red
#define EP9312_LED_DATA         0x80840020
#define EP9312_LED_GREEN_ON     0x0001
#define EP9312_LED_RED_ON       0x0002

#define EP9312_LED_DDR          0x80840024
#define EP9312_LED_GREEN_ENABLE 0x0001
#define EP9312_LED_RED_ENABLE   0x0002

#define EP9312_GPIO             0x80840000
#define EP9312_GPIO_PADR        (EP9312_GPIO + 0x0000)
#define EP9312_GPIO_PBDR        (EP9312_GPIO + 0x0004)
#define EP9312_GPIO_PCDR        (EP9312_GPIO + 0x0008)
#define EP9312_GPIO_PDDR        (EP9312_GPIO + 0x000c)
#define EP9312_GPIO_PADDR       (EP9312_GPIO + 0x0010)
#define EP9312_GPIO_PBDDR       (EP9312_GPIO + 0x0014)
#define EP9312_GPIO_PCDDR       (EP9312_GPIO + 0x0018)
#define EP9312_GPIO_PDDDR       (EP9312_GPIO + 0x001c)

// Timers
#define EP9312_TIMERS                   0x80810000
#define EP9312_TIMERS_DEBUG_LO          (EP9312_TIMERS+0x0060)
#define EP9312_TIMERS_DEBUG_HI          (EP9312_TIMERS+0x0064)
#define EP9312_TIMERS_DEBUG_HI_RESET    0x00000000
#define EP9312_TIMERS_DEBUG_HI_START    0x00000100

// Ethernet controller
#define EP9312_MAC                      0x80010000

#define EP9312_SDRAMCTRL                0x80060000

#define EP9312_SDRAMCTRL_DEVCFG_0       0x0010
#define EP9312_SDRAMCTRL_DEVCFG_1       0x0014
#define EP9312_SDRAMCTRL_DEVCFG_2       0x0018
#define EP9312_SDRAMCTRL_DEVCFG_3       0x001c
#define EP9312_SDRAMCTRL_DEVCFG_AUTOPRE 0x01000000

#define EP9312_SDRAM_PHYS_BASE          0x00000000

#define EP9312_SSP                      0x808a0000
#define EP9312_SSP_CR0                  (EP9312_SSP + 0x0000)
#define EP9312_SSP_CR1                  (EP9312_SSP + 0x0004)
#define EP9312_SSP_DR                   (EP9312_SSP + 0x0008)
#define EP9312_SSP_SR                   (EP9312_SSP + 0x000c)
#define EP9312_SSP_CPSR                 (EP9312_SSP + 0x0010)
#define EP9312_SSP_IIR_ICR              (EP9312_SSP + 0x0014)
*/
#define EP9307_GPIO             0x80840000
//data
#define EP9307_GPIO_PADR        (EP9307_GPIO + 0x0000)
#define EP9307_GPIO_PBDR        (EP9307_GPIO + 0x0004)
#define EP9307_GPIO_PCDR        (EP9307_GPIO + 0x0008)
#define EP9307_GPIO_PDDR        (EP9307_GPIO + 0x000c)
#define EP9307_GPIO_PEDR        (EP9307_GPIO + 0x0020)
#define EP9307_GPIO_PFDR        (EP9307_GPIO + 0x0030)
#define EP9307_GPIO_PGDR        (EP9307_GPIO + 0x0038)
#define EP9307_GPIO_PHDR        (EP9307_GPIO + 0x0040)
//direction
#define EP9307_GPIO_PADDR       (EP9307_GPIO + 0x0010)
#define EP9307_GPIO_PBDDR       (EP9307_GPIO + 0x0014)
#define EP9307_GPIO_PCDDR       (EP9307_GPIO + 0x0018)
#define EP9307_GPIO_PDDDR       (EP9307_GPIO + 0x001c)
#define EP9307_GPIO_PEDDR       (EP9307_GPIO + 0x0024)
#define EP9307_GPIO_PFDDR       (EP9307_GPIO + 0x0034)
#define EP9307_GPIO_PGDDR       (EP9307_GPIO + 0x003C)
#define EP9307_GPIO_PHDDR       (EP9307_GPIO + 0x0044)

#define VOLATILE(reg) (*((volatile CYG_WORD32 *)(reg)))

void
io_init(void)
{
	unsigned long devcfg;
	/* DeviceCfg */
	/* 0000 1000 0000 0000 0000 1101 0000 0000 */
	devcfg = VOLATILE(EP9312_DEVCFG);
	devcfg |= 0x08000d00; 
	VOLATILE( EP9312_SYSCON+EP9312_SYSCON_LOCK ) = 0xAA;
	VOLATILE(EP9312_DEVCFG) = devcfg;
	/* PA */
	VOLATILE(EP9307_GPIO_PADDR) |= 0x61;
	VOLATILE(EP9307_GPIO_PADR)  &= 0x9e;
	/* PB */
	VOLATILE(EP9307_GPIO_PBDDR) |= 0x8c;
	VOLATILE(EP9307_GPIO_PBDR)  &= 0x73;
	/* PC */
	VOLATILE(EP9307_GPIO_PCDDR) = 0xff;
	VOLATILE(EP9307_GPIO_PCDR)  = 0x00;
	/* PD */
	VOLATILE(EP9307_GPIO_PDDDR) |= 0xdf;
	VOLATILE(EP9307_GPIO_PDDR)  &= 0x20;
	/* PF */
	VOLATILE(EP9307_GPIO_PFDDR) = 0xa1;
	VOLATILE(EP9307_GPIO_PFDR)  = 0x00;
	/* PG */
	VOLATILE(EP9307_GPIO_PGDDR) |= 0x04;
	VOLATILE(EP9307_GPIO_PGDR)  &= 0xfb;
	/* PH */
	VOLATILE(EP9307_GPIO_PHDDR) = 0x82;
	VOLATILE(EP9307_GPIO_PHDR)  = 0x00;
}


//
// Platform specific initialization
//
void
plf_hardware_init(void)
{
    //
    // Get a copy of the memory configuration information from the startup
    // code.
    //
    ConfigInfo[CONFIG_SDRAM_BANK_SIZE] = *(unsigned long *)0x2000;
    ConfigInfo[CONFIG_SDRAM_ADDRESS_MASK] = *(unsigned long *)0x2004;
    ConfigInfo[CONFIG_SDRAM_BANK_COUNT] = *(unsigned long *)0x2008;
    ConfigInfo[CONFIG_SDRAM_BASE] = *(unsigned long *)0x200c;
    ConfigInfo[CONFIG_FLASH_BASE] = *(unsigned long *)0x2010;

    //
    // Determine the total SDRAM available.
    //
    SDRAMSize = (ConfigInfo[CONFIG_SDRAM_BANK_SIZE] *
                 ConfigInfo[CONFIG_SDRAM_BANK_COUNT]);

    //
    // Get a copy of the SDRAM address map from the startup code.
    //
    SDRAMAddressMap[0] = *(unsigned long *)0x2100;
    SDRAMAddressMap[1] = *(unsigned long *)0x2104;
    SDRAMAddressMap[2] = *(unsigned long *)0x2108;
    SDRAMAddressMap[3] = *(unsigned long *)0x210c;
    SDRAMAddressMap[4] = *(unsigned long *)0x2110;
    SDRAMAddressMap[5] = *(unsigned long *)0x2114;
    SDRAMAddressMap[6] = *(unsigned long *)0x2118;
    SDRAMAddressMap[7] = *(unsigned long *)0x211c;

    //
    // Set this bit like the Kernel expects it(runs serial off of the 14Mhz).
    //
    HAL_WRITE_UINT32(EP9312_PWRCNT, EP9312_PWRCNT_UARTBAUD);

	io_init();
}

//-----------------------------------------------------------------------------
// This will get defined to HAL_CLOCK_INITIALIZE in hal_intr.h. This routine
// should only be called by test code.
//-----------------------------------------------------------------------------
void hal_clock_initialize(cyg_uint32 period)
{
    while(1);
}

//-----------------------------------------------------------------------------
// This will get defined to HAL_CLOCK_RESET.  This routine should only be
// called by test code.
//-----------------------------------------------------------------------------
externC void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    while(1);
}

// Read the current value of the clock, returning the number of hardware
// "ticks" that have occurred (i.e. how far away the current value is from
// the start)

// Note: The "contract" for this function is that the value is the number
// of hardware clocks that have happened since the last interrupt (i.e.
// when it was reset).  This value is used to measure interrupt latencies.
// However, since the hardware counter does not automatically reset, as
// many do, the value may be larger than the system "tick" interval.
// If this turns out to be the case, the result will be adjusted to be
// within the interval [0..N-1], where N is the number of hardware clocks/tick.

void hal_clock_read(cyg_uint32 *pvalue)
{
    while(1);
}

//
// Delay for some number of micro-seconds
//
void hal_delay_us(cyg_int32 usecs)
{
    cyg_uint32 val, val2, hi, hi2;
    int i, safety;

    //
    // Reset debug timer
    //
    HAL_WRITE_UINT32(EP9312_TIMERS_DEBUG_HI, EP9312_TIMERS_DEBUG_HI_RESET);

    // TODO is this required?
    // Required for timer to properly reset??
    //
    for (i = 0;  i < 100;  i++) ;

    HAL_WRITE_UINT32(EP9312_TIMERS_DEBUG_HI, EP9312_TIMERS_DEBUG_HI_START);

    //
    // Wait for timer
    //
    do {
        HAL_READ_UINT32(EP9312_TIMERS_DEBUG_LO, val);
        HAL_READ_UINT32(EP9312_TIMERS_DEBUG_HI, hi);
        safety = 1000;  // Surely this is more than 1us
        while (safety-- > 0) {
            HAL_READ_UINT32(EP9312_TIMERS_DEBUG_LO, val2);
            HAL_READ_UINT32(EP9312_TIMERS_DEBUG_HI, hi2);
            if (val != val2) {
                // Timer is going, continue
                break;
            }
        }
        if (safety == 0) {
            // Timer must be stuck, give up
            return;
        }
    } while (val < usecs);
}

//-----------------------------------------------------------------------------
// int hal_IRQ_handler(void)
//-----------------------------------------------------------------------------
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
int hal_IRQ_handler(void)
{
    return(0);
}

//-----------------------------------------------------------------------------
// void hal_interrupt_mask(int InterruptNumber)
//-----------------------------------------------------------------------------
// Disable the interrupt indicated by InterruptNumber
//
void hal_interrupt_mask(int InterruptNumber)
{
    if (InterruptNumber<32)
    {
        //
        // Mask the interrupt
        //
        IntControl[0]->VICIntEnClear.Value = (0x1 << InterruptNumber);
    }
    else
    {
        //
        // Mask the interrupt
        //
        IntControl[1]->VICIntEnClear.Value = (0x1 << (InterruptNumber - 32));
    }
}

//-----------------------------------------------------------------------------
// void hal_interrupt_unmask(int InterruptNumber)
//-----------------------------------------------------------------------------
// Enable the interrupt indicated by InterruptNumber
//
void hal_interrupt_unmask(int InterruptNumber)
{
    if (InterruptNumber<32)
    {
        //
        // Mask the interrupt
        //
        IntControl[0]->VICIntEnable.Value = (0x1 << InterruptNumber);
    }
    else
    {
        //
        // Mask the interrupt
        //
        IntControl[1]->VICIntEnable.Value = (0x1 << (InterruptNumber - 32));
    }
}

//-----------------------------------------------------------------------------
// void hal_interrupt_acknowledge(int vector)
//-----------------------------------------------------------------------------
// There is no acknowledge at the global level in the ARM VIC.
//
void hal_interrupt_acknowledge(int vector)
{
    return;
}

//-----------------------------------------------------------------------------
// void hal_interrupt_acknowledge(int vector)
//-----------------------------------------------------------------------------
// All interrupts are level triggered, so this function cannot be implemented.
//
void hal_interrupt_configure(int vector, int level, int up)
{
    return;
}

//-----------------------------------------------------------------------------
// void hal_interrupt_acknowledge(int vector)
//-----------------------------------------------------------------------------
// This function is unimplemented because unless vectored interrupts are used,
// priority is fixed.
//
void hal_interrupt_set_level(int vector, int level)
{
    return;
}

// ------------------------------------------------------------------------
// EOF ep93xx_misc.c
