/**
 * system/va/adas/amba_adas_FCMD.h
 *
 * Header of AMBA Advanced Driver Assistance Systems(ADAS) Utility interface.
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __AMBAVA_ADAS_FCMD_H__
#define __AMBAVA_ADAS_FCMD_H__

#include "ambava_adas.h"

#define AMBA_FCMD_PROC_CYCLE (100)
/**
 * amp FCMD return event
 */
/// ERROR

typedef enum {
    AMBA_FCMD_INI_MEM = -4,
    AMBA_FCMD_NOENAB,
    AMBA_FCMD_NOCALB,
    AMBA_FCMD_ERROR,
    AMBA_FCMD_OK
} AMBA_ADAS_FCMD_ERR_e;

typedef enum {
    AMBA_FCMD_CROSS = 0,
    AMBA_FCMD_MOVE,
    AMBA_FCMD_STOP,
    AMBA_FCMD_EVENT_NUM
} AMBA_ADAS_FCMD_EVENT_e;


/*************************************************************************
 * ADAS declaration
 ************************************************************************/
/**
 * amp FCMD config structure
 */
typedef struct _AMP_FCMD_CFG_t_ {
    int IsEnabled;
    AMBA_ADAS_SENSITIVITY_LEVEL_e FCMDSensitivity;
    UINT32 Buf_Size;
    AMBA_MEM_CTRL_s Buf;
} AMP_FCMD_CFG_t;

typedef struct _AMP_FCMD_PAR_t_ {
    float HoodLevel; /**< scene_param*/
    float HorizonLevel; /**< scene_param*/
    int IsCalibrationDetected; /**< scene_param*/
    int BoundingSky; /**< the ROI TOP bounding*/
    int BoundingHood; /**< the ROI BOT bounding */
    int BoundingLeft; /**< the ROI left bounding */
    int BoundingRight; /**< the ROI right bounding */
    int BoundingH; /**< the ROI height*/
    int BoundingW; /**< the ROI width*/
    int IsUpdate;
    AMBA_ADAS_AUX_DATA_s Aux_data;
    int IsGPSandDashboard;
} AMP_FCMD_PAR_t;


/*************************************************************************
 * ADAS APIs
 ************************************************************************/
int Amba_AdasFCMD_Init(AMP_ENC_YUV_INFO_s* img, AMP_FCMD_PAR_t* pPar );
int Amba_AdasFCMD_Deinit(void);

int Amba_AdasFCMD_GetDef_Cfg_Par( AMP_ENC_YUV_INFO_s* img, AMP_FCMD_CFG_t* pCfg, AMP_FCMD_PAR_t* pPar);
int Amba_AdasFCMD_SetCfg(AMP_FCMD_CFG_t* pCfg);

int Amba_AdasFCMD_GetPar( AMP_FCMD_PAR_t* pParams);

int Amba_AdasFCMD_Proc(AMP_ENC_YUV_INFO_s* img, AMP_FCMD_PAR_t* pPar, AMBA_ADAS_FCMD_EVENT_e* pFCMDEvent);

#endif /* __AMBA_ADAS_FCMD_H__ */




