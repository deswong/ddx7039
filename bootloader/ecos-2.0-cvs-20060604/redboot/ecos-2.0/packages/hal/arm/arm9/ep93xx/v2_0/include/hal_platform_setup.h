#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H
/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
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
// Purpose:      Cirrus Logic EP92xx platform specific support routines
// Description:
// Usage:        #include <cyg/hal/hal_platform_setup.h>
//     Only used by "vectors.S"
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>             // System-wide configuration info
#include CYGBLD_HAL_VARIANT_H           // Variant specific configuration
#include CYGBLD_HAL_PLATFORM_H          // Platform specific configuration
#include <cyg/hal/hal_mmu.h>            // MMU definitions
#include <cyg/hal/ep93xx.h>             // Platform specific hardware definitions

#if defined(CYG_HAL_ARM_ARM9_EP93XX_PREAMBLE)

#define PLATFORM_PREAMBLE _platform_preamble
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
    .macro  _platform_preamble
    b       start_here

#define GLOBALCFG_INIT                       0x00000001
#define GLOBALCFG_MRS                        0x00000002
#define GLOBALCFG_SMEMBUSY                   0x00000020
#define GLOBALCFG_LCR                        0x00000040
#define GLOBALCFG_REARBEN                    0x00000080
#define GLOBALCFG_CLKSHUTDOWN                0x40000000
#define GLOBALCFG_CKE                        0x80000000

#define REFRESH_MASK                         0x0000FFFF

#define BOOTSTATUS_WIDTH_32                  0x00000002
#define BOOTSTATUS_WIDTH_16                  0x00000001
#define BOOTSTATUS_WIDTH_8                   0x00000000
#define BOOTSTATUS_WIDTH_MASK                0x00000003
#define BOOTSTATUS_MEDIA                     0x00000004

#define DEVCFG_EXTBUSWIDTH                   0x00000004
#define DEVCFG_BANKCOUNT                     0x00000008
#define DEVCFG_SROM512                       0x00000010
#define DEVCFG_SROMLL                        0x00000020
#define DEVCFG_2KPAGE                        0x00000040
#define DEVCFG_SFCONFIGADDR                  0x00000080
#define DEVCFG_CASLAT_MASK                   0x00070000
#define DEVCFG_CASLAT_2                      0x00010000
#define DEVCFG_CASLAT_3                      0x00020000
#define DEVCFG_CASLAT_4                      0x00030000
#define DEVCFG_CASLAT_5                      0x00040000
#define DEVCFG_CASLAT_6                      0x00050000
#define DEVCFG_CASLAT_7                      0x00060000
#define DEVCFG_CASLAT_8                      0x00070000
#define DEVCFG_WBL                           0x00080000
#define DEVCFG_RASTOCAS_MASK                 0x00300000
#define DEVCFG_RASTOCAS_2                    0x00200000
#define DEVCFG_RASTOCAS_3                    0x00300000
#define DEVCFG_AUTOPRECHARGE                 0x01000000

