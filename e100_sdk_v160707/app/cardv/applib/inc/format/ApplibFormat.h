/**
 * @file src/app/cardv/applib/inc/format/ApplibFormat.h
 *
 * Header of MW format utility
 *
 * History:
 *    2013/09/03 - [Martin Lai] created file
 *    2013/12/11 - [phcheng] Add a function to identify file format based on filename extension
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

#ifndef APPLIB_FORMAT_H_
#define APPLIB_FORMAT_H_
/**
 * @defgroup Format
 * @brief Format related function implementation
 *
 * Implementation of
 * Format related function
 *
 */

/**
* @defgroup ApplibFormat
* @brief MW format utility
*
*
*/

/**
 * @addtogroup ApplibFormat
 * @ingroup Format
 * @{
 */
#include <applib.h>
#include <format/Format.h>

__BEGIN_C_PROTO__

/**
 * File formats are defined as unsigned 32-bits dword
 * +------------+------------+
 * | bit 31-16  | bit 15-0   |
 * +------------+------------+
 * | Media Type |   0x0000   |
 * +------------+------------+
 */
;
#define APPLIB_GET_MOVIE_FORMAT(x) ((AMP_MEDIA_INFO_MOVIE << 16) + (x))     /**<APPLIB_GET_MOVIE_FORMAT(x)*/
#define APPLIB_GET_IMAGE_FORMAT(x) ((AMP_MEDIA_INFO_IMAGE << 16) + (x))     /**<APPLIB_GET_IMAGE_FORMAT(x)*/
#define APPLIB_GET_SOUND_FORMAT(x) ((AMP_MEDIA_INFO_SOUND << 16) + (x))     /**<APPLIB_GET_SOUND_FORMAT(x)*/
#define APPLIB_GET_MEDIA_TYPE(x)   ((AMP_MEDIA_INFO_TYPE_e)((x) >> 16))     /**<APPLIB_GET_MEDIA_TYPE(x)  */

/**
 * File format
 */
typedef enum _APPLIB_FILE_FORMAT_e_ {
    APPLIB_FILE_FORMAT_UNKNOWN = 0,
    APPLIB_FILE_FORMAT_MP4 = APPLIB_GET_MOVIE_FORMAT(0x0001),/**< MediaType: Movie MP4*/
    APPLIB_FILE_FORMAT_MOV = APPLIB_GET_MOVIE_FORMAT(0x0002),/**< MediaType: Movie MOV*/
    APPLIB_FILE_FORMAT_JPG = APPLIB_GET_IMAGE_FORMAT(0x0001),/**< MediaType: Image JPG*/
    APPLIB_FILE_FORMAT_THM = APPLIB_GET_IMAGE_FORMAT(0x0002),/**< MediaType: Image THM*/
    APPLIB_FILE_FORMAT_MP3 = APPLIB_GET_SOUND_FORMAT(0x0001),/**< MediaType: Sound MP3*/
    APPLIB_FILE_FORMAT_AAC = APPLIB_GET_SOUND_FORMAT(0x0002),/**< MediaType: Sound AAC*/
    APPLIB_FILE_FORMAT_WAV = APPLIB_GET_SOUND_FORMAT(0x0003) /**< MediaType: Sound WAV*/
} APPLIB_FILE_FORMAT_e;

typedef enum _APPLIB_EXIF_ENDIAN_TYPE_e_{
    APPLIB_EXIF_TYPE_BIG_ENDIAN = 0,
    APPLIB_EXIF_TYPE_LITTLE_ENDIAN
} APPLIB_EXIF_ENDIAN_TYPE_e;

/**
*   TIFF_BYTE = 1,      1 byte
*   TIFF_ASCII = 2,     1 byte
*   TIFF_SHORT = 3,     2 bytes
*   TIFF_LONG = 4,      4 bytes
*   TIFF_RATIONAL = 5,  8 bytes
*   TIFF_UNDEFINED = 7, 1 byte
*   TIFF_SLONG = 9,     4 bytes
*   TIFF_SRATIONAL = 10 8 bytes
*/
typedef enum _APPLIB_TAG_TYPE_e_{
    APPLIB_TAG_TYPE_BYTE        = 1,
    APPLIB_TAG_TYPE_ASCII       = 2,
    APPLIB_TAG_TYPE_SHORT       = 3,
    APPLIB_TAG_TYPE_LONG        = 4,
    APPLIB_TAG_TYPE_RATIONAL    = 5,
    APPLIB_TAG_TYPE_UNDEFINED   = 7,
    APPLIB_TAG_TYPE_SLONG       = 9,
    APPLIB_TAG_TYPE_SRATIONAL   = 10,
    APPLIB_TAG_TYPE_TOTAL       = 11,
    APPLIB_TAG_TYPE_POINTER     = 13
} APPLIB_TAG_TYPE_e;

