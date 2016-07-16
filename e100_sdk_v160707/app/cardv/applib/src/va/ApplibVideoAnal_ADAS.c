/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_LDFCWS.c
 *
 * Implementation of VA Frontal Car Moving Depature(FCMD) APIs
 *
 * History:
 *    2015/01/06 - [Bill Chou] created file
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

#include "va/ApplibVideoAnal_ADAS.h"
#include "va/ApplibVideoAnal_StmpHdlr.h"
#include <va/ambava_adas.h>
#include <gps.h>

#ifdef CONFIG_APP_ARD
#include "../../../cardv/app/apps/gui/resource/gui_resource.h"
#include "../../../cardv/app/apps/gui/resource/gui_table.h"

extern UINT32 AppPref_GetLanguageID(VOID);

/*************************************************************************
 * GUI layout settings
 ************************************************************************/
#define OSD_FCHAN_WIDTH     (960)
#define OSD_FCHAN_HEIGHT    (540)
#define OSD_DCHAN_WIDTH     (960)
#define OSD_DCHAN_HEIGHT    (480)
#define OSD_BLEND_WIDTH     (960)
#define OSD_BLEND_HEIGHT    (480)

typedef struct cardvws_s
{
    int laneValid[2]; // [0:left, 1:right]
    int lane[2][2][2]; // [0:left, 1:right][0:top, 1:bottom][0:x, 1:y]
    int vehicleValid;
    int vehicle[2][2]; // [0:left, 1:right][0:x, 1:y]
    int calibrationValid;
    int hood, horizon, pan;
} cardvws_t;
int AppLibVideoAnal_ADAS_Gui_Show(cardvws_t *cardvws_res);
int AppLibVideoAnal_ADAS_Draw_Rect(int _l, int _t, int _r, int _b, AMP_DISP_INFO_s ddev_info, int valid, int rect_num, u32 color);
void AppLibVideoAnal_ADAS_Gui_Show_timer_handler(int eid);
void AppLibVideoAnal_ADAS_Gui_Hide(void);

static cardvws_t g_cardvws;
#define OVERLAY_WIDTH 4096
#define OVERLAY_HEIGHT 4096
#endif

static int ApplibAdasInit = 0;
static int ApplibAdasIsCalibrated = 0;
static gps_data_t* ApplibAdasGpsData = NULL;
static UINT8 ApplibAdasYuvSrc = 0;

void AppLibVideoAnal_ADAS_GetDef_Setting(APPLIB_LDWS_CFG_t* pLdwsConfig, APPLIB_FCWS_CFG_t* pFcwsConfig, APPLIB_ADAS_PAR_t* pParams)
{
    pLdwsConfig->LDWSSensitivity    = ADAS_SL_MEDIUM;
    pFcwsConfig->FCWSSensitivity    = ADAS_SL_MEDIUM;
    pParams->HoodLevel              = DEFAULT_HOODLEVEL;
    pParams->HorizonLevel           = DEFAULT_HORIZLEVEL;
}

int AppLibVideoAnal_ADAS_Init(UINT8 yuvSrc, APPLIB_LDWS_CFG_t ldwsConfig, APPLIB_FCWS_CFG_t fcwsConfig, APPLIB_ADAS_PAR_t params)
{
    int ReturnValue = 0;
    AMP_ENC_YUV_INFO_s Img[1] = {0};
    int FrmSizeIsChange = 0;
    AMBA_ADAS_SCENE_PARAMS_s SceneParam = {0};
    AMBA_ADAS_LDW_PARAMS_s LdwsParam = {0};
    AMBA_ADAS_FCW_PARAMS_s FcwsParam = {0};
    AMBA_ADAS_VIEWANGLE_s ViewAngle = {0};

    ApplibAdasYuvSrc = yuvSrc;

    if ( AppLibVideoAnal_FrmHdlr_GetFrmInfo(ApplibAdasYuvSrc, Img, &FrmSizeIsChange) != OK){
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init");
        return -1;
    }
    if (ApplibAdasInit) {
        AmbaPrint("LDWS & FCWS are already init");
        return -1;
    }

    ViewAngle.HorizAngle = 95;
    ViewAngle.VertAngle = 55;

    /** Set LDWS parameter */
    Amba_Adas_GetLdwParams(&LdwsParam);

    LdwsParam.LdwSensitivity = ldwsConfig.LDWSSensitivity;

    Amba_Adas_SetLdwParams(LdwsParam);

    /** Set FCWS parameter */
    Amba_Adas_GetFcwParams(&FcwsParam);

    FcwsParam.FcwSensitivity = fcwsConfig.FCWSSensitivity;

    Amba_Adas_SetFcwParams(FcwsParam);

    /** Set Scene parameter */
    Amba_Adas_GetSceneParams(&SceneParam);

    SceneParam.HoodLevel = params.HoodLevel;
    SceneParam.HorizonLevel = params.HorizonLevel;

    AmbaPrint("Adas init size (%d,%d)", Img[0].height, Img[0].width);
    ReturnValue = Amba_Adas_Init(Img, &SceneParam, &ViewAngle);
    ApplibAdasInit = 1;

    return ReturnValue;
}

