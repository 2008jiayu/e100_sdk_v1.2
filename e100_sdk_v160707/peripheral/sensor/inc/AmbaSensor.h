/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella sensor driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_H_
#define _AMBA_SENSOR_H_

#include "AmbaDSP.h"
#include "AmbaVIN.h"
#include "bsp.h"
/*-----------------------------------------------------------------------------------------------*\
 * Common time table for sensor driver
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_SENSOR_ELEC_SHUTTER_MODE_e_ {
    AMBA_SENSOR_ESHUTTER_TYPE_ROLLING,          /* Rolling shutter */
    AMBA_SENSOR_ESHUTTER_TYPE_GLOBAL_RESET      /* Global reset shutter */
} AMBA_SENSOR_ESHUTTER_TYPE_e;

typedef enum _AMBA_SENSOR_SUBSAMPLING_TYPE_e_ {
    AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,          /* Single pixel value */
    AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING,         /* Summing/Averaging multiple pixel values */
    AMBA_SENSOR_SUBSAMPLE_TYPE_SCALING,         /* Low-pass filtering on multiple pixel values */
    AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING_SCALING  /* Binning and then scaling on multiple pixel values */
} AMBA_SENSOR_SUBSAMPLING_TYPE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Combine charges from M out of N pixels for one pixel,
 * where M is num and N is den in horizontal or vertical direction.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_SENSOR_SUBSAMPLING_s_ {
    AMBA_SENSOR_SUBSAMPLING_TYPE_e  SubsampleType;
    UINT8   FactorNum;              /* subsamping factor (numerator) */
    UINT8   FactorDen;              /* subsamping factor (denominator) */
} AMBA_SENSOR_SUBSAMPLING_s;

typedef struct _AMBA_SENSOR_AREA_INFO_s_ {
    INT16   StartX;
    INT16   StartY;
    UINT16  Width;
    UINT16  Height;
} AMBA_SENSOR_AREA_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Info.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_SENSOR_CURRENT_MODE 0x1fff

typedef union _AMBA_SENSOR_MODE_ID_u_ {
    UINT16  Data;                   /* this is an 16-bit data */

    struct {
        UINT16                  Mode:                   13; /* Mode ID */
        UINT16                  VerticalFlip:           1;  /* Vertical flip flag */
        UINT16                  BypassSyncGenerator:    1;  /* The flag only available for slave-sensor,
                                                               1: VIN don't need to generate Master Sync. */
        UINT16                  DummyMasterSync:        1;  /* for slave-sensor only.
                                                               1: program Master sync to dummy frame timing */
    } Bits;
} AMBA_SENSOR_MODE_ID_u;

typedef enum _AMBA_SENSOR_COMMUNICATION_TIME_e_ {
    AMBA_SENSOR_COMMUNICATION_AT_VBLANK,        /* assume start time of vertical blanking */
    AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK     /* assume start time of frame */
} AMBA_SENSOR_COMMUNICATION_TIME_e;

typedef struct _AMBA_SENSOR_WB_GAIN_s_ {
    float  R;
    float  Gr;
    float  Gb;
    float  B;
} AMBA_SENSOR_WB_GAIN_s;

typedef struct _AMBA_SENSOR_WB_CTRL_s_ {
    UINT32  R;
    UINT32  Gr;
    UINT32  Gb;
    UINT32  B;
} AMBA_SENSOR_WB_CTRL_s;

typedef struct _AMBA_SENSOR_CTRL_INFO_s {
    AMBA_SENSOR_COMMUNICATION_TIME_e    CommunicationTime;  /* time to access sensor registers */
    UINT8   FirstReflectedFrame;                            /* number of frames from communication end to the first frame with new settings */
    UINT8   FirstBadFrame;                                  /* number of frames from communication end to the first bad frame */
    UINT8   NumBadFrames;                                   /* number of continouse bad frames from the first bad frame */
} AMBA_SENSOR_CTRL_INFO_s;

