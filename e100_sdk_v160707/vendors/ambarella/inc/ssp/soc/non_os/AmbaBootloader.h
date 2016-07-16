/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaBootloader.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Export Bootloader APIs w/o OS.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_BOOTLOADER_H_
#define _AMBA_BOOTLOADER_H_

int     AmbaNonOS_NandLoadPartition(UINT32 PartID);
int     AmbaNonOS_NandErase(UINT32 Block);
int     AmbaNonOS_NandRead(UINT32 Block, UINT32 StartPage, UINT32 Pages, UINT8 *pMain,
                           UINT8 *pSpare, UINT32 Area);
int     AmbaNonOS_NandProgram(UINT32 Block, UINT32 StartPage, UINT32 Pages, UINT8 *pMain,
                              UINT8 *pSpare, UINT32 Area);

#endif /* _AMBA_NONOS_NAND_H_ */


