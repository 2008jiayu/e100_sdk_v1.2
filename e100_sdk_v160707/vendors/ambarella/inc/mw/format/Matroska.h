/**
 *  @file Matroska.h
 *
 *  Common definitions of Matroska format
 *
 *  @copyright 2014 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MATROSKA_H__
#define __MATROSKA_H__

#include <format/FormatDef.h>

#define MATROSKA_MAX_ID_LEN     4
#define MATROSKA_MAX_UID_LEN    16

/**
 * MATROSKA Video track information
 */
typedef struct {
    UINT8 SPS[AMP_FORMAT_MAX_SPS_LENGTH];   /**< SPS of the H264 bitstream */
    UINT8 PPS[AMP_FORMAT_MAX_PPS_LENGTH];   /**< PPS of the H264 bitstream */
    UINT16 SPSLen;          /**< The SPS size of the H264 bitstream */
    UINT16 PPSLen;          /**< The PPS size of the H264 bitstream */
} MATROSKA_VIDEO_TRACK_INFO_s;

/**
 * MATROSKA Audio track information
 */
typedef struct {
    UINT8 Profile;              /**< AAC profile */
    UINT8 Channels;             /**< The number of channels kept in codec private */
    UINT32 SamplingFrequency;   /**< The sampling frequency kept in codec private */
} MATROSKA_AUDIO_TRACK_INFO_s;

/**
 * MATROSKA Text track information
 */
typedef struct {
    UINT8 Resv[4];  /**< Reserved */
} MATROSKA_TEXT_TRACK_INFO_s;

/**
 * MATROSKA Track information
 */
typedef struct {
    UINT32 TrackUid;                        /**< Track UID */
    union {
        MATROSKA_VIDEO_TRACK_INFO_s Video;  /**< The information of the video track (See MATROSKA_VIDEO_TRACK_INFO_s.) */
        MATROSKA_AUDIO_TRACK_INFO_s Audio;  /**< The information of the audio track (See MATROSKA_AUDIO_TRACK_INFO_s.) */
        MATROSKA_TEXT_TRACK_INFO_s Text;    /**< The information of the text track (See MATROSKA_TEXT_TRACK_INFO_s.) */
    } Info;
} MATROSKA_TRACK_INFO_s;

/**
 * MATROSKA Private information
 */
typedef struct {
    UINT64 SegmentStart;    /**< The start position of Segment content, right behind tag and size */
    UINT64 InfoPos;         /**< The position of Info */
    UINT64 TracksPos;       /**< The position of Tracks */
    UINT64 CuesPos;         /**< The position of Cues */
    UINT32 TimecodeScale;   /**< Timestamp scale in nanoseconds */
    INT64 DateUtc;          /**< Date of the origin of timestamp */
    UINT8 SegmentUid[16];   /**< A randomly generated unique ID to identify the current segment between many others (128 bits) */
    MATROSKA_TRACK_INFO_s TrackInfo[AMP_FORMAT_MAX_TRACK_PER_MEDIA];    /**< Tracks (See MATROSKA_TRACK_INFO_s.) */
} AMP_MATROSKA_PRIV_INFO_s;

#endif

