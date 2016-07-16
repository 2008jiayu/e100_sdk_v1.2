/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaYuv.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella Yuv driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_YUV_H_
#define _AMBA_YUV_H_

#include "AmbaDSP.h"
#include "AmbaVIN.h"
#include "bsp.h"


/*-----------------------------------------------------------------------------------------------*\
 * Common time table for Yuv driver
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_YUV_VIDEO_STANDARD_e_ {
    AMBA_YUV_NTSC,
    AMBA_YUV_PAL
} AMBA_YUV_VIDEO_STANDARD_e;

typedef enum _AMBA_YUV_SCAN_METHOD_e_ {
    AMBA_YUV_INTERLACED,
    AMBA_YUV_PROGRESSIVE,
} AMBA_YUV_SCAN_METHOD_e;

typedef enum _AMBA_YUV_INPUT_SOURCE_e_ {
    AMBA_YUV_SVIDEO,
    AMBA_YUV_CVBS,
    AMBA_YUV_HDMI
} AMBA_YUV_INPUT_SOURCE_e;

typedef struct _AMBA_YUV_AREA_INFO_s_ {
    INT16   StartX;
    INT16   StartY;
    UINT16  Width;
    UINT16  Height;
} AMBA_YUV_AREA_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Yuv Device Info.
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_YUV_MODE_ID_u_ {
    UINT16  Data;                   /* this is an 16-bit data */

    struct {
        AMBA_YUV_VIDEO_STANDARD_e       VideoStandard:      2;  /* video standard, NTSC/PAL */
        AMBA_YUV_SCAN_METHOD_e          ScanMethod:         2;  /* scanning method, interlaced/progressive */
        AMBA_YUV_INPUT_SOURCE_e         InputSource:        4;  /* input source of YUV device, svideo/cvbs/hdmi */
        UINT16                          Reserved:           8;  /* reserved */
    } Bits;
} AMBA_YUV_MODE_ID_u;

typedef struct _AMBA_YUV_OUTPUT_INFO_s_ {
    UINT32  OutputWidth;                        /* valid pixels per line */
    UINT32  OutputHeight;                       /* valid lines per frame */
    AMBA_YUV_AREA_INFO_s    RecordingPixels;    /* maximum recording frame size */
    AMBA_DSP_YUV_ORDER_e    YuvOrder;           /* YUV pixels order */
    AMBA_DSP_FRAME_RATE_s   FrameRate;          /* framerate value of this sensor mode */
} AMBA_YUV_OUTPUT_INFO_s;

typedef struct _AMBA_YUV_MODE_INFO_s_ {
    AMBA_YUV_MODE_ID_u       Mode;
    AMBA_YUV_OUTPUT_INFO_s   OutputInfo;            /* info about output data format */
} AMBA_YUV_MODE_INFO_s;

typedef struct _AMBA_YUV_STATUS_INFO_s_ {
    AMBA_YUV_MODE_INFO_s        ModeInfo;
    AMBA_YUV_VIDEO_STANDARD_e   DetectedVideoStandard;  /* info from device detecting on the fly */
    UINT8                       IsHVSyncLocked;         /* HSync and Vsync locked status */
    UINT8                       IsCopyProtected;
} AMBA_YUV_STATUS_INFO_s;

typedef struct _AMBA_YUV_OBJ_s_ {
    char    *pName;          /* Name of the image Yuv */

    int (*Init)(AMBA_DSP_CHANNEL_ID_u Chan);
    int (*Enable)(AMBA_DSP_CHANNEL_ID_u Chan);
    int (*Disable)(AMBA_DSP_CHANNEL_ID_u Chan);
    int (*Config)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_YUV_MODE_ID_u Mode);
    int (*GetStatus)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_YUV_STATUS_INFO_s *pStatus);
    int (*GetModeInfo)(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_YUV_MODE_ID_u Mode, AMBA_YUV_MODE_INFO_s *pModeInfo);
} AMBA_YUV_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaYuv.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_YUV_OBJ_s *pAmbaYuvObj[];

#define AmbaYuv_Hook(Chan,pYuvObj)                                      pAmbaYuvObj[Chan.Bits.VinID] = pYuvObj

#define AmbaYuv_Init(Chan)                                              ((pAmbaYuvObj[Chan.Bits.VinID]->Init == NULL) ? NG : pAmbaYuvObj[Chan.Bits.VinID]->Init(Chan))
#define AmbaYuv_Enable(Chan)                                            ((pAmbaYuvObj[Chan.Bits.VinID]->Enable == NULL) ? NG : pAmbaYuvObj[Chan.Bits.VinID]->Enable(Chan))
#define AmbaYuv_Disable(Chan)                                           ((pAmbaYuvObj[Chan.Bits.VinID]->Disable == NULL) ? NG : pAmbaYuvObj[Chan.Bits.VinID]->Disable(Chan))
#define AmbaYuv_Config(Chan, Mode)                                      ((pAmbaYuvObj[Chan.Bits.VinID]->Config == NULL) ? NG : pAmbaYuvObj[Chan.Bits.VinID]->Config(Chan, Mode))
#define AmbaYuv_GetStatus(Chan, pStatus)                                ((pAmbaYuvObj[Chan.Bits.VinID]->GetStatus == NULL) ? NG : pAmbaYuvObj[Chan.Bits.VinID]->GetStatus(Chan, pStatus))
#define AmbaYuv_GetModeInfo(Chan, Mode, pModeInfo)                      ((pAmbaYuvObj[Chan.Bits.VinID]->GetModeInfo == NULL) ? NG : pAmbaYuvObj[Chan.Bits.VinID]->GetModeInfo(Chan, Mode, pModeInfo))

#endif /* _AMBA_Yuv_H_ */
