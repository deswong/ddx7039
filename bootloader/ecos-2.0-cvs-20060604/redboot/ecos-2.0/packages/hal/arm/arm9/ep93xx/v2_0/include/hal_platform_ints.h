#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock support
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
// Date:         2001-04-23
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the Cirrus Logic EP93xx are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/ep93xx.h>             // Board register definitions
#include <cyg/hal/hal_io.h>             // I/O macros
#include <cyg/hal/hal_misc.h>           // SZ_1M

// These are interrupts on the EP93xx

#define CYGNUM_HAL_INTERRUPT_DBGRX        2 // Processor debug Rx
#define CYGNUM_HAL_INTERRUPT_DBGTX        3 // Processor debug Tx
#define CYGNUM_HAL_INTERRUPT_TC1OI        4 // Timer #1
#define CYGNUM_HAL_INTERRUPT_TC2OI        5 // Timer #2
#define CYGNUM_HAL_INTERRUPT_AACINTR      6 // AAC/AC97 interrrupt.
#define CYGNUM_HAL_INTERRUPT_DMAM2P0      7 // DMA Memory to Peripheral 0
#define CYGNUM_HAL_INTERRUPT_DMAM2P1      8 // DMA Memory to Peripheral 1
#define CYGNUM_HAL_INTERRUPT_DMAM2P2      9 // DMA Memory to Peripheral 2
#define CYGNUM_HAL_INTERRUPT_DMAM2P3      10 // DMA Memory to Peripheral 3
#define CYGNUM_HAL_INTERRUPT_DMAM2P4      11 // DMA Memory to Peripheral 4
#define CYGNUM_HAL_INTERRUPT_DMAM2P5      12 // DMA Memory to Peripheral 5
#define CYGNUM_HAL_INTERRUPT_DMAM2P6      13 // DMA Memory to Peripheral 6
#define CYGNUM_HAL_INTERRUPT_DMAM2P7      14 // DMA Memory to Peripheral 7
#define CYGNUM_HAL_INTERRUPT_DMAM2P8      15 // DMA Memory to Peripheral 8
#define CYGNUM_HAL_INTERRUPT_DMAM2P9      16 // DMA Memory to Peripheral 9
#define CYGNUM_HAL_INTERRUPT_DMAM2M0      17 // DMA Memory to Memory 0
#define CYGNUM_HAL_INTERRUPT_DMAM2M1      18 // DMA Memory to Memory 1
#define CYGNUM_HAL_INTERRUPT_GPIO0INTR    19 // GPIO 0 interrupt.
#define CYGNUM_HAL_INTERRUPT_GPIO1INTR    20 // GPIO 1 interrupt.
#define CYGNUM_HAL_INTERRUPT_GPIO2INTR    21 // GPIO 2 interrupt.
#define CYGNUM_HAL_INTERRUPT_GPIO3INTR    22 // GPIO 3 interrupt.
#define CYGNUM_HAL_INTERRUPT_UART1RXINTR1 23 // Uart 1 Receive interrupt
#define CYGNUM_HAL_INTERRUPT_UART1TXINTR1 24 // Uart 1 Transmit interrupt
#define CYGNUM_HAL_INTERRUPT_UART2RXINTR2 25 // Uart 1 Receive interrupt
#define CYGNUM_HAL_INTERRUPT_UART2TXINTR2 26 // Uart 1 Transmit interrupt
#define CYGNUM_HAL_INTERRUPT_UART3RXINTR3 27 // Uart 1 Receive interrupt
#define CYGNUM_HAL_INTERRUPT_UART3TXINTR3 28 // Uart 1 Transmit interrupt
#define CYGNUM_HAL_INTERRUPT_KEY          29 // Keyboard
#define CYGNUM_HAL_INTERRUPT_TOUCH        30 // Touch panel
#define CYGNUM_HAL_INTERRUPT_EXT0         32 // External int #0
#define CYGNUM_HAL_INTERRUPT_EXT1         33 // External int #1
#define CYGNUM_HAL_INTERRUPT_EXT2         34 // External int #2
#define CYGNUM_HAL_INTERRUPT_TINTR        35 // 64 Hz tick interrupt
#define CYGNUM_HAL_INTERRUPT_WEINT        36 // Watchdog Expired Interrupt
#define CYGNUM_HAL_INTERRUPT_RTC          37 // RTC Interrupt
#define CYGNUM_HAL_INTERRUPT_IRDA         38 // IrDA
#define CYGNUM_HAL_INTERRUPT_MAC          39 // Ethernet
#define CYGNUM_HAL_INTERRUPT_EXT3         40 // External int #3

