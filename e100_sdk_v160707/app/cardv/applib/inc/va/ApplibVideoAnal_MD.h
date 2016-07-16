/**
 * @file src/app/connected/applib/inc/va/ApplibVideoAnal_MD.h
 *
 * Header of VA Motion Detection (MD) APIs
 *
 * History:
 *    2015/01/12 - [Bill Chou] created file
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
#ifndef APPLIB_VIDEO_ANAL_MD_H_
#define APPLIB_VIDEO_ANAL_MD_H_
/**
* @defgroup ApplibVideoAnal_MD
* @brief MD related function
*
*
*/

/**
 * @addtogroup ApplibVideoAnal_MD
 * @ingroup MD
 * @{
 */
#include <applib.h>
#include <recorder/Encode.h>
#include <va/ambava_adas_MD_yuv.h>
#include <va/ambava_adas_MD_ae.h>
__BEGIN_C_PROTO__

/*************************************************************************
 * Video definitions
 ************************************************************************/

/*! applib va MD outout event
*applib va MD config
*/
typedef enum {
    APPLIB_MD_MOVE = 0,
    APPLIB_MD_EVENT_NUM
} APPLIB_ADAS_MD_EVENT_e;

typedef enum {
    APPLIB_MD_YUV = 0,
    APPLIB_MD_AE,
    APPLIB_MD_YUV_MSE
} APPLIB_ADAS_MD_METHOD_e;

typedef struct APPLIB_MD_ROI_DATA_t_ {
    AMBA_ADAS_SENSITIVITY_LEVEL_e MDSensitivity;
    AMBA_VA_ROI_s Location;
} APPLIB_MD_ROI_DATA_t;
/*!
*applib va MD config
*/
typedef struct APPLIB_MD_CFG_t_ {
    APPLIB_MD_ROI_DATA_t RoiData[MOTION_DETECT_ROI_MAX];
    APPLIB_ADAS_MD_METHOD_e Method;
} APPLIB_MD_CFG_t;

/*************************************************************************
 * MD Setting APIs
 ************************************************************************/

/**
 *  Initial MD
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoAnal_MD_Init(UINT8 yuvSrc, APPLIB_MD_CFG_t* pConfig);
extern int AppLibVideoAnal_MD_DeInit(void);

extern void AppLibVideoAnal_MD_GetDef_Setting( APPLIB_MD_CFG_t* pConfig);
extern void AppLibVideoAnal_MD_SetROI(APPLIB_MD_CFG_t* pConfig);

extern int AppLibVideoAnal_MD_Enable(void);
extern int AppLibVideoAnal_MD_Disable(void);
__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_VIDEO_ANAL_MD_H_ */

