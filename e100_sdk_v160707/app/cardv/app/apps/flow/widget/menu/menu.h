/**
  * @file src/app/apps/flow/widget/menu/menu.h
  *
  *   Header of Main Menu
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
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

#ifndef APP_WIDGET_MENU_H_
#define APP_WIDGET_MENU_H_


#include <apps/flow/widget/widgetmgt.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Menu definitions
 ************************************************************************/
typedef enum _MENU_TAB_ID_e_ {
    MENU_ADAS=0,
    MENU_VIDEO,
    MENU_SETUP,
    MENU_PHOTO,
    MENU_PBACK,
    MENU_IMAGE,
    MENU_PB_FOLDER,
#ifdef CONFIG_APP_ARD 	
    MENU_FACTORY,
#endif	
    MENU_TAB_NUM
} MENU_TAB_ID_e;

#ifdef CONFIG_ECL_GUI
typedef enum _MENU_ECL_TAB_ID_e_ {
    MENU_ADAS_CALI = 0,
    MENU_ADAS_FUNC,
    MENU_ADAS_ALARM_DIS,		
    MENU_RECORDER_SETTTING,
    MENU_VERSION,
    MENU_DEFAULT,
    MENU_ECL_PBACK,
    MENU_ECL_TAB_NUM
} MENU_ECL_TAB_ID_e;
#endif
typedef int (*MENU_ADJ_GET_CUR_VAL_HANDLER)(void);
typedef int (*MENU_ADJ_GET_VAL_STR_HANDLER)(int val);
typedef int (*MENU_ADJ_VAL_SET_HANDLER)(int val);

typedef struct _MENU_MAIN_ADJ_s_ {
    int Step;
    int Min;
    int Max;
    MENU_ADJ_GET_CUR_VAL_HANDLER GetCurVal;
    MENU_ADJ_GET_VAL_STR_HANDLER GetValStr;
    MENU_ADJ_VAL_SET_HANDLER ValSet;
} MENU_MAIN_ADJ_s;

typedef struct _MENU_SEL_s_ {
    UINT8 Id;
    UINT8 Flags;
#define MENU_SEL_FLAGS_ENABLE    (0x01)
#define MENU_SEL_FLAGS_LOCKED    (0x02)
    UINT32 Str;
    UINT32 Bmp;
    UINT32 BmpHl;
    int Val;
    MENU_MAIN_ADJ_s *AdjExt;
} MENU_SEL_s;

typedef int (*MENU_ITEM_INIT_HANDLER)(void);
typedef int (*MENU_ITEM_GET_TAB_STR_HANDLER)(void);
typedef int (*MENU_ITEM_GET_SEL_STR_HANDLER)(int ref);
typedef int (*MENU_ITEM_GET_SEL_BMP_HANDLER)(int ref);
typedef int (*MENU_ITEM_SET_HANDLER)(void);
typedef int (*MENU_ITEM_SEL_SET_HANDLER)(void);

typedef struct _MENU_ITEM_s_ {
    UINT8 Id;
    UINT8 Flags;
#define MENU_ITEM_FLAGS_INIT    (0x01)
#define MENU_ITEM_FLAGS_ENABLE    (0x02)
#define MENU_ITEM_FLAGS_LOCKED    (0x04)
    INT8 SelNum;
    INT8 SelCur;
    INT8 SelSaved;
    UINT32 Str;
    UINT32 Bmp;
    UINT32 BmpHl;
    UINT32 BmpTitle;
    MENU_SEL_s **Sels;
    MENU_ITEM_INIT_HANDLER Init;
    MENU_ITEM_GET_TAB_STR_HANDLER GetTabStr;
    MENU_ITEM_GET_SEL_STR_HANDLER GetSelStr;
    MENU_ITEM_GET_SEL_BMP_HANDLER GetSelBmp;
    MENU_ITEM_SET_HANDLER Set;
    MENU_ITEM_SEL_SET_HANDLER SelSet;
} MENU_ITEM_s;

typedef int (*MENU_TAB_INIT_HANDLER)(void);
typedef int (*MENU_TAB_START_HANDLER)(void);
typedef int (*MENU_TAB_STOP_HANDLER)(void);

typedef struct _MENU_TAB_s_ {
    UINT8 Id;
    UINT8 Flags;
#define MENU_TAB_FLAGS_INIT        (0x01)
#define MENU_TAB_FLAGS_ENABLE    (0x02)
#define MENU_TAB_FLAGS_LOCKED    (0x04)
    INT8 ItemNum;
    INT8 ItemCur;
    UINT32 Bmp;
    UINT32 BmpHl;
    MENU_ITEM_s **Items;
    MENU_TAB_INIT_HANDLER Init;
    MENU_TAB_START_HANDLER Start;
    MENU_TAB_STOP_HANDLER Stop;
} MENU_TAB_s;

