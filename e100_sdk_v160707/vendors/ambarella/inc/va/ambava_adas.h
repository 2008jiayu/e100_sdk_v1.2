/**
 * vendors/ambarella/inc/va/ambava_adas.h
 *
 * Header of AMBA ADAS interface.
 *
 * Copyright (C) 2004-2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __AMBAVA_ADAS_H__
#define __AMBAVA_ADAS_H__

#include "ambava.h"
#include "ambava_adas_videoData.h"
#include "ambava_adas_laneMarkType.h"

#define DEFAULT_HORIZLEVEL (45)
#define DEFAULT_HOODLEVEL (80)

/*************************************************************************
 * ADAS declaration
 ************************************************************************/
/** Information from GPS module */
typedef struct _AMBA_ADAS_GPS_INFO_s_ {
    float Speed;                 /**< Vehicle speed in km/h */
    float Bearing;               /**< Vehicle bearing (direction of travel) 0-360 degrees, starting clockwise from true north */
} AMBA_ADAS_GPS_INFO_s;

/** Information from vehicle board system */
typedef struct _AMBA_ADAS_DASHBOARD_INFO_s_ {
    int LeftTurnSignal;  /**< 1 if On, 0 if Off, -1 if Unknown */
    int RightTurnSignal; /**< 1 if On, 0 if Off, -1 if Unknown */
} AMBA_ADAS_DASHBOARD_INFO_s;

/** Auxilary information from GPS module and vehicle board system */
typedef struct _AMBA_ADAS_AUX_DATA_s_ {
    AMBA_ADAS_GPS_INFO_s*       pGpsInfo;       /**< set to null if not available currently */
    AMBA_ADAS_DASHBOARD_INFO_s* pDashBoardInfo; /**< set to null if not available currently */
} AMBA_ADAS_AUX_DATA_s;

/** Options to adjust sensitivity of events, ex: lane departure event */
typedef enum _AMBA_ADAS_SENSITIVITY_LEVEL_e_ {
    ADAS_SL_LOW     = 0,
    ADAS_SL_MEDIUM = 1,
    ADAS_SL_HIGH    = 2
}AMBA_ADAS_SENSITIVITY_LEVEL_e;

/** Options to adjust frontal collision warning rate */
typedef enum _AMBA_ADAA_FCW_WARNING_LEVEL_e_{
    ADAS_FWL_NONE   = 0,
    ADAS_FWL_LOW    = 1,
    ADAS_FWL_MEDIUM = 2,
    ADAS_FWL_HIGH   = 3
}AMBA_ADAA_FCW_WARNING_LEVEL_e;

/** Options to estimate camera height above the road */
typedef enum _AMBA_ADAS_CAMERA_MOUNT_HEIGHT_e_ {
    ADAS_CMTGOLFCLASS  = 0,
    ADAS_CMTCROSSOVER  = 1,
    ADAS_CMTBUSORTRUCK = 2
}AMBA_ADAS_CAMERA_MOUNT_HEIGHT_e;

/** External parameters of scene: positions of horizon and car hood in frame */
typedef struct _AMBA_ADAS_SCENE_PARAMS_s_ {
    int   AutomaticCalibration;  /**< Boolean flag to enable automatic horizon, hood and pan detection.
                                      If set to 1 - ADAS will extract calibration parameters from
                                      input video. In this case hoodLevel, horizonLevel and horizontalPan
                                      are not used; autoCalibrationActive can be used to temporarily disable
                                      autocalibration by setting autoCalibrationActive=0.
                                      If set to 0 - ADAS will run in manual calibration mode which
                                      requires hoodLevel, horizonLevel and horizontalPan to be set. */
    int   AutoCalibrationActive; /**< To be used if automaticCalibration=1.
                                      Set autoCalibrationActive=1 to enable autocalibration processing;
                                      set autoCalibrationActive=0 to disable autocalibration processing.
                                      If autoCalibrationActive=0, detected calibration parameters will
                                      not be updated over time. Temporarily setting autoCalibrationActive=0
                                      can reduce CPU consumed by ADAS during critical periods of time. */
    int   AutoCalibCpuReduction; /**< To be used if automaticCalibration=1. Range 1-4.
                                      If set to 1 (default), coninuous autocalibration will be as fast as possible.
                                      If set to 2 - continuous autocalib will be 2 times slower but will take 2 times less CPU.
                                      If set to 3 - continuous autocalib will be 3 times slower but will take 3 times less CPU.
                                      If set to 4 - continuous autocalib will be 4 times slower but will take 4 times less CPU. */
    float HoodLevel;             /**< Hood position which must be manually set if automaticCalibration=0.
                                      As % of image height, range is 0-100, topmost row being 0 */
    float HorizonLevel;          /**< Horizon position which must be manually set if automaticCalibration=0.
                                      As % of image height, range is 0-100, topmost row being 0 */
    float HorizontalPan;         /**< Pan position which must be manually set if automaticCalibration=0.
                                      Leave default 50 that means center of frame.
                                      As % of image width, range is 0-100, leftmost column being 0 */
    AMBA_ADAS_CAMERA_MOUNT_HEIGHT_e CameraMountHeight; /**< Camera mount height: sedan or crossover or truck.
                                                Must be always provided by user irregardless of
                                                automaticCalibration flag. */
} AMBA_ADAS_SCENE_PARAMS_s;