//*****************************************************************************
//
// Configure the SDRAM based on the supplied settings.
//
//*****************************************************************************
SDRAMconfig:
    //
    // Program the SDRAM device configuration register.
    //
    ldr     r3, =EP9312_SDRAMCTRL
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn0
    str     r0, [r3, #0x0010]
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn1
    str     r0, [r3, #0x0014]
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn2
    str     r0, [r3, #0x0018]
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn3
    str     r0, [r3, #0x001c]
#endif

    //
    // Set the Initialize and MRS bits (issue continuous NOP commands
    // (INIT & MRS set))
    //
    ldr     r4, =(GLOBALCFG_INIT | GLOBALCFG_MRS | GLOBALCFG_CKE)
    str     r4, [r3, #0x0004]

    //
    // Delay for 200us.
    //
    mov     r4, #0x3000
delay1:
        subs    r4, r4, #1
        bne     delay1

    //
    // Clear the MRS bit to issue a precharge all.
    //
    ldr     r4, =(GLOBALCFG_INIT | GLOBALCFG_CKE)
    str     r4, [r3, #0x0004]

    //
    // The part does not issue the precharge all command, so manually cause a
    // precharge of each bank (the precharge occurs at the end of the read).
    //
    //and     r4, r2, #0xf0000000
    //ldr     r5, [r4]
    //add     r4, r4, r1
    //ldr     r5, [r4]
    //add     r4, r4, r1
    //ldr     r5, [r4]
    //add     r4, r4, r1
    //ldr     r5, [r4]

    //
    // Temporarily set the refresh timer to 0x10.  Make it really low so that
    // refresh cycles are generated.
    //
    ldr     r4, =0x10
    str     r4, [r3, #0x0008]

    //
    // Delay for at least 80 SDRAM clock cycles.
    //
    mov     r4, #80
delay2:
        subs    r4, r4, #1
        bne     delay2

    //
    // Set the refresh timer to the fastest required for any device that might
    // be used.
    //
    ldr     r4, =0x01e0
    str     r4, [r3, #0x0008]

    //
    // Select mode register update mode.
    //
    ldr     r4, =(GLOBALCFG_CKE | GLOBALCFG_MRS)
    str     r4, [r3, #0x0004]

    //
    // Program the mode register on the SDRAM.
    //
    ldr     r4, [r2]

    //
    // Select normal operating mode.
    //
    ldr     r4, =GLOBALCFG_CKE
    str     r4, [r3, #0x0004]

    //
    // Return to the caller.
    //
    mov     pc, lr

//*****************************************************************************
//
// Test to see if the SDRAM has been configured in a usable mode.
//
//*****************************************************************************
SDRAMTest:
    //
    // Load the test patterns to be written to SDRAM.
    //
    ldr     r1, =0xf00dface
    ldr     r2, =0xdeadbeef
    ldr     r3, =0x08675309
    ldr     r4, =0xdeafc0ed

    //
    // Store the test patterns to SDRAM.
    //
    stmia   r0, {r1-r4}

    //
    // Load the test patterns from SDRAM one at a time and compare them to the
    // actual pattern.
    //
    ldr     r5, [r0]
    cmp     r5, r1
    ldreq   r5, [r0, #0x0004]
    cmpeq   r5, r2
    ldreq   r5, [r0, #0x0008]
    cmpeq   r5, r3
    ldreq   r5, [r0, #0x000c]
    cmpeq   r5, r4

    //
    // Return -1 if a mismatch was encountered, 0 otherwise.
    //
    mvnne   r0, #0x00000000
    moveq   r0, #0x00000000

    //
    // Return to the caller.
    //
    mov     pc, lr

//*****************************************************************************
//
// Determine the size of the SDRAM.  Use data=address for the scan.
//
//*****************************************************************************
SDRAMSize:
    //
    // Store zero at offset zero.
    //
    str     r0, [r0]

    //
    // Start checking for an alias at 1MB into SDRAM.
    //
    ldr     r1, =0x00100000

        //
        // Store the offset at the current offset.
        //
check_block_size:
        str     r1, [r0, r1]

        //
        // Read back from zero.
        //
        ldr     r2, [r0]

        //
        // Stop searching of an alias was found.
        //
        cmp     r1, r2
        beq     found_block_size

        //
        // Advance to the next power of two boundary.
        //
        mov     r1, r1, lsl #1

        //
        // Loop back if the size has not reached 256MB.
        //
        cmp     r1, #0x10000000
        bne     check_block_size

        //
        // A full 256MB of memory was found, so return it now.
        //
        ldr     r0, =0x10000000
        ldr     r1, =0x00000000
        ldr     r2, =0x00000001
        mov     pc, lr

    //
    // An alias was found.  See if the first block is 128MB in size.
    //
found_block_size:
    cmp     r1, #0x08000000

        //
        // The first block is 128MB, so there is no further memory.  Return it
        // now.
        ldreq   r0, =0x08000000
        ldreq   r1, =0x00000000
        ldreq   r2, =0x00000001
        moveq   pc, lr

    //
    // Save the block size, set the block address bits to zero, and initialize
    // the block count to one.
    //
    mov     r3, r1
    ldr     r4, =0x00000000
    ldr     r5, =0x00000001

        //
        // Look for additional blocks of memory by searching for non-aliases.
        // Store zero back to address zero.
        //
find_blocks:
        str     r0, [r0]

        //
        // Advance to the next power of two boundary.
        //
        mov     r1, r1, lsl #1

        //
        // Store the offset at the current offset.
        //
        str     r1, [r0, r1]

        //
        // Read back from zero.
        //
        ldr     r2, [r0]

        //
        // See if a non-alias was found.
        //
        cmp     r1, r2

            //
            // If a non-alias was found, then or in the block address bit and
            // multiply the block count by two (since there are two unique
            // blocks, one with this bit zero and one with it one).
            //
            orrne   r4, r4, r1
            movne   r5, r5, lsl #1

        //
        // Continue searching if there are more address bits to check.
        //
        cmp     r1, #0x08000000
        bne     find_blocks

    //
    // Return the block size, address mask, and count.
    //
    mov     r0, r3
    mov     r1, r4
    mov     r2, r5

    //
    // Return to the caller.
    //
    mov     pc, lr

//*****************************************************************************
//
// Compute the starting address of the SDRAM memory blocks.
//
//*****************************************************************************
SDRAMFind:
    //
    // Get a pointer to the SDRAM address map.
    //
    orr     r0, r11, #0x00002100

    //
    // If there is only one bank, then there is no need to look for it.
    //
    cmp     r10, #1
        moveq   r1, #0x00000000
        streq   r1, [r0]
        moveq   pc, lr

    //
    // Create a table mapping the individual address bits used to access the
    // various banks.
    //
    add     r1, r0, #0x00000080
    mov     r2, r10, lsr #1
    mov     r3, #0x00100000
find_bit:
        tst     r9, r3
        moveq   r3, r3, lsl #1
        beq     find_bit
    str     r3, [r1], #4
    mov     r3, r3, lsl #1
    movs    r2, r2, lsr #1
    bne     find_bit

    //
    // Using the individual address bits, compute the address of each bank of
    // SDRAM.
    //
    add     r1, r0, #0x00000080
    mov     r2, r10
    mov     r3, #0x00000000
find_bank:
        mov     r4, #0x00000000
        tst     r3, #0x00000001
        ldrne   r5, [r1]
        orrne   r4, r4, r5
        tst     r3, #0x00000002
        ldrne   r5, [r1, #4]
        orrne   r4, r4, r5
        tst     r3, #0x00000004
        ldrne   r5, [r1, #8]
        orrne   r4, r4, r5
        tst     r3, #0x00000008
        ldrne   r5, [r1, #12]
        orrne   r4, r4, r5
        tst     r3, #0x00000010
        ldrne   r5, [r1, #16]
        orrne   r4, r4, r5
        tst     r3, #0x00000020
        ldrne   r5, [r1, #20]
        orrne   r4, r4, r5
        tst     r3, #0x00000040
        ldrne   r5, [r1, #24]
        orrne   r4, r4, r5
        tst     r3, #0x00000080
        ldrne   r5, [r1, #28]
        orrne   r4, r4, r5
        str     r4, [r0], #4
        add     r3, r3, #1
        subs    r2, r2, #1
        bne     find_bank

    //
    // Return to the caller.
    //
    mov     pc, lr

//*****************************************************************************
//
// Builds the page table.
//
//*****************************************************************************
BuildPagetable:
    //
    // Load a pointer to the page table.
    //
    orr     r0, r11, #0x00004000

    //
    // Clear out the page table.
    //
    mov     r1, r0
    mov     r2, #0x00004000
    mov     r3, #0x00000000
clear:
        str     r3, [r1], #4
        subs    r2, r2, #4
        bne     clear

    //
    // Create a cacheable/bufferable mapping of SDRAM at 0x0000_0000, and a
    // non-cacheable/non-bufferable mapping at 0xc000_0000.
    //
    mov     r1, r0
    add     r2, r0, #0x00003000
    orr     r3, r11, #0x00002100
    mov     r4, r10
map_sdram_bank:
        ldr     r5, [r3], #4
        orr     r5, r5, r11
        orr     r5, r5, #0x00000c10
        mov     r6, r5
        orr     r5, r5, #0x0000000e
        orr     r6, r6, #0x00000002
        mov     r7, r8
map_sdram:
            str     r5, [r1], #4
            str     r6, [r2], #4
            add     r5, r5, #0x00100000
            add     r6, r6, #0x00100000
            subs    r7, r7, #0x00100000
            bne     map_sdram
        subs    r4, r4, #1
        bne     map_sdram_bank

    //
    // Create a read-write mapping of FLASH at virtual address 0x6000_0000.
    //
    add     r1, r0, #0x00001800
#if defined(HAL_PLATFORM_EP9301) || defined(HAL_PLATFORM_EP9302)
    mov     r2, #0x00000040
#else
    mov     r2, #0x00000080
#endif
    orr     r3, r12, #0x00000012
    orr     r3, r3, #0x00000c00
map_flash_6:
        str     r3, [r1], #4
        add     r3, r3, #0x00100000
        subs    r2, r2, #4
        bne     map_flash_6

    //
    // Create a read-write mapping of FLASH at virtual address 0xe000_0000.
    //
    add     r1, r0, #0x00003800
#if defined(HAL_PLATFORM_EP9301) || defined(HAL_PLATFORM_EP9302)
    mov     r2, #0x00000040
#else
    mov     r2, #0x00000080
#endif
    orr     r3, r12, #0x00000012
    orr     r3, r3, #0x00000c00
map_flash_e:
        str     r3, [r1], #4
        add     r3, r3, #0x00100000
        subs    r2, r2, #4
        bne     map_flash_e

    //
    // Create a read-write mapping of the hardware registers at virtual address
    // 0x8000_0000.
    //
    add     r1, r0, #0x00002000
    mov     r2, #0x00000400
    ldr     r3, =0x80000c12
map_hw:
        str     r3, [r1], #4
        add     r3, r3, #0x00100000
        subs    r2, r2, #4
        bne     map_hw

    //
    // Return to the caller.
    //
    mov     pc, lr

//*****************************************************************************
//
// Perform basic platform setup.
//
//*****************************************************************************
Setup:
    //
    // Make sure the processor is in SVC32 mode with IRQ and FIQ disabled.
    //
    ldr     r0, =0xd3
    msr     cpsr, r0

    //
    // Make sure caches are off and invalidated.
    //
    ldr     r0, =0x00000000
    mcr     p15, 0, r0, c1, c0, 0
    mcr     p15, 0, r0, c7, c7, 0
    mcr     p15, 0, r0, c8, c7, 0
    nop
    nop
    nop
    nop
    nop

    //
    // Turn off the green LED and turn on the red LED.  If the red LED is left
    // on for too long, the external reset circuit described by application
    // note AN258 will cause the system to reset.
    //
    ldr     r1, =EP9312_LED_DATA
    ldr     r0, [r1]
    bic     r0, r0, #EP9312_LED_GREEN_ON
    orr     r0, r0, #EP9312_LED_RED_ON
    str     r0, [r1]

    //
    // Undo the silly static memory controller programming performed by the
    // boot rom.
    //
    ldr     r0, =0x80080000
    ldr     r1, =0x0000fbe0
    ldr     r2, [r0]
    orr     r2, r2, r1
    str     r2, [r0]
    ldr     r2, [r0, #0x04]
    orr     r2, r2, r1
    str     r2, [r0, #0x04]
    ldr     r2, [r0, #0x08]
    orr     r2, r2, r1
    str     r2, [r0, #0x08]
    ldr     r2, [r0, #0x0c]
    orr     r2, r2, r1
    str     r2, [r0, #0x0c]
    ldr     r2, [r0, #0x18]
    orr     r2, r2, r1
    str     r2, [r0, #0x18]
    ldr     r2, [r0, #0x1c]
    orr     r2, r2, r1
    str     r2, [r0, #0x1c]

    //
    // Set the PLL1 and processor clock.
    //
    ldr     r0, =EP9312_SYSCON
#ifdef HAL_PLATFORM_EP9301
    //
    // 332MHz, giving a 166MHz processor clock.
    //
    // ldr     r1, =0x02b4fa5a  This was the old wrong value for the PLL.  3 check points instead of 1.
    ldr	    r1, = 0x02b49907  // This is the correct value 9907 instead of fa5a
#else
    //
    // 400MHz, giving a 200MHz processor clock.
    //
	//    ldr     r1, =0x02a4e39e
	//	368MHZ , giving a 184MHz processor clock.
	ldr	r1,=0x02a4caf8    //this is for industrial Temp chips

#endif
    str     r1, [r0, #0x0020]
    nop
    nop
    nop
    nop
    nop

    //
    // Set the PLL2 and USB clock.
    //
    //ldr     r1, =0x700cc317   // We are going to set the clock to 384Mhz and USBDIV 8
    ldr	    r1, =0x300dc317     // We are going to set the clock to 192Mhz and USBDIV 4
    str     r1, [r0, #0x0024]

    //
    // Get the base address of SDRAM.  If SDCSn3 is used, then it must be
    // determined if the device was booted synchronous or asynchronously
    // (which determines where SDRAM is located).
    //
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn0
    mov     r11, #0xc0000000
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn1
    mov     r11, #0xd0000000
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn2
    mov     r11, #0xe0000000
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn3
    ldr     r0, =EP9312_SYSCON
    ldr     r0, [r0, #0x009c]
    ands    r0, r0, #0x00000020
    moveq   r11, #0xf0000000
    movne   r11, #0x00000000
#endif

    //
    // Try a 32-bit wide configuration of SDRAM.
    //
    ldr     r0, =(DEVCFG_BANKCOUNT | DEVCFG_SROMLL | \
                  DEVCFG_CASLAT_2 | DEVCFG_RASTOCAS_2)
    ldr     r1, =0x00400000
    orr     r2, r11, #0x00008800
    bl      SDRAMconfig

    //
    // Test the SDRAM.
    //
    mov     r0, r11
    bl      SDRAMTest
    cmp     r0, #0x00000000
    beq     SDRAMok

    //
    // Try a 16-bit wide configuration of SDRAM.
    //
    ldr     r0, =(DEVCFG_BANKCOUNT | DEVCFG_SROMLL | \
                  DEVCFG_CASLAT_2 | DEVCFG_RASTOCAS_2 | \
                  DEVCFG_EXTBUSWIDTH)
    ldr     r1, =0x00200000
    orr     r2, r11, #0x00004600
    bl      SDRAMconfig

    //
    // Test the SDRAM.
    //
    mov     r0, r11
    bl      SDRAMTest
    cmp     r0, #0x00000000
    beq     SDRAMok

    //
    // Turn off the red LED.
    //
    ldr     r0, =EP9312_LED_DATA
    ldr     r1, [r0]
    bic     r1, r1, #EP9312_LED_RED_ON
    str     r1, [r0]

    //
    // There is no SDRAM so flash the green LED.
    //
flash_green:
        orr     r1, r1, #EP9312_LED_GREEN_ON
        str     r1, [r0]
        ldr     r2, =0x00010000
flash_green_delay_1:
            subs    r2, r2, #1
            bne     flash_green_delay_1
        bic     r1, r1, #EP9312_LED_GREEN_ON
        str     r1, [r0]
        ldr     r2, =0x00010000
flash_green_delay_2:
            subs    r2, r2, #1
            bne     flash_green_delay_2
        orr     r1, r1, #EP9312_LED_GREEN_ON
        str     r1, [r0]
        ldr     r2, =0x00010000
flash_green_delay_3:
            subs    r2, r2, #1
            bne     flash_green_delay_3
        bic     r1, r1, #EP9312_LED_GREEN_ON
        str     r1, [r0]
        ldr     r2, =0x00050000
flash_green_delay_4:
            subs    r2, r2, #1
            bne     flash_green_delay_4
        b   flash_green

    //
    // Turn off the red LED.
    //
SDRAMok:
    ldr     r1, =EP9312_LED_DATA
    ldr     r0, [r1]
    bic     r0, r0, #EP9312_LED_RED_ON
    str     r0, [r1]

    //
    // Determine the size of the SDRAM.
    //
    mov     r0, r11
    bl      SDRAMSize

    //
    // Save the SDRAM characteristics.
    //
    mov     r8, r0
    mov     r9, r1
    mov     r10, r2

    //
    // Compute an appropriate refresh rate based on the memory size.  It may
    // be possible to refresh less often for particular SDRAMs, but these
    // values are appropriate for the majority of SDRAMs.
    //
    ldr     r0, =EP9312_SDRAMCTRL
    mul     r1, r8, r10
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn0
    ldr     r2, [r0, #0x0010]
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn1
    ldr     r2, [r0, #0x0014]
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn2
    ldr     r2, [r0, #0x0018]
#endif
#ifdef CYG_HAL_ARM_ARM9_EP93XX_SDRAM_CS_SDCSn3
    ldr     r2, [r0, #0x001c]
#endif
    tst     r2, #DEVCFG_EXTBUSWIDTH
    moveq   r1, r1, lsr #1
    cmp     r1, #0x02000000
#ifdef HAL_PLATFORM_EP9301
    movlt   r1, #0x03f0
    movge   r1, #0x01e0
#else
    movlt   r1, #0x0600
    movge   r1, #0x2f0
#endif
    str     r1, [r0, #0x0008]

    //
    // Find the location of each block of SDRAM.
    //
    bl      SDRAMFind

    //
    // Get the base address of FLASH.
    //
    and     r12, pc, #0xf0000000

    //
    // Save the memory configuration information.
    //
    orr     r0, r11, #0x00002000
    stmia   r0, {r8-r12}

    //
    // Construct a page table based on the memory found.
    //
    bl      BuildPagetable

    //
    // Setup the MMU.
    //
    orr     r0, r11, #0x00004000
    mcr     p15, 0, r0, c2, c0, 0
    mov     r0, #0x00000003
    mcr     p15, 0, r0, c3, c0, 0

    //
    // Enable the MMU and branch to our equivalent virtual address.
    //
    ldr     r0, =0xc0000079
    ldr     r1, =go_virtual
    ldr     r2, =__rom_vectors_vma
    sub     r1, r1, r2
    bic     r1, r1, #0xf0000000
    orr     r1, r1, #0x60000000
    mcr     p15, 0, r0, c1, c0, 0
    mov     pc, r1
go_virtual:
    nop

    //
    // If booting from flash the code needs to be moved to RAM
    //
#ifdef CYG_HAL_STARTUP_ROMRAM

    //
    // Copy our image from FLASH to SDRAM.
    //
    ldr     r0,=__rom_vectors_lma
    ldr     r1,=__rom_vectors_vma
    ldr     r2,=__ram_data_end
copy_loop:
        ldmia   r0!, {r3-r10}
        stmia   r1!, {r3-r10}
        cmp     r1, r2
        blt     copy_loop

    //
    // Zero fill the first portion of SDRAM.
    //
    mov     r0, #0x00000000
    mov     r1, #0x00000000
    mov     r2, #0x00002000
zero:
        str     r0, [r1], #4
        subs    r2, r2, #4
        bne     zero

    //
    // Branch to the copy in RAM.
    //
    ldr     r0, =RamLocation
    mov     pc, r0
RamLocation:
#endif

    //
    // Turn on Green LED.
    //
    ldr     r1, =EP9312_LED_DATA
    ldr     r0, [r1]
    orr     r0, r0, #EP9312_LED_GREEN_ON
    strb    r0,[r1]

    //
    // Return to the caller.
    //
    b       SetupReturn

    //
    // Put the current literal pool here.
    //
    .ltorg

    //
    // Put a "CRUS" at offset 0x1000 so that the boot ROM will boot this image.
    //
    .org    0x1000
    .long   0x53555243  // Put a CRUS in there so this is bootable.
start_here:
    .endm
#endif
#endif

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
#define PLATFORM_SETUP1 _platform_setup1
#define CYGHWR_HAL_ARM_HAS_MMU
#define CYGSEM_HAL_ROM_RESET_USES_JUMP

// This macro represents the initial startup code for the platform
    .macro  _platform_setup1

    b       Setup
SetupReturn:

   .endm


#else // defined(CYG_HAL_STARTUP_ROM) or ROMRAM
#define PLATFORM_SETUP1
#endif

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
