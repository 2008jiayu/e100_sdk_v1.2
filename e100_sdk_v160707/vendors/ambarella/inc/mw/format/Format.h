/**
 *  @file Format.h
 *
 *  Format common function header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __FORMAT_H__
#define __FORMAT_H__

/**
 * @defgroup Format
 * @brief Media container implementation
 *
 * The definition of common functions and common media containers
 *
 */

/**
 * @addtogroup Format
 * @{
 */
#include <stream/Stream.h>
#include <fifo/Fifo.h>
#include <index/Index.h>

/**< Include private data */
#include <format/Iso.h>
#include <format/Ext.h>
#include <format/Exif.h>
#include <format/Matroska.h>

#define AMP_FORMAT_IS_EOS_FRAME(x)    ((x)->Size == AMP_FIFO_MARK_EOS)    /**< The macro to check if the frame is a EOS frame */
#define AMP_FORMAT_IS_PAUSE_FRAME(x)    ((x)->Size == AMP_FIFO_MARK_EOS_PAUSE)    /**< The macro to check if the frame is a PAUSE EOS frame */
#define AMP_FORMAT_DURATION(fno, delta)   ((UINT64)(fno) * (UINT64)(delta)) /**< The macro to calculate the duration by the frame count and frame delta */

/**
 * Format direction
 * The item is used in Demuxing/Editing Format modules. In Demuxing Format modules, it indicates a demuxing direction. In Editing Format modules, it indicates the directions of file reading and index.
 */
typedef enum {
    AMP_FORMAT_DIR_FORWARD = 0x00,  /**< Forward direction */
    AMP_FORMAT_DIR_BACKWARD = 0x01  /**< Backward direction */
} AMP_FORMAT_DIR_e;

/**
 * Media Info object status
 * The item specifies the status of a Media Info object.
 */
typedef enum {
    AMP_MEDIA_INFO_STATUS_EMPTY = 0x00,    /**< The status indicating that a Media Info object is not in use */
    AMP_MEDIA_INFO_STATUS_DIRTY = 0x01,    /**< The status indicating that a Media Info object is in use (still under recording/parsing) */
    AMP_MEDIA_INFO_STATUS_READY = 0x02,    /**< The status indicating that a Media Info object is ready (already parsed) */
    AMP_MEDIA_INFO_STATUS_MAX = 0x03       /**< Maximum value (It is used to check the range of a status.) */
} AMP_MEDIA_INFO_STATUS_e;

#define MEDIA_INFO_LOCKED 0x80 /**< The bit mask used to indicate that a Media Info object is locked */

/**
 * Media Info type
 */
typedef enum {
    AMP_MEDIA_INFO_MOVIE = 0x01, /**< Movie Info type */
    AMP_MEDIA_INFO_IMAGE = 0x02, /**< Image Info type */
    AMP_MEDIA_INFO_SOUND = 0x03, /**< Sound Info type */
    AMP_MEDIA_INFO_MAX = 0x04    /**< Maximum value (It is used to check the range of a Media Info type) */
} AMP_MEDIA_INFO_TYPE_e;

/**
 * Format event
 * The item specifies the all of the Format events.
 */
typedef enum {
    AMP_FORMAT_EVENT_PAUSE = 6,                 /**< The event raised in reaching a PAUSE EOS */
    AMP_FORMAT_EVENT_FIFO_FULL = 5,             /**< The event raised when a FIFO is full */
    AMP_FORMAT_EVENT_EOS = 4,                   /**< The event raised in reaching an EOS */
    AMP_FORMAT_EVENT_REACH_DURATION_LIMIT = 3,  /**< The event raised in reaching the limitation of stream duration */
    AMP_FORMAT_EVENT_REACH_SIZE_LIMIT = 2,      /**< The event raised in reaching the limitation of stream size */
    AMP_FORMAT_EVENT_DATA_EMPTY = 1,            /**< The event raised when no more data to be processed */
    AMP_FORMAT_EVENT_OK = 0,                    /**< The event raised when an operation is successful */
    AMP_FORMAT_EVENT_IO_ERROR = -1,             /**< The event raised when an I/O error occurs */
    AMP_FORMAT_EVENT_FIFO_ERROR = -2,           /**< The event raised when a FIFO error occurs */
    AMP_FORMAT_EVENT_GENERAL_ERROR = -3         /**< The event raised when an operation error occurs (not I/O or FIFO error) */
} AMP_FORMAT_EVENT_e;