/** Parameters of lane departure warning */
typedef struct _AMBA_ADAS_LDW_PARAMS_s_
{
    int                    IsEnabled;             /**< Enable LDWS */
    AMBA_ADAS_SENSITIVITY_LEVEL_e LaneDetectSensitivity; /**< Lane detection sensitivity.
                                                       Set it higher for low contrast video */
    int                    bSeparateLeftRightDepartureSens; /**< If set to 1, separate ldwLeftSensitivity and ldwRightSensitivity
                                                                  are used for left and right departures */
    AMBA_ADAS_SENSITIVITY_LEVEL_e LdwSensitivity;        /**< Departure sensitivity for b_separateLeftRightDepartureSens==0 */
    AMBA_ADAS_SENSITIVITY_LEVEL_e LdwLeftSensitivity;    /**< Departure sensitivities for b_separateLeftRightDepartureSens==1 */
    AMBA_ADAS_SENSITIVITY_LEVEL_e LdwRightSensitivity;
    float                  MinLdwsSpeedKmph;      /**< Minimal car speed to generate departure warnings (km/h). Range 10-200 */
} AMBA_ADAS_LDW_PARAMS_s;

/** Parameters of frontal collision warning */
typedef struct _AMBA_ADAS_FCW_PARAMS_s_
{
    int                    IsEnabled;                /**< Enable FCWS */
    AMBA_ADAS_SENSITIVITY_LEVEL_e VehicleDetectSensitivity; /**< Sensitivity of car detection */
    AMBA_ADAS_SENSITIVITY_LEVEL_e FcwSensitivity;           /**< Sensitivity of collision warning */
    float                  HeadWayMoveOutFactor;   /**< Factor multiplied to headway zone to set move out zone for TTC2 alarm */
    float                  MinReapproachAlarmTimeGap;  /**< Minimum gap (in seconds) between alarms from re-approach condition */
} AMBA_ADAS_FCW_PARAMS_s;

/** Description of a single detected road line */
typedef struct _AMBA_ADAS_RoadMarkLine_s_
{
    int                IsDetected;
    AMBA_ADAS_LANEMARKTYPE MarkType;
    AMBA_VA_POINT_s*       p_Points;
    int                PointsCount;
} AMBA_ADAS_RoadMarkLine_s;

/** Direction of lane departure: towards left or right */
typedef enum
{
    AMBA_ADAS_DdTowardsLeft = 0,
    AMBA_ADAS_DdTowardsRight = 1
} AMBA_ADAS_DEPARTUREDIRECTION;

/** Lane departure event properties */
typedef struct _AMBA_ADAS_LDW_OUTPUTEVENT_s_
{
    AMBA_ADAS_DEPARTUREDIRECTION Direction; /**< Departure towards left or right */
    AMBA_ADAS_LANEMARKTYPE MarkType;        /**< Type of line through which the departure occurs */
} AMBA_ADAS_LDW_OUTPUTEVENT_s;

/** Frontal collision event properties */
typedef struct _AMBA_ADAS_FCW_OUTPUTEVENT_s_
{
    int unused; /**< Unused */
} AMBA_ADAS_FCW_OUTPUTEVENT_s;

/** Scene status */
typedef struct _AMBA_ADAS_SCENE_STATUS_s_ {
    int   IsCalibrationDetected;   /**< Whether initial automatic calibration is finished */
    /* The following 3 values can be used to monitor/display positions of horizon and hood in */
    /* both manual and auto calibration modes. horizontalPan describes where the camera is directed: to left, right or center.*/
    /* For example default horizontalPan in manual mode is 50 which means center on the frame. */
    /* If camera pan towards left then horizontalPan will be higher than 50 (ex. 65) */
    float HoodLevel;        /**< Current value of hood Y position (% of image height, range 0-100, topmost row being 0) */
    float HorizonLevel;     /**< Current value of horizon Y position (% of image height, range 0-100, topmost row being 0) */
    float HorizontalPan;    /**< Current value of pan (i.e. horizon X) (% of image width, range 0-100, leftmost column being 0) */
    void* p_debugInfo;
} AMBA_ADAS_SCENE_STATUS_s;

/** Couple of road lines that correspond to the current lane */
typedef struct _AMVA_ADAS_LDW_OUTPUT_s_
{
    AMBA_ADAS_RoadMarkLine_s LeftLine;
    AMBA_ADAS_RoadMarkLine_s RightLine;
} AMVA_ADAS_LDW_OUTPUT_s;

