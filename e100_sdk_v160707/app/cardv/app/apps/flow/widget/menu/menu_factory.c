/**
  * @file src/app/apps/flow/widget/menu/cardv/menu_factory.c
  *
  * Implementation of factory mode-related Menu Items
  *
  * History:
  *    2015/2/5 - [Emmett Xie] created file
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
//#pragma O0

#include <apps/apps.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/resource/gui_settle.h>
#include <system/ApplibSys_Lcd.h>
#include <AmbaRTC.h>
#include "../../../../../applib/inc/system/ApplibSys_GSensor.h"

/*************************************************************************
 * Declaration:
 ************************************************************************/
// tab
static int menu_factory_init(void);
static int menu_factory_start(void);
static int menu_factory_stop(void);
// item
static int menu_factory_version_init(void);
static int menu_factory_version_get_tab_str(void);
static int menu_factory_version_set(void);

static int menu_factory_manual_cali_init(void);
static int menu_factory_manual_cali_get_tab_str(void);
static int menu_factory_manual_cali_get_sel_str(int ref);
static int menu_factory_manual_cali_get_sel_bmp(int ref);
static int menu_factory_manual_cali_set(void);
static int menu_factory_manual_cali_sel_set(void);

static int menu_factory_gsensor_test_init(void);
static int menu_factory_gsensor_test_get_tab_str(void);
static int menu_factory_gsensor_test_set(void);

static int menu_factory_4g_test_init(void);
static int menu_factory_4g_test_get_tab_str(void);
static int menu_factory_4g_test_set(void);

static int menu_factory_gps_test_init(void);
static int menu_factory_gps_test_get_tab_str(void);
static int menu_factory_gps_test_set(void);

static int menu_factory_rec_mode_init(void);
static int menu_factory_rec_mode_get_tab_str(void);
static int menu_factory_rec_mode_get_sel_str(int ref);
static int menu_factory_rec_mode_get_sel_bmp(int ref);
static int menu_factory_rec_mode_set(void);
static int menu_factory_rec_mode_sel_set(void);

// control
static MENU_TAB_s* menu_factory_get_tab(void);
static MENU_ITEM_s* menu_factory_get_item(UINT32 itemId);
static MENU_SEL_s* menu_factory_get_sel(UINT32 itemId, UINT32 selId);
static int menu_factory_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int menu_factory_lock_tab(void);
static int menu_factory_unlock_tab(void);
static int menu_factory_enable_item(UINT32 itemId);
static int menu_factory_disable_item(UINT32 itemId);
static int menu_factory_lock_item(UINT32 itemId);
static int menu_factory_unlock_item(UINT32 itemId);
static int menu_factory_enable_sel(UINT32 itemId, UINT32 selId);
static int menu_factory_disable_sel(UINT32 itemId, UINT32 selId);
static int menu_factory_lock_sel(UINT32 itemId, UINT32 selId);
static int menu_factory_unlock_sel(UINT32 itemId, UINT32 selId);

/*************************************************************************
 * Definition:
 ************************************************************************/
/** Selection */
static MENU_SEL_s menu_factory_manual_cali_sel_tbl[MENU_FACTORY_MANUAL_CALI_SEL_NUM] = {
    {MENU_FACTORY_MANUAL_CALI_BLACK_LEVEL_CORRECTION, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_BLACK_LEVEL_CORRECTION, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_BLACK_LEVEL_CORRECTION, NULL},
    {MENU_FACTORY_MANUAL_CALI_BAD_PIXEL_CORRECTION, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_BAD_PIXEL_CORRECTION, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_BAD_PIXEL_CORRECTION, NULL},
    {MENU_FACTORY_MANUAL_CALI_CHROMA_ABERRATION, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_CHROMA_ABERRATION, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_CHROMA_ABERRATION, NULL},
    {MENU_FACTORY_MANUAL_CALI_WARP, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_WARP, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_WARP, NULL},
    {MENU_FACTORY_MANUAL_CALI_VIGNETTE, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_VIGNETTE, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_VIGNETTE, NULL},
    {MENU_FACTORY_MANUAL_CALI_WB_GOLDEN_SAMPLE, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_WB_GOLDEN_SAMPLE, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_WB_GOLDEN_SAMPLE, NULL},
    {MENU_FACTORY_MANUAL_CALI_WBH, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_WBH, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_WBH, NULL},
    {MENU_FACTORY_MANUAL_CALI_WBL, MENU_SEL_FLAGS_ENABLE,
        STR_FACTORY_WBL, BMP_ICN_BRIGHTNESS_LOW, 0,
        MENU_FACTORY_MANUAL_CALI_WBL, NULL},
};

