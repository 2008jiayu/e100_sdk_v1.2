/**
 * rtos/vendors/ambarella/inc/va/Amba_VA.h
 *
 * Header of Amba Video Analysis interface.
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __AMBAVA_H__
#define __AMBAVA_H__

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaCache_Def.h"
#include "util.h"
#include <recorder/Encode.h>
#include <AmbaFS.h>
#include <common/common.h>
#include <AmbaPrint.h>
#include <AmbaPrintk.h>

#ifndef ABS
#define ABS(a)  (((a) < 0) ? -(a) : (a))
#endif

//#define DEBUG_VA
#if defined(DEBUG_VA)
#define VADBGMSG AmbaPrint
#define VADBGMSGc(x) AmbaPrintColor(GREEN,x)
#else
#define VADBGMSG(...)
#define VADBGMSGc(...)
#endif

/*************************************************************************
 * ADAS declaration
 ************************************************************************/

/** Generic floating-point 2D point */
typedef struct _AMBA_VA_POINT_s_ {
    float X; /**< X-coordinate */
    float Y; /**< Y-coordinate */
} AMBA_VA_POINT_s;

typedef AMBA_VA_POINT_s AMBA_VA_POINT_t;

/** Generic floating-point 2D point */
typedef struct _AMBA_VA_ROI_s_ {
    UINT16 X;
    UINT16 Y;
    UINT16 W;
    UINT16 H;
} AMBA_VA_ROI_s;

typedef enum _AMBAVA_COLOR_FORMAT_e_ {
    AMBAVA_YUV_420,     ///< color format yuv420
    AMBAVA_YUV_422      ///< color format yuv422
} AMBAVA_COLOR_FORMAT_e;

typedef struct _AMBAVA_YUV_INFO_s_ {
    UINT32 ChannelID;
    AMBAVA_COLOR_FORMAT_e colorFmt; /**< Yuv color format */
    UINT8  *yAddr;                  /**< Yuv buffer address */
    UINT8  *uvAddr;                 /**< Yuv buffer address */
    UINT32  ySize;                  /**< Yuv buffer size, ie. Pitch*Height */
    UINT16  pitch;                  /**< Yuv buffer pitch */
    UINT16  width;                  /**< Yuv buffer width */
    UINT16  height;                 /**< Yuv buffer height */
} AMBAVA_YUV_INFO_s;

typedef AMBA_VA_ROI_s AMBA_VA_ROI_t;

#endif // __AMBA_VA_H__
