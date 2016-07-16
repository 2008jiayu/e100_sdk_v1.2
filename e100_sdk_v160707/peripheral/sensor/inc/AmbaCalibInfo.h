/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions and Constands for Camera Information Management
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CALIB_INFO_H_
#define _AMBA_CALIB_INFO_H_

#include "AmbaDataType.h"
#include "AmbaVIN.h"

typedef struct _AMBA_CALIB_INFO_OBJ_s_ {
    const UINT32 AmbaVignetteMode;              /* Mode-id of the mode for vignette calib */
    const UINT32 AmbaNumBadPixelMode;           /* number of modes for bad pixel calb */
    const UINT32 *AmbaBadPixelModeTable;        /* Mode-id of the modes for bad pixel calib */
    const UINT32 *AmbaBadPixelModeAssignment;   /* bad pixel map for each sensor mode */
    const UINT32 AmbaCalibWidth;
    const UINT32 AmbaCalibHeight;
} AMBA_CALIB_INFO_OBJ_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaCalibInfo.c
\*---------------------------------------------------------------------------*/
extern AMBA_CALIB_INFO_OBJ_s *pAmbaCalibInfoObj[];

#define AmbaCalibInfo_Hook(Chan,pCalibInfoObj)  pAmbaCalibInfoObj[Chan.Bits.VinID] = pCalibInfoObj

#endif /* _AMBA_CALIB_INFO_H_ */