static MENU_SEL_s menu_factory_rec_mode_sel_tbl[MENU_FACTORY_REC_MODE_SEL_NUM] = {
    {MENU_FACTORY_REC_MODE_VIDEO_MODE, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_MODE, BMP_ICN_VIDEO_FILM, 0,
        MENU_FACTORY_REC_MODE_VIDEO_MODE, NULL},
    {MENU_FACTORY_REC_MODE_STILL_MODE, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_MODE, BMP_ICN_PRECISE_QUALITY, 0,
        MENU_FACTORY_REC_MODE_STILL_MODE, NULL}
};

static MENU_SEL_s *menu_factory_item_sel_tbls[MENU_FACTORY_ITEM_NUM] = {
    NULL,   //Version
    menu_factory_manual_cali_sel_tbl,
    NULL,   //G sensor test
    NULL,   //4G test
    NULL,   //Gps test
    menu_factory_rec_mode_sel_tbl,
};


/*** Currently activated object id arrays ***/
static MENU_SEL_s *menu_factory_manual_cali_sels[MENU_FACTORY_MANUAL_CALI_SEL_NUM];
static MENU_SEL_s *menu_factory_rec_mode_sels[MENU_FACTORY_REC_MODE_SEL_NUM];

/*** Item ***/
/**
 * @brief Menu_factory item: version
 */
static MENU_ITEM_s menu_factory_version = {
    MENU_FACTORY_VERSION, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_factory_version_init,
    menu_factory_version_get_tab_str,
    NULL,
    NULL,
    menu_factory_version_set,
    NULL
};

static MENU_ITEM_s menu_factory_manual_cali = {
    MENU_FACTORY_MANUAL_CALI, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_factory_manual_cali_sels,
    menu_factory_manual_cali_init,
    menu_factory_manual_cali_get_tab_str,
    menu_factory_manual_cali_get_sel_str,
    menu_factory_manual_cali_get_sel_bmp,
    menu_factory_manual_cali_set,
    menu_factory_manual_cali_sel_set
};

static MENU_ITEM_s menu_factory_gsensor_test = {
    MENU_FACTORY_GSENSOR_TEST, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_factory_gsensor_test_init,
    menu_factory_gsensor_test_get_tab_str,
    NULL,
    NULL,
    menu_factory_gsensor_test_set,
    NULL
};

static MENU_ITEM_s menu_factory_4g_test = {
    MENU_FACTORY_4G_TEST, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_factory_4g_test_init,
    menu_factory_4g_test_get_tab_str,
    NULL,
    NULL,
    menu_factory_4g_test_set,
    NULL
};

static MENU_ITEM_s menu_factory_gps_test = {
    MENU_FACTORY_GPS_TEST, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_factory_gps_test_init,
    menu_factory_gps_test_get_tab_str,
    NULL,
    NULL,
    menu_factory_gps_test_set,
    NULL
};

static MENU_ITEM_s menu_factory_rec_mode = {
    MENU_FACTORY_REC_MODE, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_factory_rec_mode_sels,
    menu_factory_rec_mode_init,
    menu_factory_rec_mode_get_tab_str,
    menu_factory_rec_mode_get_sel_str,
    menu_factory_rec_mode_get_sel_bmp,
    menu_factory_rec_mode_set,
    menu_factory_rec_mode_sel_set
};

static MENU_ITEM_s *menu_factory_item_tbl[MENU_FACTORY_ITEM_NUM] = {
    &menu_factory_version,
    &menu_factory_manual_cali,
    &menu_factory_gsensor_test,
    &menu_factory_4g_test,
    &menu_factory_gps_test,
    &menu_factory_rec_mode
};

