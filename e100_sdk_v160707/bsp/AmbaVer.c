/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaVer.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: the module of Ambarella Network Support
\*-------------------------------------------------------------------------------------------------------------------*/

#include <AmbaDataType.h>
#include <AmbaVer.h>

void AmbaVer_SetBsp(AMBA_VerInfo_s *pVerInfo)
{
	/* pVerInfo->Key is initialized in other place, pls keep it. */

	pVerInfo->MachStr	= AMBA_BUILD_MACHINE;
	pVerInfo->DateStr	= AMBA_BUILD_DATE;
	pVerInfo->DateNum	= AMBA_BUILD_DATE_NUM;
	pVerInfo->SecNum	= AMBA_BUILD_SEC_NUM;
	pVerInfo->CiCount	= AMBA_CI_COUNT;
	pVerInfo->CiIdStr	= AMBA_CI_ID;
}