typedef MENU_TAB_s* (*MENU_TABLE_CTRL_GET_TAB)(void);
typedef MENU_ITEM_s* (*MENU_TABLE_CTRL_GET_ITEM)(UINT32 itemId);
typedef MENU_SEL_s* (*MENU_TABLE_CTRL_GET_SEL)(UINT32 itemId, UINT32 selId);
typedef int (*MENU_TABLE_CTRL_SET_SEL_TABLE)(UINT32 itemId, MENU_SEL_s *selTbl);
typedef int (*MENU_TABLE_CTRL_LOCK_TAB)(void);
typedef int (*MENU_TABLE_CTRL_UNLOCK_TAB)(void);
typedef int (*MENU_TABLE_CTRL_ENABLE_ITEM)(UINT32 itemId);
typedef int (*MENU_TABLE_CTRL_DISABLE_ITEM)(UINT32 itemId);
typedef int (*MENU_TABLE_CTRL_LOCK_ITEM)(UINT32 itemId);
typedef int (*MENU_TABLE_CTRL_UNLOCK_ITEM)(UINT32 itemId);
typedef int (*MENU_TABLE_CTRL_ENABLE_SEL)(UINT32 itemId, UINT32 selId);
typedef int (*MENU_TABLE_CTRL_DISABLE_SEL)(UINT32 itemId, UINT32 selId);
typedef int (*MENU_TABLE_CTRL_LOCK_SEL)(UINT32 itemId, UINT32 selId);
typedef int (*MENU_TABLE_CTRL_UNLOCK_SEL)(UINT32 itemId, UINT32 selId);

typedef struct _MENU_TAB_CTRL_s_ {
    MENU_TABLE_CTRL_GET_TAB GetTab;
    MENU_TABLE_CTRL_GET_ITEM GetItem;
    MENU_TABLE_CTRL_GET_SEL GetSel;
    MENU_TABLE_CTRL_SET_SEL_TABLE SetSelTable;
    MENU_TABLE_CTRL_LOCK_TAB LockTab;
    MENU_TABLE_CTRL_UNLOCK_TAB UnlockTab;
    MENU_TABLE_CTRL_ENABLE_ITEM EnableItem;
    MENU_TABLE_CTRL_DISABLE_ITEM DisableItem;
    MENU_TABLE_CTRL_LOCK_ITEM LockItem;
    MENU_TABLE_CTRL_UNLOCK_ITEM UnlockItem;
    MENU_TABLE_CTRL_ENABLE_SEL EnableSel;
    MENU_TABLE_CTRL_DISABLE_SEL DisableSel;
    MENU_TABLE_CTRL_LOCK_SEL LockSel;
    MENU_TABLE_CTRL_UNLOCK_SEL UnlockSel;
} MENU_TAB_CTRL_s;

/*************************************************************************
 * Menu APIs for widget management
 ************************************************************************/
extern WIDGET_ITEM_s* AppMenu_GetWidget(void);

/*************************************************************************
 * Public Menu Widget APIs
 ************************************************************************/
extern int AppMenu_Reset(void);
extern int AppMenu_RegisterTab(UINT32 tabId);
extern int AppMenu_ReflushItem(void);
extern int language_sel(UINT8 value,int pre_value);
extern int Adas_Functions_sel(UINT8 value,int pre_value);
extern MENU_TAB_s* AppMenu_GetTab(UINT32 tabId);
extern MENU_ITEM_s* AppMenu_GetItem(UINT32 tabId, UINT32 itemId);
extern MENU_SEL_s* AppMenu_GetSel(UINT32 tabId, UINT32 itemId, UINT32 selId);
extern int AppMenu_SetSelTable(UINT32 tabId, UINT32 itemId, MENU_SEL_s *selTbl);
extern int AppMenu_LockTab(UINT32 tabId);
extern int AppMenu_UnlockTab(UINT32 tabId);
extern int AppMenu_EnableItem(UINT32 tabId, UINT32 itemId);
extern int AppMenu_DisableItem(UINT32 tabId, UINT32 itemId);
extern int AppMenu_LockItem(UINT32 tabId, UINT32 itemId);
extern int AppMenu_UnlockItem(UINT32 tabId, UINT32 itemId);
extern int AppMenu_EnableSel(UINT32 tabId, UINT32 itemId, UINT32 selId);
extern int AppMenu_DisableSel(UINT32 tabId, UINT32 itemId, UINT32 selId);
extern int AppMenu_LockSel(UINT32 tabId, UINT32 itemId, UINT32 selId);
extern int AppMenu_UnlockSel(UINT32 tabId, UINT32 itemId, UINT32 selId);

__END_C_PROTO__

#endif /* APP_WIDGET_MENU_H_ */