/*** Currently activated object id arrays ***/
static MENU_ITEM_s *menu_factory_items[MENU_FACTORY_ITEM_NUM];

/*** Tab ***/
static MENU_TAB_s menu_factory = {
    MENU_FACTORY, 1,
    0, 0,
    BMP_ICN_BRIGHTNESS_LOW, BMP_ICN_BRIGHTNESS_LOW_HL,
    menu_factory_items,
    menu_factory_init,
    menu_factory_start,
    menu_factory_stop
};

MENU_TAB_CTRL_s menu_factory_ctrl = {
    menu_factory_get_tab,
    menu_factory_get_item,
    menu_factory_get_sel,
    menu_factory_set_sel_table,
    menu_factory_lock_tab,
    menu_factory_unlock_tab,
    menu_factory_enable_item,
    menu_factory_disable_item,
    menu_factory_lock_item,
    menu_factory_unlock_item,
    menu_factory_enable_sel,
    menu_factory_disable_sel,
    menu_factory_lock_sel,
    menu_factory_unlock_sel
};

/*** APIs ***/
// tab
static int menu_factory_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;
    AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);
    menu_factory_disable_item(MENU_FACTORY_REC_MODE);
    menu_factory_disable_item(MENU_FACTORY_4G_TEST);

    APP_ADDFLAGS(menu_factory.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_factory.ItemNum > 0) {
        cur_item_id = menu_factory_items[menu_factory.ItemCur]->Id;
    }

    menu_factory.ItemNum = 0;
    menu_factory.ItemCur = 0;

    for (i=0; i<MENU_FACTORY_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(menu_factory_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_factory_items[menu_factory.ItemNum] = menu_factory_item_tbl[i];
            if (cur_item_id == menu_factory_item_tbl[i]->Id) {
                menu_factory.ItemCur = menu_factory.ItemNum;
            }
            menu_factory.ItemNum++;
        }
    }

    return 0;
}

static int menu_factory_start(void)
{
    return 0;
}

static int menu_factory_stop(void)
{
    return 0;
}

// item
/**
 * @brief Initialization function of menu_factory_version
 * @return success or not
 */
static int menu_factory_version_init(void)
{

    return 0;
}

/**
 * @brief Function to get string Id to display on main menu
 * @return int String ID
 */
static int menu_factory_version_get_tab_str(void)
{
    return STR_FACTORY_VERSION;
}

extern const char *pAmbaVer_LinkVer_Date;
/**
 * @brief Dummy function to handle SET operation
 * @return Don't care
 */
static int menu_factory_version_set(void)
{
    UINT16 OutputStr0[64] = {'S', 'D', 'K', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', ' ', 'i', 's', ':', '\0'};
    UINT16 OutputStr1[64] = {'\0'};
    UINT16 *PtrStrDst;
    const char *PtrStrSrc;

    PtrStrDst = OutputStr1;
    PtrStrSrc=pAmbaVer_LinkVer_Date;

    //Omit week info
    while (*PtrStrSrc != ' ')
        PtrStrSrc++;

    PtrStrSrc++;
    while (*PtrStrSrc) {
        if(*PtrStrSrc == 'C')
            PtrStrSrc += 4;

        *PtrStrDst++ = *PtrStrSrc++;
    }

    *PtrStrDst = '\0';

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM0, OutputStr0);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0, STR_INFO_PROMPT_CONTENT_ITEM0);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr1);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1, STR_INFO_PROMPT_CONTENT_ITEM1);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM2, (UINT16 *)"");
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2, STR_INFO_PROMPT_CONTENT_ITEM2);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM3, (UINT16 *)"");
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3, STR_INFO_PROMPT_CONTENT_ITEM3);

    AppWidget_On(WIDGET_INFO_PROMPT, 0);

    return 0;

}

