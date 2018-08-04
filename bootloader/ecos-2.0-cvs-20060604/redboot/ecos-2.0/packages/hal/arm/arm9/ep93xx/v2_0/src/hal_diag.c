/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
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
// Author(s):   nickg, gthomas
// Contributors:nickg, gthomas
// Date:        2001-04-23
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>
#include CYGBLD_HAL_VARIANT_H           // Variant specific configuration
#include CYGBLD_HAL_PLATFORM_H          // Platform specific configuration

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // basic machine info
#include <cyg/hal/hal_intr.h>           // interrupt macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/ep93xx.h>               // platform definitions

/*---------------------------------------------------------------------------*/

//
// Default the source clock to the UARTs to be 7Mhz
//
#define BAUD_RATE(n) (((14745600)/(16*n))-1)
#define UARTBASE_TO_PORT(n) (((((cyg_uint32)n)>>16)&0x3)+1)
//-----------------------------------------------------------------------------
typedef struct {
    cyg_uint8 *base;
    cyg_int32  msec_timeout;
    int        isr_vector;
} channel_data_t;

static channel_data_t ep9312_ser_channels[] = {
    { (cyg_uint8 *)EP9312_UART1, 1000, CYGNUM_HAL_INTERRUPT_UART1 },
    { (cyg_uint8 *)EP9312_UART2, 1000, CYGNUM_HAL_INTERRUPT_UART2 },
    { (cyg_uint8 *)EP9312_UART3, 1000, CYGNUM_HAL_INTERRUPT_UART3 }
};

static void
DisableUART(cyg_uint32 Channel)
{
    cyg_uint32 DevCfg;

    //
    // Enable the clock to the UART
    //
    HAL_READ_UINT32(EP9312_DEVCFG, DevCfg);

    switch (Channel)
    {
        case 1:
        {
            DevCfg &= ~EP9312_DEVCFG_U1EN;
            break;
        }
        case 2:
        {
            DevCfg &= ~EP9312_DEVCFG_U2EN;
            break;
        }
        case 3:
        {
            DevCfg &= ~EP9312_DEVCFG_U3EN;
            break;
        }
        default:
        {
        }
    }
    HAL_WRITE_UINT32(EP9312_SYSCON + EP9312_SYSCON_LOCK, 0xaa);
    HAL_WRITE_UINT32(EP9312_DEVCFG, DevCfg);
}

static void
EnableUART(cyg_uint32 Channel)
{
    cyg_uint32 DevCfg;

    //
    // Enable the clock to the UART
    //
    HAL_READ_UINT32(EP9312_DEVCFG, DevCfg);

    switch (Channel)
    {
        case 1:
        {
            DevCfg |= EP9312_DEVCFG_U1EN;
            break;
        }
        case 2:
        {
            DevCfg |= EP9312_DEVCFG_U2EN;
            break;
        }
        case 3:
        {
            DevCfg |= EP9312_DEVCFG_U3EN;
            break;
        }
        default:
        {
        }
    }
    HAL_WRITE_UINT32(EP9312_SYSCON + EP9312_SYSCON_LOCK, 0xaa);
    HAL_WRITE_UINT32(EP9312_DEVCFG, DevCfg);
}


//-----------------------------------------------------------------------------

static void
cyg_hal_plf_serial_init_channel(void* __ch_data)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint32 baud;
    
    DisableUART(UARTBASE_TO_PORT(base));
    
    // 
    // Disable while configuring
    //
    HAL_WRITE_UINT32(base+EP9312_UART_CR, 0);
    HAL_WRITE_UINT32(base+EP9312_UART_MCR, 0);
    HAL_WRITE_UINT32(base+EP9312_UART_SR, 0);

    //
    // Set the baud rate.
    //
    baud = BAUD_RATE(CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD);
    HAL_WRITE_UINT32(base+EP9312_UART_LCR_L, baud & 0xFF);
    HAL_WRITE_UINT32(base+EP9312_UART_LCR_M, baud >> 8);

    // 
    // 8bits - 1 stop bit - no parity.
    //
    HAL_WRITE_UINT32(base+EP9312_UART_LCR_H, EP9312_UART_LCR_H_FE | 
        EP9312_UART_LCR_H_WLEN8);

    // 
    // Enable
    //
    HAL_WRITE_UINT32(base+EP9312_UART_CR, EP9312_UART_CR_UARTE);

    EnableUART(UARTBASE_TO_PORT(base));
}

