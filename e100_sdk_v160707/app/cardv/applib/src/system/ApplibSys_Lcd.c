/**
 * @file src/app/connected/applib/src/system/ApplibSys_Lcd.c
 *
 * Implementation of LCD panel interface.
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

#include <system/ApplibSys_Lcd.h>
#include <AmbaUtility.h>

//#define DEBUG_APPLIB_SYS_LCD
#if defined(DEBUG_APPLIB_SYS_LCD)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static APPLIB_LCD_s AppLibLcdGlobal[2] = {0};

static APPLIB_LCD_PARAM_s AppLibLcdParam[2] = {
    {0, 0, 65, 65, 125},
    {0, 0, 65, 65, 125}
};

/*************************************************************************
 * LCD Internal APIs
 ************************************************************************/

/**
 *  @brief Remove the LCD output device
 *
 *  Remove the LCD output device
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_Remove(UINT32 lcdChanID)
{
    memset(&AppLibLcdGlobal[lcdChanID], 0x0, sizeof(APPLIB_LCD_s));
    return 0;
}

/**
 *  @brief Attach the LCD output device and enable the device control.
 *
 *  Attach the LCD output device and enable the device control.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_Attach(UINT32 lcdChanID, APPLIB_LCD_s *dev)
{
    char Ft[32] = {0};
    if (dev == NULL)
        return -1;

    AppLibSysLcd_Remove(lcdChanID);
    memcpy(&AppLibLcdGlobal[lcdChanID], dev, sizeof(APPLIB_LCD_s));
    AmbaUtility_Unicode2Ascii(AppLibLcdGlobal[lcdChanID].Name, Ft);
    AmbaPrint("[Applib - LCD] Registered LCD[%d] %s", lcdChanID, Ft);
    AppLibLcdParam[lcdChanID].Brightness = AppLibLcdGlobal[lcdChanID].DefaultBrightness;
    AppLibLcdParam[lcdChanID].Contrast = AppLibLcdGlobal[lcdChanID].DefaultContrast;
    memcpy(&AppLibLcdParam[lcdChanID].ColorBalance, &AppLibLcdGlobal[lcdChanID].DefaultColorBalance, sizeof (AMBA_LCD_COLOR_BALANCE_s));
    return 0;
}

/*************************************************************************
 * LCD Public APIs
 ************************************************************************/

/**
 *  @brief Clean LCD configuration
 *
 *  Clean LCD configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_PreInit(void)
{
    /* Clear AppLibLcdGlobal.*/
    memset(&AppLibLcdGlobal, 0x0, 2*sizeof(APPLIB_LCD_s));

    return 0;
}

/**
 *  @brief LCD initiation
 *
 *  LCD initiation
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_Init(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].Init();
}

/**
 *  @brief To check the LCD enabled.
 *
 *  To check the LCD enabled.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_CheckEnabled(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].Enable;
}

/**
 *  @brief To check the 3D capacity of LCD
 *
 *  To check the 3D capacity of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_Check3DCap(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].ThreeDCapacity;
}

/**
 *  @brief To check the capacity of LCD Flip function capacity.
 *
 *  To check the capacity of LCD Flip function capacity.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_CheckFlipCap(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].FlipCapacity;
}

/**
 *  @brief To check the capacity of LCD color balance function
 *
 *  To check the capacity of LCD color balance function
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_CheckColorBalanceCap(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].ColorbalanceCapacity;
}

/**
 *  @brief To check the capacity of LCD backlight function
 *
 *  To check the capacity of LCD rotate function
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_CheckBacklightCap(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].BacklightCapacity;
}

/**
 *  @brief To check the capacity of LCD rotate function
 *
 *  To check the capacity of LCD rotate function
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_CheckRotate(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].Rotate;
}

/**
 *  @brief To get the width and height of LCD.
 *
 *  To get the width and height of LCD.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_GetDimensions(UINT32 lcdChanID, UINT16 *width, UINT16 *height)
{
    *width = AppLibLcdGlobal[lcdChanID].Width;
    *height = AppLibLcdGlobal[lcdChanID].Height;
    return 0;
}

/**
 *  @brief To get the display mode of LCD
 *
 *  To get the display mode of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The display mode of LCD
 */
int AppLibSysLcd_GetDispMode(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].GetDispMode();
}

/**
 *  @brief To get the display aspect ratio of LCD
 *
 *  To get the display aspect ratio of LCD
 *
 *  @param [in] lcdChanID Display channel ID
 *
 *  @return The display aspect ratio of LCD
 */