/**
 * Media track type (in UINT8)
 */
typedef enum {
    AMP_MEDIA_TRACK_TYPE_VIDEO = 0x01,  /**< The video track type */
    AMP_MEDIA_TRACK_TYPE_AUDIO = 0x02,  /**< The audio track type */
    AMP_MEDIA_TRACK_TYPE_TEXT = 0x03,   /**< The text track type */
    AMP_MEDIA_TRACK_TYPE_MAX = 0x04     /**< Maximum value (It is used to check the range of a media track type.) */
} AMP_MEDIA_TRACK_TYPE_e;

/**
 * Video track information
 */
typedef struct {
    UINT32 CodecTimeScale;          /**< The time scale of a codec (The item is only used in AmpFormat_ConvertPTS.) */
    UINT32 GOPSize;                 /**< The number of pictures between IDR pictures */
    UINT32 FrameCountAfterResume;   /**< The number of frames after resuming a video */
    UINT64 RefDTS;                  /**< The DTS of the frame with PTS 0 (If a GOP has B frames, the DTS of the IDR frame is 0 references.) */
    UINT64 InitPTS;                 /**< The DSP PTS of the first frame (The item is only used in AmpFormat_ConvertPTS.) */
    UINT64 PTS;                     /**< Video PTS */
    UINT64 DiffPTS;                 /**< For the case that the frame with the minimum PTS is B frame (The item is only used in AmpFormat_ConvertPTS.) */
    UINT16 PixelArX;                /**< The aspect ratio X of the pixel (If the value is not 1, it means the pixel is not square.) */
    UINT16 PixelArY;                /**< The aspect ratio Y of the pixel (if the value is not 1, it means the pixel is not square.) */
    UINT16 Width;                   /**< Video width */
    UINT16 Height;                  /**< Video height */
    UINT16 M;                       /**< The number of pictures between reference pictures (IDR, I, P) */
    UINT16 N;                       /**< The number of pictures between I pictures */
    BOOL8 IsDefault;                /**< The flag indicating the track is the default video track  */
    UINT8 Mode;                     /**< The value indicating the picture mode of the video (It has progressive and interlaced modes. Interlaced mode has Field Per Sample and Frame Per Sample. See AMP_VIDEO_MODE_s.) */
    BOOL8 ClosedGOP;                /**< The flag indicating that the structure of a video track is closed GOP (The structure of the closed GOP is I P B B P B B. The structure of the open GOP is I B B P B B. If the functions of resuming or auto splitting a video are enabled, the value is always false, i.e., open GOP.) */
    UINT8 VFR;                      /**< The factor of the variable frame rate (For example, 2 and 4 mean that their frame rate are 1/2 and 1/4 respectively. (e.g., VFR = 2, 60P->30P)) */
    UINT8 ColorStyle;               /**< The color space of the video (The value 0 is for TV. The value 1 is for PC.) */
    UINT8 EntropyMode;              /**< The entropy mode of the H.264 bitstream (If the mode is CAVLC, the value is 0. If the mode is CABAC, the value is 1.) */
    BOOL8 IsVFR;                    /**< The flag indicating if a track is variable frame rate (If the value is TRUE and this is a VFR track. If the value is FALSE, this is a Fix frame rate track.) */
} AMP_VIDEO_TRACK_INFO_s;

/**
 * Audio track information
 */
typedef struct {
    UINT32 SampleRate;          /**< The sample rate (Hz) of the audio track */
    BOOL8 IsDefault;            /**< The flag indicating the track is the default audio track */
    UINT8 Channels;             /**< The number of channels in the audio track */
    UINT8 BitsPerSample;        /**< The bits per sample of the audio track (e.g., 8 bits and 16 bits) */
    UINT8 Endian;               /**< The endian type of the audio track (e.g., Big endian or little endian) */
} AMP_AUDIO_TRACK_INFO_s;

