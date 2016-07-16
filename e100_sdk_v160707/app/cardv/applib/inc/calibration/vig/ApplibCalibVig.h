/**
 * @file src/app/connected/applib/inc/calibration/vig/ApplibCalibVig.h
 *
 * header file for vignette calibration
 *
 * History:
 *    07/10/2013  Allen Chiu Created
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
#include <calibration/ApplibCalibMgr.h>

#define ANTI_FLICKER_60HZ 1
#define ANTI_FLICKER_50HZ 2


#define VIG_DEBUG_TABLE 0xF1

#define VIG_UPDATE_NONE         0x00
#define VIG_UPDATE_VIN_ENABLE   0x01
#define VIG_UPDATE_ALL          0x03
#define VIG_UPDATE_VIN_INFO     0x08



// Turn on the flag to replace Gr and Gb with (Gr+Gb)/2 before calibration */
// #define GR_GB_MISMATCH

/* Vignette compensation */
#define CAL_VIGNETTE_BASE        (0x00000000)
#define CAL_VIGNETTE_DATA(x)        ((x) + CAL_VIGNETTE_BASE)
#define CAL_VIGNETTE_ENABLE        CAL_VIGNETTE_DATA(0x00)
#define CAL_VIGNETTE_TABLE_COUNT    CAL_VIGNETTE_DATA(0x01)
#define CAL_VIGNETTE_CHECKSUM_BASE  CAL_VIGNETTE_DATA(0x04)
//prevent exceed 512byte Maximum:127 vig table
#define CAL_VIGNETTE_CHECKSUM(x)    (CAL_VIGNETTE_CHECKSUM_BASE+(x)*sizeof(UINT32))
#define CAL_VIGNETTE_TABLE_BASE        CAL_VIGNETTE_DATA(0x200)
#define CAL_VIGNETTE_TABLE(x)        (CAL_VIGNETTE_TABLE_BASE + sizeof(Vignette_Pack_Storage_s)*(x))


typedef struct Vignette_Select_Param_s_ {
    UINT8  Enable;
    UINT8  Channel;              // Channel no.
    UINT8  Id1;
    UINT8  Id2;
    INT32  Blend;
    UINT32 VignetteLumaStrength;
    UINT32 VignetteChromaStrength;
} Vignette_Select_Param_s;
/**
 * @defgroup ApplibCalibVig
 * @brief header file for vignette calibration
 *
 */

/**
 * @addtogroup ApplibCalibVig
 * @{
 */

//#define MAX_GAIN_TABLE_COUNT //define in <mw/image/image_algo_vignette_a5.h>
/**
 * Vignette table information
 */
typedef struct Vignette_Control_s_ {
    UINT8  Enable;			/**< Function Enable control */
    UINT8  GainTableCount;	/**< Number of gain tables */
    UINT8  Debug;			/**< Debug message enable flag */
    UINT8  BufferLocked;	/**< Buffer Locked, 1 Locked, 0 unlock */
    Vignette_Pack_Storage_s *GainTable[MAX_VIGNETTE_GAIN_TABLE_COUNT]; /**< Vignette table information */
} Vignette_Control_s;

extern Vignette_Control_s AppVignetteControl;
extern double pow(double x, double y);  /* For calculating gain_magnitude */

/**
*  Vignette calibration config
*
*  @param [in] Channel 			Channel id
*  @param [in] VignetteConfig 	Vignette setting
*
*  @return 0 - OK
*/
extern int AmpCalib_ConfigVignette(UINT8 Channel, Vignette_Global_Map_Config_Info_s *VignetteConfig);

/**
*  Vignette enable control
*
*  @param [in] Enable 	Disable:0, Enable:1
*
*  @return 0 - OK
*/
extern int AmpCalib_VigSetEnable(UINT8 Enable);
extern int AppLib_CalibSiteInit(void);

/**
*  Control vignette table with blend and decay ratio for multi-channel
*
*  @param [in] Enable 	Disable:0, Enable:1
*  @param [in] Channel 	Channel id, for different sensor input
*  @param [in] Id1     	The ID number for blending
*  @param [in] Id2     	The ID number for blending
*  @param [in] Blend   	Blend ratio for the two tables
*  @param [in] VigLumaStrength Strength for vignette calibration data (Luma)
*  @param [in] VigChromaStrength Strength for vignette calibration data (Chroma)
*
*  @return 0 - OK
*/
extern int AppLibCalibVig_SelectVigTable(UINT8 Enable, UINT8 Channel, UINT8 Id1, UINT8 Id2, int Blend, UINT32 VigLumaStr,UINT32 VigChromaStr);
extern int AppLib_CalibGetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);

/**
 *  @brief update vignette map
 *
 *  update vignette map
 *
 */
extern void AppLibCalibVignette_MapUpdate(void);

/**
 *  @brief get vignette parameter for calibration
 *
 *  get vignette parameter for calibration
 *
 *  @param [in]Channel Channel ID 
 *  @param [in]Mode video or still
 *  @param [in]Param vignette setting parameter
 *
 *  @return 0 success, -1 failure
 */
extern int AppLibCalibVignette_SetParameter(Vignette_Select_Param_s *Param,UINT8 Channel, UINT8 Mode);

/**
 *  @brief set vignette parameter for calibration
 *
 *  set vignette parameter for calibration
 *
 *  @param [in]Channel Channel ID 
 *  @param [in]Mode video or still
 *  @param [out]Param vignette setting parameter
 *
 *  @return 0 success, -1 failure
 */
extern int AppLibCalibVignette_GetParameter(Vignette_Select_Param_s *Param,UINT8 Channel, UINT8 Mode);

/**
 *  @brief control vignette table with blend and decay ratio for multiple channel
 *
 *  control vignette table with blend and decay ratio for multiple channel
 *
 *  @param [in]Enable  Disable:0, Enable:1
 *  @param [in]Channel channel id, for different sensor input
 *  @param [in]Id1 the ID number for blending
 *  @param [in]Id2 the ID number for blending
 *  @param [in]Blend blend ratio for the two tables
 *  @param [in]VignetteLumaStrength Strength for vignette calibration data (Luma)
 *  @param [in]VignetteChromaStrength Strength for vignette calibration data (Chroma)
 *
 *  @return 0 success, -1 failure
 */
extern int AppLibCalibVignette_SelectVignetteTable(UINT8 Enable, UINT8 Channel, UINT8 Id1, UINT8 Id2, int Blend, UINT32 VignetteLumaStrength,UINT32 VignetteChromaStrength);


/**
 *  @brief get vignette calibration enable Flag
 *
 *   get vignette calibration enable Flag
 *
 *  @return enable Flag
 */
extern UINT8  AppLibCalibVignette_GetVignetteCalDataEnable(void);


extern void AppLibCalibAdjust_Debug(UINT8 En);
extern void AppLibCalibAdjust_SetControlEnable(UINT8 Enable);
extern int AppLibCalibAdjust_Init(void);
/**
 * @}
 */