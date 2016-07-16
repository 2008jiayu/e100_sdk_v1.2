/**
  * @file app/connected/app/system/app_pref.c
  *
  * Implementation of Application Preference
  *
  * History:
  *    2013/08/16 - [Martin Lai] created file
  *    2013/12/27 - [Hsunying Huang] modified
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
#include <string.h>
#include <stdio.h>
#include <AmbaDataType.h>
#include <AmbaNAND_Def.h>
#include <AmbaNFTL.h>
#include "app_pref.h"
#include "status.h"
#ifdef CONFIG_ENABLE_EMMC_BOOT
#include <AmbaPartition_Def.h>
#endif
#ifdef CONFIG_APP_ARD
#include "AmbaAudio.h"
#endif

//#define APP_PREF_DEBUG
#ifdef APP_PREF_DEBUG
#define AppPrefPrint        AmbaPrint
#else
#define AppPrefPrint(...)
#endif

// Unit: Bytes
#ifdef CONFIG_APP_ARD
#define SYSTEM_PREF_SIZE    (2048)
#else
#define SYSTEM_PREF_SIZE    (512)
#endif
static UINT8 DefaultTable[SYSTEM_PREF_SIZE] = {
     /*System */
#ifdef CONFIG_APP_ARD
    (AMBA_APP_BUILD_DATE_NUM&0x00ff), ((AMBA_APP_BUILD_DATE_NUM&0xff00)>>8), 0x00, 0x00,
#else
    0x00, 0x11, 0x00, 0x00,
#endif
    /* Video */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#ifdef CONFIG_APP_ARD
    0x00, 0x00, 0x00, 0x00,
#endif
    /* Photo */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    /* Image */
#ifdef CONFIG_APP_ARD
    ANTI_FLICKER_50HZ, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#else
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Audio */
#ifdef CONFIG_APP_ARD
    MAX_AUDIO_VOLUME_LEVLE, 0x00, 0x00, 0x00,
#else
    0x00, 0x00, 0x00, 0x00,
#endif
    /* Playback*/
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
#ifndef CONFIG_APP_ARD
    /* Setup */
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x7D, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
    /* VA*/
    0x00, 0x37, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00,
#ifdef CONFIG_APP_ARD
    /* Factory_mode*/
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Motion detect*/
    0x00, 0x00, 0x00, 0x00,
    /* G-sensor*/
    0x02, 0x00, 0x00, 0x00,
    /* Video stamp*/
    0x00, 0x00, 0x00, 0x00,
    /* Setup */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x7D, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,    
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
#endif
    /* Reserved*/
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static UINT8 SettingTable[SYSTEM_PREF_SIZE];
APP_PREF_USER_s *UserSetting;

/*************************************************************************
 * App status definitons
 ************************************************************************/
APP_STATUS_s app_status;

/*************************************************************************
 * App preference APIs
 ************************************************************************/
