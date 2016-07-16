/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_time.c
  *
  * Implementation of Time Menu
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
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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
#include "menu_time.h"
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/menu/gui_menu_time.h>
#include <AmbaRTC.h>

/*************************************************************************
 * Time Menu definitions
 ************************************************************************/
typedef enum _MENU_TIME_VAL_ID_e_ {
    TIME_VALUE_YEAR = 0,
    TIME_VALUE_MONTH,
    TIME_VALUE_DAY,
    TIME_VALUE_HOUR,
    TIME_VALUE_MINUTE,
    TIME_VALUE_SECOND,
    TIME_VALUE_NUM
} MENU_TIME_VAL_ID_e;

#define YEAR_MAX    (2033)
#define YEAR_MIN    (2016)
#define MONTH_MAX    (12)
#define MONTH_MIN    (1)
#define DAY_MIN        (1)
#define HOUR_MAX_24    (23)
#define HOUR_MIN    (0)
#define MINUTE_MAX    (59)
#define MINUTE_MIN    (0)
#define SECOND_MAX    (59)
#define SECOND_MIN    (0)

typedef enum _MENU_TIME_FUNC_ID_e_ {
    MENU_TIME_SWITCH_TO_VAL = 0,
    MENU_TIME_CUR_VAL_SET,
    MENU_TIME_SET_YEAR,
    MENU_TIME_SET_MONTH,
    MENU_TIME_SET_DAY,
    MENU_TIME_SET_HOUR,
    MENU_TIME_SET_MINUTE,
    MENU_TIME_SET_SECOND
} MENU_TIME_FUNC_ID_e;

static int menu_time_func(UINT32 funcId, UINT32 param1, UINT32 param2);

static int menu_time_button_up(void);
static int menu_time_button_down(void);
static int menu_time_button_left(void);
static int menu_time_button_right(void);
static int menu_time_button_set(void);
static int menu_time_button_menu(void);

typedef struct _MENU_TIME_OP_s_ {
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} MENU_TIME_OP_s;

static MENU_TIME_OP_s menu_time_op = {
    menu_time_button_up,
    menu_time_button_down,
    menu_time_button_left,
    menu_time_button_right,
    menu_time_button_set,
    menu_time_button_menu
};

typedef struct _MENU_TIME_s_ {
    int ValNum;
    int ValCur;
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    MENU_ITEM_s *Item;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 gui_cmd, UINT32 param1, UINT32 param2);
    MENU_TIME_OP_s *Op;
} MENU_TIME_s;

static MENU_TIME_s menu_time = {0};

static int app_menu_time_on(UINT32 param);
static int app_menu_time_off(UINT32 param);
static int app_menu_time_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu_time = {
    app_menu_time_on,
    app_menu_time_off,
    app_menu_time_on_message
};

/*************************************************************************
 * Time Menu APIs
 ************************************************************************/
static int get_day_max(int year, int month)
{
    int Day = 31;
    switch (month) {
    case 2:
        /// Leap year check.
        if (year%400==0 || (year%100!=0 && year%4==0)) {
            Day = 29;
        } else {
            Day = 28;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        Day = 30;
        break;
    default:
        Day = 31;
        break;
    }
    return Day;
}

static int menu_time_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MENU_TIME_SWITCH_TO_VAL:
        menu_time.ValCur = param1;
        menu_time.Gui(GUI_VALUE_HL, param1, 0);
        menu_time.Gui(GUI_FLUSH, 0, 0);
        break;
    case MENU_TIME_CUR_VAL_SET:
        {
            AMBA_RTC_TIME_SPEC_u TimeSpec = {0};

            TimeSpec.Calendar.Year = menu_time.Year;
            TimeSpec.Calendar.Month = menu_time.Month;
            TimeSpec.Calendar.Day = menu_time.Day;
            TimeSpec.Calendar.Hour = menu_time.Hour;
            TimeSpec.Calendar.Minute = menu_time.Minute;
            TimeSpec.Calendar.Second = menu_time.Second;
            ReturnValue = AmbaRTC_SetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED, "[Menu Time] AmbaRTC_SetSystemTime failure. ReturnValue = %d", ReturnValue);
            }
        }
        break;
    case MENU_TIME_SET_YEAR:
        if ((int)param1 < YEAR_MIN) {
            param1 = YEAR_MIN;
        } else if ((int)param1 > YEAR_MAX) {
            param1 = YEAR_MAX;
        }
        menu_time.Year = param1;
        break;
    case MENU_TIME_SET_MONTH:
        if ((int)param1 < MONTH_MIN) {
            param1 = MONTH_MIN;
        } else if ((int)param1 > MONTH_MAX) {
            param1 = MONTH_MAX;
        }
        menu_time.Month = param1;
        break;
    case MENU_TIME_SET_DAY:
        {
            int day_max = get_day_max(menu_time.Year, menu_time.Month);
            if ((int)param1 < DAY_MIN) {
                param1 = DAY_MIN;
            } else if ((int)param1 > day_max) {
                param1 = day_max;
            }
            menu_time.Day = param1;
        }
        break;
    case MENU_TIME_SET_HOUR:
        if ((int)param1 < HOUR_MIN) {
            param1 = HOUR_MIN;
        } else if ((int)param1 > HOUR_MAX_24) {
            param1 = HOUR_MAX_24;
        }
        menu_time.Hour = param1;
        break;
    case MENU_TIME_SET_MINUTE:
        if ((int)param1 < MINUTE_MIN) {
            param1 = MINUTE_MIN;
        } else if ((int)param1 > MINUTE_MAX) {
            param1 = MINUTE_MAX;
        }
        menu_time.Minute = param1;
        break;
    case MENU_TIME_SET_SECOND:
        if ((int)param1 < SECOND_MIN) {
            param1 = SECOND_MIN;
        } else if ((int)param1 > SECOND_MAX) {
            param1 = SECOND_MAX;
        }
        menu_time.Second= param1;
        break;    
    default:
        AmbaPrint("The function is undefined");
        break;
    }

    return ReturnValue;
}