/**
 * Text track information
 */
typedef struct {
    BOOL8 IsDefault;            /**< The flag indicating the track is the default text track  */
} AMP_TEXT_TRACK_INFO_s;

/**
 * Media track information
 */
typedef struct {
    UINT32 MediaId;             /**< The media type of the track (The ID is a media ID. See AMP_FORMAT_MID_e.) */
    UINT32 TimeScale;           /**< The ticks per second */
    UINT32 OrigTimeScale;       /**< The original TimeScale */
    UINT32 TimePerFrame;        /**< The ticks per frame */
    UINT32 FrameNo;             /**< Frame number (It is a logical number. In muxing or demuxing, the value is the frame number handled currently. If Demuxer is forward demuxing, the number will progressively increases. If Demuxer is backward demuxing, the number will progressively decreases.) */
    UINT32 FrameCount;          /**< The count of frames in the track */
    UINT64 InitDTS;             /**< The initial value of the DTS (If a file is a split file, this specifies the start decode offset which the value generated by the previous clip.) */
    UINT64 DTS;                 /**< Decode time stamp (The value is based on the time scale of the track. In Muxer, the value is the DTS of next frame; in Demuxer, the value is the duration of the track.) */
    UINT64 NextDTS;             /**< Next Decode time stamp (In Muxer, it predicates the DTS of the next frame. In Demuxer, the value is the same as DTS.) */
    AMP_FIFO_HDLR_s *Fifo;      /**< The FIFO handler of the track (Each track has an individual FIFO handler.) */
    UINT8 *BufferBase;          /**< The start address of the FIFO buffer (Users push data to the FIFO, the FIFO will write the data to its buffer according to the address.) */
    UINT8 *BufferLimit;         /**< The end address of the FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
    union {
        AMP_VIDEO_TRACK_INFO_s Video;   /**< The information of the video track (See AMP_VIDEO_TRACK_INFO_s.) */
        AMP_AUDIO_TRACK_INFO_s Audio;   /**< The information of the audio track (See AMP_AUDIO_TRACK_INFO_s.) */
        AMP_TEXT_TRACK_INFO_s Text;     /**< The information of the text track (See AMP_TEXT_TRACK_INFO_s.) */
    } Info;
    UINT8 TrackId;              /**< Track id (Every track ID is unique.) */
    UINT8 TrackType;            /**< Track type (See AMP_MEDIA_TRACK_TYPE_e.) */
} AMP_MEDIA_TRACK_INFO_s;

/**
 * The process information in muxing
 */
typedef struct {
    UINT64 ResumeDTS[AMP_FORMAT_MAX_TRACK_PER_MEDIA];   /**< The DTS of the last resuming */
} AMP_MUX_PROC_INFO_s;

/**
 * The process information of tracks in demuxing
 */
typedef struct {
    UINT64 EndPTS[AMP_FORMAT_MAX_TRACK_PER_MEDIA];      /**< The largest PTS of the frame that has been fed to FIFO */
} AMP_DMX_TRACK_PROC_INFO_s;

/**
 * The process information in demuxing
 */
typedef struct {
    AMP_DMX_TRACK_PROC_INFO_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA];    /**< The demuxing information of each track (See AMP_DMX_TRACK_PROC_INFO_s.) */
} AMP_DMX_PROC_INFO_s;

/**
 * Thumb frame information
 */
typedef struct {
    UINT32 SeqNum;          /**< The sequence number */
    UINT32 Type;            /**< The image type */
    UINT32 Pos;             /**< The image position */
    UINT32 Size;            /**< The image size */
    UINT16 Width;           /**< The image width */
    UINT16 Height;          /**< The image height */
} AMP_THUMB_FRAME_INFO_s;

/**
 * Movie Info object
 */
