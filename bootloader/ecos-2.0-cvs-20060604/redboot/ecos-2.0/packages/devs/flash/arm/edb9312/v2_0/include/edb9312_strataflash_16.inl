#ifndef CYGONCE_DEVS_FLASH_EDB9312_STRATAFLASH_16_INL
#define CYGONCE_DEVS_FLASH_EDB9312_STRATAFLASH_16_INL
//==========================================================================
//
//      edb9312_strataflash_16.inl
//
//      Flash programming - device constants, etc.
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
// Author(s):    gthomas, hmt
// Contributors: gthomas
// Date:         2001-04-23
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


// The edb9312 system has one 16-bit wide device, with a strange connection
// scheme for the address lines due to the ability to jumper it between 16 and
// 32 bit mode.
// StrataFlash 28F1280J3A150. This means 128Mbit or 16Mbyte.
#define CYGNUM_FLASH_DEVICES 	(1)
#define CYGNUM_FLASH_BASE 	    (0x60000000u)
#define CYGNUM_FLASH_BASE_MASK  (0xFF000000u) // 16Mb devices
#define CYGNUM_FLASH_WIDTH 	    (16)
#define CYGNUM_FLASH_BLANK      (1)
#define FLASH_P2V(_a_) ((volatile flash_t *)(((unsigned int)(_a_) & 0xfffe0000) | (((unsigned int)(_a_) & 0x00010000) >> 15) | (((unsigned int)(_a_) & 0x0000ffff) << 1)))
#define CYGOPT_FLASH_IS_NOT_ADVANCED

#endif  // CYGONCE_DEVS_FLASH_EDB9312_STRATAFLASH_16_INL
// ------------------------------------------------------------------------
// EOF edb9312_strataflash_16.inl