int AppLibSysLcd_GetDispAR(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].GetDispAR();
}

/**
 *  @brief To get the pip rectangle line width of LCD
 *
 *  To get the pip rectangle line width of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The pip rectangle line width of LCD
 */
int AppLibSysLcd_GetPipRectLineWidth(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].GetPipRectLineWidth();
}

/**
 *  @brief To get the default value of flip capacity.
 *
 *  To get the default value of flip capacity.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The default value of flip capacity.
 */
UINT8 AppLibSysLcd_GetDefFlip(UINT32 lcdChanID)
{
    return AppLibLcdParam[lcdChanID].Flip;
}

/**
 *  @brief To get the default value of brightness.
 *
 *  To get the default value of brightness.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The default value of brightness
 */
INT32 AppLibSysLcd_GetDefBrightness(UINT32 lcdChanID)
{
    return (int)AppLibLcdGlobal[lcdChanID].DefaultBrightness;
}

/**
 *  @brief To get the default value of contrast.
 *
 *  To get the default value of contrast.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return The default value of contrast.
 */
float AppLibSysLcd_GetDefContrast(UINT32 lcdChanID)
{
    return (int)AppLibLcdGlobal[lcdChanID].DefaultContrast;
}

/**
 *  @brief To get the default value of color balance.
 *
 *  To get the default value of color balance.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return Color balance
 */
AMBA_LCD_COLOR_BALANCE_s AppLibSysLcd_GetDefColorBalance(UINT32 lcdChanID)
{
    return AppLibLcdGlobal[lcdChanID].DefaultColorBalance;
}

