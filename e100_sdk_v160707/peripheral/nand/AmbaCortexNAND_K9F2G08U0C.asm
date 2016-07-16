/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaARM11NAND_K9F2G08U0C.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND Parameters for ARM11
 *
 *  @History        ::
 *      Date        Name        Comments
 *      11/08/2013  W.Shi       Created
 *
 *-------------------------------------------------------------------------------------------------------------------*/

        @MODULE  ?AmbaARM11NAND_Param

        @SECTION BstNandParam:CODE:ROOT(2)

        .global  AmbaWarmBootMagicCode
        .global  AmbaNandTiming

        @.data
	.section BstNandParam

#if !defined(CONFIG_ENABLE_SPINOR_BOOT)
AmbaWarmBootMagicCode:
        @DC8     'AmbarellaCamera', 0x00
        .string  "AmbarellaCamera"
#endif

AmbaNandTiming:
        .word    0x20202020
        .word    0x20202020
        .word    0x20204020
        .word    0x20202020
        .word    0x20202020
        .word    0x20202020
