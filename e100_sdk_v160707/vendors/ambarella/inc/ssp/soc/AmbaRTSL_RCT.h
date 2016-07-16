/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_RCT.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for RCT RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_RCT_H_
#define _AMBA_RTSL_RCT_H_

#include "AmbaVIN_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_RCT.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTSL_RctSetMphyConfig(AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);
int AmbaRTSL_RctSetPipMphyConfig(AMBA_VIN_MPHY_CONFIG_s * pMPhyConfig);
int AmbaRTSL_RctGetMphyConfig(AMBA_VIN_MPHY_CONFIG_s * pMPhyConfig);
int AmbaRTSL_RctGetPipMphyConfig(AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);
int AmbaRTSL_RctGetBootMode(void);

void AmbaRTSL_RctChipSoftReset(void);

#endif /* _AMBA_RTSL_RCT_H_ */
