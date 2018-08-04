#ifndef CYGONCE_EP93XX_H
#define CYGONCE_EP93XX_H

/*=============================================================================
//
//      ep93xx.h
//
//      Platform specific support (register layout, etc)
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-04-23
// Purpose:      Cirrus Logic/EP93XX platform specific information
// Description: 
// Usage:        #include <cyg/hal/ep93xx.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

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

/*---------------------------------------------------------------------------*/
/* end of ep93xx.h                                                          */
#endif /* CYGONCE_EP93XX_H */
