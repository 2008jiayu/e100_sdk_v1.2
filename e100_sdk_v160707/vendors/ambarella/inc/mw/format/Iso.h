/**
 *  @file Iso.h
 *
 *  Common definitions for ISO format
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __ISO_H__
#define __ISO_H__

#include <format/FormatDef.h>

#define ISO_BOX_SIZE_TAG_SIZE   8
#define ISO_BOX_PAYLOAD_SIZE(BoxSize)   (BoxSize - 8)

/*
 *  ISO Type
 */
typedef enum {
    ISO_TYPE_MP4,   /**< MP4 */
    ISO_TYPE_MOV    /**< MOV */
} ISO_TYPE_e;

/*
 *  Rotation definition
 */
typedef enum {
    ISO_ROTATION_NONE = 0x01,       /**< No rotation */
    ISO_ROTATION_FLIP_HORI = 0x02,  /**< Flip horizontally */
    ISO_ROTATION_ROTATE180 = 0x03,  /**< Rotation by 180 degrees */
    ISO_ROTATION_FLIP_VERT = 0x04,  /**< Flip vertically */
    ISO_ROTATION_TRANSPOSE = 0x05,  /**< Transpose */
    ISO_ROTATION_ROTATE90 = 0x06,   /**< Rotation by 90 degrees */
    ISO_ROTATION_TRANSVERSE = 0x07, /**< Rotate transversely */
    ISO_ROTATION_ROTATE270 = 0x08,  /**< Rotation by 270 degrees */
    ISO_ROTATION_MAX
} ISO_ROTATION_e;

/**
 * ISO Video track information
 */
typedef struct {
    UINT8 SPS[AMP_FORMAT_MAX_SPS_LENGTH];   /**< The SPS of H264 */
    UINT8 PPS[AMP_FORMAT_MAX_PPS_LENGTH];   /**< The PPS of H264 */
    UINT32 KeyFrameNo;      /**< The number of the key frame in current index buffer */
    UINT32 KeyFrameCount;   /**< The count of the key frame in current index buffer */
    UINT32 SttsCount;       /**< The count of the STTS entry in current index buffer */
    UINT32 FrameNumGOP;     /**< The frame number counter in a GOP (Reset in Idr) */
    UINT64 TmpV;            /**< The start offset of the index (It stores the size entry of the video track.) */
    UINT64 TmpVo;           /**< The start offset of the index (It stores the file offset entry of the video track.) */
    UINT64 TmpK;            /**< The start offset of the index (It stores the key frame entry of the video track.) */
    UINT64 TmpCtts;         /**< The start offset of the index (It stores the CTTS entry of the video track.) */
    UINT64 TmpStts;         /**< The start offset of the index (It stores the STTS entry of the video track.) */
    UINT16 SPSLen;          /**< The SPS size of H264 */
    UINT16 PPSLen;          /**< The PPS size of H264 */
} ISO_VIDEO_TRACK_INFO_s;

/**
 * ISO Audio track information
 */
typedef struct {
    UINT32 SttsCount;       /**< The count of the STTS entry in current index buffer */
    UINT64 TmpA;           /**< The start offset of the index (It stores the size entry of the audio track.) */
    UINT64 TmpAo;          /**< The start offset of the index (It stores the file offset entry of the audio track.) */
    UINT64 TmpStts;         /**< The start offset of the index (It stores the STTS entry of the audio track.) */
} ISO_AUDIO_TRACK_INFO_s;

/**
 * ISO Text track information
 */
typedef struct {
    UINT32 SttsCount;       /**< The count of the STTS entry in current index buffer */
    UINT64 TmpT;           /**< The start offset of the index (It stores the size entry of the text track.) */
    UINT64 TmpTo;          /**< The start offset of the index (It stores the file offset entry of the text track.) */
    UINT64 TmpStts;         /**< The start offset of the index (It stores the STTS entry of the text track.) */
} ISO_TEXT_TRACK_INFO_s;

/**
 * ISO Track information
 */
typedef struct {
    union {
        ISO_VIDEO_TRACK_INFO_s Video;   /**< Video track information (See ISO_VIDEO_TRACK_INFO_s.) */
        ISO_AUDIO_TRACK_INFO_s Audio;   /**< Audio track information (See ISO_AUDIO_TRACK_INFO_s.) */
        ISO_TEXT_TRACK_INFO_s Text;     /**< Text track information (See ISO_TEXT_TRACK_INFO_s.) */
    } Info;
} ISO_TRACK_INFO_s;

/**
 * ISO Private information
 */
typedef struct {
    UINT32 CreateTime;      /**< Creation time of the media */
    UINT32 ModifyTime;      /**< Modification time of the media */
    UINT32 FtypSize;        /**< The box size of the Iso ftyp */
    UINT32 TrickRecDivisor; /**< The divisor factor for HFR */
    ISO_TRACK_INFO_s TrackInfo[AMP_FORMAT_MAX_TRACK_PER_MEDIA]; /**< The track information in the private data (See ISO_TRACK_INFO_s.) */
    UINT64 FrameDataSize;
    UINT64 FrameDataPos;
    UINT64 MaxCachedSize;
    BOOL8 EnableCO64;
} AMP_ISO_PRIV_INFO_s;

#endif

