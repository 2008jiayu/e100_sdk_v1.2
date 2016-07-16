/**
 * @file src/app/connected/applib/src/system/ApplibSys_Vout.c
 *
 *  Implementation of vout Utility interface.
 *
 * History:
 *    2013/07/10 - [Martin Lai] created file
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
#include <applib.h>

/*************************************************************************
 * Vout system declaration
 ************************************************************************/
static APPLIB_VOUT_SETTING_s ApplibVoutSetting = {0};

static int VoutModeTableNtsc[VOUT_DISP_MODE_NUM] = {
    AMP_DISP_ID_2160P30,        /** VOUT_DISP_MODE_2160P_HALF */
    AMP_DISP_ID_2160P24,      /** VOUT_DISP_MODE_2160P24 */
    AMP_DISP_ID_1080P,         /** VOUT_DISP_MODE_1080P */
    AMP_DISP_ID_1080P30,       /** VOUT_DISP_MODE_1080P_HALF */
    AMP_DISP_ID_1080I,         /** VOUT_DISP_MODE_1080I */
    AMP_DISP_ID_1080P24,       /** VOUT_DISP_MODE_1080P24 */
    AMP_DISP_ID_720P,          /** VOUT_DISP_MODE_720P */
    AMP_DISP_ID_720P30,        /** VOUT_DISP_MODE_720P_HALF */
    AMP_DISP_ID_720P24,        /** VOUT_DISP_MODE_720P24 */
    AMP_DISP_ID_480P,          /** VOUT_DISP_MODE_SDP */
    AMP_DISP_ID_480I,          /** VOUT_DISP_MODE_SDI */
	AMP_DISP_ID_DMT0659        /** VOUT_DISP_ID_DMT0659 */
};

static int VoutModeTablePal[VOUT_DISP_MODE_NUM] = {
    AMP_DISP_ID_2160P25,        /** VOUT_DISP_MODE_2160P_HALF */
    AMP_DISP_ID_2160P24,        /** VOUT_DISP_MODE_2160P24 */
    AMP_DISP_ID_1080P50,       /** VOUT_DISP_MODE_1080P */
    AMP_DISP_ID_1080P25,       /** VOUT_DISP_MODE_1080P_HALF */
    AMP_DISP_ID_1080I25,       /** VOUT_DISP_MODE_1080I */
    AMP_DISP_ID_1080P24,       /** VOUT_DISP_MODE_1080P24 */
    AMP_DISP_ID_720P50,        /** VOUT_DISP_MODE_720P */
    AMP_DISP_ID_720P25,        /** VOUT_DISP_MODE_720P_HALF */
    AMP_DISP_ID_720P24,        /** VOUT_DISP_MODE_720P24 */
    AMP_DISP_ID_576P,          /** VOUT_DISP_MODE_SDP */
    AMP_DISP_ID_576I,          /** VOUT_DISP_MODE_SDI */
	AMP_DISP_ID_DMT0659        /** VOUT_DISP_ID_DMT0659 */
};

/*************************************************************************
 * Vout system APIs
 ************************************************************************/

/**
 *  @brief Set the vout system type NTSC/PAL.
 *
 *  Set the vout system type.
 *
 *  @param [in] voutSys vout system type.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVout_SetSystemType(int voutSys)
{
    ApplibVoutSetting.SystemType = voutSys;
    return 0;
}

/**
 *  @brief Get the system type of vout.
 *
 *  Get the system type of vout.
 *
 *  @return The system type
 */
int AppLibSysVout_GetSystemType(void)
{
    return ApplibVoutSetting.SystemType;
}

/**
 *  @brief Set the status of HDMI jack
 *
 *  Set the flag of HDMI jack
 *
 *  @param [in] jackState HDMI jack status
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVout_SetJackHDMI(int jackState)
{
    ApplibVoutSetting.VoutJackHDMI = jackState;
    return 0;
}

/**
 *  @brief Get the status of HDMI jack
 *
 *  Get the status of HDMI jack
 *
 *  @return The status of HDMI jack
 */
int AppLibSysVout_CheckJackHDMI(void)
{
    return ApplibVoutSetting.VoutJackHDMI;
}