/**
 * GPSIFD is a set of tags for recording information such as GPS information,
 * and is pointed by the TIFF_GPSInfoIFDPointer(34853) tag in IFD0 or IFD1.
 */
typedef enum _APPLIB_GPS_TAG_ID_e_ {
    APPLIB_GPS_TAG_VersionID        = 0,            /**<  GPS tag version */
    APPLIB_GPS_TAG_LatitudeRef,                     /**<  North or south latitude */
    APPLIB_GPS_TAG_Latitude,                        /**<  Latitude */
    APPLIB_GPS_TAG_LongitudeRef,                    /**<  East or west latitude */
    APPLIB_GPS_TAG_Longitude,                       /**<  Longitude */
    APPLIB_GPS_TAG_AltitudeRef,                     /**<  Altitude reference */
    APPLIB_GPS_TAG_Altitude,                        /**<  Altitude */
    APPLIB_GPS_TAG_TimeStamp,                       /**<  GPS time(atomic clock) */
    APPLIB_GPS_TAG_Satellites,                      /**<  GPS satellites used for measurement */
    APPLIB_GPS_TAG_Status,                          /**<  GPS receiver status */
    APPLIB_GPS_TAG_MeasureMode,                     /**<  GPS measurement mode */
    APPLIB_GPS_TAG_DOP,                             /**<  Measurement precision */
    APPLIB_GPS_TAG_SpeedRef,                        /**<  Speed unit */
    APPLIB_GPS_TAG_Speed,                           /**<  Speed of GPS receiver */
    APPLIB_GPS_TAG_TrackRef,                        /**<  Reference for direction of movement */
    APPLIB_GPS_TAG_Track,                           /**<  Direction of movement */
    APPLIB_GPS_TAG_ImgDirectionRef,                 /**<  Reference for direction of image */
    APPLIB_GPS_TAG_ImgDirection,                    /**<  Direction of image */
    APPLIB_GPS_TAG_MapDatum,                        /**<  Geodetic survey data used */
    APPLIB_GPS_TAG_DestLatitudeRef,                 /**<  Reference for latitude of destination */
    APPLIB_GPS_TAG_DestLatitude,                    /**<  Latitude of destination */
    APPLIB_GPS_TAG_DestLongitudeRef,                /**<  Reference for longitude of destination */
    APPLIB_GPS_TAG_DestLongitude,                   /**<  Longitude of destination */
    APPLIB_GPS_TAG_DestBearingRef,                  /**<  Reference for bearing of destination */
    APPLIB_GPS_TAG_DestBearing,                     /**<  Bearing of destination */
    APPLIB_GPS_TAG_DestDistanceRef,                 /**<  Reference for distance of destination */
    APPLIB_GPS_TAG_DestDistance,                    /**<  Distance to destination */
    APPLIB_GPS_TAG_ProcessingMethod,                /**<  Name of GPS processing method */
    APPLIB_GPS_TAG_AreaInformatio,                  /**<  Name of GPS area */
    APPLIB_GPS_TAG_DateStamp,                       /**<  GPS date */
    APPLIB_GPS_TAG_Differential,                    /**<  GPS differential correction */
    APPLIB_GPS_TAG_HPositioningError                /**<  Horizontal position error */
} APPLIB_GPS_TAG_ID_e;

typedef enum _APPLIB_MUXER_TYPE_ID_e_ {
    APPLIB_MUXER_TYPE_MP4 = 0,
    APPLIB_MUXER_TYPE_EVENT,
    APPLIB_MUXER_TYPE_EXIF,
    APPLIB_MUXER_TYPE_EXIF_PIV
} APPLIB_MUXER_TYPE_ID_e;

/**
 * EXIFIFD is a set of tags for recording exif-specific attribute information
 * and is pointed by the TIFF_ExifIFDPointer(34665) tag in IFD0 or IFD1.
 */