#define CYGNUM_HAL_INTERRUPT_1HZ          42 // 1Hz clock interrupt
#define CYGNUM_HAL_INTERRUPT_V_SYNC       43 // Vertical sync
#define CYGNUM_HAL_INTERRUPT_V_FIFO       44 // Raster Video FIFO interrupt
#define CYGNUM_HAL_INTERRUPT_SSP_RX       45 // SSP Rx interrupt
#define CYGNUM_HAL_INTERRUPT_SSP_TX       46 // SPI Tx interrupt
#define CYGNUM_HAL_INTERRUPT_GPIO4INTR    47 // GPIO 0 interrupt.
#define CYGNUM_HAL_INTERRUPT_GPIO5INTR    48 // GPIO 1 interrupt.
#define CYGNUM_HAL_INTERRUPT_GPIO6INTR    49 // GPIO 2 interrupt.
#define CYGNUM_HAL_INTERRUPT_GPIO7INTR    50 // GPIO 3 interrupt.
#define CYGNUM_HAL_INTERRUPT_TC3OI        51 // Timer #3
#define CYGNUM_HAL_INTERRUPT_UART1        52 // Uart #1 combined interrupt
#define CYGNUM_HAL_INTERRUPT_SSP          53 // SSP combined interrupt.
#define CYGNUM_HAL_INTERRUPT_UART2        54 // Uart #2 combined interrupt
#define CYGNUM_HAL_INTERRUPT_UART3        55 // Uart #3 combined interrupt
#define CYGNUM_HAL_INTERRUPT_USB          56 // USB Host Controller
#define CYGNUM_HAL_INTERRUPT_MAC_PME      57 // Ethernet MAC PME interrupt
#define CYGNUM_HAL_INTERRUPT_CU           58 // Crunch(tm) Unit
#define CYGNUM_HAL_INTERRUPT_GPIO         59 // Combined GPIO
#define CYGNUM_HAL_INTERRUPT_I2S          60 // I2S combined interrupt
#define CYGNUM_HAL_INTERRUPT_NONE         -1

#define CYGNUM_HAL_ISR_MIN            (CYGNUM_HAL_INTERRUPT_DBGRX)
#define CYGNUM_HAL_ISR_MAX            (CYGNUM_HAL_INTERRUPT_I2S)

#define CYGNUM_HAL_ISR_COUNT          (CYGNUM_HAL_ISR_MAX-CYGNUM_HAL_ISR_MIN+1)