void
cyg_hal_plf_serial_putc(void *__ch_data, char c)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint32 status;

    do {
        HAL_READ_UINT32(base+EP9312_UART_FR, status);
    } while ((status & EP9312_UART_FR_TXFF) != 0);

    HAL_WRITE_UINT32(base+EP9312_UART_DATA, c);
}

static cyg_bool
cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint32 status;
    cyg_uint32 c;

    HAL_READ_UINT32(base+EP9312_UART_FR, status);
    if ((status & EP9312_UART_FR_RXFE) != 0)
        return false;

    HAL_READ_UINT32(base+EP9312_UART_DATA, c);
    *ch = c;

    return true;
}

cyg_uint8
cyg_hal_plf_serial_getc(void* __ch_data)
{
    cyg_uint8 ch;

    while(!cyg_hal_plf_serial_getc_nonblock(__ch_data, &ch));
    return ch;
}

static void
cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    while(__len-- > 0)
        cyg_hal_plf_serial_putc(__ch_data, *__buf++);
}

static void
cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_serial_getc(__ch_data);
}

cyg_bool
cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    int delay_count;
    channel_data_t* chan = (channel_data_t*)__ch_data;
    cyg_bool res;

    delay_count = chan->msec_timeout * 10; // delay in .1 ms steps

    for(;;) {
        res = cyg_hal_plf_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        
        CYGACC_CALL_IF_DELAY_US(100);
    }
    return res;
}

static int
cyg_hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
#if 0
    static int irq_state = 0;
#endif
    channel_data_t* chan = (channel_data_t*)__ch_data;
    int ret = 0;
    CYGARC_HAL_SAVE_GP();

    switch (__func) {
#if 0
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;

        HAL_WRITE_UINT8(chan->base+CYG_DEV_IER, SIO_IER_RCV);
        HAL_WRITE_UINT8(chan->base+CYG_DEV_MCR, SIO_MCR_INT|SIO_MCR_DTR|SIO_MCR_RTS);

        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;

        HAL_WRITE_UINT8(chan->base+CYG_DEV_IER, 0);

        HAL_INTERRUPT_MASK(chan->isr_vector);
        break;
#endif
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = chan->isr_vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
    {
        va_list ap;

        va_start(ap, __func);

        ret = chan->msec_timeout;
        chan->msec_timeout = va_arg(ap, cyg_uint32);

        va_end(ap);
    }        
    default:
        break;
    }
    CYGARC_HAL_RESTORE_GP();
    return ret;
}

// TODO this needs to be implemented.
static int
cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc, 
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    return(0);
#if 0
    int res = 0;
    channel_data_t* chan = (channel_data_t*)__ch_data;
    char c;
    cyg_uint8 lsr;
    CYGARC_HAL_SAVE_GP();

    cyg_drv_interrupt_acknowledge(chan->isr_vector);

    *__ctrlc = 0;
    HAL_READ_UINT8(chan->base+CYG_DEV_LSR, lsr);
    if ( (lsr & SIO_LSR_DR) != 0 ) {

        HAL_READ_UINT8(chan->base+CYG_DEV_RBR, c);
        if( cyg_hal_is_break( &c , 1 ) )
            *__ctrlc = 1;

        res = CYG_ISR_HANDLED;
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
#endif
 
}

static void
cyg_hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Init channels
    // TODO shouldn't this be default value, not 0?
    cyg_hal_plf_serial_init_channel(&ep9312_ser_channels[0]);

    // Setup procs in the vector table

    // Set channel 0
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &ep9312_ser_channels[0]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}

void
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;

    if (initialized)
        return;

    initialized = 1;

    cyg_hal_plf_serial_init();
}

//=============================================================================
// Compatibility with older stubs
//=============================================================================

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#include <cyg/hal/hal_stub.h>           // cyg_hal_gdb_interrupt
#endif