typedef struct {
    MEDIA_INFO_BASE;    /**< The base information of a media (See MEDIA_INFO_BASE.) */
    AMP_MEDIA_TRACK_INFO_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA];   /**< The tracks (See AMP_MEDIA_TRACK_INFO_s.) */
    union {
        AMP_MUX_PROC_INFO_s Mux;    /**< The muxing information (See AMP_MUX_PROC_INFO_s.) */
        AMP_DMX_PROC_INFO_s Dmx;    /**< The demuxing information (See AMP_DMX_PROC_INFO_s.) */
    } ProcInfo;
    union {
        AMP_EXT_PRIV_INFO_s Ext;    /**< The private data for the information of Ext format. (See AMP_EXT_PRIV_INFO_s.) */
        AMP_ISO_PRIV_INFO_s Iso;    /**< The private data for the information of Iso format. (See AMP_ISO_PRIV_INFO_s.) */
        AMP_MATROSKA_PRIV_INFO_s Matroska;   /**< The private data for the information of Matroska format. (See AMP_ISO_PRIV_INFO_s.) */
    } PrivInfo;
    AMP_FIFO_HDLR_s *ThmFifo;       /**< The FIFO handler for getting image thumbnail */
    UINT8 *ThmBase;                 /**< The start address of the buffer for image thumbnail */
    UINT8 *ThmLimit;                /**< The end address of the buffer for image thumbnail */
    AMP_THUMB_FRAME_INFO_s Thumb[AMP_FORMAT_MAX_FRAME_PER_IMAGE];   /**< The thumbnail frames (See AMP_IMAGE_THUMB_INFO_s.) */
    UINT8 TrackCount;               /**< The number of tracks in a movie */
    UINT8 ThumbCount;               /**< The number of image thumbnail */
} AMP_MOVIE_INFO_s;

/**
 * Image frame information
 */
typedef struct {
    UINT32 SeqNum;          /**< The sequence number of an image frame */
    EXIF_INFO_s ExifInfo;   /**< Exif information (See EXIF_INFO_s.) */
    GPS_INFO_s GpsInfo;     /**< GPS information (See GPS_INFO_s.) */
    UINT32 Type;            /**< Image type */
    UINT32 Pos;             /**< Image position */
    UINT32 Size;            /**< Image size */
    UINT16 Width;           /**< Image width */
    UINT16 Height;          /**< Image height */
} AMP_IMAGE_FRAME_INFO_s;

/**
 * Image Info object
 */
typedef struct {
    MEDIA_INFO_BASE;        /**< The base information of a media (See MEDIA_INFO_BASE.) */
    AMP_FIFO_HDLR_s *Fifo;  /**< The FIFO handler of an image (Each image has individual FIFO handler.) */
    UINT8 *BufferBase;      /**< The start address of the FIFO buffer (User pushs data to the FIFO. The FIFO will write data to the address of the buffer.) */
    UINT8 *BufferLimit;     /**< The end address of the FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
    AMP_IMAGE_FRAME_INFO_s Frame[AMP_FORMAT_MAX_FRAME_PER_IMAGE]; /**< The image frames (See AMP_IMAGE_FRAME_INFO_s.) */
    union {
        AMP_EXT_PRIV_INFO_s Ext;    /**< The private data for the information of Ext Format (See AMP_EXT_PRIV_INFO_s.) */
        AMP_EXIF_PRIV_INFO_s Exif;  /**< The private data for the information of Exif Format (See AMP_EXIF_PRIV_INFO_s.) */
    } PrivInfo;
    UINT8 UsedFrame;    /**< The number of frames stored in an image file (the used entries of Frame[AMP_FORMAT_MAX_FRAME_PER_IMAGE]) */
    UINT8 TotalFrame;   /**< The total number of frames referred to by an image (Muxer only) */
    UINT8 Endian;       /**< The endian type of an image (e.g., big endian or little endian) */
} AMP_IMAGE_INFO_s;

/**
 * Sound Info object
 */
typedef struct {
    MEDIA_INFO_BASE;    /**< The base information of a media (See MEDIA_INFO_BASE.) */
    AMP_MEDIA_TRACK_INFO_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA]; /**< The tracks in a sound file (See AMP_MEDIA_TRACK_INFO_s.) */
    union {
        AMP_EXT_PRIV_INFO_s Ext;    /**< The private data for the information of Ext Format (See AMP_EXT_PRIV_INFO_s.) */
        AMP_ISO_PRIV_INFO_s Iso;    /**< The private data for the information of Iso Format (See AMP_ISO_PRIV_INFO_s.) */
        AMP_MATROSKA_PRIV_INFO_s Matroska;   /**< The private data for the information of Matroska Format (See AMP_ISO_PRIV_INFO_s.) */
    } PrivInfo;
    UINT8 TrackCount;   /**< The number of tracks */
} AMP_SOUND_INFO_s;

