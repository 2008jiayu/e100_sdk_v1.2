/**
 * @file src/app/connected/applib/inc/system/ApplibSys_Lcd.h
 *
 * Header of LCD panel interface.
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
#ifndef APPLIB_LCD_H_
#define APPLIB_LCD_H_
/**
* @defgroup ApplibSys_Lcd
* @brief LCD panel interface.
*
*
*/

/**
 * @addtogroup ApplibSys_Lcd
 * @ingroup System
 * @{
 */
#include <mw.h>
#include "AmbaLCD.h"
#include "AmbaDSP_VOUT.h"
#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * LCD definitions
 ************************************************************************/
#define LCD_CH_DCHAN    (0)      /**<LCD_CH_DCHAN    (0)*/
#define LCD_CH_FCHAN    (1)      /**<LCD_CH_FCHAN    (1)*/

#define LCD_PARAM_DEFCONFIG  (0x0FFFFFFF)  /**<LCD_PARAM_DEFCONFIG  (0x0FFFFFFF)*/
#define LCD_PARAM_RECONFIG  (0xFFFFFFFF)   /**<LCD_PARAM_RECONFIG  (0xFFFFFFFF) */

/*************************************************************************
 * LCD structure
 ************************************************************************/
/**
 *  This data structure describes the interface of a LCD panel
 */
typedef struct _APPLIB_LCD_s_ {
    WCHAR Name[32];/**< Module name */
    /* UINT16 start */
    UINT16 Enable:1; /**<Enable*/
    UINT16 ThreeDCapacity:1; /**<3D Capacity*/
#define LCD_3D_CAP_DISABLE  (0)  /**<LCD_3D_CAP_DISABLE*/
#define LCD_3D_CAP_ENABLE   (1)  /**<LCD_3D_CAP_ENABLE */
    UINT16 FlipCapacity:1; /**<Flip Capacity*/
    UINT16 ColorbalanceCapacity:1;/**< The capability to adjust color balance */
    UINT16 BacklightCapacity:1;/**<The capability to control back light */
    UINT16 Reserved:11; /**<Reserved*/
    /* UINT16 end */
    UINT16 Rotate; /**<Rotate*/
    UINT16 Width; /**<Width*/
    UINT16 Height; /**<Height*/
    INT32 DefaultBrightness; /**<Default Brightness*/
    float DefaultContrast; /**<Default Contrast*/
    AMBA_LCD_COLOR_BALANCE_s DefaultColorBalance; /**<Default Color Balance*/
    UINT8 LcdDelayTime;/**< LCD panel needs to delay few ms to avoid white flash. */
    int (*Init)(void);/**< Module init interface */
    /* Module parameter get interface */
    int (*GetDispMode)(void); /**< Get Display Mode*/
    int (*GetDispAR)(void); /**< Get Display Aspect Ratio*/
    int (*SetLcdMode)(int mode); /**< Set Lcd Mode*/
    int (*GetPipRectLineWidth)(void); /**<Get Pipe Rect Line Width*/
} APPLIB_LCD_s;

/**
 *
 * applib lcd parameter
 *
 */
typedef struct _APPLIB_LCD_PARAM_s_ {
    UINT8 Backlight; /**<Backlight*/
    UINT8 Flip; /**<Flip*/
    UINT16 Reserved; /**<Reserved*/
    INT32 Brightness; /**<Brightness*/
    float Contrast;/**<Contrast*/
    AMBA_LCD_COLOR_BALANCE_s ColorBalance;/**<Color Balance*/
} APPLIB_LCD_PARAM_s;


/*************************************************************************
 * LCD Internal APIs
 ************************************************************************/
/**
 *  Remove the LCD output device
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_Remove(UINT32 lcdChanID);

/**
 *  Attach the LCD output device and enable the device control.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_Attach(UINT32 lcdChanID, APPLIB_LCD_s *dev);

/*************************************************************************
 * LCD Public APIs
 ************************************************************************/
/**
 *  Clean LCD configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_PreInit(void);

/**
 *  LCD initiation
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_Init(UINT32 lcdChanID);

/**
 *  To check the LCD enabled.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_CheckEnabled(UINT32 lcdChanID);

/**
 *  To check the 3D capacity of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_Check3DCap(UINT32 lcdChanID);

/**
 *  To check the capacity of LCD Flip function capacity.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_CheckFlipCap(UINT32 lcdChanID);

/**
 *  To check the capacity of LCD color balance function
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_CheckColorBalanceCap(UINT32 lcdChanID);

/**
 *  To check the capacity of LCD rotate function
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_CheckBacklightCap(UINT32 lcdChanID);

/**
 *  To check the capacity of LCD rotate function
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_CheckRotate(UINT32 lcdChanID);

/**
 *  To get the width and height of LCD.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_GetDimensions(UINT32 lcdChanID, UINT16 *width, UINT16 *height);

/**
 *  To get the display mode of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The display mode of LCD
 */
extern int AppLibSysLcd_GetDispMode(UINT32 lcdChanID);

/**
 *  To get the display aspect ratio of LCD
 *
 *  @param [in] lcdChanID Display channel ID
 *
 *  @return The display aspect ratio of LCD
 */
extern int AppLibSysLcd_GetDispAR(UINT32 lcdChanID);

/**
 *  To get the pip rectangle line width of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The pip rectangle line width of LCD
 */
extern int AppLibSysLcd_GetPipRectLineWidth(UINT32 lcdChanID);

/**
 *  To get the default value of flip capacity.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The default value of flip capacity.
 */
extern UINT8 AppLibSysLcd_GetDefFlip(UINT32 lcdChanID);

/**
 *  To get the default value of brightness.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The default value of brightness
 */
extern INT32 AppLibSysLcd_GetDefBrightness(UINT32 lcdChanID);

/**
 *  @brief To get the default value of contrast.
 *
 *  To get the default value of contrast.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The default value of contrast.
 */
extern float AppLibSysLcd_GetDefContrast(UINT32 lcdChanID);

/**
 *  To get the default value of color balance.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return Color balance
 */
extern AMBA_LCD_COLOR_BALANCE_s AppLibSysLcd_GetDefColorBalance(UINT32 lcdChanID);

/**
 *  @brief To get the LCD delay time.
 *
 *  To get the LCD delay time.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The LCD delay time.
 */
extern int AppLibSysLcd_GetLcdDelayTime(UINT32 lcdChanID);

/**
 *  Set LCD mode
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] mode Mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetMode(UINT32 lcdChanID, int mode);

/**
 *  @brief Set the flip function.
 *
 *  Set the flip function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] flip Flip
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetFlip(UINT32 lcdChanID, UINT32 flip, UINT32 flag);

/**
 *  Set the back light function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetBacklight(UINT32 lcdChanID, UINT32 param);

/**
 *  Set the brightness function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] brightness Brightness
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetBrightness(UINT32 lcdChanID, INT32 brightness, UINT32 flag);

/**
 *  Set the contrast function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] contrast Contrast
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetContrast(UINT32 lcdChanID, float contrast, UINT32 flag);

/**
 *  Set the color balance function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] colorbalance Color balance
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetColorBalance(UINT32 lcdChanID, AMBA_LCD_COLOR_BALANCE_s colorbalance, UINT32 flag);

/**
 *  Reset the parameters of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_ParamReconfig(UINT32 lcdChanID);

/**
 *  Set the seamless vout size.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLcd_SetSeamless(UINT32 lcdChanID);

#ifdef CONFIG_APP_ARD
extern int AppLibSysLcd_SetBacklight_Directly(UINT32 lcdChanID, UINT32 EnableFlag);
#endif

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_LCD_H_ */