#if CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL==0
// This is the base address of the A-channel
#define CYG_DEV_SERIAL_BASE      0x0D800000
#define CYG_DEVICE_SERIAL_INT    CYGNUM_HAL_INTERRUPT_SERIALA
#else
// This is the base address of the B-channel
#define CYG_DEV_SERIAL_BASE      0x0D800020
#define CYG_DEVICE_SERIAL_INT    CYGNUM_HAL_INTERRUPT_SERIALB
#endif

static channel_data_t pid_ser_channel = {
    (cyg_uint8*)CYG_DEV_SERIAL_BASE, 0, 0 
};

// Assumption: all diagnostic output must be GDB packetized unless this is a ROM (i.e.
// totally stand-alone) system.

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM) || !defined(CYGDBG_HAL_DIAG_TO_DEBUG_CHAN)
#define HAL_DIAG_USES_HARDWARE
#endif

#ifndef HAL_DIAG_USES_HARDWARE
#if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL != CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL)
#define HAL_DIAG_USES_HARDWARE
#endif
#endif

#ifdef HAL_DIAG_USES_HARDWARE

void hal_diag_init(void)
{
    static int init = 0;
    char *msg = "\n\rARM eCos\n\r";

    if (init++) return;

    cyg_hal_plf_serial_init_channel(&pid_ser_channel);

    while (*msg) cyg_hal_plf_serial_putc(&pid_ser_channel, *msg++);
}

#ifdef DEBUG_DIAG
#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#define DIAG_BUFSIZE 32
#else
#define DIAG_BUFSIZE 2048
#endif
static char diag_buffer[DIAG_BUFSIZE];
static int diag_bp = 0;
#endif

void hal_diag_write_char(char c)
{
    hal_diag_init();

    cyg_hal_plf_serial_putc(&pid_ser_channel, c);

#ifdef DEBUG_DIAG
    diag_buffer[diag_bp++] = c;
    if (diag_bp == DIAG_BUFSIZE) diag_bp = 0;
#endif
}

void hal_diag_read_char(char *c)
{
    *c = cyg_hal_plf_serial_getc(&pid_ser_channel);
}

#else // HAL_DIAG relies on GDB

// Initialize diag port - assume GDB channel is already set up
void hal_diag_init(void)
{
    if (0) cyg_hal_plf_serial_init_channel(&pid_ser_channel); // avoid warning
}

// Actually send character down the wire
static void
hal_diag_write_char_serial(char c)
{
    hal_diag_init();

    cyg_hal_plf_serial_putc(&pid_ser_channel, c);
}

static bool
hal_diag_read_serial(char *c)
{
    long timeout = 1000000000;  // A long time...
    while (!cyg_hal_plf_serial_getc_nonblock(&pid_ser_channel, c))
        if (0 == --timeout) return false;

    return true;
}

void 
hal_diag_read_char(char *c)
{
    while (!hal_diag_read_serial(c)) ;
}

void 
hal_diag_write_char(char c)
{
    static char line[100];
    static int pos = 0;

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {
        CYG_INTERRUPT_STATE old;

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
        
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION(old);
#else
        HAL_DISABLE_INTERRUPTS(old);
#endif

        while(1)
        {
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
            char c1;
        
            hal_diag_write_char_serial('$');
            hal_diag_write_char_serial('O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                hal_diag_write_char_serial(h);
                hal_diag_write_char_serial(l);
                csum += h;
                csum += l;
            }
            hal_diag_write_char_serial('#');
            hal_diag_write_char_serial(hex[(csum>>4)&0xF]);
            hal_diag_write_char_serial(hex[csum&0xF]);

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.  This is the reason for this clause
            // being a loop.
            if (!hal_diag_read_serial(&c1))
                continue;   // No response - try sending packet again

            if( c1 == '+' )
                break;              // a good acknowledge

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
            cyg_drv_interrupt_acknowledge(CYG_DEVICE_SERIAL_INT);
            if( c1 == 3 ) {
                // Ctrl-C: breakpoint.
                cyg_hal_gdb_interrupt ((target_register_t)__builtin_return_address(0));
                break;
            }
#endif
            // otherwise, loop round again
        }
        
        pos = 0;

        // And re-enable interrupts
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION(old);
#else
        HAL_RESTORE_INTERRUPTS(old);
#endif
        
    }
}
#endif
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