typedef enum _AMBA_SENSOR_HDR_TYPE_e_ {
    AMBA_SENSOR_HDR_NONE,                           /* sensor doesn't support HDR mechanism */
    AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,               /* sensor output a frame composed of hdr slices */
    AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND,            /* sensor done HDR blending internally, and output the result */
} AMBA_SENSOR_HDR_TYPE_e;

typedef struct _AMBA_SENSOR_HDR_CHANNEL_INFO_s {
    AMBA_SENSOR_AREA_INFO_s   EffectiveArea;        /* effective area for each channel */
    AMBA_SENSOR_AREA_INFO_s   OpticalBlackPixels;   /* user clamp area for each channel */
    AMBA_SENSOR_CTRL_INFO_s   ShutterSpeedCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s   OutputFormatCtrlInfo;
    UINT32 MaxExposureLine;                          /* maximum exposure line (in row time) */
    UINT32 MinExposureLine;                          /* minimum exposure line. (in row time) */
} AMBA_SENSOR_HDR_CHANNEL_INFO_s;

typedef struct _AMBA_SENSOR_HDR_GAIN_Factor_s_ {
    float DesiredFactor;
    AMBA_SENSOR_WB_GAIN_s DesiredWbFactor;
} AMBA_SENSOR_HDR_GAIN_Factor_s;

typedef struct _AMBA_SENSOR_HDR_GAIN_CTRL_s_ {
    float ActualFactor;
    AMBA_SENSOR_WB_GAIN_s ActualWbFactor;
    UINT32 AnalogGainCtrl;
    UINT32 DigitalGainCtrl;
    AMBA_SENSOR_WB_CTRL_s WbGainCtrl;
} AMBA_SENSOR_HDR_GAIN_CTRL_s;

typedef struct _AMBA_SENSOR_HDR_INFO_s_ {
    AMBA_SENSOR_HDR_TYPE_e HdrType;
    UINT8 NotSupportIndividualGain;                 /* 0: sensor supports individual gain in hdr mode, 1: sensor doesn't support individual gain in hdr mode */
    UINT8 ActiveChannels;                           /* support up to 4 channels */
    AMBA_SENSOR_HDR_CHANNEL_INFO_s ChannelInfo[4];  /* support up to 4 channels */
} AMBA_SENSOR_HDR_INFO_s;

typedef struct _AMBA_SENSOR_DEVICE_INFO_s {
    float   UnitCellWidth;      /* pixel width (um) */
    float   UnitCellHeight;     /* pixel height (um) */

    UINT16  NumTotalPixelCols;
    UINT16  NumTotalPixelRows;
    UINT16  NumEffectivePixelCols;
    UINT16  NumEffectivePixelRows;

    AMBA_SENSOR_CTRL_INFO_s     FrameRateCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     ShutterSpeedCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     AnalogGainCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     DigitalGainCtrlInfo;

    float   MinAnalogGainFactor;
    float   MaxAnalogGainFactor;
    float   MinDigitalGainFactor;
    float   MaxDigitalGainFactor;

    UINT8   HdrIsSupport;       /* 0: sensor doesn't support hdr, 1: sensor supports hdr */
} AMBA_SENSOR_DEVICE_INFO_s;

typedef struct _AMBA_SENSOR_OUTPUT_INFO_s_ {
    UINT32  DataRate;                               /* output bit/pixel clock frequency from image sensor */
    UINT8   NumDataLanes;                           /* active data channels */
    UINT8   NumDataBits;                            /* pixel data bit depth */
    AMBA_DSP_SENSOR_PATTERN_e   CfaPattern;         /* CFA filter type (color transmission order) */
    AMBA_DSP_PHASE_SHIFT_CTRL_s DspPhaseShift;      /* DSP Phase Shift */
    UINT32  OutputWidth;                            /* valid pixels per line */
    UINT32  OutputHeight;                           /* valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s     RecordingPixels;    /* maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s     OpticalBlackPixels; /* user clamp area */
} AMBA_SENSOR_OUTPUT_INFO_s;

