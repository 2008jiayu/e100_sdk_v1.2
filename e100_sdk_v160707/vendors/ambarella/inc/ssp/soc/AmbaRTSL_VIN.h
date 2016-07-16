/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_VIN.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Video Input Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_VIN_H_
#define _AMBA_RTSL_VIN_H_

#include "AmbaVIN_Def.h"

typedef enum _VIN_RESET_CONTROL_e_ {
    VIN_RESET_RELEASE = 0,                 /* Release RESET */
    VIN_RESET_LOGIC,                       /* Reset hardware logic */
    VIN_RESET_LOGIC_PHY                    /* Reset hardware logic and PHY */
} VIN_RESET_CONTROL_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_VIN.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_VinReset(AMBA_VIN_CHANNEL_e VinChanNo, VIN_RESET_CONTROL_e VinResetCtrl);
void AmbaRTSL_VinResetHvSyncOutput(AMBA_VIN_CHANNEL_e VinChanNo, UINT8 ResetFlag);
void AmbaRTSL_VinConfig(AMBA_VIN_CHANNEL_e VinChanNo, AMBA_VIN_INTERFACE_e VinInterface, AMBA_VIN_MPHY_CONFIG_s *pMPhyConfg);

#endif /* _AMBA_RTSL_VIN_H_ */
