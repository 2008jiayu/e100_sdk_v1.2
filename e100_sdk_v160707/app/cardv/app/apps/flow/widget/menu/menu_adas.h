#ifndef APP_WIDGET_MENU_ADAS_H_
#define APP_WIDGET_MENU_ADAS_H_


__BEGIN_C_PROTO__
/*************************************************************************
 * Adas menu definitions
 ************************************************************************/
typedef enum _MENU_ADAS_ITEM_ID_e_ {
    MENU_ADAS_BEFORE_ALARM = 0,
    MENU_ADAS_ORBIT_WARNING,
    MENU_ADAS_SET,    
    MENU_ADAS_ITEM_NUM
} MENU_ADAS_ITEM_ID_e;

typedef enum _MENU_ADAS_SEL_ID_e_ {
    MENU_ADAS_OFF = 0,
    MENU_ADAS_ON,
    MENU_ADAS_SEL_NUM
} MENU_VIDEO_ADAS_SEL_ID_e;

typedef enum _MENU_ADAS_ORBIT_WARNING_SEL_ID_e_ {
    MENU_ADAS_ORBIT_WARNING_ON = 0,
    MENU_ADAS_ORBIT_WARNING_OFF,
    MENU_ADAS_ORBIT_WARNING_SEL_NUM
} MENU_ADAS_ORBIT_WARNING_SEL_ID_e;

typedef enum _MENU_ADAS_BEFORE_ALARM_SEL_ID_e_ {
    MENU_ADAS_BEFORE_ALARM_OFF = 0,
    MENU_ADAS_BEFORE_ALARM_NEAR,
    MENU_ADAS_BEFORE_ALARM_MIDDLE,
    MENU_ADAS_BEFORE_ALARM_FAR,    
    MENU_ADAS_BEFORE_ALARM_SEL_NUM
} MENU_ADAS_BEFORE_ALARM_SEL_ID_e;

__END_C_PROTO__
#endif