typedef struct _AMBA_SENSOR_INPUT_INFO_s_ {
    AMBA_SENSOR_AREA_INFO_s     PhotodiodeArray;    /* effective photodiode array position */
    AMBA_SENSOR_SUBSAMPLING_s   HSubsample;         /* info about frame data readout from active pixel array */
    AMBA_SENSOR_SUBSAMPLING_s   VSubsample;         /* info about frame data readout from active pixel array */
    UINT8   SummingFactor;                          /* number of pixel data to be sumed up */
} AMBA_SENSOR_INPUT_INFO_s;

typedef struct _AMBA_SENSOR_FRAME_TIMING_s_ {
    UINT32  InputClk;                           /* Sensor side input clock frequency */
    AMBA_DSP_FRAME_RATE_s       FrameRate;      /* framerate value of this sensor mode */
} AMBA_SENSOR_FRAME_TIMING_s;

typedef struct _AMBA_SENSOR_MODE_INFO_s_ {
    AMBA_SENSOR_MODE_ID_u   Mode;
    UINT32  LineLengthPck;                      /* number of pixel clock cycles per line of frame */
    UINT32  FrameLengthLines;                   /* number of lines per frame */
    UINT32  NumExposureStepPerFrame;            /* number of exposure time unit per frame */
    float   InternalExposureOffset;             /* internal exposure offset (unit: row period) */
    float   PixelRate;                          /* pixel data rate (unit: pixel/sec) */
    float   RowTime;                            /* time interval of LineLengthPck (in seconds) */
    AMBA_DSP_FRAME_RATE_s       MinFrameRate;   /* minimum framerate value */
    AMBA_SENSOR_INPUT_INFO_s    InputInfo;      /* info about photodetector */
    AMBA_SENSOR_OUTPUT_INFO_s   OutputInfo;     /* info about data output interface */
    AMBA_SENSOR_FRAME_TIMING_s  FrameTime;      /* info about master sync */
    AMBA_SENSOR_HDR_INFO_s      HdrInfo;        /* HDR information */
} AMBA_SENSOR_MODE_INFO_s;

typedef struct _AMBA_SENSOR_DEVICE_STAT_s_ {
    UINT8   Data;

    struct {
        UINT8   Sensor0Power:   1;  /* [0] 1 = Power enabled */
        UINT8   Sensor0Standby: 1;  /* [1] 1 = Streaming disabled */
        UINT8   Sensor1Power:   1;  /* [2] 1 = Power enabled */
        UINT8   Sensor1Standby: 1;  /* [3] 1 = Streaming disabled */
        UINT8   Sensor2Power:   1;  /* [4] 1 = Power enabled */
        UINT8   Sensor2Standby: 1;  /* [5] 1 = Streaming disabled */
        UINT8   Sensor3Power:   1;  /* [6] 1 = Power enabled */
        UINT8   Sensor3Standby: 1;  /* [7] 1 = Streaming disabled */
    } Bits;
} AMBA_SENSOR_DEVICE_STAT_s;

typedef struct _AMBA_SENSOR_STATUS_INFO_s_ {
    AMBA_SENSOR_DEVICE_STAT_s   DevStat;
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode;
    float   TimeToAllPelExposed;    /* the time from V-sync end to all recording pixels of the array are exposed (in seconds) */
} AMBA_SENSOR_STATUS_INFO_s;

