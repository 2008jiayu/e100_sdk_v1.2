#include <apps/apps.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <system/ApplibSys_Lcd.h>
#include <AmbaRTC.h>

static int pbfolder_setup_init(void);
static int pbfolder_setup_start(void);
static int pbfolder_setup_stop(void);

static MENU_TAB_s*  pbfolder_setup_get_tab(void);
static MENU_ITEM_s* pbfolder_setup_get_item(UINT32 itemId);
static MENU_SEL_s*  pbfolder_setup_get_sel(UINT32 itemId, UINT32 selId);

static int pbfolder_setup_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int pbfolder_setup_lock_tab(void);
static int pbfolder_setup_unlock_tab(void);
static int pbfolder_setup_enable_item(UINT32 itemId);
static int pbfolder_setup_disable_item(UINT32 itemId);
static int pbfolder_setup_lock_item(UINT32 itemId);
static int pbfolder_setup_unlock_item(UINT32 itemId);
static int pbfolder_setup_enable_sel(UINT32 itemId, UINT32 selId);
static int pbfolder_setup_disable_sel(UINT32 itemId, UINT32 selId);
static int pbfolder_setup_lock_sel(UINT32 itemId, UINT32 selId);
static int pbfolder_setup_unlock_sel(UINT32 itemId, UINT32 selId);


static MENU_SEL_s *pbfolder_setup_item_sel_tbls[MENU_PB_FOLDER_ITEM_NUM] = {
    adas_before_alarm_sel_tbl,
    adas_orbit_warning_sel_tbl,
    menu_video_adas_sel_tbl,
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
    MENU_PB_FOLDER,MENU_ITEM_FLAGS_ENABLE,
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
static int pb_folder_setup_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;
    APP_ADDFLAGS(pbfolder_setup.Flags, MENU_TAB_FLAGS_INIT);
    if (pb_folder_setup.ItemNum > 0) {
        cur_item_id = pb_folder_setup_items[pb_folder_setup.ItemCur]->Id;
    }
    pb_folder_setup.ItemNum = 0;
    pb_folder_setup.ItemCur = 0;
    for (i=0; i<MENU_ADAS_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(pb_folder_setup_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            pb_folder_setup_items[pb_folder_setup.ItemNum] = pb_folder_setup_item_tbl[i];
            if (cur_item_id == pb_folder_setup_item_tbl[i]->Id) {
                pb_folder_setup.ItemCur = pb_folder_setup.ItemNum;
            }
            pb_folder_setup.ItemNum++;
        }
    }
    return 0;
}

static int pb_folder_setup_start(void)
{
    return 0;
}

static int pb_folder_setup_stop(void)
{
    return 0;
}
// item

// control

static MENU_TAB_s* pb_folder_setup_get_tab(void)
{
    return &pb_folder_setup;
}

static MENU_ITEM_s* pb_folder_setup_get_item(UINT32 itemId)
{
    return pb_folder_setup_item_tbl[itemId];
}

static MENU_SEL_s* pb_folder_setup_get_sel(UINT32 itemId, UINT32 selId)
{
    return &pb_folder_setup_item_tbl[itemId][selId];
}

static int pb_folder_setup_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    pb_folder_setup_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int pb_folder_setup_lock_tab(void)
{
    APP_ADDFLAGS(pb_folder_setup.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int pb_folder_setup_unlock_tab(void)
{
    APP_REMOVEFLAGS(pb_folder_setup.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int pb_folder_setup_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(pb_folder_setup.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int pb_folder_setup_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(pb_folder_setup.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int pb_folder_setup_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int pb_folder_setup_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int pb_folder_setup_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((pb_folder_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((pb_folder_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int pb_folder_setup_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((pb_folder_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((pb_folder_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(pb_folder_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int pb_folder_setup_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((pb_folder_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int pb_folder_setup_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((pb_folder_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
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


