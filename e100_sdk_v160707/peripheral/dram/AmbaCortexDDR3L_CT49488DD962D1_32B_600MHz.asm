/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCortexDDR3L_CT49488DD962D1_32B_600Mhz.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Samsung K4B4G1646B-HCK0 (4Gb x 2) Parameters for ARM11
 *
 *  @History        ::
 *      Date        Name        Comments
 *      11/08/2013  W.Shi       Created
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

        /* DDR3 Initializations */
        .global  DDR3_Delay3

        .global  DDR3_ModeReg0Param
        .global  DDR3_ModeReg1Param
        .global  DDR3_ModeReg2Param
        .global  DDR3_ModeReg3Param

        /* for another DDR frequency support */
        .global  PLL_DdrCtrlParam_1

        .global  DRAM_Timing1Param_1
        .global  DRAM_Timing2Param_1
        .global  DRAM_Timing3Param_1

        @.data

	.section BstDramParam

/* DDR PLL parameters */
PLL_ProgramDelay:           .word     0x50000

PLL_DdrCtrlParam:           .word     0x31100000
PLL_DdrCtrl2Param:          .word     0x3F770000
PLL_DdrCtrl3Param:          .word     0x00068304

/* DLL parameters */
DLL_ProgramDelay:           .word     0x30000
DLL_ResetDelay:             .word     0x1000

DLL_Setting0Param:          .word     0x00212200
DLL_Setting1Param:          .word     0x00212200
DLL_Setting2Param:          .word     0x00212200
DLL_Setting3Param:          .word     0x00212200

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
DRAM_ConfigParam:           .word     0x082700b8
DRAM_Timing1Param:          .word     0x2146d99e
DRAM_Timing2Param:          .word     0x672736d3
DRAM_Timing3Param:          .word     0x70447124
DRAM_ByteMapParam:          .word     0xE4

DRAM_DqsSyncPreRttParam:    .word     0xA0
DRAM_ZqCalibPreRttParam:    .word     0x60
DRAM_DqsSyncParam:          .word     0x40
DRAM_ZqCalibParam:          .word     0x26

/* Cortex PLL parameters */
PLL_CortexProgramDelay:     .word     12000

DRAM_PadTermParam:          .word     0x00020029

/* DDR3 Initializations */
DDR3_Delay3:                .word     0x02

DDR3_ModeReg0Param:         .word     0x01001d51
DDR3_ModeReg1Param:         .word     0x01010000
DDR3_ModeReg2Param:         .word     0x01020010
DDR3_ModeReg3Param:         .word     0x01030000

/* for another DDR frequency support */
PLL_DdrCtrlParam_1:         .word     0x31100000

DRAM_Timing1Param_1:        .word     0x2146d99e
DRAM_Timing2Param_1:        .word     0x672736d3
DRAM_Timing3Param_1:        .word     0x70447124