/**
 * Frame information
 */
typedef struct {
    UINT8 FrameType;        /**< Frame type (See AMP_FIFO_FRMAE_TYPE_e.) */
    UINT32 FrameSize;       /**< Frame size (byte) */
    UINT64 FramePos;        /**< Frame position */
    UINT64 PTS;             /**< Frame PTS */
    UINT64 DTS;             /**< Frame DTS */
} AMP_FRAME_INFO_s;

/**
 * The parameters of movie muxing
 */
typedef struct {
    UINT32 MaxDuration;     /**< The maximum duration of a movie file */
    UINT64 MaxSize;         /**< The maximum size of a movie file */
} AMP_MUX_PARAM_MOVIE_s;

/**
 * The parameters of image muxing
 */
typedef struct {
    UINT8 Resv[4];          /**< Reserved */
} AMP_MUX_PARAM_IMAGE_s;

/**
 * The parameters of sound muxing
 */
typedef struct {
    UINT32 MaxDuration;     /**< The maximum duration of a sound file */
    UINT64 MaxSize;         /**< The maximum size of a sound file */
} AMP_MUX_PARAM_SOUND_s;

struct _AMP_MUX_FORMAT_s_;  /**< The interface of Muxing Format */

/**
 * Muxing Format handler
 */
typedef struct {
    struct _AMP_MUX_FORMAT_s_ *Func;    /**< The interface of Muxing Format (See AMP_MUX_FORMAT_s.) */
    AMP_MEDIA_INFO_s *Media;            /**< Media Info object */
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    union {
        AMP_MUX_PARAM_MOVIE_s Movie;    /**< The muxing parameters of a Movie Info object (See AMP_MUX_PARAM_MOVIE_s.) */
        AMP_MUX_PARAM_SOUND_s Sound;    /**< The muxing parameters of a Sound Info object (See AMP_MUX_PARAM_SOUND_s.) */
        AMP_MUX_PARAM_IMAGE_s Image;    /**< The muxing parameters of a Image Info object (See AMP_MUX_PARAM_IMAGE_s.) */
    } Param;
} AMP_MUX_FORMAT_HDLR_s;

/**
 * Muxer close mode (in UINT8)
 */
typedef enum {
    AMP_MUX_FORMAT_CLOSE_DEFAULT = 0x00,    /**< Trim mode (Muxer will trim all of the track duration, and keep the duration of all tracks being shorter than the one of the default track.) */
    AMP_MUX_FORMAT_CLOSE_NOT_END = 0x01,    /**< No-trim mode (Muxer will keep the duration of all tracks.) */
    AMP_MUX_FORMAT_CLOSE_RECOVER = 0x40,    /**< Recover mode (Muxer will keep the media index and clip for recovering.) */
    AMP_MUX_FORMAT_CLOSE_ABORT = 0x80       /**< Abort mode (Muxer will remove the index.) */
} AMP_MUX_FORMAT_CLOSE_MODE_e;

/**
 * The interface of Muxing Format (called by Muxer only)
 */
typedef struct _AMP_MUX_FORMAT_s_ {
    int (*Open)(AMP_MUX_FORMAT_HDLR_s *);                       /**< The interface to open a Format handler (Handler) */
    int (*Close)(AMP_MUX_FORMAT_HDLR_s *, UINT8);               /**< The interface to close a Format handler (Handler, Close mode) */
    int (*Process)(AMP_MUX_FORMAT_HDLR_s *, UINT64);            /**< The interface to perform the muxing of a media (Handler, The data unit to mux) */
    UINT64 (*GetMediaTime)(AMP_MUX_FORMAT_HDLR_s *);            /**< The interface to get the current media time (ms) (Handler) */
    BOOL (*CheckDataReady)(AMP_MUX_FORMAT_HDLR_s *, UINT32);    /**< The interface to check if the data is ready (Handler, Delay time) */
    int (*Func)(AMP_MUX_FORMAT_HDLR_s *, UINT32, UINT32);       /**< The interface to to execute special commands (Handler, Parameter1, Parameter2) */
} AMP_MUX_FORMAT_s;

