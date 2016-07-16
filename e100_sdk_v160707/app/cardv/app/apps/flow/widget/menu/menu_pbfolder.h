#ifndef APP_WIDGET_MENU_ADAS_H_
#define APP_WIDGET_MENU_ADAS_H_


__BEGIN_C_PROTO__
/*************************************************************************
 * Adas menu definitions
 ************************************************************************/
typedef enum _MENU_ADAS_ITEM_ID_e_ {
    MENU_PB_FOLDER_ITEM_AUTO = 0,
    MENU_PB_FOLDER_ITEM_EVENT,
    MENU_PB_FOLDER_ITEM_PHOTO,
    MENU_PB_FOLDER_ITEM_NUM
} MENU_ADAS_ITEM_ID_e;

typedef enum _MENU_PB_FOLDER_ID_e_ {
    MENU_PB_FOLDER_AUTO = 0,
    MENU_PB_FOLDER_EVENT,
    MENU_PB_FOLDER_PHOTO,
    MENU_PB_FOLDER_NUM
} MENU_PB_FLDER_ID_e;

__END_C_PROTO__
#endif