//----------------------------------------------------------------------------
// Reset.
#define HAL_PLATFORM_RESET()                                                  \
    CYG_MACRO_START                                                           \
    cyg_uint32 ctrl;                                                          \
                                                                              \
    HAL_DISABLE_INTERRUPTS(ctrl);                                             \
                                                                              \
    __asm__("mrc p15, 0, r0, c1, c0, 0                                      \n\
             bic r0, r0, #0xc0000006                                        \n\
             bic r0, r0, #0x00001000                                        \n\
             mcr p15, 0, r0, c1, c0, 0                                      \n\
             mov r0, #7 << 5                                                \n\
1:           orr r1, r0, #63 << 26                                          \n\
2:           mcr p15, 0, r1, c7, c14, 2                                     \n\
             subs r1, r1, #1 << 26                                          \n\
             bcs 2b                                                         \n\
             subs r0, r0, #1 << 5                                           \n\
             bcs 1b                                                         \n\
             mov r0, #0x00000000                                            \n\
             mcr p15, 0, r0, c7, c5, 0                                      \n\
             mcr p15, 0, r0, c7, c10, 4                                     \n\
             mcr p15, 0, r0, c8, c7, 0\n");                                   \
                                                                              \
    HAL_WRITE_UINT32(0x00000000, 0x00f000f0);                                 \
    HAL_WRITE_UINT32(0x00000000, 0x00ff00ff);                                 \
    HAL_WRITE_UINT32(EP9312_MAC + 0x20, 1);                                   \
    do {                                                                      \
        HAL_READ_UINT32(EP9312_MAC + 0x20, ctrl);                             \
    } while (ctrl & 1);                                                       \
    do {                                                                      \
        HAL_READ_UINT32(EP9312_UART1 + EP9312_UART_FR, ctrl);                 \
    } while (!(ctrl & (EP9312_UART_FR_TXFE | EP9312_UART_FR_BUSY)));          \
    HAL_WRITE_UINT32(EP9312_UART1 + EP9312_UART_CR, 0);                       \
    HAL_WRITE_UINT32(EP9312_SYSCON + EP9312_SYSCON_LOCK, 0xaa);               \
    HAL_WRITE_UINT32(EP9312_DEVCFG, 0);                                       \
    HAL_WRITE_UINT32(EP9312_CLKSET1, 0x000398e7);                             \
    __asm__("nop");                                                           \
    __asm__("nop");                                                           \
    __asm__("nop");                                                           \
    __asm__("nop");                                                           \
    __asm__("nop");                                                           \
    HAL_WRITE_UINT32(EP9312_CLKSET2, 0x0003c317);                             \
                                                                              \
    HAL_READ_UINT32(EP9312_SYSCON + 0x9c, ctrl);                              \
    if(ctrl & 0x8)                                                            \
        __asm__("ldr r0, =0x80090000                                        \n\
                 mrc p15, 0, r1, c1, c0, 0                                  \n\
                 bic r1, r1, #0x00000001                                    \n\
                 mcr p15, 0, r1, c1, c0, 0                                  \n\
                 mov pc, r0");                                                \
    else                                                                      \
        __asm__("ldr r0, =0x00000000                                        \n\
                 mrc p15, 0, r1, c1, c0, 0                                  \n\
                 bic r1, r1, #0x00000001                                    \n\
                 mcr p15, 0, r1, c1, c0, 0                                  \n\
                 mov pc, r0");                                                \
    CYG_MACRO_END

#define HAL_PLATFORM_RESET_ENTRY 0x00000000

// ------------------------------------------------------------------------
// Cachable/uncachable memory address manipulations

// Pagesize at a particular virtual address.
// This does not depend on the vaddr.
#define HAL_MM_PAGESIZE( vaddr, pagesize ) CYG_MACRO_START                     \
    (pagesize) = SZ_1M;                                                        \
CYG_MACRO_END

// Get the physical address from a virtual address:
// Everything is direct-mapped, aside from 128Mb of SDRAM at
// physical:         0x000 Mb
// virtual cached:   0x000 Mb
// virtual uncached: 0xC00 Mb

#define HAL_PHYS_TO_VIRT_ADDRESS( paddr, vaddr ) CYG_MACRO_START               \
	vaddr = hal_phys_to_virt_address(paddr);    						       \
CYG_MACRO_END

#ifndef CYG_HAL_STARTUP_RAM
#define HAL_VIRT_TO_UNCACHED_ADDRESS( vaddr, uaddr ) CYG_MACRO_START           \
	uaddr = hal_virt_to_uncached_address(vaddr);						       \
CYG_MACRO_END
#else
#define HAL_VIRT_TO_UNCACHED_ADDRESS( vaddr, uaddr ) CYG_MACRO_START           \
    (uaddr) = (cyg_uint32)(vaddr);                                             \
CYG_MACRO_END
#endif

//---------------------------------------------------------------------------

#endif // CYGONCE_HAL_PLATFORM_INTS_H

// EOF hal_platform_ints.h
