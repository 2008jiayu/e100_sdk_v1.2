/**
 * @file src/app/connected/applib/inc/va/ApplibVideoAnal_FCMD.h
 *
 * Header of VA Frontal Car Moving Depature(FCMD) APIs
 *
 * History:
 *    2015/01/06 - [Bill Chou] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef APPLIB_VIDEO_ANAL_ADAS_H_
#define APPLIB_VIDEO_ANAL_ADAS_H_
/**
* @defgroup ApplibVideoAnal_FCMD
* @brief FCMD related function
*
*
*/

/**
 * @addtogroup ApplibVideoAnal_FCMD
 * @ingroup FCMD
 * @{
 */
#include <applib.h>
#include <recorder/Encode.h>
#include <va/ambava_adas.h>
__BEGIN_C_PROTO__

/*************************************************************************
 * Video definitions
 ************************************************************************/

/*! applib va ADAS outout event
*applib va ADAS config
*/

/*!
*applib va ADAS config
*/
typedef struct APPLIB_LDWS_CFG_t_ {
    AMBA_ADAS_SENSITIVITY_LEVEL_e LDWSSensitivity;
} APPLIB_LDWS_CFG_t;

typedef struct APPLIB_FCWS_CFG_t_ {
    AMBA_ADAS_SENSITIVITY_LEVEL_e FCWSSensitivity;
} APPLIB_FCWS_CFG_t;

typedef struct APPLIB_ADAS_PAR_t_ {
    float HoodLevel; /**< scene_param*/
    float HorizonLevel; /**< scene_param*/
} APPLIB_ADAS_PAR_t;

typedef struct _APPLIB_ADAS_VIEWANGLE_t_ {
    float HorizAngle;
    float VertAngle;
} APPLIB_ADAS_VIEWANGLE_t;
/*************************************************************************
 * FCMD Setting APIs
 ************************************************************************/

/**
 *  Initial ADAS
 *
 *  @return >=0 success, <0 failure
 */
extern void AppLibVideoAnal_ADAS_GetDef_Setting(APPLIB_LDWS_CFG_t* pLdwsConfig, APPLIB_FCWS_CFG_t* pFcwsConfig, APPLIB_ADAS_PAR_t* pParams);
extern int AppLibVideoAnal_ADAS_Init(UINT8 YuvSrc, APPLIB_LDWS_CFG_t ldwsConfig, APPLIB_FCWS_CFG_t fcwsConfig, APPLIB_ADAS_PAR_t params);
extern int AppLibVideoAnal_ADAS_Process(UINT32 event, AMP_ENC_YUV_INFO_s* img);
extern int AppLibVideoAnal_ADAS_LDWS_SetCfg( APPLIB_LDWS_CFG_t* pParams);
extern int AppLibVideoAnal_ADAS_FCWS_SetCfg( APPLIB_FCWS_CFG_t* pParams);
extern int AppLibVideoAnal_ADAS_SetPar( APPLIB_ADAS_PAR_t* pParams);
extern int AppLibVideoAnal_ADAS_Enable(void);
extern int AppLibVideoAnal_ADAS_Disable(void);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_VIDEO_ANAL_FCMD_H_ */