#ifdef CONFIG_ENABLE_EMMC_BOOT
    extern int AmbaEMMC_MediaPartRead(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
    extern int AmbaEMMC_MediaPartWrite(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
#endif

/**
 *  @brief Check the version of preference
 *
 *  Check the version of preference
 *
 *  @param[in] Ori the original preference data
 *  @param[in] New the new preference data
 *
 *  @return 1 the same, 0 NOT the same version
 */
static UINT8 AppPref_IsSameVersion(UINT8* ori, UINT8* new)
{
    UINT8 Ret = 0;
    UINT16 SysVerOld = 0;
    UINT16 SysVernew = 0;

    SysVerOld = (ori[1] << 8) + ori[0];
    SysVernew = (new[1] << 8) + new[0];
    if (SysVernew != SysVerOld) {
        AppPrefPrint("[APP PREF] System version doesn't match.");
        AppPrefPrint("[APP PREF] Old System version: %d.%d.%d",
                      GET_VER_MAJOR(SysVerOld),
                      GET_VER_MINOR(SysVerOld),
                      GET_VER_PATCH(SysVerOld));
        AppPrefPrint("[APP PREF] Default System version: %d.%d.%d",
                      GET_VER_MAJOR(SysVernew),
                      GET_VER_MINOR(SysVernew),
                      GET_VER_PATCH(SysVernew));
        Ret = 0;
    } else {
        AppPrefPrint("[APP PREF] System version is the same.");
        Ret = 1;
    }
    return Ret;
}

static int _AppPref_Init(void)
{
    int Rval = 0;

#if defined(CONFIG_ENABLE_EMMC_BOOT)
    // nothing
#else
    if (AmbaNFTL_IsInit(NFTL_ID_PRF) != OK) {
        #if 0
        NandRval = AmbaNFTL_InitLock(NFTL_ID_PRF);
        if (NandRval == OK) {
            AppPrefPrint("[PREF] Init OK" );
        } else {
            AppPrefPrint("[PREF] Init NG" );
        }
        #endif
        Rval = AmbaNFTL_Init(NFTL_ID_PRF, NFTL_MODE_NO_SAVE_TRL_TBL);
        if (Rval == OK) {
            AppPrefPrint("[APP PREF] NAND Init OK" );
        } else {
            AppPrefPrint("[APP PREF] NAND Init NG" );
            Rval = -1;
        }
    } else {
        AppPrefPrint("[APP PREF] NANS Init OK" );
    }
#endif
    return Rval;
}

static int _AppPref_Read(UINT8 *pBuf)
{
    int Rval = 0;

#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartRead(MP_UserSetting, pBuf, 0, 1);
#else
#ifdef CONFIG_APP_ARD
    Rval = AmbaNFTL_Read(NFTL_ID_PRF, pBuf, 0, 4);
#else
    Rval = AmbaNFTL_Read(NFTL_ID_PRF, pBuf, 0, 1);
#endif
#endif
    return Rval;
}

static int _AppPref_Write(UINT8 *pBuf)
{
    int Rval = 0;

#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval |= AmbaEMMC_MediaPartWrite(MP_UserSetting, pBuf, 0, 1);
#else
#ifdef CONFIG_APP_ARD
    Rval |= AmbaNFTL_Write(NFTL_ID_PRF, pBuf, 0, 4);
#else
    Rval |= AmbaNFTL_Write(NFTL_ID_PRF, pBuf, 0, 1);
#endif
#endif
    return Rval;
}

/**
 *  @brief Initialize the preferece part in NAND
 *
 *  Initialize the preferece part in NAND
 *
 *  @return >=0 valid item, <0 failure
 */
INT8 AppPref_InitPref(void)
{
    INT8 ReturnValue = 0;

    AppPrefPrint("[APP PREF] _AppPref_InitPref");
    UserSetting = (APP_PREF_USER_s *)&SettingTable;
    _AppPref_Init();
    return ReturnValue;
}

/**
 *  @brief Load the preferece data from NAND
 *
 *  Load the preferece data from NAND
 *
 *  @return PREF_RET_STATUS_LOAD_RESET re-load happened, PREF_RET_STATUS_LOAD_NORMAL load normal
 */
APP_PREF_RET_STATUS_e AppPref_Load(void)
{
    APP_PREF_RET_STATUS_e ReturnValue = PREF_RET_STATUS_LOAD_NORMAL;
    UINT8 reload = 0;
    int NandRval = OK;
    UINT32 idx = 0;

    /* Load data from NAND */
    NandRval = _AppPref_Read(SettingTable);

    AppPrefPrint("=== Load (%d Bytes) ===", sizeof(SettingTable));
    for (idx=0; idx<sizeof(SettingTable); idx+=4) {
        AppPrefPrint("0x%X, 0x%X, 0x%X, 0x%X", SettingTable[idx], SettingTable[idx+1], SettingTable[idx+2], SettingTable[idx+3]);
    }

    if (NandRval == OK) {
        AppPrefPrint("[APP PREF] load success");
        /* Check whether the firmware is updated */
        if (AppPref_IsSameVersion(DefaultTable, SettingTable)) {
            reload = 0;
        } else {
            reload = 1;
        }
    } else {
        AppPrefPrint("[APP PREF] load fail");
        reload = 1;
    }

    /* Save and re-Load the user preference */
    if (reload) {
        NandRval = OK;
        AppPrefPrint("[APP PREF] Save default preference");
        NandRval = _AppPref_Write(DefaultTable);

        if (NandRval == OK) {
            AppPrefPrint("[APP PREF] save success");
        } else {
            AppPrefPrint("[APP PREF] save fail");
            K_ASSERT(NandRval >= 0);
        }

        AppPrefPrint("[APP PREF] Re-load default preference");
        NandRval = _AppPref_Read(SettingTable);

        if (NandRval == OK) {
            AppPrefPrint("[APP PREF] load success");
        } else {
            AppPrefPrint("[APP PREF] load fail");
            K_ASSERT(NandRval >= 0);
        }
    }

    if (reload) {
        ReturnValue = PREF_RET_STATUS_LOAD_RESET;
    } else {
        ReturnValue = PREF_RET_STATUS_LOAD_NORMAL;
    }
    return ReturnValue;
}

/**
 *  @brief Save the preferece data to NAND
 *
 *  Save the preferece data to NAND
 *
 *  @return >=0 valid item, <0 failure
 */
INT8 AppPref_Save(void)
{
    INT8 ReturnValue = 0;
    int NandRval = OK;

    /** Save and re-Load the user preference */
    NandRval = OK;
    AppPrefPrint("[APP PREF] Save preference");

    NandRval = _AppPref_Write(SettingTable);

    if (NandRval == OK) {
        UINT32 idx = 0;
        ReturnValue = 0;

        AppPrefPrint("[APP PREF] save success");
        AppPrefPrint("=== Write (%d Bytes) ===", sizeof(SettingTable));
        for (idx=0; idx<sizeof(SettingTable); idx+=4) {
            AppPrefPrint("0x%X, 0x%X, 0x%X, 0x%X", SettingTable[idx], SettingTable[idx+1], SettingTable[idx+2], SettingTable[idx+3]);
        }
    } else {
        AppPrefPrint("[APP PREF] save fail");
        ReturnValue = -1;
    }
    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
UINT32 AppPref_GetLanguageID(VOID) {
    return UserSetting->SetupPref.LanguageID;
}
#endif