typedef enum _APPLIB_EXIF_TAG_ID_e_ {
    APPLIB_EXIF_TAG_ExposureTime    = 0,            /**<  Exposure time, given in seconds */
    APPLIB_EXIF_TAG_FNumber,                        /**<  The F number */
    APPLIB_EXIF_TAG_ExposureProgram,                /**<  The class of the program used to set exposure */
    APPLIB_EXIF_TAG_SpectralSensitivity,            /**<  The spectra sensitivity of each channel of the camera used */
    APPLIB_EXIF_TAG_ISOSpeedRatings,                /**<  ISOSpeedRatings value up to 2.21; PhohographicSentivity in 2.3 */
    APPLIB_EXIF_TAG_OECF,                           /**<  Indicates the OECF specified in ISO 14524 */
    APPLIB_EXIF_TAG_SensitivityType,                /**<  Sensitivity type; defined in exif 2.3 */
    APPLIB_EXIF_TAG_StandardOutputSensitivity,      /**<  The standard output sensitivity of a camera; define in exif 2.3 */
    APPLIB_EXIF_TAG_RecommendedExposureIndex,       /**<  The Recommended exposure index value of a camera; defined in exif 2.3 */
    APPLIB_EXIF_TAG_ISOSpeed,                       /**<  The ISO speed value of a camera; defined in exif 2.3 */
    APPLIB_EXIF_TAG_ISOSpeedLatitudeyyy,            /**<  The ISO speed latitude yyy value of a camera; defined in exif 2.3 */
    APPLIB_EXIF_TAG_ISOSpeedLatitudezzz,            /**<  The ISO speed latitude zzz value of a camera; defined in exif 2.3 */
    APPLIB_EXIF_TAG_ExifVersion,                    /**<  The exif version is supported */
    APPLIB_EXIF_TAG_DateTimeOriginal,               /**<  The Date/Time when the image was generated */
    APPLIB_EXIF_TAG_DateTimeDigitized,              /**<  The Date/Time when the image was stored */
    APPLIB_EXIF_TAG_ComponentsConfiguration,        /**<  Meaning of each component */
    APPLIB_EXIF_TAG_CompressedBitsPerPixel,         /**<  Image compression mode */
    APPLIB_EXIF_TAG_ShutterSpeedValue,              /**<  Shutter speed */
    APPLIB_EXIF_TAG_ApertureValue,                  /**<  The lens aperture */
    APPLIB_EXIF_TAG_BrightnessValue,                /**<  The value of brightness */
    APPLIB_EXIF_TAG_ExposureBiasValue,              /**<  The exposure bias */
    APPLIB_EXIF_TAG_MaxApertureValue,               /**<  The smallest F number of the lens */
    APPLIB_EXIF_TAG_SubjectDistance,                /**<  The distance to the subject */
    APPLIB_EXIF_TAG_MeteringMode,                   /**<  The metering mode */
    APPLIB_EXIF_TAG_LightSource,                    /**<  The kind of light source */
    APPLIB_EXIF_TAG_Flash,                          /**<  The status of flash when the image was shot */
    APPLIB_EXIF_TAG_FocalLength,                    /**<  The actual focal length of the lens */
    APPLIB_EXIF_TAG_SubjectArea,                    /**<  The location and area of the main subject */
    APPLIB_EXIF_TAG_MakerNote,                      /**<  Manufacturer notes */
    APPLIB_EXIF_TAG_UserComment,                    /**<  User comments */
    APPLIB_EXIF_TAG_SubSecTime,                     /**<  DateTime subseconds */
    APPLIB_EXIF_TAG_SubSecTimeOriginal,             /**<  DateTimeOriginal subseconds */
    APPLIB_EXIF_TAG_SubSecTimeDigitized,            /**<  DateTimeDigitized subseconds */
    APPLIB_EXIF_TAG_FlashpixVersion,                /**<  Supported flash version */
    APPLIB_EXIF_TAG_ColorSpace,                     /**<  Color space information */
    APPLIB_EXIF_TAG_PixelXDimension,                /**<  Valid image width */
    APPLIB_EXIF_TAG_PixelYDimension,                /**<  Valid image height */
    APPLIB_EXIF_TAG_RelatedSoundFile,               /**<  Related audio file */
    APPLIB_EXIF_TAG_InteroperabilityIFDPointer,     /**<  The offset to the interoperability tags */
    APPLIB_EXIF_TAG_FlashEnergy,                    /**<  Flash energy*/
    APPLIB_EXIF_TAG_SpatialFrequencyResponse,       /**<  Spatial frequency table and SFR values */
    APPLIB_EXIF_TAG_FocalPlaneXResolution,          /**<  The number of pixels in the image width(X) direction */
    APPLIB_EXIF_TAG_FocalPlaneYResolution,          /**<  The number of pixels in the image height(Y) direction */
    APPLIB_EXIF_TAG_FocalPlaneResolutionUnit,       /**<  The unit for measuring FocalPlaneXResolution/FocalPlaneYResolution */
    APPLIB_EXIF_TAG_SubjectLocation,                /**<  The location of the object in the scene */
    APPLIB_EXIF_TAG_ExposureIndex,                  /**<  The exposure index selected on the camera */
    APPLIB_EXIF_TAG_SensingMethod,                  /**<  The image sensor type on the camera */
    APPLIB_EXIF_TAG_FileSource,                     /**<  The image source */
    APPLIB_EXIF_TAG_SceneType,                      /**<  The type of scene */
    APPLIB_EXIF_TAG_CFAPattern,                     /**<  The color filter array geometric pattern of the image sensor */
    APPLIB_EXIF_TAG_CustomRendered,                 /**<  The use of special processing on image data */
    APPLIB_EXIF_TAG_ExposureMode,                   /**<  The exposure mode set when the image was shot */
    APPLIB_EXIF_TAG_WhiteBalance,                   /**<  The white balance mode set when the image was shot */
    APPLIB_EXIF_TAG_DigitalZoomRation,              /**<  The digital zoom ratio when the image was shot */
    APPLIB_EXIF_TAG_FocalLengthIn35mmFilm,          /**<  Focal length in 35 mm film */
    APPLIB_EXIF_TAG_SceneCaptureType,               /**<  Scene capture type */
    APPLIB_EXIF_TAG_GainControl,                    /**<  The degree of overall image gain adjustment */
    APPLIB_EXIF_TAG_Contrast,                       /**<  The direction of contrast processing */
    APPLIB_EXIF_TAG_Saturation,                     /**<  The direction of saturation processing */
    APPLIB_EXIF_TAG_Sharpness,                      /**<  The direction of sharpness processing */
    APPLIB_EXIF_TAG_DeviceSettingDescription,       /**<  Information on the picture-taking condition */
    APPLIB_EXIF_TAG_SubjectDistanceRange,           /**<  The distance to the subject */
    APPLIB_EXIF_TAG_ImageUniqueID,                  /**<  An identifier assigned uniquely to each image */
    APPLIB_EXIF_TAG_CameraOwnerName,                /**<  The owner of the camera */
    APPLIB_EXIF_TAG_BodySerialNumber,               /**<  The serial number of the body of the camera */
    APPLIB_EXIF_TAG_LensSpecification,              /**<  The specification of the lens */
    APPLIB_EXIF_TAG_LensMake,                       /**<  The lens manufacture */
    APPLIB_EXIF_TAG_LensModel,                      /**<  The lens model name and numeber */
    APPLIB_EXIF_TAG_LensSerialNumber,               /**<  The serial number of the interchangeble lens */
    APPLIB_EXIF_TAG_Gamma                           /**<  The value coefficient gamma */
} APPLIB_EXIF_TAG_ID_e;