typedef struct _AMBA_SENSOR_OBJ_s_ {
    char *pName;          /* Name of the image sensor */

    int (*Init)(AMBA_DSP_CHANNEL_ID_u Chan);
    int (*Enable)(AMBA_DSP_CHANNEL_ID_u Chan);
    int (*Disable)(AMBA_DSP_CHANNEL_ID_u Chan);
    int (*Config)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode);
    int (*ChangeFrameRate)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_DSP_FRAME_RATE_s *pFrameRate);
    int (*GetStatus)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus);
    int (*GetModeInfo)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo);
    int (*GetDeviceInfo)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo);
    int (*GetCurrentGainFactor)(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor);
    int (*GetCurrentShutterSpeed)(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime);

    int (*ConvertGainFactor)(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl);
    int (*ConvertShutterSpeed)(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl);
    int (*SetAnalogGainCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl);
    int (*SetDigitalGainCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl);
    int (*SetShutterCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl);
    int (*SetSlowShutterCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl);

    int (*SetGainFactor)(AMBA_DSP_CHANNEL_ID_u Chan, float GainFactor);
    int (*SetEshutterSpeed)(AMBA_DSP_CHANNEL_ID_u Chan, float EshutterSpeed);

    int (*ConvertWbGainFactor)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_WB_GAIN_s DesiredWbFactor, AMBA_SENSOR_WB_GAIN_s *pActualWbFactor, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl);
    int (*SetWbGainCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_WB_CTRL_s WbGainCtrl);
    int (*SetHdrWbGainCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl);

    int (*SetHdrAnalogGainCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAnalogGainCtrl);
    int (*SetHdrDigitalGainCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pDigitalGainCtrl);
    int (*SetHdrShutterCtrl)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl);

    int (*RegisterRead)(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16* pData);
    int (*RegisterWrite)(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data);

    int (*GetHdrInfo)(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo);

    int (*ConvertHdrGainFactor)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_HDR_GAIN_Factor_s *pHdrGainFactor, AMBA_SENSOR_HDR_GAIN_CTRL_s *pHdrGainCtrl);
    int (*ConvertHdrShutterSpeed)(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime, UINT32 *pShutterCtrl);

} AMBA_SENSOR_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSensor.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_SENSOR_OBJ_s *pAmbaSensorObj[];

#define AmbaSensor_Hook(Chan,pSensorObj)                                     pAmbaSensorObj[Chan.Bits.VinID] = pSensorObj

#define AmbaSensor_Init(Chan)                                                ((pAmbaSensorObj[Chan.Bits.VinID]->Init == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->Init(Chan))
#define AmbaSensor_Enable(Chan)                                              ((pAmbaSensorObj[Chan.Bits.VinID]->Enable == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->Enable(Chan))
#define AmbaSensor_Disable(Chan)                                             ((pAmbaSensorObj[Chan.Bits.VinID]->Disable == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->Disable(Chan))
#define AmbaSensor_Config(Chan,Mode,EshutterType)                            ((pAmbaSensorObj[Chan.Bits.VinID]->Config == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->Config(Chan,Mode,EshutterType))
#define AmbaSensor_ChangeFrameRate(Chan,pFrameRate)                          ((pAmbaSensorObj[Chan.Bits.VinID]->ChangeFrameRate == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->ChangeFrameRate(Chan,pFrameRate))
#define AmbaSensor_GetStatus(Chan,pStatus)                                   ((pAmbaSensorObj[Chan.Bits.VinID]->GetStatus == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->GetStatus(Chan,pStatus))
#define AmbaSensor_GetModeInfo(Chan,Mode,pModeInfo)                          ((pAmbaSensorObj[Chan.Bits.VinID]->GetModeInfo == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->GetModeInfo(Chan,Mode,pModeInfo))
#define AmbaSensor_GetDeviceInfo(Chan,pDeviceInfo)                           ((pAmbaSensorObj[Chan.Bits.VinID]->GetDeviceInfo == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->GetDeviceInfo(Chan,pDeviceInfo))
#define AmbaSensor_GetCurrentGainFactor(Chan,pGainFactor)                    ((pAmbaSensorObj[Chan.Bits.VinID]->GetCurrentGainFactor == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->GetCurrentGainFactor(Chan,pGainFactor))
#define AmbaSensor_GetCurrentShutterSpeed(Chan,pExposureTime)                ((pAmbaSensorObj[Chan.Bits.VinID]->GetCurrentShutterSpeed == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->GetCurrentShutterSpeed(Chan,pExposureTime))
#define AmbaSensor_GetHdrInfo(Chan,pShutterCtrl,pHdrInfo)                    ((pAmbaSensorObj[Chan.Bits.VinID]->GetHdrInfo == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->GetHdrInfo(Chan,pShutterCtrl,pHdrInfo))