static int menu_time_button_up(void)
{
    int Val = 0;
    switch (menu_time.ValCur) {
    case TIME_VALUE_YEAR:
        Val = menu_time.Year+1;
        if (Val > YEAR_MAX) {
            Val = YEAR_MIN;
        }
        menu_time.Func(MENU_TIME_SET_YEAR, Val, 0);
        menu_time.Func(MENU_TIME_SET_DAY, menu_time.Day, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_YEAR, menu_time.Year);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
        break;
    case TIME_VALUE_MONTH:
        Val = menu_time.Month+1;
        if (Val > MONTH_MAX) {
            Val = MONTH_MIN;
        }
        menu_time.Func(MENU_TIME_SET_MONTH, Val, 0);
        menu_time.Func(MENU_TIME_SET_DAY, menu_time.Day, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_MONTH, menu_time.Month);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
        break;
    case TIME_VALUE_DAY:
        Val = menu_time.Day+1;
        if (Val > get_day_max(menu_time.Year, menu_time.Month)) {
            Val = DAY_MIN;
        }
        menu_time.Func(MENU_TIME_SET_DAY, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
        break;
    case TIME_VALUE_HOUR:
        Val = menu_time.Hour+1;
        if (Val > HOUR_MAX_24) {
            Val = HOUR_MIN;
        }
        menu_time.Func(MENU_TIME_SET_HOUR, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_HOUR, menu_time.Hour);
        break;
    case TIME_VALUE_MINUTE:
        Val = menu_time.Minute+1;
        if (Val > MINUTE_MAX) {
            Val = MINUTE_MIN;
        }
        menu_time.Func(MENU_TIME_SET_MINUTE, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_MINUTE, menu_time.Minute);
        break;
    case TIME_VALUE_SECOND:
        Val = menu_time.Second+1;
        if (Val > SECOND_MAX) {
            Val = SECOND_MIN;
        }
        menu_time.Func(MENU_TIME_SET_SECOND, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_SECOND, menu_time.Second);
        break;
    default:
        break;
    }
    menu_time.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int menu_time_button_down(void)
{
    int Val = 0;
    switch (menu_time.ValCur) {
    case TIME_VALUE_YEAR:
        Val = menu_time.Year-1;
        if (Val < YEAR_MIN) {
            Val = YEAR_MAX;
        }
        menu_time.Func(MENU_TIME_SET_YEAR, Val, 0);
        menu_time.Func(MENU_TIME_SET_DAY, menu_time.Day, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_YEAR, menu_time.Year);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
        break;
    case TIME_VALUE_MONTH:
        Val = menu_time.Month-1;
        if (Val < MONTH_MIN) {
            Val = MONTH_MAX;
        }
        menu_time.Func(MENU_TIME_SET_MONTH, Val, 0);
        menu_time.Func(MENU_TIME_SET_DAY, menu_time.Day, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_MONTH, menu_time.Month);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
        break;
    case TIME_VALUE_DAY:
        Val = menu_time.Day-1;
        if (Val < DAY_MIN) {
            Val = get_day_max(menu_time.Year, menu_time.Month);
        }
        menu_time.Func(MENU_TIME_SET_DAY, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
        break;
    case TIME_VALUE_HOUR:
        Val = menu_time.Hour-1;
        if (Val < HOUR_MIN) {
            Val = HOUR_MAX_24;
        }
        menu_time.Func(MENU_TIME_SET_HOUR, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_HOUR, menu_time.Hour);
        break;
    case TIME_VALUE_MINUTE:
        Val = menu_time.Minute-1;
        if (Val < MINUTE_MIN) {
            Val = MINUTE_MAX;
        }
        menu_time.Func(MENU_TIME_SET_MINUTE, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_MINUTE, menu_time.Minute);
        break;
    case TIME_VALUE_SECOND:
        Val = menu_time.Second-1;
        if (Val < SECOND_MIN) {
            Val = SECOND_MAX;
        }
        menu_time.Func(MENU_TIME_SET_SECOND, Val, 0);
        menu_time.Gui(GUI_VALUE_UPDATE, GUI_SECOND, menu_time.Second);
        break;    
    default:
        break;
    }
    menu_time.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int menu_time_button_left(void)
{
    int Target = menu_time.ValCur-1;
    if (Target < 0) {
        Target = menu_time.ValNum-1;
    }
    menu_time.Func(MENU_TIME_SWITCH_TO_VAL, Target, 0);
    return 0;
}

static int menu_time_button_right(void)
{
int Target = menu_time.ValCur+1;
    if (Target == menu_time.ValNum) {
        Target = 0;
    }
    menu_time.Func(MENU_TIME_SWITCH_TO_VAL, Target, 0);
    return 0;

#if 0
    int Target = menu_time.ValCur+1;
    if (Target == menu_time.ValNum) {
        Target = 0;
    }
    menu_time.Func(MENU_TIME_SWITCH_TO_VAL, Target, 0);
#endif
    return 0;
}

static int menu_time_button_set(void)
{
    menu_time.Func(MENU_TIME_CUR_VAL_SET, 0, 0);
    return AppWidget_Off(WIDGET_MENU_TIME, 0);
}

static int menu_time_button_menu(void)
{
    
#ifdef CONFIG_BSP_ORTHRUS
    menu_time.Func(MENU_TIME_CUR_VAL_SET, 0, 0);
    return AppWidget_Off(WIDGET_MENU_TIME, 0);
#else
    return AppWidget_Off(WIDGET_MENU_TIME, 0);
#endif
}

static int app_menu_time_on(UINT32 param)
{
    int ReturnValue = 0;
    int i = 0;

    menu_time.Func = menu_time_func;
    menu_time.Gui = gui_menu_time_func;
    menu_time.Op = &menu_time_op;

    menu_time.ValNum = TIME_VALUE_NUM;

    /** Show time menu frame */
    menu_time.Gui(GUI_TMENU_UPDATE, 0, 0);	
    menu_time.Gui(GUI_TMENU_SHOW, 0, 0);
    /** Selection content should be loaded */
    {
        AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
        ReturnValue = AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Menu Time] AmbaRTC_GetSystemTime failure. ReturnValue = %d", ReturnValue);
        }
        menu_time.Year = TimeSpec.Calendar.Year;
        menu_time.Month = TimeSpec.Calendar.Month;
        menu_time.Day = TimeSpec.Calendar.Day;
        menu_time.Hour = TimeSpec.Calendar.Hour;
        menu_time.Minute = TimeSpec.Calendar.Minute;
        menu_time.Second = TimeSpec.Calendar.Second;
    }
    /** Show adjustment value */
    menu_time.Gui(GUI_VALUE_UPDATE, GUI_YEAR, menu_time.Year);
    menu_time.Gui(GUI_VALUE_UPDATE, GUI_MONTH, menu_time.Month);
    menu_time.Gui(GUI_VALUE_UPDATE, GUI_DAY, menu_time.Day);
    menu_time.Gui(GUI_VALUE_UPDATE, GUI_HOUR, menu_time.Hour);
    menu_time.Gui(GUI_VALUE_UPDATE, GUI_MINUTE, menu_time.Minute);
    menu_time.Gui(GUI_VALUE_UPDATE, GUI_SECOND, menu_time.Second);
    for (i=0; i<GUI_VALUE_NUM; i++) {
        menu_time.Gui(GUI_VALUE_SHOW, i, 0);
    }

    menu_time.Gui(GUI_VALUE_HL, menu_time.ValCur, 0);

    /** Flush GUI */
    menu_time.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int app_menu_time_off(UINT32 param)
{
    int i = 0;
    menu_time.Gui(GUI_TMENU_HIDE, 0, 0);
    for (i=0; i<GUI_VALUE_NUM; i++) {
        menu_time.Gui(GUI_VALUE_HIDE, i, 0);
    }
    menu_time.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int app_menu_time_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        ReturnValue = menu_time.Op->ButtonUp();
        break;
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        ReturnValue = menu_time.Op->ButtonDown();
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        ReturnValue = menu_time.Op->ButtonLeft();
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        ReturnValue = menu_time.Op->ButtonRight();
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        ReturnValue = menu_time.Op->ButtonSet();
        break;
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = menu_time.Op->ButtonMenu();
        break;
    default:
        ReturnValue = WIDGET_PASSED_MSG;
        break;
    }

    return ReturnValue;
}

/*************************************************************************
 * Time Menu APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppMenuTime_GetWidget(void)
{
    return &widget_menu_time;
}

/*************************************************************************
 * Public Time Menu Widget APIs
 ************************************************************************/
int AppMenuTime_SetItem(UINT32 tabId, UINT32 itemId)
{
    menu_time.Item = AppMenu_GetItem(tabId, itemId);
    return 0;
}
