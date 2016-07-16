/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFwUpdaterSD.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Firmware Updater through SD
 *
 *  @History        ::
 *      Date        Name        Comments
 *      03/26/2014  Evan Chen   Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaNAND.h"
#include "AmbaNandBadBlockTable.h"
#include "AmbaNAND_PartitionTable.h"
#include "AmbaPrintk.h"
#include "AmbaUtility.h"

#define AMBA_ROM_WARM_BOOT_MAGIC_CODE_ADDR          (0xE00 + 0x10)

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwUpdaterSD_SetMagicCode
 *
 *  @Description::
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaFwUpdaterSD_SetMagicCode(void)
{
	AMBA_NAND_PART_META_s Meta;

	if (AmbaNAND_GetMeta(&Meta) < 0)
		return -1;

	memcpy((void *)Meta.FwProgMagic,
		   (void *)AmbaRTSL_MmuPhysToVirt(AMBA_ROM_WARM_BOOT_MAGIC_CODE_ADDR), FW_PROG_MAGIC_SIZE);

	return AmbaNAND_SetMeta(&Meta);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwUpdaterSD_GetMagicCode
 *
 *  @Description::
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaFwUpdaterSD_GetMagicCode(UINT8 *pBuf)
{
	AMBA_NAND_PART_META_s Meta;

	if (AmbaNAND_GetMeta(&Meta) < 0)
		return -1;

	memcpy((void *)pBuf, (void *)Meta.FwProgMagic, FW_PROG_MAGIC_SIZE);

	return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwUpdaterSD_ClearMagicCode
 *
 *  @Description::
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaFwUpdaterSD_ClearMagicCode(void)
{
	AMBA_NAND_PART_META_s Meta;

	if (AmbaNAND_GetMeta(&Meta) < 0)
		return -1;

	memset((void *)Meta.FwProgMagic, 0x0, FW_PROG_MAGIC_SIZE);

	return AmbaNAND_SetMeta(&Meta);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwUpdaterSD_GetStatus
 *
 *  @Description::
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaFwUpdaterSD_GetStatus(AMBA_FW_PARTITION_ID_e PartID)
{
	AMBA_NAND_PART_META_s Meta;

	if (AmbaNAND_GetMeta(&Meta) < 0)
		return -1;

	return Meta.FwProgStatus[PartID];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwUpdaterSD_SetStatus
 *
 *  @Description::
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaFwUpdaterSD_SetStatus(AMBA_FW_PARTITION_ID_e PartID, UINT32 Status)
{
	AMBA_NAND_PART_META_s Meta;

	if (AmbaNAND_GetMeta(&Meta) < 0)
		return -1;

	Meta.FwProgStatus[PartID] = Status;
	return AmbaNAND_SetMeta(&Meta);
}