static int menu_factory_manual_cali_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_factory_manual_cali.Flags, MENU_ITEM_FLAGS_INIT);

    menu_factory_manual_cali.SelSaved = 0;
    menu_factory_manual_cali.SelNum = 0;
    menu_factory_manual_cali.SelCur = 0;

    for (i=0; i<MENU_FACTORY_MANUAL_CALI_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_factory_item_sel_tbls[MENU_FACTORY_MANUAL_CALI]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_factory_manual_cali_sels[menu_factory_manual_cali.SelNum] = menu_factory_item_sel_tbls[MENU_FACTORY_MANUAL_CALI]+i;
            if (menu_factory_manual_cali_sels[menu_factory_manual_cali.SelNum]->Val == UserSetting->FactoryPref.ManualCali) {
                menu_factory_manual_cali.SelSaved = menu_factory_manual_cali.SelNum;
                menu_factory_manual_cali.SelCur = menu_factory_manual_cali.SelNum;
            }
            menu_factory_manual_cali.SelNum++;
        }
    }

    return 0;
}

static int menu_factory_manual_cali_get_tab_str(void)
{
    return STR_FACTORY_MANUAL_CALIBRATION;
}

static int menu_factory_manual_cali_get_sel_str(int ref)
{
    return menu_factory_manual_cali_sels[ref]->Str;
}

static int menu_factory_manual_cali_get_sel_bmp(int ref)
{
    return menu_factory_manual_cali_sels[ref]->Bmp;
}

static int menu_factory_manual_cali_set(void)
{
    AppMenuQuick_SetItem(MENU_FACTORY, MENU_FACTORY_MANUAL_CALI);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_factory_manual_cali_sel_set(void)
{
    int cali_id = 0;
    if (menu_factory_manual_cali.SelSaved != menu_factory_manual_cali.SelCur) {
        menu_factory_manual_cali.SelSaved = menu_factory_manual_cali.SelCur;
        UserSetting->FactoryPref.ManualCali = menu_factory_manual_cali_sels[menu_factory_manual_cali.SelCur]->Val;
    }
    cali_id = UserSetting->FactoryPref.ManualCali;
    AppLibComSvcAsyncOp_DoCalibration(cali_id);
    return 0;
}

static int menu_factory_gsensor_test_init(void)
{

    return 0;
}

static int menu_factory_gsensor_test_get_tab_str(void)
{
    return STR_FACTORY_G_SENSOR_TEST_MODE;
}

static int menu_factory_gsensor_test_set(void)
{
    UINT16 OutputStr0[64] = {'G', 'S', 'e', 'n', 's', 'o', 'r', ' ', 'v', 'a', 'l', 'u', 'e', ' ', 'i', 's', ':', '\0'};
    UINT16 OutputStr1[64] = {'x', ':', '\0'};
    UINT16 OutputStr2[64] = {'y', ':', '\0'};
    UINT16 OutputStr3[64] = {'z', ':', '\0'};
    char FmtStr[64] = {'\0'};
    float gx = 0, gy = 0, gz = 0;

    AppLibSysGSensor_Get_Cur_Value(&gx, &gy, &gz);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM0, OutputStr0);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0, STR_INFO_PROMPT_CONTENT_ITEM0);

    sprintf(FmtStr, "x: %f", gx);
    AmbaUtility_Ascii2Unicode(FmtStr, OutputStr1);
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr1);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1, STR_INFO_PROMPT_CONTENT_ITEM1);

    sprintf(FmtStr, "y: %f", gy);
    AmbaUtility_Ascii2Unicode(FmtStr, OutputStr2);
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM2, OutputStr2);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2, STR_INFO_PROMPT_CONTENT_ITEM2);

    sprintf(FmtStr, "z: %f", gz);
    AmbaUtility_Ascii2Unicode(FmtStr, OutputStr3);
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM3, OutputStr3);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3, STR_INFO_PROMPT_CONTENT_ITEM3);

    AppWidget_On(WIDGET_INFO_PROMPT, 0);
    return 0;
}

static int menu_factory_4g_test_init(void)
{

    return 0;
}

static int menu_factory_4g_test_get_tab_str(void)
{
    return STR_FACTORY_4G_TEST_MODE;
}

static int menu_factory_4g_test_set(void)
{
    return 0;
}

static int menu_factory_gps_test_init(void)
{

    return 0;
}

