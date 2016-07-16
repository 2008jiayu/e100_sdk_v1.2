/**
 * @file src/app/connected/applib/inc/display/ApplibDisplay.h
 *
 * Header of display Utilities
 *
 * History:
 *    2013/07/17 - [Martin Lai] created file
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

#ifndef APPLIB_DISPLAY_H_
#define APPLIB_DISPLAY_H_

/**
* @defgroup Display
* @brief display Utilities
*
*/

/**
 * @addtogroup Display
 * @{
 */

#include <display/Display.h>
#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Display declaration
 ************************************************************************/
/** Display channel id */
#define DISP_CH_FCHAN   (0x01) /**<DISP_CH_FCHAN (0x01)*/
#define DISP_CH_DCHAN   (0x02) /**<DISP_CH_DCHAN (0x02)*/
#define DISP_CH_DUAL    (0x03) /**<DISP_CH_DUAL  (0x03)*/
#define DISP_CH_NUM     (0x02) /**<DISP_CH_NUM   (0x02)*/

/** Number of display channel */

#define DISP_CMD_LCD_BOOT_REPROGRAM     HMSG_MODULE_DISPLAY(0x0001)  /**<DISP_CMD_LCD_BOOT_REPROGRAM */
#define DISP_CMD_TV_BOOT                HMSG_MODULE_DISPLAY(0x0002)  /**<DISP_CMD_TV_BOOT            */

#define DISP_COLOR_AUTO (0xFFFFFFFF) /**<DISP_COLOR_AUTO (0xFFFFFFFF)*/

#define DISP_FCHAN_NO_DEVICE    (0x01)     /**<DISP_FCHAN_NO_DEVICE    (0x01)  */
#define DISP_FCHAN_NO_CHANGE    (0x02)     /**<DISP_FCHAN_NO_CHANGE    (0x02)  */
#define DISP_DCHAN_NO_DEVICE    (0x04)     /**<DISP_DCHAN_NO_DEVICE    (0x04)  */
#define DISP_DCHAN_NO_CHANGE    (0x08)     /**<DISP_DCHAN_NO_CHANGE    (0x08)  */
#define DISP_ANY_DEV            (0xFFFF)   /**<DISP_ANY_DEV            (0xFFFF)*/

/**
 *
 * applib vout preview param
 *
 */
typedef struct _APPLIB_VOUT_PREVIEW_PARAM_s_ {
    AMP_AREA_s Preview;/**< Preview size. */
    UINT32 ChanID;     /**< Channel ID. */
    int AspectRatio;   /**< Aspect Ratio. */
} APPLIB_VOUT_PREVIEW_PARAM_s;

/*************************************************************************
 * Display APIs
 ************************************************************************/

/**
 *  Initialize the display.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_Init(void);

/**
 *  Enable the flag of DChan or FChan.
 *
 *  @param [in] dispChanID Channel ID.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_EnableChan(UINT32 dispChanID);

/**
 *  Disable the flag of DChan or FChan.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_DisableChan(UINT32 dispChanID);

/**
 *  Check the flag of DChan or FChan.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return 1 enable, 0 disable
 */
extern int AppLibDisp_CheckChanEnabled(UINT32 dispChanID);

/**
 *  Get the MW channel id.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 MW channel id, <0 failure
 */
extern int AppLibDisp_GetChanID(UINT32 dispChanID);

/**
 *  Get the information of display configuration
 *
 *  @param [in] dispChanID Channel ID
 *  @param [out] config The configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_GetChanConfig(UINT32 dispChanID, AMP_DISP_DEV_CFG_s *config);

/**
 *  Get device ID
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return The device ID
 */
extern int AppLibDisp_GetDeviceID(UINT32 dispChanID);

/**
 *  Get display mode
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return The display mode, 0xFFFF is error.
 */
extern int AppLibDisp_GetDispMode(UINT32 dispChanID);

/**
 *  Set color mapping.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] srcColor Source color type
 *  @param [in] dispColor Display color type
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetColorMapping(UINT32 dispChanID, UINT32 srcColor, UINT32 dispColor);

/**
 *  Get color mapping.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [out] srcColor Source color type
 *  @param [out] dispColor Display color type
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_GetColorMapping(UINT32 dispChanID, UINT32 *srcColor, UINT32 *dispColor);

/**
 *  Set 3D output mode.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] mode 3D output mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_Set3DMode(UINT32 dispChanID, UINT32 mode);

/**
 *  Get 3D output mode.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return 3D output mode.
 */