#define VIDEO_DUAL_SAVING_OFF  (0)    /**< VIDEO DUAL STREAM OFF*/
#define VIDEO_DUAL_SAVING_ON   (1)    /**< VIDEO DUAL STREAM ON*/

/*************************************************************************
 * MW format union declaration
 ************************************************************************/
/**
 * Media info union
 */
typedef union _APPLIB_MEDIA_INFO_u_ {
    AMP_MOVIE_INFO_s* Movie; /**<movie info*/
    AMP_IMAGE_INFO_s* Image; /**<image info*/
    AMP_SOUND_INFO_s* Sound; /**<sound info*/
} APPLIB_MEDIA_INFO_u;

/*************************************************************************
 * MW format structure declaration
 ************************************************************************/
/**
 * Media info
 */
typedef struct _APPLIB_MEDIA_INFO_s_ {
    /**
     * Media info type.
     */
    AMP_MEDIA_INFO_TYPE_e MediaInfoType;
    /**
     * Media info.
     */
    APPLIB_MEDIA_INFO_u MediaInfo;
} APPLIB_MEDIA_INFO_s;

/**
 * Exif tag configuration
 */
typedef struct _APPLIB_GPS_TAG_s_{
    UINT8 *Data;        /**< Data */
    UINT32 Value;       /**< Value */
    UINT32 Count;       /**< Count */
    UINT16 Tag;         /**< Tag */
    UINT16 Type;        /**< Tag type */
    UINT8 Set;          /**< Set */
} APPLIB_GPS_TAG_s;

