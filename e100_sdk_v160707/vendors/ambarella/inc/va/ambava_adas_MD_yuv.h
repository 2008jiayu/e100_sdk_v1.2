/**
 *\vendors\ambarella\inc\va\ambava_adas_MD_yuv.h
 * Header of Implementation of Low Light Warning System(MDY).
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __AMBAVA_ADAS_MD_YUV_H__
#define __AMBAVA_ADAS_MD_YUV_H__
#include <math.h>
#include "ambava_adas.h"

#ifndef MD_ME_SENSITIVITY_LEVEL
#define MD_ME_SENSITIVITY_LEVEL (100)
#endif
#ifndef MD_ME_THRESHOLD_MAX
#define MD_ME_THRESHOLD_MAX (100)
#endif

#ifndef MOTION_DETECT_ROI_MAX
#define MOTION_DETECT_ROI_MAX   (4)
#endif

#define AMBA_MD_YUV_PROC_FRQ (100)
/**
 * amp FCMDY return event
 */
/// ERROR

typedef enum {
    AMBA_MDY_INI_MEM = -5,
    AMBA_MDY_FLAG_FAIL,
    AMBA_MDY_EVENT_HANDLER_FAIL,
    AMBA_MDY_CGF_FULL,
    AMBA_MDY_ERROR,
    AMBA_MDY_OK
} AMBA_ADAS_MDY_ERR_e;

typedef enum {
    AMBA_MDY_NO_CHANGE = 0,
    AMBA_MDY_MOVE_DET,
    AMBA_MDY_EVENT_NUM
} AMBA_ADAS_MDY_EVENT_e;

typedef enum {
    MDY_DEFAULT = 0,
    MDY_MSE
} AMBA_MDY_METHOD_e;

/*************************************************************************
 * ADAS declaration
 ************************************************************************/
/**
 * amp MDY config structure
 */
typedef struct _AMBA_MD_ROI_STATUS_t_ {
    AMBA_MOTION_FLAGS_e Motion_Flags;
    AMBA_MOTION_EVENT_e Motion_Event;
} AMBA_MD_ROI_STATUS_t;

typedef struct _AMBA_MDY_ROI_DATA_t_ {
    UINT8 Valid;
    UINT8 Sensitivity;
    UINT16 Luma_Diff_Threshold; /// base on Threshold
    AMBA_VA_ROI_s Location;
} AMBA_MDY_ROI_DATA_t;

typedef struct _AMBA_MDY_CFG_t_ {
    AMBA_MDY_ROI_DATA_t RoiData[MOTION_DETECT_ROI_MAX];
    UINT32 MDYBuf_Size;
    AMBA_MEM_CTRL_s MDYBuf;
} AMBA_MDY_CFG_t;

typedef struct _AMBA_MDY_PAR_t_ {
    AMBA_MDY_METHOD_e Method;
    AMBA_MD_ROI_STATUS_t Roi_Status[MOTION_DETECT_ROI_MAX];
    int IsUpdate;
} AMBA_MDY_PAR_t;





/*************************************************************************
 * ADAS APIs
**************************************************************************/

int Amba_AdasMDY_Init( AMBA_MDY_PAR_t* pPar );
int Amba_AdasMDY_Deinit(void);

void Amba_AdasMDY_GetDefCfg(AMP_ENC_YUV_INFO_s* Img, AMBA_MDY_CFG_t* pCfg );
int Amba_AdasMDY_SetCfg( AMBA_MDY_CFG_t* pCfg );

int Amba_AdasMDY_GetPar( AMBA_MDY_PAR_t* pPar );

int Amba_AdasMDY_Proc( AMP_ENC_YUV_INFO_s* Img, AMBA_MDY_PAR_t* pPar ,int* pMDEvent);


#endif /* __AMBAVA_ADAS_MD_YUV_H__ */