/** Information from the frontal collision detector */
typedef struct _AMBA_ADAS_FCW_OUTPUT_s_
{
    int                   FcwActive;               /**< 0 if suppressed, for example if speed is too slow */
    AMBA_VA_POINT_s*          p_FrontVehiclePoints;    /**< Position of front car. NULL if not available */
    int                   FrontVehiclePointsCount; /**< Number of points in p_frontVehiclePoints array. 0 if not available */
    float                 FrontVehicleDistance;    /**< Distance to front vehicle (meters), -1.0f if not available */
    float                 FrontVehicleTtc;         /**< Time to collision (seconds), -1.0f if not available */
    AMBA_ADAA_FCW_WARNING_LEVEL_e WarningLevel;
    void*                 p_DebugInfo;
} AMBA_ADAS_FCW_OUTPUT_s;

typedef struct _AMBA_ADAS_VIEWANGLE_s_
{
    float HorizAngle;
    float VertAngle;
} AMBA_ADAS_VIEWANGLE_s;

/** adas output event pointer set */
typedef struct _AMBA_ADAS_OUTPUTEVENT_s_
{
    AMBA_ADAS_LDW_OUTPUTEVENT_s* LdwEvent;
    AMBA_ADAS_FCW_OUTPUTEVENT_s* FcwEvent;
    AMBA_ADAS_SCENE_STATUS_s* SceneStatus;
} AMBA_ADAS_OUTPUTEVENT_s;

/** Options to estimate camera height above the road */
typedef enum _AMBA_ADAS_LIGHTING_LEVEL_s_ {
    ADAS_LightLow   = 0,
    ADAS_LightOk    = 1,
}AMBA_ADAS_LIGHTING_LEVEL_s;

typedef enum {
    MD_NO_MOTION = 0,
    MD_IN_MOTION
} AMBA_MOTION_FLAGS_e; ///< Motion indicators

typedef enum {
    MD_MOTION_NO_EVENT = 0,
    MD_MOTION_START,
    MD_MOTION_END
} AMBA_MOTION_EVENT_e; ///< Motion events

/* API*/

/** adas initial
 * @param Img yuv info
 * @param ScenePar input scene params pointer.
 * @param ViewAngle input ViewAngle params pointer.
 * @return Status
 */
int Amba_Adas_Init(AMP_ENC_YUV_INFO_s* Img, AMBA_ADAS_SCENE_PARAMS_s* ScenePar, AMBA_ADAS_VIEWANGLE_s* ViewAngle);

/** adas processing in one frame
 * @param Img yuv info
 * @param AuxData cars system params ex gps or speed
 * @param OutEvent adas output event pointer set
 * @param ts adas inner time stamp
 * @return Status
 */
int Amba_Adas_Proc(AMP_ENC_YUV_INFO_s* Img, AMBA_ADAS_AUX_DATA_s* AuxData, AMBA_ADAS_OUTPUTEVENT_s* OutEvent, int ts);

/** Set parameters of scene
 * @param params Structure with scene parameters to apply. Can be first filled with ivAdas_GetSceneParams()
 * @return Status
 */
int Amba_Adas_SetSceneParams(AMBA_ADAS_SCENE_PARAMS_s* Src);

/** Set lane departure warning parameters
 * @param params Structure with LDWS parameters to apply. Can be first filled with ivAdas_GetLdwParams()
 * @return Status
 */
int Amba_Adas_SetLdwParams( AMBA_ADAS_LDW_PARAMS_s params);

/** Set frontal collision warning parameters
 * @param params Structure with FCWS parameters to apply. Can be first filled with ivAdas_GetFcwParams()
 * @return Status
 */
int Amba_Adas_SetFcwParams( AMBA_ADAS_FCW_PARAMS_s params);

/** Get parameters of scene
 * @param p_params Current scene parameters
 * @return Status
 */
void Amba_Adas_GetSceneParams(AMBA_ADAS_SCENE_PARAMS_s* Des);

/** Get lane departure warning parameters
 * @param p_params Current LDWS parameters
 * @return Status
 */
int Amba_Adas_GetLdwParams( AMBA_ADAS_LDW_PARAMS_s* p_params);

/** Get frontal collision warning parameters
 * @param p_params Current FCWS parameters
 * @return Status
 */
int Amba_Adas_GetFcwParams( AMBA_ADAS_FCW_PARAMS_s* p_params);

/** Get scene characteristics
 * @param pp_sceneStatus Pointer to scene status info. Must not be freed.
 * @return Status
 */
int Amba_Adas_GetSceneStatus( AMBA_ADAS_SCENE_STATUS_s** pp_sceneStatus);

/** Get additional information from lane departure system
 * @param pp_ldwOutput Pointer all information returned by LDWS except LDW event. Must not be freed.
 * @return Status
 */
int Amba_Adas_GetLdwOutput( AMVA_ADAS_LDW_OUTPUT_s** pp_ldwOutput);

/** Get additional information from frontal collision system
 * @param pp_fcwOutput Pointer all information returned by FCWS except FCW event. Must not be freed.
 * @return Status
 */
int Amba_Adas_GetFcwOutput( AMBA_ADAS_FCW_OUTPUT_s** pp_fcwOutput);

#endif /* __APPLIB_ADAS_H__ */