/**
 *  The parameters of movie demuxing
 */
typedef struct {
    UINT8 Direction;    /**< The direction in demuxing (See AMP_FORMAT_DIR_e.) */
    UINT8 Speed;        /**< Demuxing speed (e.q., 1, 2, 4, 8, and 16) */
    BOOL8 End;          /**< Notify EOS once the last frame is fed */
} AMP_DMX_PARAM_MOVIE_s;

/**
 *  The parameters of image demuxing
 */
typedef struct {
    UINT8 Resv[4];      /**< Reserved */
} AMP_DMX_PARAM_IMAGE_s;

/**
 *  The parameters of sound demuxing
 */
typedef struct {
    UINT8 Direction;    /**< The direction in demuxing (See AMP_FORMAT_DIR_e.) */
    UINT8 Speed;        /**< Demuxing speed (e.q., 1, 2, 4, 8, and 16) */
} AMP_DMX_PARAM_SOUND_s;

struct _AMP_DMX_FORMAT_s_;  /**< The interface of Demuxing Format (called by Demuxer only) */

/**
 * Demuxing Format handler
 */
typedef struct {
    struct _AMP_DMX_FORMAT_s_ *Func;    /**< The interface of Demuxing Format (See AMP_DMX_FORMAT_s.) */
    AMP_MEDIA_INFO_s *Media;            /**< Media Info object */
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    union {
        AMP_DMX_PARAM_MOVIE_s Movie;    /**< The demuxing parameters of a Movie Info object (See AMP_DMX_PARAM_MOVIE_s.) */
        AMP_DMX_PARAM_SOUND_s Sound;    /**< The demuxing parameters of a Sound Info object (See AMP_DMX_PARAM_SOUND_s.) */
        AMP_DMX_PARAM_IMAGE_s Image;    /**< The demuxing parameters of an Image Info object (See AMP_DMX_PARAM_IMAGE_s.) */
    } Param;
} AMP_DMX_FORMAT_HDLR_s;

/**
 *  The interface of Demuxing Format (called by Demuxer only)
 */
typedef struct _AMP_DMX_FORMAT_s_ {
    int (*Open)(AMP_DMX_FORMAT_HDLR_s *);                               /**< The interface to open a Format handler (Handler) */
    int (*Close)(AMP_DMX_FORMAT_HDLR_s *);                              /**< The interface to close a Format handler (Handler) */
    int (*Process)(AMP_DMX_FORMAT_HDLR_s *, UINT64);                    /**< The interface to perform the demuxing of a media (Handler, the data unit to demux) */
    UINT64 (*GetMediaTime)(AMP_DMX_FORMAT_HDLR_s *);                    /**< The interface to get the current media time (ms) (Handler) */
    int (*Seek)(AMP_DMX_FORMAT_HDLR_s *, UINT64, UINT32, UINT32);       /**< The interface to seek to a specified time offset (ms) and continue with the specified direction and speed (Handler, Target time, Direction, Speed) */
    int (*FeedFrame)(AMP_DMX_FORMAT_HDLR_s *, UINT8, UINT64, UINT8);    /**< The interface to feed a specified frame to FIFO (Handler, Track Id, Target time, Frame type) */
    int (*Func)(AMP_DMX_FORMAT_HDLR_s *, UINT32, UINT32);               /**< The interface to execute special commands (Handler, Parameter1, Parameter2) */
} AMP_DMX_FORMAT_s;

/**
 * Editing Format handler
 */
typedef struct {
    struct _AMP_EDT_FORMAT_s_ *Func;    /**< The interface of Editing Format (See AMP_EDT_FORMAT_s.) */
    AMP_CALLBACK_f OnEvent;
} AMP_EDT_FORMAT_HDLR_s;

