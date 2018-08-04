//=============================================================================
//
//  FILE: regs_VIC.h
//
//  DESCRIPTION: EP???? register definition
//
//  Copyright Cirrus Logic Corporation, 2002.  All rights reserved
//
//=============================================================================
#ifndef _REGS_VIC_H_
#define _REGS_VIC_H_

//=============================================================================
// Interrupt Controller Defines
//=============================================================================
typedef struct _INTMAP_0
{
    unsigned int Int0:1;
    unsigned int Int1:1;
    unsigned int Int2:1;
    unsigned int Int3:1;
    unsigned int Int4:1;
    unsigned int Int5:1;
    unsigned int Int6:1;
    unsigned int Int7:1;
    unsigned int Int8:1;
    unsigned int Int9:1;
    unsigned int Int10:1;
    unsigned int Int11:1;
    unsigned int Int12:1;
    unsigned int Int13:1;
    unsigned int Int14:1;
    unsigned int Int15:1;
    unsigned int Int16:1;
    unsigned int Int17:1;
    unsigned int Int18:1;
    unsigned int Int19:1;
    unsigned int Int20:1;
    unsigned int Int21:1;
    unsigned int Int22:1;
    unsigned int Int23:1;
    unsigned int Int24:1;
    unsigned int Int25:1;
    unsigned int Int26:1;
    unsigned int Int27:1;
    unsigned int Int28:1;
    unsigned int Int29:1;
    unsigned int Int30:1;
    unsigned int Int31:1;
} INTMAP_0;

typedef struct _INTMAP_1
{
    unsigned int Int0:1;
    unsigned int Int1:1;
    unsigned int Int2:1;
    unsigned int Int3:1;
    unsigned int Int4:1;
    unsigned int Int5:1;
    unsigned int Int6:1;
    unsigned int Int7:1;
    unsigned int Int8:1;
    unsigned int Int9:1;
    unsigned int Int10:1;
    unsigned int Int11:1;
    unsigned int Int12:1;
    unsigned int Int13:1;
    unsigned int Int14:1;
    unsigned int Int15:1;
    unsigned int Int16:1;
    unsigned int Int17:1;
    unsigned int Int18:1;
    unsigned int Int19:1;
    unsigned int Int20:1;
    unsigned int Int21:1;
    unsigned int Int22:1;
    unsigned int Int23:1;
    unsigned int Int24:1;
    unsigned int Int25:1;
    unsigned int Int26:1;
    unsigned int Int27:1;
    unsigned int Int28:1;
    unsigned int Int29:1;
    unsigned int Int30:1;
    unsigned int Int31:1;
} INTMAP_1;

