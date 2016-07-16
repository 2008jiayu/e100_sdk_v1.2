/**
  * @file src/app/peripheral_mod/vout/lcd/ssd2828/lcd_ssd2828.c
  *
  * Implementation of LCD Ssd2828 panel interface.
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
#include <common/common.h>
#include <AmbaSPI.h>
#include <AmbaLCD_WDF9648W.h>
#include <wchar.h>

static int LcdSsd2828_Init(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

static int LcdSsd2828_GetDispMode(void)
{
    int ReturnValue = -1;
    int VoutSystem = 0;

    VoutSystem = AppLibSysVout_GetSystemType();

    if (VoutSystem == VOUT_SYS_PAL) {
        ReturnValue = AMBA_LCD_WDF9648W_960_480_50HZ;
    } else {    // VOUT_SYS_NTSC
        ReturnValue = AMBA_LCD_WDF9648W_960_480_60HZ;
    }

    return ReturnValue;
}

static int LcdSsd2828_GetDispAr(void)
{
    return VAR_9x16;
}

static int LcdSsd2828_SetMode(int mode)
{
    int ReturnValue = 0;
    return ReturnValue;
}

static int LcdSsd2828_GetPipRectLineWidth(void)
{
    return 3;
}

int AppLcd_RegisterSsd2828(UINT32 lcdChannelId)
{
    APPLIB_LCD_s Dev = {0};
    WCHAR DevName[] = {'s','s','d','2','8','2','8','\0'};
    AMBA_LCD_COLOR_BALANCE_s ColorBalance = {0, 0, 0, (0.015625 * 64), (0.015625 * 64), (0.015625 * 64)};
    w_strcpy(Dev.Name, DevName);
    Dev.Enable = 1;
    Dev.ThreeDCapacity = 0;
    Dev.FlipCapacity = 1;
    Dev.ColorbalanceCapacity = 1;
    Dev.BacklightCapacity = 1;//Temp
    Dev.Rotate = AMP_ROTATE_90;
    Dev.Width = 480;
    Dev.Height = 854;
    Dev.DefaultBrightness = 0;
    Dev.DefaultContrast = 0.015625 * 64;
    memcpy(&Dev.DefaultColorBalance, &ColorBalance, sizeof (AMBA_LCD_COLOR_BALANCE_s));
    Dev.LcdDelayTime = 150;

    Dev.Init = LcdSsd2828_Init;
    Dev.GetDispMode = LcdSsd2828_GetDispMode;
    Dev.GetDispAR = LcdSsd2828_GetDispAr;
    Dev.SetLcdMode = LcdSsd2828_SetMode;
    Dev.GetPipRectLineWidth = LcdSsd2828_GetPipRectLineWidth;

    AppLibSysLcd_Attach(lcdChannelId, &Dev);

    return 0;
}
