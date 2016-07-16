/**
 *  @file FormatDef.h
 *
 *  Format definition header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __FORMATDEF_H__
#define __FORMATDEF_H__


/**
 * @addtogroup Format
 * @{
 */

#include <AmbaDataType.h>
#include <image/imgproc/AmbaImg_Exif.h>
#include <cfs/AmpCfs.h>

#define AMP_FORMAT_MAX_DATE_SIZE    24      /**< The max size of the date information */
#define AMP_FORMAT_MAX_TIME_SIZE    12      /**< the max size of the time information */

#define AMP_FORMAT_MAX_TRACK_PER_MEDIA  4   /**< The maximum number of tracks in a media */
#define AMP_FORMAT_MAX_THUMB_PER_MOVIE  2   /**< The maximum number of image thumbnails in a movie */
#define AMP_FORMAT_MAX_FRAME_PER_IMAGE  6   /**< The maximum number of frames in an image */

#define CLOCK   90000                       /**< The standard clock frequency of the system (It is 90KHz in the system.) */

#define AMP_FORMAT_MAX_TIMESTAMP    0xFFFFFFFFFFFFFFFFull   /**< The max timestamp of the format */

#define AMP_FORMAT_MAX_SPS_LENGTH   64  /**< The maximum length of the SPS */
#define AMP_FORMAT_MAX_PPS_LENGTH   64  /**< The maximum length of the PPS */

#define AMP_FORMAT_MAX_VFR_FACTOR   16

/**
 * The base structure of a media information
 * MediaType : The media type (See AMP_MEDIA_INFO_TYPE_e.)
 * Valid : The value indicating if the media context is valid
 * SubFormat : The sub_format of the media (MSMP4, FUJIMOV)
 * MagicPattern : The Magic pattern 0x12345678 (used for recovery)
 * Size : The file size
 * Name : The media name
 * Date : The creation date of the media
 * Time : The creation time of the media
 */
#define MEDIA_INFO_BASE\
    UINT8 MediaType;\
    BOOL8 Valid;\
    UINT8 SubFormat;\
    UINT32 MagicPattern;\
    UINT64 Size;\
    char Name[MAX_FILENAME_LENGTH];\
    char Date[AMP_FORMAT_MAX_DATE_SIZE];\
    char Time[AMP_FORMAT_MAX_TIME_SIZE]

/**
 * Media Info object
 */
typedef struct {
    MEDIA_INFO_BASE;    /**< Base items of Media Info objects (See MEDIA_INFO_BASE.) */
} AMP_MEDIA_INFO_s;

typedef enum {
    AMP_VIDEO_MODE_P = 0,               /**< Progressive */
    AMP_VIDEO_MODE_I_A_FRM_PER_SMP = 1, /**< Frame per sample */
    AMP_VIDEO_MODE_I_A_FLD_PER_SMP = 2, /**< Field per sample */
} AMP_VIDEO_MODE_s;

#define AMP_FORMAT_MID_A_MASK   0x20
#define AMP_FORMAT_MID_T_MASK   0x40

typedef enum {
    AMP_FORMAT_MID_H264 = 0x01,                             /**< H264 media id */
    AMP_FORMAT_MID_AVC = 0x02,                              /**< AVC media id */
    AMP_FORMAT_MID_MJPEG = 0x03,                            /**< MJPEG media id */
    AMP_FORMAT_MID_AAC = (AMP_FORMAT_MID_A_MASK | 0x01),    /**< AAC media id */
    AMP_FORMAT_MID_PCM = (AMP_FORMAT_MID_A_MASK | 0x02),    /**< PCM media id */
    AMP_FORMAT_MID_ADPCM = (AMP_FORMAT_MID_A_MASK | 0x03),  /**< ADPCM media id */
    AMP_FORMAT_MID_MP3 = (AMP_FORMAT_MID_A_MASK | 0x04),    /**< MP3 media id */
    AMP_FORMAT_MID_AC3 = (AMP_FORMAT_MID_A_MASK | 0x05),    /**< AC3 media id */
    AMP_FORMAT_MID_WMA = (AMP_FORMAT_MID_A_MASK | 0x06),    /**< WMA media id */
    AMP_FORMAT_MID_OPUS = (AMP_FORMAT_MID_A_MASK | 0x07),   /**< OPUS media id */
    AMP_FORMAT_MID_TEXT = (AMP_FORMAT_MID_T_MASK | 0x01),   /**< Text media id */
    AMP_FORMAT_MID_MP4S = (AMP_FORMAT_MID_T_MASK | 0x02)    /**< MP4S media id */
} AMP_FORMAT_MID_e;

/**
 * @}
 */

#endif
