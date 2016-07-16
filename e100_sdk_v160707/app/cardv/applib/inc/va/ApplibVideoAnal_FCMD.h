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
#ifndef APPLIB_VIDEO_ANAL_FCMD_H_
#define APPLIB_VIDEO_ANAL_FCMD_H_
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
#include <va/ambava_adas_FCMD.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Video definitions
 ************************************************************************/

/*! applib va FCMD outout event
*applib va FCMD config
*/
typedef enum {
    APPLIB_FCMD_CROSS = 0,
    APPLIB_FCMD_MOVE,
    APPLIB_FCMD_STOP,
    APPLIB_FCMD_EVENT_NUM
} APPLIB_ADAS_FCMD_EVENT_e;
/*!
*applib va FCMD config
*/
typedef struct APPLIB_FCMD_CFG_t_ {
    AMBA_ADAS_SENSITIVITY_LEVEL_e FCMDSensitivity;
} APPLIB_FCMD_CFG_t;

typedef struct APPLIB_FCMD_PAR_t_ {
    float HoodLevel; /**< scene_param*/
    float HorizonLevel; /**< scene_param*/
    int IsUpdate;
} APPLIB_FCMD_PAR_t;
/*************************************************************************
 * FCMD Setting APIs
 ************************************************************************/

/**
 *  Initial FCMD
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoAnal_FCMD_Init(UINT8 yuvSrc, APPLIB_FCMD_CFG_t pConfig, APPLIB_FCMD_PAR_t pParams);
extern int AppLibVideoAnal_FCMD_DeInit(void);
extern int AppLibVideoAnal_FCMD_GetDef_Setting( APPLIB_FCMD_CFG_t* pAppLibFCMDConfig, APPLIB_FCMD_PAR_t* pAppLibFCMDParams);
extern int AppLibVideoAnal_FCMD_SetCfg(APPLIB_FCMD_CFG_t* pConfig);
extern int AppLibVideoAnal_FCMD_SetPar( APPLIB_FCMD_PAR_t* pParams);

extern int AppLibVideoAnal_FCMD_Enable(void);
extern int AppLibVideoAnal_FCMD_Disable(void);
__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_VIDEO_ANAL_FCMD_H_ */

