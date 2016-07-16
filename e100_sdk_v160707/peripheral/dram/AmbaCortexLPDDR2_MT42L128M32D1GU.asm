/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaARM11DDR2_MT42L128M32D1GU.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Micron MT42L128M32D1GU (2Gb x 2 = 512MB) Parameters for ARM11
\*-------------------------------------------------------------------------------------------------------------------*/

        @MODULE  ?AmbaARM11DRAM_Param

        @SECTION BstDramParam:CODE:ROOT(2)

        /* DDR PLL parameters */
        .global  PLL_ProgramDelay
        .global  PLL_DdrCtrlParam
        .global  PLL_DdrCtrl2Param
        .global  PLL_DdrCtrl3Param

        /* DLL parameters */
        .global DLL_ProgramDelay
        .global DLL_ResetDelay

        .global DLL_Setting0Param
        .global DLL_Setting1Param
        .global DLL_Setting2Param
        .global DLL_Setting3Param

        .global DLL_CtrlSel0Param
        .global DLL_CtrlSel1Param
        .global DLL_CtrlSel2Param
        .global DLL_CtrlSel3Param
        .global DLL_CtrlSelMiscParam

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

        .global  CG_DdrInitParam

        /* LPDDR2 Initializations */
        .global  LPDDR2_ResetDelay

        .global  LPDDR2_ModeReg63Param
        .global  LPDDR2_ModeReg1Param
        .global  LPDDR2_ModeReg2Param
        .global  LPDDR2_ModeReg3Param

        /* for another DDR frequency support */
        .global  PLL_DdrCtrlParam_1

        .global  DRAM_Timing1Param_1
        .global  DRAM_Timing2Param_1
        .global  DRAM_Timing3Param_1

        @.data

	.section BstDramParam

/* DDR PLL parameters */
PLL_ProgramDelay:           .word     0x50000

PLL_DdrCtrlParam:           .word     0x2B100000
PLL_DdrCtrl2Param:          .word     0x3F710000
PLL_DdrCtrl3Param:          .word     0x00068300

/* DLL parameters */
DLL_ProgramDelay:           .word     0x30000
DLL_ResetDelay:             .word     0x1000

DLL_Setting0Param:          .word     0x00202022
DLL_Setting1Param:          .word     0x00202022
DLL_Setting2Param:          .word     0x00202022
DLL_Setting3Param:          .word     0x00202022

DLL_CtrlSel0Param:          .word     0x8001
DLL_CtrlSel1Param:          .word     0x8001
DLL_CtrlSel2Param:          .word     0x8001
DLL_CtrlSel3Param:          .word     0x8001
DLL_CtrlSelMiscParam:       .word     0x12E50000

/* DDR Controller parameters */
DRAM_PorDelay:              .word     0x2710
DRAM_Delay1:                .word     0x61A8
DRAM_Delay2:                .word     0x0A

DRAM_RsvdSpaceParam:        .word     0xC0000200
DRAM_ConfigParam:           .word     0x0008009B
DRAM_Timing1Param:          .word     0x28DA6A93
DRAM_Timing2Param:          .word     0x002E2A79
DRAM_Timing3Param:          .word     0x00000051
DRAM_ByteMapParam:          .word     0xE4

DRAM_DqsSyncPreRttParam:    .word     0xA0
DRAM_ZqCalibPreRttParam:    .word     0x60
DRAM_DqsSyncParam:          .word     0x40
DRAM_ZqCalibParam:          .word     0x26

DRAM_PadTermParam:          .word     0x20031

CG_DdrInitParam:            .word     0x0A

/* Cortex PLL parameters */
PLL_CortexProgramDelay:     .word     12000

/* LPDDR2 Initializations */
LPDDR2_ResetDelay:          .word     0x1000

LPDDR2_ModeReg63Param:     .word     0x013F0001
LPDDR2_ModeReg1Param:       .word     0x01010083
LPDDR2_ModeReg2Param:       .word     0x01020004
LPDDR2_ModeReg3Param:       .word     0x01030002

/* for another DDR frequency support */
PLL_DdrCtrlParam_1:         .word     0x2B100000

DRAM_Timing1Param_1:        .word     0x28DA6A93
DRAM_Timing2Param_1:        .word     0x002E2A79
DRAM_Timing3Param_1:        .word     0x00000051