#define AmbaSensor_ConvertGainFactor(Chan,Factor,pFactor,pAgc,pDgc)          ((pAmbaSensorObj[Chan.Bits.VinID]->ConvertGainFactor == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->ConvertGainFactor(Chan,Factor,pFactor,pAgc,pDgc))
#define AmbaSensor_ConvertWbGainFactor(Chan,WbFactor,pWbFactor,pWbCtrl)      ((pAmbaSensorObj[Chan.Bits.VinID]->ConvertWbGainFactor == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->ConvertWbGainFactor(Chan,WbFactor,pWbFactor,pWbCtrl))
#define AmbaSensor_ConvertShutterSpeed(Chan,ExposureTime,pShutterCtrl)       ((pAmbaSensorObj[Chan.Bits.VinID]->ConvertShutterSpeed == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->ConvertShutterSpeed(Chan,ExposureTime,pShutterCtrl))
#define AmbaSensor_SetAnalogGainCtrl(Chan,Agc)                               ((pAmbaSensorObj[Chan.Bits.VinID]->SetAnalogGainCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetAnalogGainCtrl(Chan,Agc))
#define AmbaSensor_SetDigitalGainCtrl(Chan,Dgc)                              ((pAmbaSensorObj[Chan.Bits.VinID]->SetDigitalGainCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetDigitalGainCtrl(Chan,Dgc))
#define AmbaSensor_SetWbGainCtrl(Chan,WbCtrl)                                ((pAmbaSensorObj[Chan.Bits.VinID]->SetWbGainCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetWbGainCtrl(Chan,WbCtrl))
#define AmbaSensor_SetShutterCtrl(Chan,ShutterCtrl)                          ((pAmbaSensorObj[Chan.Bits.VinID]->SetShutterCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetShutterCtrl(Chan,ShutterCtrl))
#define AmbaSensor_SetSlowShutterCtrl(Chan,SlowShutterCtrl)                  ((pAmbaSensorObj[Chan.Bits.VinID]->SetSlowShutterCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetSlowShutterCtrl(Chan,SlowShutterCtrl))

#define AmbaSensor_ConvertHdrGainFactor(Chan,pHdrGainFactor,pHdrGainCtrl)    ((pAmbaSensorObj[Chan.Bits.VinID]->ConvertHdrGainFactor == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->ConvertHdrGainFactor(Chan,pHdrGainFactor,pHdrGainCtrl))
#define AmbaSensor_ConvertHdrShutterSpeed(Chan,pExposureTime,pShutterCtrl)   ((pAmbaSensorObj[Chan.Bits.VinID]->ConvertHdrShutterSpeed == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->ConvertHdrShutterSpeed(Chan,pExposureTime,pShutterCtrl))
#define AmbaSensor_SetHdrAnalogGainCtrl(Chan,pAgc)                           ((pAmbaSensorObj[Chan.Bits.VinID]->SetHdrAnalogGainCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetHdrAnalogGainCtrl(Chan,pAgc))
#define AmbaSensor_SetHdrDigitalGainCtrl(Chan,pDgc)                          ((pAmbaSensorObj[Chan.Bits.VinID]->SetHdrDigitalGainCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetHdrDigitalGainCtrl(Chan,pDgc))
#define AmbaSensor_SetHdrWbGainCtrl(Chan,pWbCtrl)                            ((pAmbaSensorObj[Chan.Bits.VinID]->SetHdrWbGainCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetHdrWbGainCtrl(Chan,pWbCtrl))
#define AmbaSensor_SetHdrShutterCtrl(Chan,pShutterCtrl)                      ((pAmbaSensorObj[Chan.Bits.VinID]->SetHdrShutterCtrl == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->SetHdrShutterCtrl(Chan,pShutterCtrl))

#define AmbaSensor_RegisterRead(Chan,Addr,pData)                             ((pAmbaSensorObj[Chan.Bits.VinID]->RegisterRead == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->RegisterRead(Chan,Addr,pData))
#define AmbaSensor_RegisterWrite(Chan,Addr,Data)                             ((pAmbaSensorObj[Chan.Bits.VinID]->RegisterWrite == NULL) ? NG : pAmbaSensorObj[Chan.Bits.VinID]->RegisterWrite(Chan,Addr,Data))

#endif /* _AMBA_SENSOR_H_ */
