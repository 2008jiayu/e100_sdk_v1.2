/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaARM11NAND_MT29F4G08ABADA.asm
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND Parameters for ARM11
\*-------------------------------------------------------------------------------------------------------------------*/

        MODULE  ?AmbaARM11NAND_Param

        SECTION BstNandParam:CODE:ROOT(2)

        PUBLIC  AmbaWarmBootMagicCode
        PUBLIC  AmbaNandTiming

        DATA

AmbaWarmBootMagicCode:
        DC8     'AmbarellaCamera', 0x00

AmbaNandTiming:
        DC32    0x20202020
        DC32    0x20202020
        DC32    0x20204020
        DC32    0x20202020
        DC32    0x20202020
        DC32    0x20202020

        END