/**
 * The interface of Editing Format
 */
typedef struct _AMP_EDT_FORMAT_s_ {
    int (*GetMediaInfo)(char *, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s **);                                                                             /**< The interface to get media information (File name, Stream handler, Return media information) */
    int (*Crop2New)(AMP_EDT_FORMAT_HDLR_s *, UINT32, UINT32, BOOL, AMP_STREAM_HDLR_s *, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s *, AMP_MEDIA_INFO_s *);   /**< The interface to prepare the materials required by cropping a part of an input file to create a new one (Handler, Crop start time, Crop end time, Trim flag, Origional file stream, New file stream, Origional media information, New media information) */
    INT64 (*Divide)(AMP_EDT_FORMAT_HDLR_s *, UINT32, BOOL, UINT8, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s *, AMP_MEDIA_INFO_s *, UINT32);                 /**< The interface to prepare the materials required by dividing an input file into two parts according to a specified time (Handler, Divide time, Trim flag, Round mode, Origional file stream, Origional media information, New media information, alignment flag) */
    int (*Merge)(AMP_EDT_FORMAT_HDLR_s *, BOOL, BOOL, AMP_STREAM_HDLR_s *, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s *, AMP_MEDIA_INFO_s *, UINT32);        /**< The interface to prepare the materials required by merging two input files into a single one (Handler, Trim flag, Directed flag, Origional file stream 1, Origional file stream 2, Origional media information 1, Origional media information 2, alignment flag) */
    int (*Recover)(AMP_EDT_FORMAT_HDLR_s *, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s *);                                                                                       /**< The interface to prepare the materials required by recovering a bad or abnormal closed recording file (Handler, File stream, Trim flag, File name) */
    int (*Finalize)(AMP_EDT_FORMAT_HDLR_s *, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s *);                                                                  /**< The interface to finalized the specified media file (Handler, File stream, Media information) */
    int (*UpdateAtom)(AMP_EDT_FORMAT_HDLR_s *, UINT32, void *, AMP_STREAM_HDLR_s *, AMP_MEDIA_INFO_s *);                                                /**< The interface to update an atom (Handler, Atom Id, Parameter, File stream, Media information) */
    int (*Func)(AMP_EDT_FORMAT_HDLR_s *, UINT32, void *);                                                                                               /**< The interface to execute special commands (Handler, Command, Parameter) */
} AMP_EDT_FORMAT_s;

/**
 * The configuration for initializing the Format module
 */
typedef struct {
    UINT32 MaxMovie;        /**< The maximum number of Movie Info objects in the Format module */
    UINT32 MaxImage;        /**< The maximum number of Image Info objects in the Format module */
    UINT32 MaxSound;        /**< The maximum number of Sound Info objects in the Format module */
    UINT8 *Buffer;          /**< The work buffer of the Format module */
    UINT32 BufferSize;      /**< The work buffer size */
    UINT8 AmbaMainVer;      /**< The main verson of AMBA Box. */
    UINT8 AmbaSubVer;       /**< The sub verson of AMBA Box. */
} AMP_FORMAT_INIT_CFG_s;

/**
 * The prototype of functions parsing media format
 */
typedef int (*AMP_DMX_FORMAT_PARSE_FP)(AMP_MEDIA_INFO_s *, AMP_STREAM_HDLR_s *);

/**
 * The function prototype of putting ISO index to moov
 */
typedef int (*AMP_ISO_PUT_MOOV_FP)(AMP_MEDIA_INFO_s *, AMP_STREAM_HDLR_s *, void *, void *);

/**
 * The function prototype of getting moov size when EnableMoovHead
 */
typedef UINT32 (*AMP_ISO_GET_MOOV_SIZE_FP)(AMP_MEDIA_INFO_s *, void *, void *);


/**
 * Get the default configuration for initializing the Format module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_GetInitDefaultCfg(AMP_FORMAT_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the Format module.
 * @param [in] maxMovie The maximum number of Movie Info objects in the Format module
 * @param [in] maxImage The maximum number of Image info objects in the Format module
 * @param [in] maxSound The maximum number of Sound info objects in the Format module
 * @return The required buffer size
 */