/**
 * Exif tag configuration
 */
typedef struct _APPLIB_GPS_DATA_s_{
    APPLIB_GPS_TAG_ID_e GpsTagId;
    APPLIB_TAG_TYPE_e GpsDataType;
    UINT8 GpsDataLength;
    UINT8 *GpsData;
} APPLIB_GPS_DATA_s;

/**
 * Exif tag configuration
 */
typedef struct _APPLIB_EXIF_DATA_s_{
    APPLIB_EXIF_TAG_ID_e TagId;
    APPLIB_TAG_TYPE_e DataType;
    UINT8 DataLength;
    UINT64 Value;
    UINT8 Data[8];
} APPLIB_EXIF_DATA_s;

/*************************************************************************
 * MW format module declaration
 ************************************************************************/
/**
 *  Initialize the format module
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_Init(void);

/**
 *  Initialize the muxer.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_MuxerInit(void);

/**
 *  @brief The initialization of mp4 muxer.
 *
 *  The initialization of mp4 muxer.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_Mp4MuxerInit(void);

/**
 *  @brief The initialization of exif muxer.
 *
 *  The initialization of exif muxer.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_ExifMuxerInit(void);

/**
 *  @brief Force split function
 *
 *  Set the max duration to force clip split
 *
 *  @param [in] SplitTime
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_ForceSplit(int splitTime);

/**
 *  @brief create split file
 *
 *   By AutoSplitFileType create split file
 *
 *  @param [in] Type
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_SetAutoSplitFileType(UINT32 type);

/**
 *  Start the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_Start(void);

/**
 *  Start the mp4 muxer while recording
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_StartOnRecording(void);

/**
 *  Start the eventrecord muxer while recording
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_StartOnRecording_EventRecord(int preEventTime);

/**
 *  @brief Close the mp4 muxer
 *
 *  Close the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_Close(void);

/**
 *  @brief Close the eventrecord muxer
 *
 *  Close the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_Close_EventRecord(void);

/**
 *  @brief Stop the eventrecord muxer
 *
 *  Close the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMp4_Event_End(void);

/**
 *  @brief Close the mp4 muxer
 *
 *  Close the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_StreamError(void);

/**
 *  @brief Start the EXIF muxer
 *
 *  Start the EXIF muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxExif_Start(void);

/**
 *  @brief End the exif muxer
 *
 *  End the exif muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxExif_End(void);

/**
 *  @brief Close the EXIF muxer
 *
 *  Close the EXIF muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxExif_Close(void);

/**
 *  @brief Set Exif endian type
 *
 *  Set Exif endian type
 *
 *  @return 0
 */
extern int AppLibFormatMuxExif_SetEndianType(APPLIB_EXIF_ENDIAN_TYPE_e Endian);

/**
 *  @brief Get Exif endian type
 *
 *  Get Exif endian type
 *
 *  @return 0
 */
extern UINT8 AppLibFormatMuxExif_GetEndianType(void);

/**
 *  @brief Confige GPS information
 *
 *  Confige GPS information
 *
 *  @return 0
 */
extern int AppLibFormatMuxExif_ConfigGpsTag(APPLIB_GPS_DATA_s *GpsData);

/**
 *  @brief Confige Exif information
 *
 *  Confige Exif information
 *
 *  @return 0
 */
extern int AppLibFormatMuxExif_ConfigExifTag(APPLIB_EXIF_DATA_s *ExifData);

/**
 *  @brief Increase one raw data
 *
 *  Increase one raw data for sync between recorder and muxer
 *
 *  @return 0
 */
extern int ApplibFormatMuxExif_IncreaseRawCount(void);

/**
 *  @brief Start the EXIF muxer for PIV
 *
 *  Start the EXIF muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxExifPIV_Start(void);

/**
 *  @brief Close the Exif muxer for PIV
 *
 *  Close the Exif muxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxExifPIV_Close(void);

/**
 *  @brief Initialize demuxer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_DemuxerInit(void);

/**
 *  @brief Get file format based on file name extension.
 *
 *  @param [in] fn File name
 *
 *  @return enumeration of file format
 */