typedef struct _INTERRUPT
{
    union 
    {
        INTMAP_0 const Field;
        unsigned int const Value;
    } VICIRQStatus;                                 // 0x0000
    
    union 
    {
        INTMAP_0 const Field;
        unsigned int const Value;
    } VICFIQStatus;                                 // 0x0004
    
    union 
    {
        INTMAP_0 const Field;
        unsigned int const Value;
    } VICRawIntr;                                   // 0x0008

    union 
    {
        INTMAP_0 const Field;
        unsigned int Value;
    } VICIntSelect;                                 // 0x000c

    union 
    {
        INTMAP_0 const Field;
        unsigned int Value;
    } VICIntEnable;                                 // 0x0010

    union 
    {
        INTMAP_0 const Field;
        unsigned int Value;
    } VICIntEnClear;                                // 0x0014

    union 
    {
        INTMAP_0 const Field;
        unsigned int Value;
    } VICSoftInt;                                   // 0x0018

    union 
    {
        INTMAP_0 const Field;
        unsigned int Value;
    } VICSoftIntClear;                              // 0x001c

    union 
    {
        struct 
        {
            unsigned int Protection:1;
            unsigned int RSVD:31;
        } Field;
        unsigned int Value;
    } VICProtection;                                // 0x0020

    unsigned int const RSVD0[3];

    union 
    {
        unsigned int Value;
    } VICCurVectAddr;                               // 0x0030

    union 
    {
        unsigned int Value;
    } VICDefVectAddr;                               // 0x0034

    unsigned int const RSVD1[0x32];

    union 
    {
        unsigned int Value;
    } VICVectAddr[16];                              // 0x0100
                                
    unsigned int const RSVD2[0x30];

    union 
    {
        struct 
        {
            unsigned int IntSource:5;
            unsigned int Enable:1;
            unsigned int RSVD:26;
        } Field;
        unsigned int Value;
    } VICVectCntl[16];                              // 0x0200

    union 
    {
        struct 
        {
            unsigned int ITEN:1;
        } Field;
        unsigned int Value;
    } VICITCR;                                      // 0x0300

    union 
    {
        struct 
        {
            unsigned int RSVD:6;
            unsigned int F:1;
            unsigned int I:1;
            unsigned int RSVD1:24;
        } Field;
        unsigned int Value;
    } VICITIP1;                                     // 0x0304

    union 
    {
        struct 
        {
            unsigned int VectorAddrIn:32;
        } Field;
        unsigned int Value;
    } VICITIP2;                                     // 0x0308

    union 
    {
        struct 
        {
            unsigned int RSVD:6;
            unsigned int F:1;
            unsigned int I:1;
            unsigned int RSVD1:24;
        } Field;
        unsigned int Value;
    } VICITOP1;                                     // 0x030c

    union 
    {
        struct 
        {
            unsigned int VectorAddrOut:32;
        } Field;
        unsigned int Value;
    } VICITOP2;                                     // 0x0310

    unsigned int const RSVD3[0x3ebb];

    union 
    {
        struct 
        {
            unsigned int Partnumber0:8;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICPeriphID0;                                 // 0xfe00

    union 
    {
        struct 
        {
            unsigned int Partnumber1:4;
            unsigned int Designer0:4;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICPeriphID1;                                 // 0xfe4

    union 
    {
        struct 
        {
            unsigned int Designer1:4;
            unsigned int Revision:4;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICPeriphID2;                                 // 0xfe8

    union 
    {
        struct 
        {
            unsigned int Configuration:8;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICPeriphID3;                                 // 0xfec

    union 
    {
        struct 
        {
            unsigned int VICPCellID0:8;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICCellID0;                                   // 0xff0

    union 
    {
        struct 
        {
            unsigned int VICPCellID1:8;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICCellID1;                                   // 0xff4

    union 
    {
        struct 
        {
            unsigned int VICPCellID2:8;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICCellID2;                                   // 0xff8

    union 
    {
        struct 
        {
            unsigned int VICPCellID3:8;
            unsigned int RSVD:24;
        } Field;
        unsigned int Value;
    } VICCellID3;                                   // 0xffc
} INTERRUPT;

#define VIC_INT_0                           0x00000001
#define VIC_INT_1                           0x00000002
#define VIC_INT_2                           0x00000004
#define VIC_INT_3                           0x00000008
#define VIC_INT_4                           0x00000010
#define VIC_INT_5                           0x00000020
#define VIC_INT_6                           0x00000040
#define VIC_INT_7                           0x00000080
#define VIC_INT_8                           0x00000100
#define VIC_INT_9                           0x00000200
#define VIC_INT_10                          0x00000400
#define VIC_INT_11                          0x00000800
#define VIC_INT_12                          0x00001000
#define VIC_INT_13                          0x00002000
#define VIC_INT_14                          0x00004000
#define VIC_INT_15                          0x00008000
#define VIC_INT_16                          0x00010000
#define VIC_INT_17                          0x00020000
#define VIC_INT_18                          0x00040000
#define VIC_INT_19                          0x00080000
#define VIC_INT_20                          0x00100000
#define VIC_INT_21                          0x00200000
#define VIC_INT_22                          0x00400000
#define VIC_INT_23                          0x00800000
#define VIC_INT_24                          0x01000000
#define VIC_INT_25                          0x02000000
#define VIC_INT_26                          0x04000000
#define VIC_INT_27                          0x08000000
#define VIC_INT_28                          0x10000000
#define VIC_INT_29                          0x20000000
#define VIC_INT_30                          0x40000000
#define VIC_INT_31                          0x80000000

//
// The VICProtection register defines.
//
#define VICProtection_Protection            0x00000001

//
// The VICVectCntl register defines.
//
#define VICVectCntl_IntSource_MASK          0x0000001f
#define VICVectCntl_IntSource_SHIFT         0x00000000
#define VICVectCntl_Enable                  0x00000020

//
// The VICITCR register defines.
//
#define VICITCR_ITEN                        0x00000001

//
// The VICITIP1 register defines.
//
#define VICITIP1_F                          0x00000040
#define VICITIP1_I                          0x00000080

//
// The VICITOP1 register defines.
//
#define VICITOP1_F                          0x00000040
#define VICITOP1_I                          0x00000080

//
// The ID register defines.
//
#define VICPeriphID0_Partnumber0_MASK       0x000000ff
#define VICPeriphID0_Partnumber0_SHIFT      0
#define VICPeriphID1_Partnumber1_MASK       0x0000000f
#define VICPeriphID1_Partnumber1_SHIFT      0
#define VICPeriphID1_Designer0_MASK         0x000000f0
#define VICPeriphID1_Designer0_SHIFT        4
#define VICPeriphID2_Designer1_MASK         0x0000000f
#define VICPeriphID2_Designer1_SHIFT        0
#define VICPeriphID2_Revision_MASK          0x000000f0
#define VICPeriphID2_Revision_SHIFT         4
#define VICPeriphID3_Configuration_MASK     0x000000ff
#define VICPeriphID3_Configuration_SHIFT    0
#define VICCellID0_VICPCellID0_MASK         0x000000ff
#define VICCellID0_VICPCellID0_SHIFT        0
#define VICCellID0_VICPCellID1_MASK         0x000000ff
#define VICCellID0_VICPCellID1_SHIFT        0
#define VICCellID0_VICPCellID2_MASK         0x000000ff
#define VICCellID0_VICPCellID2_SHIFT        0
#define VICCellID0_VICPCellID3_MASK         0x000000ff
#define VICCellID0_VICPCellID3_SHIFT        0

//-----------------------------------------------------------------------------
// Register Definitions
//-----------------------------------------------------------------------------
static INTERRUPT volatile * const IntControl[2]  = 
{
    (INTERRUPT *) 0x800b0000,
    (INTERRUPT *) 0x800c0000
};

#endif /* _REGS_VIC_H_ */