/**
 *  @brief Set the status of composite jack
 *
 *  Set the status of composite jack
 *
 *  @param [in] jackState composite jack status
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVout_SetJackCs(int jackState)
{
    ApplibVoutSetting.VoutJackCS = jackState;
    return 0;
}

/**
 *  @brief Get the status of composite jack
 *
 *  Get the status of composite jack
 *
 *  @return the status of composite jack
 */
int AppLibSysVout_CheckJackCs(void)
{
    return ApplibVoutSetting.VoutJackCS;
}

/**
 *  @brief Get the vout mode.
 *
 *  Get the vout mode.
 *
 *  @param [in] voutDispModeID Application vout mode ID
 *
 *  @return vout mode.
 */
int AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_ID_e voutDispModeID)
{
    if (ApplibVoutSetting.SystemType == VOUT_SYS_PAL) {
        return VoutModeTablePal[voutDispModeID];
    } else {    // VOUT_SYS_NTSC
        return VoutModeTableNtsc[voutDispModeID];
    }
}

/**
 *  @brief Get the HDMI frame rate
 *
 *  Get the HDMI frame rate
 *
 *  @param [in] voutDispMode Vout mode
 *
 *  @return frame rate
 */
int AppLibSysVout_GetHDMIFrameRate(AMP_DISP_HDMI_MODE_e voutDispMode)
{
    int ReturnValue = -1;

    if ((voutDispMode == AMP_DISP_ID_1080P) ||       /** NTSC - VOUT_DISP_MODE_1080P */
        (voutDispMode == AMP_DISP_ID_1080I) ||       /** NTSC - VOUT_DISP_MODE_1080I */
        (voutDispMode == AMP_DISP_ID_720P) ||        /** NTSC - VOUT_DISP_MODE_720P */
        (voutDispMode == AMP_DISP_ID_480I) ||        /** NTSC - VOUT_DISP_MODE_SDI */
        (voutDispMode == AMP_DISP_ID_480I)) {        /** NTSC - VOUT_DISP_DMT0659 */
        ReturnValue = 60;
    } else if ((voutDispMode == AMP_DISP_ID_1080P50) || /** PAL - VOUT_DISP_MODE_1080P */
        (voutDispMode == AMP_DISP_ID_1080I25) ||        /** PAL - VOUT_DISP_MODE_1080I */
        (voutDispMode == AMP_DISP_ID_720P50) ||         /** PAL - VOUT_DISP_MODE_720P */
        (voutDispMode == AMP_DISP_ID_576I)) {            /** PAL - VOUT_DISP_MODE_SDI */
        ReturnValue = 50;
    } else if ((voutDispMode == AMP_DISP_ID_2160P30) || /** NTSC - VOUT_DISP_MODE_2160P_HALF */
        (voutDispMode == AMP_DISP_ID_1080P30) ||       /** NTSC - VOUT_DISP_MODE_1080P_HALF */
        (voutDispMode == AMP_DISP_ID_720P30) ||        /** NTSC - VOUT_DISP_MODE_720P_HALF */
        (voutDispMode == AMP_DISP_ID_480P)) {           /** NTSC - VOUT_DISP_MODE_SDP */
        ReturnValue = 30;
    } else if ((voutDispMode == AMP_DISP_ID_2160P25) || /** NTSC - VOUT_DISP_MODE_2160P_HALF */
        (voutDispMode == AMP_DISP_ID_1080P25) ||       /** PAL - VOUT_DISP_MODE_1080P_HALF */
        (voutDispMode == AMP_DISP_ID_720P25) ||        /** PAL - VOUT_DISP_MODE_720P_HALF */
        (voutDispMode == AMP_DISP_ID_576P)) {           /** PAL - VOUT_DISP_MODE_SDP */
        ReturnValue = 25;
    } else if ((voutDispMode == AMP_DISP_ID_2160P24) || /** VOUT_DISP_MODE_2160P24 */
        (voutDispMode == AMP_DISP_ID_1080P24) ||       /** VOUT_DISP_MODE_1080P24 */
        (voutDispMode == AMP_DISP_ID_720P24)) {         /** VOUT_DISP_MODE_720P24 */
        ReturnValue = 24;
    }

    return ReturnValue;
}