int AppLibVideoAnal_ADAS_Process(UINT32 event, AMP_ENC_YUV_INFO_s* img)
{
    int ReturnValue = 0;

    unsigned int ts = (unsigned int) AmbaTimer_GetSysTickCount();
    AMBA_ADAS_OUTPUTEVENT_s OutputEven = {0};
    AMBA_ADAS_GPS_INFO_s GpsInfo = {0};
    AMBA_ADAS_AUX_DATA_s AuxData = {0};
#ifdef CONFIG_APP_ARD
#ifdef APP_GPS_VALID
    ApplibAdasGpsData = AppLibSysGps_GetData();

    if (ApplibAdasGpsData->status == 0) {
        return ReturnValue;
    } else {
        GpsInfo.Speed = ApplibAdasGpsData->fix.speed*MPS_TO_KMPH;
        GpsInfo.Bearing = ApplibAdasGpsData->fix.track;
    }
#else
    GpsInfo.Speed = 100*MPS_TO_KMPH;
    GpsInfo.Bearing = 0;
#endif
#else
    ApplibAdasGpsData = AppLibSysGps_GetData();

    if (ApplibAdasGpsData->status == 0) {
        return ReturnValue;
    } else {
        GpsInfo.Speed = ApplibAdasGpsData->fix.speed*MPS_TO_KMPH;
        GpsInfo.Bearing = ApplibAdasGpsData->fix.track;
    }
#endif

    AuxData.pGpsInfo = &GpsInfo;

    ReturnValue = Amba_Adas_Proc(img, &AuxData, &OutputEven, ts);

    if (0 == ApplibAdasIsCalibrated) {
        AMBA_ADAS_SCENE_STATUS_s* pSsceneStatus = NULL;
        Amba_Adas_GetSceneStatus(&pSsceneStatus);
        if (pSsceneStatus->IsCalibrationDetected) {
            ApplibAdasIsCalibrated = 1;
            AppLibVideoAnal_StmpHdlr_AddEvent(VA_STMPHDLR_CALIB);
            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_CLIBRATION_DONE, 0, 0);
        }
    }

    if (OutputEven.LdwEvent != NULL) {
        AppLibVideoAnal_StmpHdlr_AddEvent(VA_STMPHDLR_LDWS);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_LDW, 0, 0);
        AmbaPrintColor(RED, "Departure: %s \n",(OutputEven.LdwEvent->Direction == AMBA_ADAS_DdTowardsLeft) ? "left" : "right");
        //AmbaKAL_TaskSleep(3000);
    }

    if (OutputEven.FcwEvent != NULL) {
        AppLibVideoAnal_StmpHdlr_AddEvent(VA_STMPHDLR_FCWS);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_FCW, 0, 0);
        AmbaPrintColor(RED, "Frontal Collision Warning \n");
        //AmbaKAL_TaskSleep(3000);
    }