extern APPLIB_FILE_FORMAT_e AppLibFormat_GetFileFormat(const char* fn);

/**
 *  @brief Determine the type of media info (video, image, or sound) by filename extension.
 *
 *  @param [in] fn File name
 *  @param [in] mediaInfo Media info
 *
 *  @return execution result
 */
extern int AppLibFormat_GetMediaInfo(char *fn, APPLIB_MEDIA_INFO_s *mediaInfo);

/**
 *  @brief reset muxer media info
 *
 *  @param [in] media media info
 *
 *  @return >=0 success, <0 failure
 */
extern int ApplibFormatLib_ResetMuxMediaInfo(AMP_MEDIA_INFO_s *media);

/**
 *  @brief format library adjust DTS
 *
 *  @param [in] movie movie info
 *
 *  @return >=0 success, <0 failure
 */
extern int ApplibFormatLib_AdjustDTS(AMP_MOVIE_INFO_s *movie);

/**
 *  @brief format library restore DTS
 *
 *  @param [in] media media info
 *
 *  @return >=0 success, <0 failure
 */
extern int ApplibFormatLib_RestoreDTS(AMP_MEDIA_INFO_s *media);

/**
 *  @brief format library reset PTS
 *
 *  @param [in] track media track info
 *
 *  @return >=0 success, <0 failure
 */
extern void ApplibFormatLib_ResetPTS(AMP_MEDIA_TRACK_INFO_s *track);

/**
 *  @brief format library get shorest track
 *
 *  @param [in] media media info
 *
 *  @return >=0 success, <0 failure
 */
extern AMP_MEDIA_TRACK_INFO_s *ApplibFormatLib_GetShortestTrack(AMP_MEDIA_INFO_s *media);

/**
 *  @brief format library get default track
 *
 *  @param [in] media media info
 *  @param [in] trackType Track Type
 *
 *  @return >=0 success, <0 failure
 */
extern AMP_MEDIA_TRACK_INFO_s *ApplibFormatLib_GetDefaultTrack(AMP_MEDIA_INFO_s *media, UINT8 trackType);

/**
 *  @brief Set dual file saving setting.
 *
 *  @param [in] dualStreams Dual stream
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_SetDualFileSaving(int dualFileSaving);

/**
 *  @brief Set the file alignment size of primary stream.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_SetPriStreamFileSizeAlignment(UINT32 size);

/**
 *  @brief Set the file alignment size of secondary stream.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_SetSecStreamFileSizeAlignment(UINT32 size);

/**
 *  @brief Get dual file saving setting.
 *
 *  @return The dual file saving setting.
 */
extern int AppLibFormat_GetDualFileSaving(void);

/**
 *  @brief Get the file alignment size of primary stream
 *
 *  @return Alignment size of primary stream
 */
extern UINT32 AppLibFormat_GetPriStreamFileSizeAlignment(void);

/**
 *  @brief Get the file alignment size of secondary stream.
 *
 *  @return Alignment size of secondary stream
 */
extern UINT32 AppLibFormat_GetSecStreamFileSizeAlignment(void);

/**
 *  @brief format library get media modification time
 *
 *  @param [in] Fn File name
 *  @param [in] TimeBuf Buffer for time information
 *  @param [in] TimeBufSize Time information buffer size
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormat_GetMediaTimeInfo(char *fn, char *timeBuf, int timeBufSize);

/**
 *  @brief Set text track enable/disable status
 *
 *  Set text track module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
extern int AppLibFormatMuxMp4_TextTrack_SetEnableStatus(UINT8 enableTextTrack);

/**
 *  @brief Get text track enable/disable status
 *
 *  Get text track module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
extern UINT8 AppLibFormatMuxMp4_TextTrack_GetEnableStatus(void);

#ifdef CONFIG_APP_ARD
extern int AppLibFormatMuxMp4_SetMaxDurationMax(void);
extern int AppLibFormatCreateMux(void);
extern int AppLibFormatTakeMux(void);
extern int AppLibFormatGiveMux(void);
#endif

__END_C_PROTO__


/**
 * @}
 */

#endif /* APPLIB_FORMAT_H_ */
