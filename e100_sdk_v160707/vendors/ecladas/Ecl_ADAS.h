#ifndef __ECL_FUN_ADAS__
#define __ECL_FUN_ADAS__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <recorder/Encode.h>


#define _RGB(r,g,b)			(((U32)b<<16) | ((U32)g<<8) | r)

#ifndef U8
typedef unsigned char  	U8;      
#endif

#ifndef S8           
typedef signed   char  		S8; 
#endif

#ifndef U16              
typedef unsigned short 	U16;    
#endif

#ifndef S16      
typedef signed   short 		S16;   
#endif

#ifndef U32            
typedef unsigned long  	U32;
#endif

#ifndef S32                  
typedef signed   long  		S32;  
#endif
#ifndef U64
typedef unsigned long long	U64;
#endif

#ifndef S64
typedef signed long long	S64;
#endif

#define ADAS_PROC_GPS_BASED       0x01
#define ADAS_PROC_NO_SPEED_LIMIT  0x02

#define ECL_LDW_LAN_NUM					2
#define ECL_FCW_OBJ_NUM					5

typedef struct _K_POINT_UNI
{
	float x;
    float y;
} K_POINT_UNI;

typedef struct _K_RECT_UNI_F
{
	float x;
	float y;
	float w;
	float h;
} K_RECT_UNI;

typedef struct _K_RECT_INT
{
	int x;
	int y;
	int w;
	int h;
} K_RECT_INT;


typedef struct _K_POINT_INT
{
	int x;
    int y;
} K_POINT_INT;

typedef struct tagADAS_INFO
{
	int speed;
	int lose;
	int v_num;
	int init;
	int ldws_mode_onoff;
	int fcws_mode_onoff;
	int hmws_mode_onoff;
	int fcmrs_mode_onoff;
	int adas_sensitivity_level;
	float radar_offset;
	float radar_mode_enable;
} ADAS_INFO;

typedef struct _ADAS_FCW_INFO_UNI_s
{
	int           FcwActive; 
	unsigned char ObjectVal[ECL_FCW_OBJ_NUM];
    K_RECT_UNI    ObjectRect[ECL_FCW_OBJ_NUM];
    unsigned char ObjectWarring[ECL_FCW_OBJ_NUM];
    unsigned char ObjectNeedCare[ECL_FCW_OBJ_NUM];
    unsigned int  ObjectDistance[ECL_FCW_OBJ_NUM];
}ADAS_FCW_INFO_UNI_s;

typedef struct _ADAS_LDW_INFO_UNI_s
{
    int                IsDetected;
    unsigned char      LwdWarringFlg;
    K_POINT_UNI        Points[32];
    int                PointsCount;
} ADAS_LDW_INFO_UNI_s;

typedef struct _ADAS_LDW_OUTPUT_s_
{
    ADAS_LDW_INFO_UNI_s LanNum0;
    ADAS_LDW_INFO_UNI_s LanNum1;
} ADAS_LDW_DETECT_OUTPUT_s;

typedef enum
{
	MSG_OBJ_FRONT_LEFT=0,
	MSG_OBJ_FRONT_MIDDLE,
	MSG_OBJ_FRONT_RIGHT,
	MSG_OBJ_BACK_LEFT,
	MSG_OBJ_BACK_MIDDLE,
	MSG_OBJ_BACK_RIGHT,
	MSG_LAN_LEFT,
	MSG_LAN_RIGHT,
	MSG_ADAS_FRONT_MONITORING,

	MSG_ADAS_NULL,
	MSG_ADAS_MSG_NUM,
} ECL_ADAS_MSG_E;

typedef struct
{
    unsigned short s;//distance
    short v;//speed
    short h;//hor distance
    unsigned short p;//power
    unsigned short r;
}ECL_RADAR_OBJ2;


typedef struct 
{
	#define RADAR_MAX_OBJ_NUM  8
    // K_ADC_PACKAGE     radar_data_info;
    short             abs_speed;// m/s
    unsigned short    obj_num;
    ECL_RADAR_OBJ2       objs[RADAR_MAX_OBJ_NUM];
}ECL_RADAR_RESULT;

extern int Ecl_ADAS_CombinWithRadar(ECL_RADAR_RESULT *pRadar ,float *diff_ph);

typedef void (*ECL_ADAS_EVENT_CALLBACK)(ECL_ADAS_MSG_E msg, int t);

K_RECT_INT k_Rect_int(int x, int y, int w, int h);

extern void Ecl_ADAS_Set_Speed(int speed);//car speed in km/h
extern void Ecl_Adas_Set_Proc_Mode(unsigned char Adas_Mode);
extern unsigned char Ecl_Adas_Get_Proc_Mode(void);

extern int ECL_Video_ADAS_Set_Paramentes(K_RECT_INT rect,int v_ratio);

extern int ECL_Video_ADAS_Proc(unsigned char *pImgMem);

extern int Ecl_ADAS_FCWD_SetEventCallback(ECL_ADAS_EVENT_CALLBACK pCallbakcFun);

extern int Ecl_ADAS_Lane_SetEventCallback(ECL_ADAS_EVENT_CALLBACK pCallbakcFun);

extern void Ecl_ADAS_Set_TTC_Limit_t(int t_ms);

extern void ECL_Adas_GetLdwOutput(ADAS_LDW_DETECT_OUTPUT_s* pLaneOut);

extern void ECL_Adas_GetFcwOutput(ADAS_FCW_INFO_UNI_s* pFcwOut);

//===================ECL Draw api For Test===============================================================================
extern void Ecl_dc_draw_init(void);

extern int Ecl_dc_draw_get_init_state(void);//0->DC hasn't been initialized ;1->Dc has been initialized

extern void Ecl_DrawUsePen(U8 size, U32 Color);

extern void Ecl_DrawText(U16 x, U16 y, char *pStr,U32 Color);

extern void Ecl_DrawLine(K_POINT_INT *pPoints, U16 num);

extern int Ecl_draw_dc_create(int draw_w,int draw_h,U32 DrawAddr);

//==============================Radar process ============================================================================
extern int Calc_fcws_ttc(int abs_v, int ref_v, int range);

#endif //__ECL_FUN_ADAS__

