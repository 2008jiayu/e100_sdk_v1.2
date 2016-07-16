/**
 * rtos\vendors\ambarella\inc\va\ambava_adas_videoData.h
 *
 * Header of AMBA ADAS interface.
 *
 * Copyright (C) 2004-2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _AMBAVA_ADAS_VIDEO_FRAME_H_
#define _AMBAVA_ADAS_VIDEO_FRAME_H_

/** Format of pixel packing in amba_adas_VideoData structure */
typedef enum _AMBA_ADAS_FRAME_FORMAT_e_ {
    ADAS_FF_PACKED_RGB = 0, /**< Full-size RGB (interleaved colors) */
    ADAS_FF_YUV420SP  = 1, /**< YV12 */
    ADAS_FF_YUV422SP  = 2, /**< YV16 */
    ADAS_FF_YUV       = 3  /**< Full-size YUV (planar components) */
}AMBA_ADAS_FRAME_FORMAT_e;

/** Image plane */
typedef struct _AMBA_ADAS_IMAGE_s_
{
    unsigned char* Data;   /**< Array of pixels */
    unsigned int   Size;   /**< Size of array of pixels (bytes) */
    unsigned int   Width;  /**< Width of image (pixels) */
    unsigned int   Height; /**< Height of image (pixels) */
    unsigned int   Step;   /**< Interval between lines of image (bytes) */
} AMBA_ADAS_IMAGE_s;

/** Full frame description */
typedef struct _AMBA_ADAS_VIDEODATA_s_ {
    unsigned int TimeStampMs; /**< Timestamp of frame in milliseconds */
    AMP_ENC_YUV_INFO_s* pPlane0;    /**< Image plane */
    AMP_ENC_YUV_INFO_s* pPlane1;    /**< Image plane (optional) */
    AMP_ENC_YUV_INFO_s* pPlane2;    /**< Image plane (optional) */
} AMBA_ADAS_VIDEODATA_s;

#endif
