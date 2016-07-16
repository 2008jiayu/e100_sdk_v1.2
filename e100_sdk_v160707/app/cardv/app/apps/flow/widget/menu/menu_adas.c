#include <apps/apps.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <system/ApplibSys_Lcd.h>
#include <AmbaRTC.h>

static int adas_setup_init(void);
static int adas_setup_start(void);
static int adas_setup_stop(void);
static MENU_TAB_s* adas_setup_get_tab(void);
static MENU_ITEM_s* adas_setup_get_item(UINT32 itemId);
static MENU_SEL_s* adas_setup_get_sel(UINT32 itemId, UINT32 selId);
static int adas_setup_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int adas_setup_lock_tab(void);
static int adas_setup_unlock_tab(void);
static int adas_setup_enable_item(UINT32 itemId);
static int adas_setup_disable_item(UINT32 itemId);
static int adas_setup_lock_item(UINT32 itemId);
static int adas_setup_unlock_item(UINT32 itemId);
static int adas_setup_enable_sel(UINT32 itemId, UINT32 selId);
static int adas_setup_disable_sel(UINT32 itemId, UINT32 selId);
static int adas_setup_lock_sel(UINT32 itemId, UINT32 selId);
static int adas_setup_unlock_sel(UINT32 itemId, UINT32 selId);
static int adas_before_alarm_init(void);
static int adas_before_alarm_get_tab_str(void);
static int adas_before_alarm_get_sel_str(int ref);
static int adas_before_alarm_get_sel_bmp(int ref);
static int adas_before_alarm_set(void);
static int adas_before_alarm_sel_set(void);
static int adas_orbit_warning_init(void);
static int adas_orbit_warning_get_tab_str(void);
static int adas_orbit_warning_get_sel_str(int ref);
static int adas_orbit_warning_get_sel_bmp(int ref);
static int adas_orbit_warning_set(void);
static int adas_orbit_warning_sel_set(void);
static int menu_video_adas_init(void);
static int menu_video_adas_get_tab_str(void);
static int menu_video_adas_get_sel_str(int ref);
static int menu_video_adas_get_sel_bmp(int ref);
static int menu_video_adas_set(void);
static int menu_video_adas_sel_set(void);

static MENU_SEL_s *adas_orbit_warning_sels[MENU_ADAS_ORBIT_WARNING_SEL_NUM];
static MENU_SEL_s *adas_before_alarm_sels[MENU_ADAS_BEFORE_ALARM_SEL_NUM];
static MENU_SEL_s *menu_video_adas_sels[MENU_ADAS_ITEM_NUM];

static MENU_SEL_s adas_orbit_warning_sel_tbl[MENU_ADAS_ORBIT_WARNING_SEL_NUM] = {
    {MENU_ADAS_ORBIT_WARNING_ON, MENU_SEL_FLAGS_ENABLE,
        STR_ORBIT_WARNING_ON, BMP_ICON_ADAS_ON, 0,
        ADAS_ORBIT_WARNING_ON, NULL},
    {MENU_ADAS_ORBIT_WARNING_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_ORBIT_WARNING_OFF, BMP_ICON_ADAS_OFF, 0,
        ADAS_ORBIT_WARNING_OFF, NULL},
};

static MENU_SEL_s adas_before_alarm_sel_tbl[MENU_ADAS_BEFORE_ALARM_SEL_NUM] = {
    {MENU_ADAS_BEFORE_ALARM_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_BEFORE_ALARM_OFF, BMP_ICON_ADAS_ON, 0,
        ADAS_BEFORE_ALARM_OFF, NULL},
    {MENU_ADAS_BEFORE_ALARM_NEAR, MENU_SEL_FLAGS_ENABLE,
        STR_BEFORE_ALARM_NEAR, BMP_ICON_ADAS_OFF, 0,
        ADAS_BEFORE_ALARM_NEAR, NULL},
    {MENU_ADAS_BEFORE_ALARM_MIDDLE, MENU_SEL_FLAGS_ENABLE,
        STR_BEFORE_ALARM_MIDDLE, BMP_ICON_ADAS_OFF, 0,
        ADAS_BEFORE_ALARM_MIDDLE, NULL},
    {MENU_ADAS_BEFORE_ALARM_FAR, MENU_SEL_FLAGS_ENABLE,
        STR_BEFORE_ALARM_FAR, BMP_ICON_ADAS_OFF, 0,
        ADAS_BEFORE_ALARM_FAR, NULL},        
};

