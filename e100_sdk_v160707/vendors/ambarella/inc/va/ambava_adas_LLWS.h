/**
 * \va\inc\ambava_adas_LLWS.h
 *
 * Header of Implementation of Low Light Warning System(LLWS).
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __AMBAVA_ADAS_LLWS_H__
#define __AMBAVA_ADAS_LLWS_H__

#include "ambava_adas.h"
#include "AmbaDSP_Event.h"

#define AMBA_LLWS_PROC_CYCLE (100)
/**
 * amp FCMD return event
 */
/// ERROR

typedef enum {
    AMBA_LLWS_INI_MEM = -4,
    AMBA_LLWS_FLAG_FAIL,
    AMBA_LLWS_EVENT_HANDLER_FAIL,
    AMBA_LLWS_ERROR,
    AMBA_LLWS_OK
} AMBA_ADAS_LLWS_ERR_e;

typedef enum {
    AMBA_LLWS_NO_CHANGE = 0,
    AMBA_LLWS_LOW_LIGHT,
    AMBA_LLWS_EVENT_NUM
} AMBA_ADAS_LLWS_EVENT_e;


/*************************************************************************
 * ADAS declaration
 ************************************************************************/
/**
 * amp LLWS config structure
 */
typedef struct _AMP_LLWS_CFG_t_ {
    int IsEnabled;
    AMBA_ADAS_SENSITIVITY_LEVEL_e LLWSSensitivity;
} AMP_LLWS_CFG_t;

typedef struct _AMP_LLWS_PAR_t_ {
    float HoodLevel; /**< scene_param*/
    float HorizonLevel; /**< scene_param*/
    int IsCalibrationDetected; /**< scene_param*/
    int IsUpdate;
} AMP_LLWS_PAR_t;

/*************************************************************************
 * ADAS APIs
 ************************************************************************/
int Amba_AdasLLWS_Init(/*in*/AMP_LLWS_PAR_t* pPar );
int Amba_AdasLLWS_Deinit(void);
/*
*/
int Amba_AdasLLWS_GetDefCfg(AMP_LLWS_CFG_t* pCfg );
int Amba_AdasLLWS_SetCfg( AMP_LLWS_CFG_t* pCfg );

int Amba_AdasLLWS_GetPar( AMP_LLWS_PAR_t* pPar );

int Amba_AdasLLWS_Proc( AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A, AMP_LLWS_PAR_t* Ppar , int* pLLWSEvent);


#endif /* __AMBAVA_ADAS_LLWS_H__ */
