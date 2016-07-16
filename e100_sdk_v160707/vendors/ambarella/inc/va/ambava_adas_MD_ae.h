/**
 * \vendors\ambarella\inc\va\ambava_adas_MD_ae.h
 *
 * Header of Implementation of Low Light Warning System(MDAE).
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __AMBAVA_ADAS_MD_AE_H__
#define __AMBAVA_ADAS_MD_AE_H__
#include <math.h>
#include "AmbaDSP_Event.h"
#include "ambava_adas.h"


#ifndef MD_ME_SENSITIVITY_LEVEL
#define MD_ME_SENSITIVITY_LEVEL (100)
#endif
#ifndef MD_ME_THRESHOLD_MAX
#define MD_ME_THRESHOLD_MAX (100)
#endif

#ifndef MOTION_DETECT_AE_ROI_MAX
#define MOTION_DETECT_AE_ROI_MAX   (2)
#endif


#define AMBA_MD_AE_PROC_FRQ (100)
/// ERROR

typedef enum {
    AMBA_MDAE_INI_MEM = -5,
    AMBA_MDAE_FLAG_FAIL,
    AMBA_MDAE_EVENT_HANDLER_FAIL,
    AMBA_MDAE_CGF_FULL,
    AMBA_MDAE_ERROR,
    AMBA_MDAE_OK
} AMBA_ADAS_MDAE_ERR_e;

typedef enum {
    AMBA_MDAE_NO_CHANGE = 0,
    AMBA_MDAE_MOVE_DET,
    AMBA_MDAE_EVENT_NUM
} AMBA_ADAS_MDAE_EVENT_e;

typedef enum {
    MDAE_DEFAULT = 0
} AMBA_MDAE_METHOD_e;

/*************************************************************************
 * ADAS declaration
 ************************************************************************/
/**
 * amp MDAE config structure
 */
typedef struct _AMBA_MDAE_ROI_STATUS_t_ {
    AMBA_MOTION_FLAGS_e Motion_Flags;
    AMBA_MOTION_EVENT_e Motion_Event;
} AMBA_MDAE_ROI_STATUS_t;

typedef struct _AMBA_MDAE_ROI_DATA_t_ {
    UINT8 Valid;
    UINT8 Sensitivity;
    UINT16 Tiles_Num;
    UINT16 Threshold;
    UINT16 Update_Freq;
    UINT16 Update_Cnt;
    AMBA_VA_ROI_s Location;
} AMBA_MDAE_ROI_DATA_t;

typedef struct _AMBA_MDAE_CFG_t_ {
    AMBA_MDAE_ROI_DATA_t RoiData[MOTION_DETECT_AE_ROI_MAX];
    UINT32 MDAEBuf_Size;
    UINT32 Lv_Mm_Avg_Size;
    UINT32 Lv_Snake_Size;
    AMBA_MEM_CTRL_s MDAEBuf;
} AMBA_MDAE_CFG_t;

typedef struct _AMBA_MDAE_PAR_t_ {
    AMBA_MDAE_METHOD_e Method;
    AMBA_MDAE_ROI_STATUS_t Roi_Status[MOTION_DETECT_AE_ROI_MAX];
    int IsUpdate;
} AMBA_MDAE_PAR_t;


/*************************************************************************
 * ADAS APIs
**************************************************************************/

int Amba_AdasMDAE_Init( AMBA_MDAE_PAR_t* pPar);
int Amba_AdasMDAE_Deinit(void);

void Amba_AdasMDAE_GetDefCfg(AMBA_MDAE_CFG_t* pCfg );
int Amba_AdasMDAE_SetCfg( AMBA_MDAE_CFG_t* pCfg );

int Amba_AdasMDAE_GetPar( AMBA_MDAE_PAR_t* pPar );

int Amba_AdasMDAE_Proc( AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A, AMBA_MDAE_PAR_t* pPar ,int* pMDEvent);


#endif /*__AMBAVA_ADAS_MD_AE_H__*/