static MENU_SEL_s menu_video_adas_sel_tbl[MENU_ADAS_SEL_NUM] = {
    {MENU_ADAS_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_ADAS_OFF, BMP_ICON_ADAS_OFF, 0,
        ADAS_OFF, NULL},
    {MENU_ADAS_ON, MENU_SEL_FLAGS_ENABLE,
        STR_ADAS_ON, BMP_ICON_ADAS_ON, 0,
        ADAS_ON, NULL}
};

static MENU_SEL_s *adas_setup_item_sel_tbls[MENU_ADAS_ITEM_NUM] = {
    adas_before_alarm_sel_tbl,
    adas_orbit_warning_sel_tbl,
    menu_video_adas_sel_tbl,
};

static MENU_ITEM_s adas_before_alarm = {
    MENU_ADAS_BEFORE_ALARM, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, adas_before_alarm_sels,
    adas_before_alarm_init,
    adas_before_alarm_get_tab_str,
    adas_before_alarm_get_sel_str,
    adas_before_alarm_get_sel_bmp,
    adas_before_alarm_set,
    adas_before_alarm_sel_set
};

static MENU_ITEM_s adas_orbit_warning = {
    MENU_ADAS_ORBIT_WARNING, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, adas_orbit_warning_sels,
    adas_orbit_warning_init,
    adas_orbit_warning_get_tab_str,
    adas_orbit_warning_get_sel_str,
    adas_orbit_warning_get_sel_bmp,
    adas_orbit_warning_set,
    adas_orbit_warning_sel_set
};

static MENU_ITEM_s menu_video_adas = {
    MENU_ADAS_SET, MENU_ITEM_FLAGS_ENABLE, 
    0, 0,
    0, 0,
    0, 0, 0, menu_video_adas_sels,
    menu_video_adas_init,
    menu_video_adas_get_tab_str,
    menu_video_adas_get_sel_str,
    menu_video_adas_get_sel_bmp,
    menu_video_adas_set,
    menu_video_adas_sel_set
};


static MENU_ITEM_s *adas_setup_item_tbl[MENU_ADAS_ITEM_NUM] = {
    &adas_before_alarm,
    &adas_orbit_warning,
    &menu_video_adas,
};

/*** Currently activated object id arrays ***/
static MENU_ITEM_s *adas_setup_items[MENU_ADAS_ITEM_NUM];

/*** Tab ***/
static MENU_TAB_s adas_setup = {
    MENU_ADAS,MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    BMP_MENU_TAB_SETUP, BMP_MENU_TAB_SETUP_HL,
    adas_setup_items,
    adas_setup_init,
    adas_setup_start,
    adas_setup_stop
};

MENU_TAB_CTRL_s adas_setup_ctrl = {
    adas_setup_get_tab,
    adas_setup_get_item,
    adas_setup_get_sel,
    adas_setup_set_sel_table,
    adas_setup_lock_tab,
    adas_setup_unlock_tab,
    adas_setup_enable_item,
    adas_setup_disable_item,
    adas_setup_lock_item,
    adas_setup_unlock_item,
    adas_setup_enable_sel,
    adas_setup_disable_sel,
    adas_setup_lock_sel,
    adas_setup_unlock_sel
};

/*** APIs ***/
// tab

static int adas_setup_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;
    APP_ADDFLAGS(adas_setup.Flags, MENU_TAB_FLAGS_INIT);
    if (adas_setup.ItemNum > 0) {
        cur_item_id = adas_setup_items[adas_setup.ItemCur]->Id;
    }
    adas_setup.ItemNum = 0;
    adas_setup.ItemCur = 0;
    for (i=0; i<MENU_ADAS_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(adas_setup_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            adas_setup_items[adas_setup.ItemNum] = adas_setup_item_tbl[i];
            if (cur_item_id == adas_setup_item_tbl[i]->Id) {
                adas_setup.ItemCur = adas_setup.ItemNum;
            }
            adas_setup.ItemNum++;
        }
    }
    return 0;
}
static int adas_setup_start(void)
{
    //return 0;
}

static int adas_setup_stop(void)
{
    //return 0;
}
// item

// control

static MENU_TAB_s* adas_setup_get_tab(void)
{
    return &adas_setup;
}

static MENU_ITEM_s* adas_setup_get_item(UINT32 itemId)
{
    return adas_setup_item_tbl[itemId];
}

static MENU_SEL_s* adas_setup_get_sel(UINT32 itemId, UINT32 selId)
{
    return &adas_setup_item_sel_tbls[itemId][selId];
}