static int menu_factory_gps_test_get_tab_str(void)
{
    return STR_FACTORY_GPS_TEST_MODE;
}

static int menu_factory_gps_test_set(void)
{
    UINT16 OutputStr0[64] = {'G', 'P', 'S', ' ', 's', 'i', 'g', 'n', 'a', 'l', ' ', 'i', 's', ':', '\0'};
    UINT16 OutputStr1[64] = {'S', 'i', 'g', 'n', 'a', 'l', ':', '0', '\0'};
    UINT16 OutputStr2[64] = {'G', 'P', 'S', ' ', 'i', 's', ' ', 'd', 'i', 's', 'c', 'o', 'n', 'n', 'e', 'c', 't', 'e', 'd', '\0'};

    OutputStr1[7] = '0' + app_status.gps_status;

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM0, OutputStr0);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0, STR_INFO_PROMPT_CONTENT_ITEM0);

    if(-1 == app_status.gps_status) {
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr2);
    } else {
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr1);
    }
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1, STR_INFO_PROMPT_CONTENT_ITEM1);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM2, (UINT16 *)"");
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2, STR_INFO_PROMPT_CONTENT_ITEM2);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM3, (UINT16 *)"");
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3, STR_INFO_PROMPT_CONTENT_ITEM3);

    AppWidget_On(WIDGET_INFO_PROMPT, 0);

    return 0;
}

static int menu_factory_rec_mode_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_factory_rec_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_factory_rec_mode.SelSaved = 0;
    menu_factory_rec_mode.SelNum = 0;
    menu_factory_rec_mode.SelCur = 0;


    for (i=0; i<MENU_FACTORY_REC_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_factory_item_sel_tbls[MENU_FACTORY_REC_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_factory_rec_mode_sels[menu_factory_rec_mode.SelNum] = menu_factory_item_sel_tbls[MENU_FACTORY_REC_MODE]+i;
            if (menu_factory_rec_mode_sels[menu_factory_rec_mode.SelNum]->Val == app_status.CurrEncMode) {
                menu_factory_rec_mode.SelSaved = menu_factory_rec_mode.SelNum;
                menu_factory_rec_mode.SelCur = menu_factory_rec_mode.SelNum;
            }
            menu_factory_rec_mode.SelNum++;
        }
    }

    return 0;
}

static int menu_factory_rec_mode_get_tab_str(void)
{
    return menu_factory_rec_mode_sels[menu_factory_rec_mode.SelSaved]->Str;
}

static int menu_factory_rec_mode_get_sel_str(int ref)
{
    return menu_factory_rec_mode_sels[ref]->Str;
}

static int menu_factory_rec_mode_get_sel_bmp(int ref)
{
    return menu_factory_rec_mode_sels[ref]->Bmp;
}

static int menu_factory_rec_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_FACTORY, MENU_FACTORY_REC_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_factory_rec_mode_sel_set(void)
{
    if (menu_factory_rec_mode.SelSaved != menu_factory_rec_mode.SelCur) {
        menu_factory_rec_mode.SelSaved = menu_factory_rec_mode.SelCur;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_RECORD_MODE, menu_factory_rec_mode.Sels[menu_factory_rec_mode.SelCur]->Val, 0);
        }
    }
    return 0;
}

// control

static MENU_TAB_s* menu_factory_get_tab(void)
{
    return &menu_factory;
}

static MENU_ITEM_s* menu_factory_get_item(UINT32 itemId)
{
    return menu_factory_item_tbl[itemId];
}

static MENU_SEL_s* menu_factory_get_sel(UINT32 itemId, UINT32 selId)
{
    return &menu_factory_item_sel_tbls[itemId][selId];
}

static int menu_factory_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    menu_factory_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int menu_factory_lock_tab(void)
{
    APP_ADDFLAGS(menu_factory.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_factory_unlock_tab(void)
{
    APP_REMOVEFLAGS(menu_factory.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_factory_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_factory.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_factory_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_factory.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_factory_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_factory_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_factory_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((menu_factory_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((menu_factory_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_factory_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((menu_factory_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((menu_factory_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_factory_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_factory_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((menu_factory_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int menu_factory_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((menu_factory_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}
