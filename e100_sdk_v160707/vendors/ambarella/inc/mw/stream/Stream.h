/**
 *  @file Stream.h
 *
 *  encode common function header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __STREAM_H__
#define __STREAM_H__

/**
 * @defgroup Stream
 * @brief I/O stream implementation
 *
 * Stream module provides the APIs and interface to access I/O streams.
 */

/**
 * @addtogroup Stream
 * @{
 */

#include <format/FormatDef.h>

#define AMP_STREAM_OP_INSERT_SPACE  0x00000001          /**< The opcode of the function to insert spaces at a specified position */
#define AMP_STREAM_OP_GET_MAX_CACHED_SIZE  0x00000002   /**< The opcode of the function to get cached data size*/

// for external muxer/demuxer
#define AMP_STREAM_OP_SET_MEDIA_INFO    0x00000010  /**< The opcode of the function to set media information */
#define AMP_STREAM_OP_SET_FRAME_INFO    0x00000011  /**< The opcode of the function to set frame information */
#define AMP_STREAM_OP_GET_MEDIA_INFO    0x00000012  /**< The opcode of the function to get media information */
#define AMP_STREAM_OP_GET_FRAME_INFO    0x00000013  /**< The opcode of the function to get frame information */
#define AMP_STREAM_OP_SYNC              0x00000014  /**< The opcode of the function to sync a stream */

#define AMP_STREAM_SEEK_CUR     AMP_CFS_SEEK_CUR    /**< The current position of a stream (seek to the position relative to current file position) */
#define AMP_STREAM_SEEK_START   AMP_CFS_SEEK_START  /**< The beginning of a stream (seek to the position relative to start position of file) */
#define AMP_STREAM_SEEK_END     AMP_CFS_SEEK_END    /**< The end of a stream (seek to the position relative to end position of file) */

/**
 * The parameter of AMP_STREAM_OP_INSERT_SPACE
 */
typedef struct {
    UINT64 Pos;     /**< The position to insert spaces */
    UINT64 Size;    /**< The size of the space to be inserted */
} AMP_STREAM_PARAM_INSERT_SPACE_s;

/**
 * Ext video track information
 */
typedef struct {
    UINT32 GOPSize;             /**< The number of pictures between IDR pictures */
    UINT16 PixelArX;            /**< The aspect ration X of the pixel (If the value is not 1, it means the pixel is not square.) */
    UINT16 PixelArY;            /**< The aspect ration Y of the pixel (if the value is not 1, it means the pixel is not square.) */
    UINT16 Width;               /**< Video width */
    UINT16 Height;              /**< Video height */
    UINT16 M;                   /**< The number of pictures between reference pictures (IDR, I, P) */
    UINT16 N;                   /**< The number of pictures between I pictures */
    BOOL8 IsDefault;            /**< The flag indicating the track is the default video track */
    UINT8 Mode;                 /**< The picture mode of the video (e.g., progressive, Field-Per-Sample, and Frame-Per-Sample. See AMP_VIDEO_MODE_s.) */
    UINT8 ColorStyle;           /**< The color space of the video (0: used for TV, 1: used for PC) */
    UINT8 EntropyMode;          /**< The entropy mode of the H.264 bitstream (0: CAVLC, 1: CABAC) */
} AMP_STREAM_EXT_VIDEO_TRACK_INFO_s;

/**
 * Ext audio track information
 */
typedef struct {
    UINT32 SampleRate;          /**< The sample rate (Hz) of the audio track */
    BOOL8 IsDefault;            /**< The flag indicating that the track is the default audio track */
    UINT8 Channels;             /**< The number of channels in the audio track */
    UINT8 BitsPerSample;        /**< The bits per sample of the audio track (e.g., 8 bits and 16 bits) */
} AMP_STREAM_EXT_AUDIO_TRACK_INFO_s;

/**
 * Ext text track information
 */
typedef struct {
    BOOL8 IsDefault;            /**< The flag indicating the track is the default text track */
} AMP_STREAM_EXT_TEXT_TRACK_INFO_s;

/**
 * Ext media track information
 */
typedef struct {
    UINT32 MediaId;             /**< The media type of the track (See AMP_FORMAT_MID_e.) */
    UINT32 TimeScale;           /**< The ticks per second */
    UINT32 TimePerFrame;        /**< The ticks per frame */
    UINT32 FrameCount;          /**< The count of frames in the track */
    union {
        AMP_STREAM_EXT_VIDEO_TRACK_INFO_s Video;   /**< The information of the video track (See AMP_VIDEO_TRACK_INFO_s.) */
        AMP_STREAM_EXT_AUDIO_TRACK_INFO_s Audio;   /**< The information of the audio track (See AMP_AUDIO_TRACK_INFO_s.) */
        AMP_STREAM_EXT_TEXT_TRACK_INFO_s Text;     /**< The information of the text track (See AMP_TEXT_TRACK_INFO_s.) */
    } Info;
    UINT8 TrackType;           /**< Track type (See AMP_MEDIA_TRACK_TYPE_e.) */
} AMP_STREAM_EXT_MEDIA_TRACK_INFO_s;

/**
 * Ext movie information
 */
typedef struct {
    AMP_STREAM_EXT_MEDIA_TRACK_INFO_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA];    /**< The tracks in the movie (See AMP_STREAM_EXT_MEDIA_TRACK_INFO_s.) */
    UINT8 TrackCount;       /**< The number of tracks in the movie */
} AMP_STREAM_EXT_MOVIE_INFO_s;

