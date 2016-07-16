/*-------------------------------------------------------------------------------------------------------------------*\ 
 *  @FileName       :: AmbaCortexLPDDR3_EDF8132A3MA-JD-F-D_600MHz.asm
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: AmbaCortexLPDDR3_EDF8132A3MA-JD-F-D_600MHz (8 Gbits) Parameters for Cortex
 *
 *  @History        ::
 *      Date        Name        Comments
 *
 *-------------------------------------------------------------------------------------------------------------------*/

        @MODULE  ?AmbaARM11DRAM_Param

        @SECTION BstDramParam:CODE:ROOT(2)

        /* DDR PLL parameters */
        .global  PLL_ProgramDelay
        .global  PLL_DdrCtrlParam
        .global  PLL_DdrCtrl2Param
        .global  PLL_DdrCtrl3Param

        /* DLL parameters */
        .global  DLL_ProgramDelay
        .global  DLL_ResetDelay

        .global  DLL_Setting0Param
        .global  DLL_Setting1Param
        .global  DLL_Setting2Param
        .global  DLL_Setting3Param

        .global  DLL_CtrlSel0Param
        .global  DLL_CtrlSel1Param
        .global  DLL_CtrlSel2Param
        .global  DLL_CtrlSel3Param
        .global  DLL_CtrlSelMiscParam

        /* Cortex PLL parameters */
        .global  PLL_CortexProgramDelay

        /* DDR Controller parameters */
        .global  DRAM_PorDelay
        .global  DRAM_Delay1
        .global  DRAM_Delay2

        .global  DRAM_RsvdSpaceParam
        .global  DRAM_ConfigParam
        .global  DRAM_Timing1Param
        .global  DRAM_Timing2Param
        .global  DRAM_Timing3Param
        .global  DRAM_ByteMapParam

        .global  DRAM_DqsSyncPreRttParam
        .global  DRAM_ZqCalibPreRttParam
        .global  DRAM_DqsSyncParam
        .global  DRAM_ZqCalibParam

        .global  DRAM_PadTermParam

        .global  DRAM_DllCalibParam
        .global  LPDDR3_ModeReg10Param
        .global  LPDDR3_ModeReg11Param
        .global  DRAM_PowerDownParam

        .global  LPDDR3_ModeReg63Param
        .global  LPDDR3_ModeReg1Param
        .global  LPDDR3_ModeReg2Param
        .global  LPDDR3_ModeReg3Param

        /* for another DDR frequency support */
        .global  PLL_DdrCtrlParam_1

        .global  DRAM_Timing1Param_1
        .global  DRAM_Timing2Param_1
        .global  DRAM_Timing3Param_1

        .section BstDramParam

/* DDR PLL parameters */
PLL_ProgramDelay:           .word     0x50000

PLL_DdrCtrlParam:           .word     0x31100000
PLL_DdrCtrl2Param:          .word     0x3f770000
PLL_DdrCtrl3Param:          .word     0x00068304

/* DLL parameters */
DLL_ProgramDelay:           .word     0x30000
DLL_ResetDelay:             .word     0x1000

DLL_Setting0Param:          .word     0x00270221
DLL_Setting1Param:          .word     0x00270221
DLL_Setting2Param:          .word     0x00270221
DLL_Setting3Param:          .word     0x00270221

DLL_CtrlSel0Param:          .word     0x8001
DLL_CtrlSel1Param:          .word     0x8001
DLL_CtrlSel2Param:          .word     0x8001
DLL_CtrlSel3Param:          .word     0x8001
DLL_CtrlSelMiscParam:       .word     0x12e50000

/* DDR Controller parameters */
DRAM_PorDelay:              .word     0x2710
DRAM_Delay1:                .word     0x61a8
DRAM_Delay2:                .word     0xa

DRAM_RsvdSpaceParam:        .word     0xc0000000
DRAM_ConfigParam:           .word     0x20262898
DRAM_Timing1Param:          .word     0x1d892e64
DRAM_Timing2Param:          .word     0x6104f8e3
DRAM_Timing3Param:          .word     0x70447092
DRAM_ByteMapParam:          .word     0xe4

DRAM_DqsSyncPreRttParam:    .word     0xa0
DRAM_ZqCalibPreRttParam:    .word     0x60
DRAM_DqsSyncParam:          .word     0x5f
DRAM_ZqCalibParam:          .word     0x26

/* Cortex PLL parameters */
PLL_CortexProgramDelay:     .word     12000

DRAM_PadTermParam:          .word     0x0002002c

DRAM_DllCalibParam:         .word     0x3204
DRAM_PowerDownParam:        .word     0x321
LPDDR3_ModeReg10Param:      .word     0x010a00ff
LPDDR3_ModeReg11Param:      .word     0x010b0003

LPDDR3_ModeReg63Param:      .word     0x013f0001
LPDDR3_ModeReg1Param:       .word     0x010100e3
LPDDR3_ModeReg2Param:       .word     0x01020007
LPDDR3_ModeReg3Param:       .word     0x01030001

/* for another DDR frequency support */
PLL_DdrCtrlParam_1:         .word     0x31100000

DRAM_Timing1Param_1:        .word     0x1d892e64
DRAM_Timing2Param_1:        .word     0x6104f8e3
DRAM_Timing3Param_1:        .word     0x70447092

