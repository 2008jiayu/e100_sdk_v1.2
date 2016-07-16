/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCorexNOR_W25Q64FV.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND Parameters for ARM11
\*-------------------------------------------------------------------------------------------------------------------*/

        @MODULE  ?AmbaARM11DRAM_Param

        @SECTION BstNandParam:CODE:ROOT(2)

        .global  AmbaWarmBootMagicCode
        .global  BytePerBlock
        .global  NOR_PTB_NUM_BLOCK
        .global  NOR_PTB_START_BLOCK

	.section BstNorParam

AmbaWarmBootMagicCode:
        .string  "AmbarellaCamera"

BytePerBlock:
        .word    0x10000

NOR_PTB_NUM_BLOCK:
        .word   0x4

NOR_PTB_START_BLOCK:
        .word    0x1

        @END