/**
 *  @brief To get the LCD delay time.
 *
 *  To get the LCD delay time.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_GetLcdDelayTime(UINT32 lcdChanID)
{
    return (int)AppLibLcdGlobal[lcdChanID].LcdDelayTime;
}

/**
 *  @brief Set LCD mode
 *
 *  Set LCD mode
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] mode Mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_SetMode(UINT32 lcdChanID, int mode)
{
    return AppLibLcdGlobal[lcdChanID].SetLcdMode(mode);
}

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
int AppLibSysLcd_SetFlip(UINT32 lcdChanID, UINT32 flip, UINT32 flag)
{
    //int LcdDispChanId = 0;
    if (AppLibSysLcd_CheckFlipCap(lcdChanID) == 0) {
        return 0;
    }
    if (flag != LCD_PARAM_RECONFIG) {
        AppLibLcdParam[lcdChanID].Flip = flip;
    }
    if (lcdChanID == LCD_CH_FCHAN) {
        if (AppLibDisp_GetDeviceID(DISP_CH_FCHAN) == AMP_DISP_LCD) {
            //LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_FCHAN);
        } else {
            return -1;
        }
    } else {
        if (AppLibDisp_GetDeviceID(DISP_CH_DCHAN) == AMP_DISP_LCD) {
            //LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_DCHAN);
        } else {
            return -1;
        }
    }
    return 0;//lcd_flip(LcdDispChanId, AppLibLcdParam[lcdChanID].flip);
}

#ifdef CONFIG_APP_ARD
extern void app_screen_save_set(int en);
int AppLibSysLcd_SetBacklight_Directly(UINT32 lcdChanID, UINT32 EnableFlag)
{
    int ReturnValue = 0;
    int LcdDispChanId = 0;

    if (lcdChanID == LCD_CH_FCHAN) {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_FCHAN);
    } else {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_DCHAN);
    }

    if (AppLibSysLcd_CheckBacklightCap(lcdChanID) == 0) {
    // ReturnValue = return AppLibSysLcd_SetBrightness(lcdChanID, 0, LCD_PARAM_RECONFIG);
    } else {
        ReturnValue = AmbaLCD_SetBacklight(LcdDispChanId, EnableFlag);
    }

    return ReturnValue;
}
#endif

/**
 *  @brief Set the back light function.
 *
 *  Set the back light function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_SetBacklight(UINT32 lcdChanID, UINT32 param)
{
    int ReturnValue = 0;
    int LcdDispChanId = 0;

    AppLibLcdParam[lcdChanID].Backlight = param;

#ifdef CONFIG_APP_ARD
	if(param ==0){
		app_screen_save_set(0);
	}else{
		app_screen_save_set(1);
	}
#endif

    if (lcdChanID == LCD_CH_FCHAN) {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_FCHAN);
    } else {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_DCHAN);
    }

    if (AppLibSysLcd_CheckBacklightCap(lcdChanID) == 0) {
    // ReturnValue = return AppLibSysLcd_SetBrightness(lcdChanID, 0, LCD_PARAM_RECONFIG);
    } else {
        ReturnValue = AmbaLCD_SetBacklight(LcdDispChanId, AppLibLcdParam[lcdChanID].Brightness);
    }

    return ReturnValue;
}

/**
 *  @brief Set the brightness function.
 *
 *  Set the brightness function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] brightness Brightness
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_SetBrightness(UINT32 lcdChanID, INT32 brightness, UINT32 flag)
{
    int LcdDispChanId = 0;

    if (lcdChanID == LCD_CH_FCHAN) {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_FCHAN);
    } else {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_DCHAN);
    }

    if (flag != LCD_PARAM_RECONFIG) {
        if (brightness < -64) {
            AppLibLcdParam[lcdChanID].Brightness = -64;
        } else {
            AppLibLcdParam[lcdChanID].Brightness = brightness;
        }
    }


    if ((AppLibLcdParam[lcdChanID].Backlight == 0)) {
        return AmbaLCD_SetBrightness(LcdDispChanId, -64);
    } else {
        return AmbaLCD_SetBrightness(LcdDispChanId, AppLibLcdParam[lcdChanID].Brightness);
    }
}

/**
 *  @brief Set the contrast function.
 *
 *  Set the contrast function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] contrast Contrast
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_SetContrast(UINT32 lcdChanID, float contrast, UINT32 flag)
{
    int LcdDispChanId = 0;
    if (flag != LCD_PARAM_RECONFIG) {
        AppLibLcdParam[lcdChanID].Contrast = contrast;
    }
    if (lcdChanID == LCD_CH_FCHAN) {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_FCHAN);
    } else {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_DCHAN);
    }
    return AmbaLCD_SetContrast(LcdDispChanId, AppLibLcdParam[lcdChanID].Contrast);
}

/**
 *  @brief Set the color balance function.
 *
 *  Set the color balance function.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *  @param [in] colorbalance Color balance
 *  @param [in] flag Re-config flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_SetColorBalance(UINT32 lcdChanID, AMBA_LCD_COLOR_BALANCE_s colorbalance, UINT32 flag)
{
    int LcdDispChanId = 0;
    if (AppLibSysLcd_CheckColorBalanceCap(lcdChanID) == 0) {
        return 0;
    }
    if (flag != LCD_PARAM_RECONFIG) {
        memcpy(&AppLibLcdParam[lcdChanID].ColorBalance , &colorbalance, sizeof(AMBA_LCD_COLOR_BALANCE_s));
    }
    if (lcdChanID == LCD_CH_FCHAN) {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_FCHAN);
    } else {
        LcdDispChanId = AppLibDisp_GetChanID(DISP_CH_DCHAN);
    }
    return AmbaLCD_SetColorBalance(LcdDispChanId, &AppLibLcdParam[lcdChanID].ColorBalance);
}

/**
 *  @brief Reset the parameters of LCD
 *
 *  Reset the parameters of LCD
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_ParamReconfig(UINT32 lcdChanID)
{
    int ReturnValue = 0;
    AMBA_LCD_COLOR_BALANCE_s colorbalance = {0};
    DBGMSG("[Applib - LCD] <ParamReconfig>");
    AppLibSysLcd_SetFlip(lcdChanID, 0, LCD_PARAM_RECONFIG);
    AppLibSysLcd_SetBrightness(lcdChanID, 0, LCD_PARAM_RECONFIG);
    AppLibSysLcd_SetContrast(lcdChanID, 0, LCD_PARAM_RECONFIG);
    AppLibSysLcd_SetColorBalance(lcdChanID, colorbalance, LCD_PARAM_RECONFIG);

    return ReturnValue;
}

/**
 *  @brief Set the seamless vout size.
 *
 *  Set the seamless vout size.
 *
 *  @param [in] lcdChanID LCD display channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysLcd_SetSeamless(UINT32 lcdChanID)
{
    int ReturnValue = 0;

    AmpDisplay_SetMaxVout0Size(AppLibLcdGlobal[lcdChanID].Width, AppLibLcdGlobal[lcdChanID].Width);

    return ReturnValue;
}