#ifdef CONFIG_APP_ARD
    AMBA_ADAS_SCENE_STATUS_s* pSsceneStatus = NULL;
    Amba_Adas_GetSceneStatus(&pSsceneStatus);

    g_cardvws.calibrationValid = pSsceneStatus->IsCalibrationDetected;
    g_cardvws.hood = (int) (OVERLAY_HEIGHT * pSsceneStatus->HoodLevel/ 100);
    g_cardvws.horizon = (int) (OVERLAY_HEIGHT * pSsceneStatus->HorizonLevel/ 100);
    g_cardvws.pan = (int) (OVERLAY_WIDTH * pSsceneStatus->HorizontalPan/ 100);

    AMVA_ADAS_LDW_OUTPUT_s* pSLdwOutput = NULL;
    Amba_Adas_GetLdwOutput(&pSLdwOutput);

    g_cardvws.laneValid[0] = 0;
    g_cardvws.laneValid[1] = 0;

    if (pSLdwOutput) {
        if (pSLdwOutput->LeftLine.IsDetected && pSLdwOutput->LeftLine.PointsCount >= 2) {
            g_cardvws.laneValid[0] = 1;
            g_cardvws.lane[0][0][0] = OVERLAY_WIDTH * pSLdwOutput->LeftLine.p_Points[0].X / 100;
            g_cardvws.lane[0][0][1] = OVERLAY_HEIGHT * pSLdwOutput->LeftLine.p_Points[0].Y / 100;
            g_cardvws.lane[0][1][0] = OVERLAY_WIDTH * pSLdwOutput->LeftLine.p_Points[1].X / 100;
            g_cardvws.lane[0][1][1] = OVERLAY_HEIGHT * pSLdwOutput->LeftLine.p_Points[1].Y / 100;
        }

        if (pSLdwOutput->RightLine.IsDetected && pSLdwOutput->RightLine.PointsCount >= 2) {
            g_cardvws.laneValid[0] = 1;
            g_cardvws.lane[0][0][0] = OVERLAY_WIDTH * pSLdwOutput->RightLine.p_Points[0].X / 100;
            g_cardvws.lane[0][0][1] = OVERLAY_HEIGHT * pSLdwOutput->RightLine.p_Points[0].Y / 100;
            g_cardvws.lane[0][1][0] = OVERLAY_WIDTH * pSLdwOutput->RightLine.p_Points[1].X / 100;
            g_cardvws.lane[0][1][1] = OVERLAY_HEIGHT * pSLdwOutput->RightLine.p_Points[1].Y / 100;
        }
    }

    AMBA_ADAS_FCW_OUTPUT_s* pSFcwOutput = NULL;
    Amba_Adas_GetFcwOutput(&pSFcwOutput);

    g_cardvws.vehicleValid = (pSFcwOutput && pSFcwOutput->FrontVehiclePointsCount >= 2);
    if (g_cardvws.vehicleValid) {
        int x0 = pSFcwOutput->p_FrontVehiclePoints[0].X;
        int y0 = pSFcwOutput->p_FrontVehiclePoints[0].Y;
        int x1 = pSFcwOutput->p_FrontVehiclePoints[1].X;
        int y1 = pSFcwOutput->p_FrontVehiclePoints[1].Y;
        if (y0 == y1) y0 -= labs(x0-x1);
        g_cardvws.vehicle[0][0] = OVERLAY_WIDTH * x0 / 100;
        g_cardvws.vehicle[0][1] = OVERLAY_HEIGHT * y0 / 100;
        g_cardvws.vehicle[1][0] = OVERLAY_WIDTH * x1 / 100;
        g_cardvws.vehicle[1][1] = OVERLAY_HEIGHT * y1 / 100;
    }
#endif

    return ReturnValue;
}

int AppLibVideoAnal_ADAS_LDWS_SetCfg( APPLIB_LDWS_CFG_t* pParams)
{
    int ReturnValue = 0;
    AMBA_ADAS_LDW_PARAMS_s cfg = {0};

    Amba_Adas_GetLdwParams(&cfg);

    cfg.LdwSensitivity = pParams->LDWSSensitivity;

    Amba_Adas_SetLdwParams(cfg);

    return ReturnValue;
}

int AppLibVideoAnal_ADAS_FCWS_SetCfg( APPLIB_FCWS_CFG_t* pParams)
{
    int ReturnValue = 0;
    AMBA_ADAS_FCW_PARAMS_s cfg = {0};

    Amba_Adas_GetFcwParams(&cfg);

    cfg.FcwSensitivity = pParams->FCWSSensitivity;

    Amba_Adas_SetFcwParams(cfg);

    return ReturnValue;
}


int AppLibVideoAnal_ADAS_SetPar( APPLIB_ADAS_PAR_t* pParams)
{
    int ReturnValue = 0;
    AMBA_ADAS_SCENE_PARAMS_s par = {0};

    Amba_Adas_GetSceneParams(&par);

    par.HoodLevel = pParams->HoodLevel;
    par.HorizonLevel = pParams->HorizonLevel;

    Amba_Adas_GetSceneParams(&par);

    return ReturnValue;
}

