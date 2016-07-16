/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaVIN.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Video Input Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_VIN_H_
#define _AMBA_VIN_H_

#include "AmbaVIN_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaVIN.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaVIN_Init(void);
int AmbaVIN_Reset(AMBA_VIN_CHANNEL_e VinChanNo, AMBA_VIN_INTERFACE_e VinInterface, AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);
int AmbaVIN_ResetHvSyncOutput(AMBA_VIN_CHANNEL_e VinChanNo);

int AmbaVIN_SetMPhyConfig(AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);
int AmbaVIN_SetPipMPhyConfig(AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);

int AmbaVIN_GetMPhyConfig(AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);
int AmbaVIN_GetPipMPhyConfig(AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);

int AmbaVIN_CalculateMphyConfig(UINT32 BitRate, AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);

#endif /* _AMBA_VIN_H_ */