/**
 * Ext sound information
 */
typedef struct {
    AMP_STREAM_EXT_MEDIA_TRACK_INFO_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA];    /**< The tracks in the sound (See AMP_STREAM_EXT_MEDIA_TRACK_INFO_s.) */
    UINT8 TrackCount;       /**< The number of tracks in the sound */
} AMP_STREAM_EXT_SOUND_INFO_s;

/**
 * Ext image frame information
 */
typedef struct {
    UINT32 SeqNum;          /**< The sequence number of the image */
    UINT32 Type;            /**< Image type */
    EXIF_INFO_s ExifInfo;   /**< The EXIF INFO (See EXIF_INFO_s.) */
    UINT32 Pos;             /**< The position of the image */
    UINT32 Size;            /**< The size of the image */
    UINT16 Width;           /**< Image width */
    UINT16 Height;          /**< Image height */
} AMP_STREAM_EXT_IMAGE_FRAME_INFO_s;

/**
 * Ext image information
 */
typedef struct {
    AMP_STREAM_EXT_IMAGE_FRAME_INFO_s Frame[AMP_FORMAT_MAX_FRAME_PER_IMAGE];   /**< The frames in an image (See AMP_STREAM_EXT_IMAGE_FRAME_INFO_s.) */
    UINT8 UsedFrame;    /**< The number of frames stored in an image (the used entries of Frame[AMP_FORMAT_MAX_FRAME_PER_IMAGE]) */
    UINT8 Endian;       /**< The endian type of an image (e.g., Big endian and little endian) */
} AMP_STREAM_EXT_IMAGE_INFO_s;

/**
 * Ext media information
 */
typedef struct {
    union {
        AMP_STREAM_EXT_IMAGE_INFO_s Image;  /**< Image information (See AMP_STREAM_EXT_IMAGE_INFO_s.) */
        AMP_STREAM_EXT_MOVIE_INFO_s Movie;  /**< Movie information (See AMP_STREAM_EXT_MOVIE_INFO_s.) */
        AMP_STREAM_EXT_SOUND_INFO_s Sound;  /**< Sound information (See AMP_STREAM_EXT_SOUND_INFO_s.) */
    } Info;
    char Date[AMP_FORMAT_MAX_DATE_SIZE];   /**< The creation date of the media */
    char Time[AMP_FORMAT_MAX_TIME_SIZE];   /**< The creation time of the media */
    UINT8 MediaType;                        /**< Media type (See AMP_MEDIA_INFO_TYPE_e.) */
} AMP_STREAM_EXT_MEDIA_INFO_s;

/**
 * The data structure used by external stream to describe the frame info
 */
typedef struct {
    UINT32 FrameNo;     /**< Frame number */
    UINT64 PTS;         /**< Presentation time stamp */
    UINT32 Type;        /**< Frame type */
    UINT32 Size;        /**< Frame size */
    UINT8 TrackId;      /**< Track id */
    BOOL8 Complete;     /**< The flag used to indicate if the frame is complete (for slice encoding) */
    UINT8 Field;        /**< The field of this frame (used in interlace mode) */
} AMP_STREAM_EXT_FRAME_INFO_s;

/**
 *  Stream open mode
 */
typedef enum {
    AMP_STREAM_MODE_RDONLY = 1, /**< Read only */
    AMP_STREAM_MODE_WRONLY = 2, /**< Write only */
    AMP_STREAM_MODE_RDWR = 3,   /**< Read write */
    AMP_STREAM_MODE_WRRD = 4,   /**< Write read */
    AMP_STREAM_MODE_MAX = 5     /**< Max value (used to check the range of mode) */
} AMP_STREAM_MODE_e;

struct _AMP_STREAM_s_;

/**
 * Stream handler
 */
typedef struct {
    struct _AMP_STREAM_s_ *Func;   /**< Stream interface */
} AMP_STREAM_HDLR_s;

/**
 * Stream interface
 */
typedef struct _AMP_STREAM_s_ {
    int (*Open)(AMP_STREAM_HDLR_s *, char *, UINT32);      /**< The interface to open a stream handler (*pHdlr, *szName, Mode) */
    int (*Close)(AMP_STREAM_HDLR_s *);                      /**< The interface to close a stream handler (*pHdlr) */
    int (*Read)(AMP_STREAM_HDLR_s *, UINT32, UINT8 *);      /**< The interface to read data from the stream handler (*pHdlr, Size, *pBuffer) */
    int (*Write)(AMP_STREAM_HDLR_s *, UINT32, UINT8 *);     /**< The interface to write data to the stream handler (*pHdlr, Size, *pBuffer) */
    int (*Seek)(AMP_STREAM_HDLR_s *, UINT64, int);          /**< The interface to seek the stream handler (*pHdlr, llPos, iOrig) */
    INT64 (*GetPos)(AMP_STREAM_HDLR_s *);                   /**< The interface to get the current position of the stream handler (*pHdlr) */
    INT64 (*GetLength)(AMP_STREAM_HDLR_s *);                /**< The interface to get length of the stream handler (*pHdlr) */
    UINT64 (*GetFreeSpace)(AMP_STREAM_HDLR_s *);            /**< The interface to get the free space of the storage used by the stream handler (*pHdlr) */
    int (*Func)(AMP_STREAM_HDLR_s *, UINT32, UINT32);       /**< The interface to execute special stream commands (*pHdlr, nCmd, nParam) */
} AMP_STREAM_s;

/**
 *  @}
 */

#endif /* STREAM_H_ */