int AppLibVideoAnal_ADAS_Enable(void)
{
    int ReturnValue = 0;

    if ( !(AppLibVideoAnal_FrmHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init \n");
        return -1;
    }
    if (ApplibAdasInit == 0) {
        AmbaPrint("AppLibVideoAnal_ADAS is not init \n");
        return -1;
    }
    ReturnValue = AppLibVideoAnal_FrmHdlr_Register(ApplibAdasYuvSrc, AppLibVideoAnal_ADAS_Process);
#ifdef CONFIG_APP_ARD
    //AppLibComSvcTimer_Register(TIMER_4HZ, AppLibVideoAnal_ADAS_Gui_Show_timer_handler);
#endif

    return ReturnValue;
}

int AppLibVideoAnal_ADAS_Disable(void)
{
    int ReturnValue = 0;
    if ( !(AppLibVideoAnal_FrmHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init \n");
        return -1;
    }
    ReturnValue = AppLibVideoAnal_FrmHdlr_UnRegister(ApplibAdasYuvSrc, AppLibVideoAnal_ADAS_Process);
#ifdef CONFIG_APP_ARD
    //AppLibComSvcTimer_Unregister(TIMER_4HZ, AppLibVideoAnal_ADAS_Gui_Show_timer_handler);
    //AppLibVideoAnal_ADAS_Gui_Hide();
#endif

    return ReturnValue;
}
#ifdef CONFIG_APP_ARD
/**
 * Draw ADAS info
 *
 * @return >=0 support
 *         <0 unsupport
 */
int AppLibVideoAnal_ADAS_Gui_Show(cardvws_t *cardvws_res)
{
    int duplicate_3d = 0, changed = 0;
    AMP_DISP_INFO_s DispDev = {0};

    int valid, _left, _top, _right, _bottom;
    int w = 50, h = 100;

    // Get DCHAN device info and GUIs display on DCHAN as long as DCHAN device is DISP_LCD.
    if (AppLibDisp_GetDeviceID(DISP_CH_DCHAN) == AMP_DISP_LCD) {
        AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
    } else {
        AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
    }

    //==========================================================================
    // ans_w = init_w * (gui_w / vout_w) * (disp_w / ref_w)
    //==========================================================================

    // Get DCHAN device info and GUIs display on DCHAN as long as DCHAN device is DISP_LCD.
    if (AppLibDisp_GetDeviceID(DISP_CH_DCHAN) == AMP_DISP_LCD) {
        valid = cardvws_res->vehicleValid;
        _left = cardvws_res->vehicle[0][0];
        _top = cardvws_res->vehicle[0][1];
        _right = cardvws_res->vehicle[1][0];
        _bottom = cardvws_res->vehicle[1][1];
        if (_top == _bottom) _top -= labs(_right-_left);

        AppLibVideoAnal_ADAS_Draw_Rect(_left, _top, _right, _bottom, DispDev,
            valid, GOBJ_FACE_RECT_1, COLOR_RED);

        valid = cardvws_res->laneValid[0];
        _left = cardvws_res->lane[0][0][0] - w;
        _top = cardvws_res->lane[0][0][1] - h;
        _right = cardvws_res->lane[0][0][0] + w;
        _bottom = cardvws_res->lane[0][0][1] + h;

        AppLibVideoAnal_ADAS_Draw_Rect(_left, _top, _right, _bottom, DispDev,
            valid, GOBJ_FACE_RECT_2, COLOR_GREEN);

        valid = cardvws_res->laneValid[0];
        _left = cardvws_res->lane[0][1][0] - w;
        _top = cardvws_res->lane[0][1][1] - h;
        _right = cardvws_res->lane[0][1][0] + w;
        _bottom = cardvws_res->lane[0][1][1] + h;

        AppLibVideoAnal_ADAS_Draw_Rect(_left, _top, _right, _bottom, DispDev,
            valid, GOBJ_FACE_RECT_3, COLOR_GREEN);

        valid = cardvws_res->laneValid[1];
        _left = cardvws_res->lane[1][0][0] - w;
        _top = cardvws_res->lane[1][0][1] - h;
        _right = cardvws_res->lane[1][0][0] + w;
        _bottom = cardvws_res->lane[1][0][1] + h;

        AppLibVideoAnal_ADAS_Draw_Rect(_left, _top, _right, _bottom, DispDev,
            valid, GOBJ_FACE_RECT_4, COLOR_GREEN);

        valid = cardvws_res->laneValid[1];
        _left = cardvws_res->lane[1][1][0] - w;
        _top = cardvws_res->lane[1][1][1] - h;
        _right = cardvws_res->lane[1][1][0] + w;
        _bottom = cardvws_res->lane[1][1][1] + h;

        AppLibVideoAnal_ADAS_Draw_Rect(_left, _top, _right, _bottom, DispDev,
            valid, GOBJ_FACE_RECT_5, COLOR_GREEN);
    }
    AppLibGraph_Draw(GRAPH_CH_DUAL);

    return changed;
}

int AppLibVideoAnal_ADAS_Draw_Rect(int _l, int _t, int _r, int _b, AMP_DISP_INFO_s ddev_info, int valid, int rect_num, u32 color)
{
    unsigned int left, top, right, bottom, width, height;
    u16 vout_width, vout_height;
    int changed = 0;
    u16 pos[4][2] = {0};
    unsigned int _left, _top, _right, _bottom;
    int osd_width, osd_height;

    _l = MAX(0, MIN(4096, _l));
    _t = MAX(0, MIN(4096, _t));
    _r = MAX(0, MIN(4096, _r));
    _b = MAX(0, MIN(4096, _b));

    _left = (unsigned int) MIN(_l, _r);
    _top = (unsigned int) MIN(_t, _b);
    _right = (unsigned int) MAX(_l, _r);
    _bottom = (unsigned int) MAX(_t, _b);

    vout_width = ddev_info.DeviceInfo.VoutWidth;
    vout_height = ddev_info.DeviceInfo.VoutHeight;

    if (AppLibDisp_GetDeviceID(DISP_CH_DCHAN) == AMP_DISP_LCD) {
        osd_width = OSD_DCHAN_WIDTH;
        osd_height = OSD_DCHAN_HEIGHT;
    } else {
        osd_width = OSD_FCHAN_WIDTH;
        osd_height = OSD_FCHAN_HEIGHT;
    }

    if (valid)
    {
        unsigned int vout_w_vd_w = ((unsigned int) vout_width)* 4096;
        unsigned int vout_h_vd_h = ((unsigned int) vout_height)* 4096;
        unsigned int gt_f_w_dd_i_w = osd_width * 960;
        unsigned int gt_f_h_dd_i_h = osd_height * 240;

        left   = (_left*gt_f_w_dd_i_w)/vout_w_vd_w;
        top    = (_top*gt_f_h_dd_i_h)/vout_h_vd_h;
        right  = (_right*gt_f_w_dd_i_w)/vout_w_vd_w;
        bottom = (_bottom*gt_f_h_dd_i_h)/vout_h_vd_h;
        width  = (right-left)+1;
        height = (bottom-top)+1;

        pos[0][0] = (u16) (left+(osd_width/vout_width));
        pos[1][0] = (u16) (top+(osd_height/vout_height));
        pos[2][0] = (u16) (width);
        pos[3][0] = (u16) (height);

        AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, rect_num, width, height, 0);
        AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, rect_num, pos[0][0], pos[1][0]);
        AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, rect_num, color, COLOR_CLR );
        AppLibGraph_Show(GRAPH_CH_DUAL, rect_num);

        if(GOBJ_FACE_RECT_1 == rect_num) {
            AMBA_ADAS_FCW_OUTPUT_s* FcwOutput = NULL;
            UINT16 OutputStr[10] = {'0','0','0','m', '\0'};
            int distance = 0;

            Amba_Adas_GetFcwOutput(&FcwOutput);
            distance = (int)FcwOutput->FrontVehicleDistance;
            if (distance > 0) {
                OutputStr[0] = 0x0030 + distance/100;
                OutputStr[1] = 0x0030 + (distance%100)/10;
                OutputStr[2] = 0x0030 + (distance%10);
                OutputStr[3] = 'm';

                AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_FACE_RECT_INFO, OutputStr);
                AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_FACE_RECT_INFO, STR_FACE_RECT_INFO);
                AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_INFO, pos[0][0], pos[1][0]);
                AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_INFO, color, COLOR_TEXT_BORDER );
                AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_FACE_RECT_INFO);
            }
        }
        changed = 1;
    } else {
        AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, rect_num, COLOR_CLR, COLOR_CLR );
        AppLibGraph_Hide(GRAPH_CH_DUAL, rect_num);

        if(GOBJ_FACE_RECT_1 == rect_num) {
            AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_INFO, COLOR_CLR, COLOR_CLR );
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_INFO);
        }
        changed = 1;
    }

    return changed;
}

/**
 *  @brief The timer handler that shows gui for ADAS.
 *
 *  @param[in] eid timer id.
 *
 */
void AppLibVideoAnal_ADAS_Gui_Show_timer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    AppLibVideoAnal_ADAS_Gui_Show(&g_cardvws);

}

void AppLibVideoAnal_ADAS_Gui_Hide(void)
{
    AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_1, COLOR_CLR, COLOR_CLR );
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_1);

    AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_2, COLOR_CLR, COLOR_CLR );
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_2);

    AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_3, COLOR_CLR, COLOR_CLR );
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_3);

    AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_4, COLOR_CLR, COLOR_CLR );
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_4);

    AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_5, COLOR_CLR, COLOR_CLR );
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_5);

    AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_FACE_RECT_INFO, COLOR_CLR, COLOR_CLR );
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FACE_RECT_INFO);

    AppLibGraph_Draw(GRAPH_CH_DUAL);
}
#endif