static int adas_setup_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    adas_setup_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int adas_setup_lock_tab(void)
{
    APP_ADDFLAGS(adas_setup.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int adas_setup_unlock_tab(void)
{
    APP_REMOVEFLAGS(adas_setup.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int adas_setup_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(adas_setup.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int adas_setup_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(adas_setup.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int adas_setup_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int adas_setup_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int adas_setup_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((adas_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((adas_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int adas_setup_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((adas_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((adas_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(adas_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int adas_setup_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((adas_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int adas_setup_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((adas_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int adas_before_alarm_init(void)
{
    int i = 0;
    APP_ADDFLAGS(adas_before_alarm.Flags, MENU_ITEM_FLAGS_INIT);
    adas_before_alarm.SelSaved = 0;
    adas_before_alarm.SelNum = 0;
    adas_before_alarm.SelCur = 0;
    for (i=0; i<MENU_ADAS_BEFORE_ALARM_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((adas_setup_item_sel_tbls[MENU_ADAS_BEFORE_ALARM]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            adas_before_alarm_sels[adas_before_alarm.SelNum] = adas_setup_item_sel_tbls[MENU_ADAS_BEFORE_ALARM]+i;
            if (adas_before_alarm_sels[adas_before_alarm.SelNum]->Val == UserSetting->VAPref.AdasBeforeAlarm) {
                adas_before_alarm.SelSaved = adas_before_alarm.SelNum;
                adas_before_alarm.SelCur = adas_before_alarm.SelNum;
            }
            adas_before_alarm.SelNum++;
        }
    }
    return 0;
}

/**
 * @brief Function to get string Id to display on main menu
 * @return int String ID
 */
static int adas_before_alarm_get_tab_str(void)
{
    return adas_before_alarm_sels[adas_before_alarm.SelSaved]->Str;
}

static int adas_before_alarm_get_sel_str(int ref)
{
    return adas_before_alarm_sels[ref]->Str;

}

static int adas_before_alarm_get_sel_bmp(int ref)
{
    return adas_before_alarm_sels[ref]->Bmp;
}

/**
 * @brief Dummy function to handle SET operation
 * @return Don't care
 */
static int adas_before_alarm_set(void)
{
    AppMenuQuick_SetItem(MENU_ADAS, MENU_ADAS_BEFORE_ALARM);
    AppWidget_On(WIDGET_MENU_QUICK, 0);

    return 0;
}
static int adas_before_alarm_sel_set(void)
{
    UINT8 enable;
    if (adas_before_alarm.SelSaved != adas_before_alarm.SelCur) {
        adas_before_alarm.SelSaved = adas_before_alarm.SelCur;
        UserSetting->VAPref.AdasBeforeAlarm = adas_before_alarm.Sels[adas_before_alarm.SelCur]->Val;

    }
   return 0;
}

static int adas_orbit_warning_init(void)
{
    int i = 0;

    APP_ADDFLAGS(adas_orbit_warning.Flags, MENU_ITEM_FLAGS_INIT);
    adas_orbit_warning.SelSaved = 0;
    adas_orbit_warning.SelNum = 0;
    adas_orbit_warning.SelCur = 0;
    for (i=0; i<MENU_ADAS_ORBIT_WARNING_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((adas_setup_item_sel_tbls[MENU_ADAS_ORBIT_WARNING]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            adas_orbit_warning_sels[adas_orbit_warning.SelNum] = adas_setup_item_sel_tbls[MENU_ADAS_ORBIT_WARNING]+i;
            if (adas_orbit_warning_sels[adas_orbit_warning.SelNum]->Val == UserSetting->VAPref.AdasOrbitWarning) {
                adas_orbit_warning.SelSaved = adas_orbit_warning.SelNum;
                adas_orbit_warning.SelCur = adas_orbit_warning.SelNum;
            }
            adas_orbit_warning.SelNum++;
        }
    }
    return 0;
}

static int adas_orbit_warning_get_tab_str(void)
{
    return adas_orbit_warning_sels[adas_orbit_warning.SelSaved]->Str;
}

static int adas_orbit_warning_get_sel_str(int ref)
{
    return adas_orbit_warning_sels[ref]->Str;
}

static int adas_orbit_warning_get_sel_bmp(int ref)
{
    return adas_orbit_warning_sels[ref]->Bmp;
}

static int adas_orbit_warning_set(void)
{
//#if defined(SZ_ARD_APP_EN)
//    app_widget_menu_quick_set_item_title_str(STR_BEEP_ON_OFF_SET);
//#endif
    AppMenuQuick_SetItem(MENU_ADAS, MENU_ADAS_ORBIT_WARNING);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int adas_orbit_warning_sel_set(void)
{
    UINT8 enable;
    if (adas_orbit_warning.SelSaved != adas_orbit_warning.SelCur) {
        adas_orbit_warning.SelSaved = adas_orbit_warning.SelCur;
        UserSetting->VAPref.AdasOrbitWarning = adas_orbit_warning.Sels[adas_orbit_warning.SelCur]->Val;
        //enable = (UserSetting->SetupPref.beep_sound == BEEP_SOUND_ON)?1:0;
        //AppLibAudioDec_Beep_Enable(enable);
    }
    return 0;
}

static int menu_video_adas_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_adas.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_adas.SelSaved = 0;
    menu_video_adas.SelNum = 0;
    menu_video_adas.SelCur = 0;
    for (i=0; i<MENU_ADAS_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((adas_setup_item_sel_tbls[MENU_ADAS_SET]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_adas_sels[menu_video_adas.SelNum] = adas_setup_item_sel_tbls[MENU_ADAS_SET]+i;
            if (menu_video_adas_sels[menu_video_adas.SelNum]->Val == UserSetting->VAPref.AdasDetection) {
                menu_video_adas.SelSaved = menu_video_adas.SelNum;
                menu_video_adas.SelCur = menu_video_adas.SelNum;
            }
            menu_video_adas.SelNum++;
        }
    }
    return 0;
}

static int menu_video_adas_get_tab_str(void)
{
    return menu_video_adas_sels[menu_video_adas.SelSaved]->Str;
}

static int menu_video_adas_get_sel_str(int ref)
{
    return menu_video_adas_sels[ref]->Str;
}

static int menu_video_adas_get_sel_bmp(int ref)
{
    return menu_video_adas_sels[ref]->Bmp;
}

static int menu_video_adas_set(void)
{
    AppMenuQuick_SetItem(MENU_ADAS, MENU_ADAS_SET);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

#ifdef CONFIG_APP_ARD
static int adas_llw_fcmd_init(void)
{
    int ReturnValue = 0;
    // APPLIB_FCMD_CFG_t FCMD_Config = {0};
    // APPLIB_FCMD_PAR_t FCMD_Params = {0};
    // APPLIB_LLWS_CFG_t LLWS_Config = {0};
    // APPLIB_LLWS_PAR_t LLWS_Params = {0};
    // APPLIB_LDWS_CFG_t LDWS_Config = {0};
    // APPLIB_FCWS_CFG_t FCWS_Config = {0};
    // APPLIB_ADAS_PAR_t ADAS_Params = {0};
    // static int adas_llw_fcmd_init = 0;

    // if(1 == adas_llw_fcmd_init) {
    //     return -1;
    // }

    // adas_llw_fcmd_init = 1;
    // /** FCMD init */
    // ReturnValue = AppLibVideoAnal_FCMD_GetDef_Setting(&FCMD_Config, &FCMD_Params);
    // FCMD_Config.FCMDSensitivity = ADAS_SL_HIGH;
    // ReturnValue = AppLibVideoAnal_FCMD_Init(APPLIB_FRM_HDLR_2ND_YUV, FCMD_Config, FCMD_Params);
    // AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_FCMD_Init return = %d", ReturnValue);

    // /** LLWS init */
    // AppLibVideoAnal_LLWS_GetDef_Setting(&LLWS_Config, &LLWS_Params);
    // LLWS_Config.LLWSSensitivity = ADAS_SL_HIGH;
    // ReturnValue = AppLibVideoAnal_LLWS_Init(LLWS_Config, LLWS_Params);
    // AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_LLWS_Init return = %d", ReturnValue);

    // /** LDWS, FCWS init */
    // AppLibVideoAnal_ADAS_GetDef_Setting(&LDWS_Config, &FCWS_Config, &ADAS_Params);
    // LDWS_Config.LDWSSensitivity = ADAS_SL_HIGH;
    // FCWS_Config.FCWSSensitivity = ADAS_SL_HIGH;
    // ReturnValue = AppLibVideoAnal_ADAS_Init(APPLIB_FRM_HDLR_2ND_YUV, LDWS_Config, FCWS_Config, ADAS_Params);
    // AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_ADAS_Init return = %d", ReturnValue);

    return ReturnValue;
}
#endif

static int menu_video_adas_sel_set(void)
{
    if (menu_video_adas.SelSaved != menu_video_adas.SelCur) {
        menu_video_adas.SelSaved = menu_video_adas.SelCur;
        UserSetting->VAPref.AdasDetection = menu_video_adas.Sels[menu_video_adas.SelCur]->Val;
        
#ifndef CONFIG_ECL_GUI
        if(ADAS_ON == UserSetting->VAPref.AdasDetection) {
            adas_llw_fcmd_init();
        }
#endif
    }
    return 0;
}