extern int AppLibDisp_Get3DMode(UINT32 dispChanID);

/**
 *  Set the display dimension
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] outputDimension Output dimension.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetDispDimension(UINT32 dispChanID, UINT8 outputDimension);

/**
 *  Check the device type
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] dispDevId Display device id.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SelectDevice(UINT32 dispChanID, UINT32 dispDevId);

/**
 *  Configure the display mode.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] voutDispMode Display mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_ConfigMode(UINT32 dispChanID, int voutDispMode);

/**
 *  Setup the config
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetupChan(UINT32 dispChanID);

/**
 *  Setup the pixel format
 *
 *  @param [in] HdmiOutputFormat
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetupFchanPxlFmt(UINT32 HdmiOutputFormat);

/**
 *  Setup the color mapping
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetupColorMapping(UINT32 dispChanID);

/**
 *  Start the display channel.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_ChanStart(UINT32 dispChanID);

/**
 *  Stop the display channel.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_ChanStop(UINT32 dispChanID);

/**
 *  Trigger the flow that reprogram the Lcd after booting system.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_TriggerLcdBooReprogram(void);

/**
 *  The flag that reprogram the Lcd after booting system.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetLcdBootReprogram(UINT32 dispChanID);

/**
 *  Rotate the display
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] mode Mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_RotateVideo(UINT32 dispChanID, int mode);

/**
 *  Setup the color mapping
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] srcColor Source color type
 *  @param [in] dispColor Display color type
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_ColorMapping(UINT32 dispChanID, UINT32 srcColor, UINT32 dispColor);

/**
 *  Switch the system type NTSC<->PAL
 *
 *  @param [in] voutDispMode Display mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SwitchSystemType(int voutDispMode);

/**
 *  Switch the dual-vout type NTSC<->PAL
 *
 *  @param [out] fchanReturnValue The return value of FChan
 *  @param [out] dchanReturnValue The return value of DChan
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SwitchDualVoutType(int *fchanReturnValue, int *dchanReturnValue);

/**
 *  Get the information of display device
 *
 *  @param [in] dispChanID Channel ID
 *  @param [out] dispDev The device's information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_GetDeviceInfo(UINT32 dispChanID, AMP_DISP_INFO_s *dispDev);

/**
 *  Initialize the window module
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_InitWindow(void);

/**
 *  To get the window id
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot      slot ID
 *
 *  @return >=0 The window id, <0 failure
 */
extern int AppLibDisp_GetWindowId(UINT32 dispChanID, UINT32 slot);

/**
 *  Add a window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] config Configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_AddWindow(UINT32 dispChanID, AMP_DISP_WINDOW_CFG_s *config);

/**
 *  Delete the window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_DeleteWindow(UINT32 dispChanID, int slot);

/**
 *  Set the winsow configuration
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *  @param [in] config Configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_SetWindowConfig(UINT32 dispChanID, int slot, AMP_DISP_WINDOW_CFG_s *config);

/**
 *  Get the window configuration
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *  @param [out] config Configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_GetWindowConfig(UINT32 dispChanID, int slot, AMP_DISP_WINDOW_CFG_s *config);

/**
 *  Get the window handler
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return The window handler
 */
extern AMP_DISP_WINDOW_HDLR_s *AppLibDisp_GetWindowHandler(UINT32 dispChanID, int slot);

/**
 *  Activate the window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_ActivateWindow(UINT32 dispChanID, int slot);

/**
 *  Deactivate the window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_DeactivateWindow(UINT32 dispChanID, int slot);

/**
 *  Update window configuration
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibDisp_FlushWindow(UINT32 dispChanID);

/**
 *  Calculate the preview window size.
 *
 *  @param [in,out] prevParam The parameter of video preview.
 *
 */
extern void AppLibDisp_CalcPreviewWindowSize(APPLIB_VOUT_PREVIEW_PARAM_s *prevParam);
#ifdef CONFIG_APP_ARD
extern int AppLibDisp_Rotate(UINT32 dispChanID,AMP_ROTATION_e rotate);
extern AMP_ROTATION_e AppLibDisp_GetRotate(UINT32 dispChanID);
#endif
__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_DISPLAY_H_ */

