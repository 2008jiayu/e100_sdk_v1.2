/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFwUpdaterSD.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Firmware Updater through SD
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FW_UPDATER_SD_H_
#define _AMBA_FW_UPDATER_SD_H_

#include "AmbaNAND_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFwUpdateSD.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaFwUpdaterSD_SetMagicCode(void);
int AmbaFwUpdaterSD_GetMagicCode(UINT8 *pBuf);
int AmbaFwUpdaterSD_ClearMagicCode(void);

int AmbaFwUpdaterSD_GetStatus(AMBA_FW_PARTITION_ID_e PartID);
int AmbaFwUpdaterSD_SetStatus(AMBA_FW_PARTITION_ID_e PartID, UINT32 Status);

int AmbaEmmcFwUpdaterSD_ClearMagicCode(void);
int AmbaEmmcFwUpdaterSD_SetMagicCode(void);

#endif /* _AMBA_FW_UPDATER_SD_H_ */
