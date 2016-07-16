/**
* vendors/ambarella/inc/va/ambava_traffic_sign_detector.h
*
* Header of AMBA Advanced Driver Assistance Systems(ADAS)
* Traffic Sign Detection Utility interface.
*
* Copyright (C) 2004-2015, Ambarella, Inc.
*
* All rights reserved. No Part of this file may be reproduced, stored
* in a retrieval system, or transmitted, in any form, or by any means,
* electronic, mechanical, photocopying, recording, or otherwise,
* without the prior consent of Ambarella, Inc.
*/
#ifndef __AMBAVA_TRAFFIC_SIGN_DETECTOR_H__
#define __AMBAVA_TRAFFIC_SIGN_DETECTOR_H__

#include "ambava_adas.h"

#define AMBA_ADAS_TSD_PROC_CYCLE (100)
#define AMBA_ADAS_TSD_OUT_MAXNUM (16)

// TSD = traffic sign detector

/*************************************************************************
* traffic sign detector return event
************************************************************************/
/// traffic sign detector ERROR
typedef enum {
    AMBA_ADAS_TSD_OK = 0,
    AMBA_ADAS_TSD_INI_MEM  = -1,
    AMBA_ADAS_TSD_NOENAB   = -2,
    AMBA_ADAS_TSD_NOCASCAD = -3,
    AMBA_ADAS_TSD_ERROR    = -4,
} AMBA_ADAS_TSD_ERR_e;

/// traffic sign detector object define
typedef enum {
    AMBA_ADAS_TSD_OBJECT_MS10 = 0, /// Max Speed 10 km
    AMBA_ADAS_TSD_OBJECT_MS20,
    AMBA_ADAS_TSD_OBJECT_MS30,
    AMBA_ADAS_TSD_OBJECT_MS40,
    AMBA_ADAS_TSD_OBJECT_MS50,
    AMBA_ADAS_TSD_OBJECT_MS60,
    AMBA_ADAS_TSD_OBJECT_MS70,
    AMBA_ADAS_TSD_OBJECT_MS80,
    AMBA_ADAS_TSD_OBJECT_MS90,
    AMBA_ADAS_TSD_OBJECT_MS100,
    AMBA_ADAS_TSD_OBJECT_MS110,
    AMBA_ADAS_TSD_OBJECT_NUM
} AMBA_ADAS_TSD_OBJECT_e;

/*************************************************************************
* declaration
************************************************************************/
/**
* traffic sign detector config structure
*/
typedef struct _AMBA_ADAS_TSD_CFG_t_ {
    int IsEnabled;
    AMBA_ADAS_SENSITIVITY_LEVEL_e Sensitivity;
    UINT32 BufSize;
    AMBA_MEM_CTRL_s Buf;
    AMBA_ADAS_SCENE_PARAMS_s* ScenePar;
    AMBA_ADAS_VIEWANGLE_s* ViewAngle;
} AMBA_ADAS_TSD_CFG_t;

/**
* traffic sign detector parameter structure
*/
typedef struct _AMBA_ADAS_TSD_PAR_t_ {
    int IsUpdate;
    AMBA_ADAS_AUX_DATA_s* AuxData;
} AMBA_ADAS_TSD_PAR_t;

/**
* traffic sign detector output structure
*/
typedef struct _AMBA_ADAS_TSD_OUT_t_ {
    int ObjNum;
    AMBA_VA_ROI_t Object[AMBA_ADAS_TSD_OUT_MAXNUM];
    AMBA_ADAS_TSD_OBJECT_e TrafficSign[AMBA_ADAS_TSD_OUT_MAXNUM];
} AMBA_ADAS_TSD_OUT_t;

/*************************************************************************
* APIs
************************************************************************/
int Amba_ADAS_TrafficSignDetector_Init(AMBAVA_YUV_INFO_s* pImg, AMBA_ADAS_TSD_CFG_t* pCfg );
int Amba_ADAS_TrafficSignDetector_Deinit(void);

int Amba_ADAS_TrafficSignDetector_SetPar(AMBA_ADAS_TSD_PAR_t* pPar);
int Amba_ADAS_TrafficSignDetector_SetCfg(AMBA_ADAS_TSD_CFG_t* pCfg);

int Amba_ADAS_TrafficSignDetector_GetDefPar(AMBAVA_YUV_INFO_s* pImg, AMBA_ADAS_TSD_PAR_t* pPar);
int Amba_ADAS_TrafficSignDetector_GetDefCfg(AMBAVA_YUV_INFO_s* pImg, AMBA_ADAS_TSD_CFG_t* pCfg);

int Amba_ADAS_TrafficSignDetector_Proc(AMBAVA_YUV_INFO_s* pImg, AMBA_ADAS_TSD_PAR_t* pPar, AMBA_ADAS_TSD_OUT_t* pDetecEvent);

#endif /* __AMBAVA_TRAFFIC_SIGN_DETECTOR_H__ */