extern UINT32 AmpFormat_GetRequiredBufferSize(UINT32 maxMovie, UINT32 maxImage, UINT32 maxSound);

/**
 * Initialize the Format module.
 * @param [in] config The configuration used to initialize the Format module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_Init(AMP_FORMAT_INIT_CFG_s *config);

/**
 * New a Movie Info object (used by Muxer).
 * @param [in] name The name of a movie file
 * @param [out] info The returned Movie Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_NewMovieInfo(char *name, AMP_MOVIE_INFO_s **info);

/**
 * Get a Movie Info object, and parse it if it is not loaded (used by Demuxer).
 * @param [in] name The name of a movie file
 * @param [in] parse The function to parse the Movie Info object
 * @param [in] stream The stream to access the movie file
 * @param [out] info The returned Movie Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_GetMovieInfo(char *name, AMP_DMX_FORMAT_PARSE_FP parse, AMP_STREAM_HDLR_s *stream, AMP_MOVIE_INFO_s **info);

/**
 * Release a Movie Info object (unlock the media).
 * @param [in] info A Movie Info object being released
 * @param [in] remove Remove the Movie Info object from the Format module after it is closed
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_RelMovieInfo(AMP_MOVIE_INFO_s *info, BOOL remove);

/**
 * Copy a Movie Info object.
 * @param [out] dstInfo The destination Movie Info object
 * @param [in] srcInfo The source Movie Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_CopyMovieInfo(AMP_MOVIE_INFO_s *dstInfo, AMP_MOVIE_INFO_s *srcInfo);

/**
 * New an Image Info object.
 * @param [in] name The name of an image file
 * @param [out] info The returned Image Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_NewImageInfo(char *name, AMP_IMAGE_INFO_s **info);

/**
 * Get an Image Info object, and parse it if it is not loaded (used by Demuxer).
 * @param [in] name The name of an image file
 * @param [in] parse The function to parse the Image Info object
 * @param [in] stream The stream to access the image file
 * @param [out] info The returned Image Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_GetImageInfo(char *name, AMP_DMX_FORMAT_PARSE_FP parse, AMP_STREAM_HDLR_s *stream, AMP_IMAGE_INFO_s **info);

/**
 * Release an Image Info object (unlock the media).
 * @param [in] info The Image Info object being released
 * @param [in] remove Remove the Image Info object from the Format module after it is closed
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_RelImageInfo(AMP_IMAGE_INFO_s *info, BOOL remove);

/**
 * Copy an Image Info object to a destination object.
 * @param [out] dstInfo The destination Image Info object
 * @param [in] srcInfo The source Image Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_CopyImageInfo(AMP_IMAGE_INFO_s *dstInfo, AMP_IMAGE_INFO_s *srcInfo);

/**
 * New a Sound Info object.
 * @param [in] name The name of a sound file
 * @param [out] info The returned Sound Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_NewSoundInfo(char *name, AMP_SOUND_INFO_s **info);

/**
 * Get a Sound Info object, and parse it if it is not loaded (used by Demuxer).
 * @param [in] name The name of a sound file
 * @param [in] parse The function to parse the Sound Info object
 * @param [in] stream The stream to access the sound file
 * @param [out] info The returned Sound Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_GetSoundInfo(char *name, AMP_DMX_FORMAT_PARSE_FP parse, AMP_STREAM_HDLR_s *stream, AMP_SOUND_INFO_s **info);

/**
 * Release a Sound Info object (unlock the media).
 * @param [in] info The Sound Info being released
 * @param [in] remove Remove the Sound Info object from the Format module after it is closed
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_RelSoundInfo(AMP_SOUND_INFO_s *info, BOOL remove);

/**
 * Copy a Sound Info object to a destination object.
 * @param [out] dstInfo The destination Sound Info object
 * @param [in] srcInfo The source Sound Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_CopySoundInfo(AMP_SOUND_INFO_s *dstInfo, AMP_SOUND_INFO_s *srcInfo);

/**
 * Flush all Media Info objects in Format module.
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFormat_FlushMediaInfo(void);

/**
 * @}
 */
#endif /* FORMAT_H_ */

