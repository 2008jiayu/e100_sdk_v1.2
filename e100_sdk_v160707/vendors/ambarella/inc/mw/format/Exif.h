/**
 *  @file Exif.h
 *
 *  Common definitions for Exif format
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *   No part of this file may be reproduced, stored in a retrieval system,
 *   or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *   recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

#ifndef __EXIF_H__
#define __EXIF_H__

/**
 * @defgroup EXIF
 * @ingroup Format
 * @brief EXIF Muxing/Demuxing Format module implementation
 *
 * The implementation of EXIF Muxing/Demuxing Format module
 * Users can use ExifMux to add data into the picture and use ExifDmx to parse data from a picture.
 *
 * The ExifMux/ExifDmx module include below function implementation:
 * 1. Initialize ExifMux/ExifDmx
 * 2. Create ExifMux/ExifDmx handler
 * 3. Delete ExifMux/ExifDmx handler
 *
 */

#include <mw.h>

/**
 * Tags setting
 */
#define TAG_DISABLED        0x00    /**< The flag to disable the tag setting */
#define TAG_ENABLED         0x01    /**< The flag to enable the tag setting */
#define TAG_CONFIGURED      0x02    /**< The flag to enable and to configure the tag attribute information */

/**
 * The tags numbers used in ExifMux
 */
#define IFD0_TOTAL_TAGS 33
#define IFD1_TOTAL_TAGS 32
#define EXIF_TOTAL_TAGS 70
#define IntIFD_TOTAL_TAGS 2
#define GPS_TOTAL_TAGS  32
#define IPTC_TOTAL_TAGS  20
#define MPIDX_TOTAL_TAGS    5
#define MPATTR_TOTAL_TAGS   15

/**
 * Tags which contains image information data and used by IFD0, IFD1
 */
#define TIFF_ImageWidth                     256      /**<  The image width */
#define TIFF_ImageHeight                    257      /**<  The image height */
#define TIFF_BitsPerSample                  258      /**<  The number of bits per component for each pixel when image format is no compression */
#define TIFF_Compression                    259      /**<  The compression method */
#define TIFF_PhotometricInterpretation      262      /**<  The clolor space */
#define TIFF_ImageDescription               270      /**<  The description of image */
#define TIFF_Make                           271      /**<  The manufacturer of digicam */
#define TIFF_Model                          272      /**<  The model number of digicam */
#define TIFF_StripOffsets                   273      /**<  The offset to image data when image format is no compression */
#define TIFF_Orientation                    274      /**<  The orientation of the camera relative to the scene */
#define TIFF_SamplesPerPixel                277      /**<  Number of components */
#define TIFF_RowsPerStrip                   278      /**<  Number of rows per stripe */
#define TIFF_StripByteCounts                279      /**<  Bytes per compressed stripe */
#define TIFF_XResolution                    282      /**<  The display resolution in width direction */
#define TIFF_YResolution                    283      /**<  The display resolution in height direction */
#define TIFF_PlanarConfiguration            284      /**<  Image data arrangement */
#define TIFF_ResolutionUnit                 296      /**<  Unit of XResolution/YResolution */
#define TIFF_TransferFunction               301      /**<  Transfer function */
#define TIFF_Software                       305      /**<  Firmware version */
#define TIFF_DateTime                       306      /**<  The modified Date/Time of image*/
#define TIFF_Artist                         315      /**<  The orientation of the camera relative to the scene */
#define TIFF_WhitePoint                     318      /**<  The chromaticity of white point of image */
#define TIFF_PrimaryChromaticities          319      /**<  The chromaticity of the primaries of image */
#define TIFF_JPEGInterchangeFormat          513      /**<  Offset to JPEG 'SOI' */
#define TIFF_JPEGInterchangeFormatLength    514      /**<  Bytes of JPEG data */
#define TIFF_YCbCrCoefficients              529      /**<  YCbCr-RGB transformation coefficients matrix */
#define TIFF_YCbCrSubSampling               530      /**<  Chrominance subsampleing information */
#define TIFF_YCbCrPositioning               531      /**<  Chroma sample point of subsampling pixel array */
#define TIFF_ReferenceBlackWhite            532      /**<  Referenced values */
#define TIFF_Copyright                      33432    /**<  Copyright holder */
#define TIFF_ExifIFDPointer                 34665    /**<  The offset to the ExifIFD */
#define TIFF_GPSInfoIFDPointer              34853    /**<  The offset to the GPSIFD */
#define TIFF_PIM                            50341    /**<  The offset to the PrintIM information */

/**
 * EXIFIFD is a set of tags for recording exif-specific attribute information
 * and is pointed by the TIFF_ExifIFDPointer(34665) tag in IFD0 or IFD1.
 */
#define EXIFIFD_ExposureTime                33434    /**<  Exposure time, given in seconds */
#define EXIFIFD_FNumber                     33437    /**<  The F number */
#define EXIFIFD_ExposureProgram             34850    /**<  The class of the program used to set exposure */
#define EXIFIFD_SpectralSensitivity         34852    /**<  The spectra sensitivity of each channel of the camera used */
#define EXIFIFD_ISOSpeedRatings             34855    /**<  ISOSpeedRatings value up to 2.21; PhohographicSentivity in 2.3 */
#define EXIFIFD_OECF                        34856    /**<  Indicates the OECF specified in ISO 14524 */
#define EXIFIFD_SensitivityType             34864    /**<  Sensitivity type; defined in exif 2.3 */
#define EXIFIFD_StandardOutputSensitivity   34865    /**<  The standard output sensitivity of a camera; define in exif 2.3 */
#define EXIFIFD_RecommendedExposureIndex    34866    /**<  The Recommended exposure index value of a camera; defined in exif 2.3 */
#define EXIFIFD_ISOSpeed                    34867    /**<  The ISO speed value of a camera; defined in exif 2.3 */
#define EXIFIFD_ISOSpeedLatitudeyyy         34868    /**<  The ISO speed latitude yyy value of a camera; defined in exif 2.3 */
#define EXIFIFD_ISOSpeedLatitudezzz         34869    /**<  The ISO speed latitude zzz value of a camera; defined in exif 2.3 */
#define EXIFIFD_ExifVersion                 36864    /**<  The exif version is supported */
#define EXIFIFD_DateTimeOriginal            36867    /**<  The Date/Time when the image was generated */
#define EXIFIFD_DateTimeDigitized           36868    /**<  The Date/Time when the image was stored */
#define EXIFIFD_ComponentsConfiguration     37121    /**<  Meaning of each component */
#define EXIFIFD_CompressedBitsPerPixel      37122    /**<  Image compression mode */
#define EXIFIFD_ShutterSpeedValue           37377    /**<  Shutter speed */
#define EXIFIFD_ApertureValue               37378    /**<  The lens aperture */
#define EXIFIFD_BrightnessValue             37379    /**<  The value of brightness */
#define EXIFIFD_ExposureBiasValue           37380    /**<  The exposure bias */
#define EXIFIFD_MaxApertureValue            37381    /**<  The smallest F number of the lens */
#define EXIFIFD_SubjectDistance             37382    /**<  The distance to the subject */
#define EXIFIFD_MeteringMode                37383    /**<  The metering mode */
#define EXIFIFD_LightSource                 37384    /**<  The kind of light source */
#define EXIFIFD_Flash                       37385    /**<  The status of flash when the image was shot */
#define EXIFIFD_FocalLength                 37386    /**<  The actual focal length of the lens */
#define EXIFIFD_SubjectArea                 37396    /**<  The location and area of the main subject */
#define EXIFIFD_MakerNote                   37500    /**<  Manufacturer notes */
#define EXIFIFD_UserComment                 37510    /**<  User comments */
#define EXIFIFD_SubSecTime                  37520    /**<  DateTime subseconds */
#define EXIFIFD_SubSecTimeOriginal          37521    /**<  DateTimeOriginal subseconds */
#define EXIFIFD_SubSecTimeDigitized         37522    /**<  DateTimeDigitized subseconds */
#define EXIFIFD_FlashpixVersion             40960    /**<  Supported flash version */
#define EXIFIFD_ColorSpace                  40961    /**<  Color space information */
#define EXIFIFD_PixelXDimension             40962    /**<  Valid image width */
#define EXIFIFD_PixelYDimension             40963    /**<  Valid image height */
#define EXIFIFD_RelatedSoundFile            40964    /**<  Related audio file */
#define EXIFIFD_InteroperabilityIFDPointer  40965    /**<  The offset to the interoperability tags */
#define EXIFIFD_FlashEnergy                 41483    /**<  Flash energy*/
#define EXIFIFD_SpatialFrequencyResponse    41484    /**<  Spatial frequency table and SFR values */
#define EXIFIFD_FocalPlaneXResolution       41486    /**<  The number of pixels in the image width(X) direction */
#define EXIFIFD_FocalPlaneYResolution       41487    /**<  The number of pixels in the image height(Y) direction */
#define EXIFIFD_FocalPlaneResolutionUnit    41488    /**<  The unit for measuring FocalPlaneXResolution/FocalPlaneYResolution */
#define EXIFIFD_SubjectLocation             41492    /**<  The location of the object in the scene */
#define EXIFIFD_ExposureIndex               41493    /**<  The exposure index selected on the camera */
#define EXIFIFD_SensingMethod               41495    /**<  The image sensor type on the camera */
#define EXIFIFD_FileSource                  41728    /**<  The image source */
#define EXIFIFD_SceneType                   41729    /**<  The type of scene */
#define EXIFIFD_CFAPattern                  41730    /**<  The color filter array geometric pattern of the image sensor */
#define EXIFIFD_CustomRendered              41985    /**<  The use of special processing on image data */
#define EXIFIFD_ExposureMode                41986    /**<  The exposure mode set when the image was shot */
#define EXIFIFD_WhiteBalance                41987    /**<  The white balance mode set when the image was shot */
#define EXIFIFD_DigitalZoomRatio            41988    /**<  The digital zoom ratio when the image was shot */
#define EXIFIFD_FocalLengthIn35mmFilm       41989    /**<  Focal length in 35 mm film */
#define EXIFIFD_SceneCaptureType            41990    /**<  Scene capture type */
#define EXIFIFD_GainControl                 41991    /**<  The degree of overall image gain adjustment */
#define EXIFIFD_Contrast                    41992    /**<  The direction of contrast processing */
#define EXIFIFD_Saturation                  41993    /**<  The direction of saturation processing */
#define EXIFIFD_Sharpness                   41994    /**<  The direction of sharpness processing */
#define EXIFIFD_DeviceSettingDescription    41995    /**<  Information on the picture-taking condition */
#define EXIFIFD_SubjectDistanceRange        41996    /**<  The distance to the subject */
#define EXIFIFD_ImageUniqueID               42016    /**<  An identifier assigned uniquely to each image */
#define EXIFIFD_CameraOwnerName             42032    /**<  The owner of the camera */
#define EXIFIFD_BodySerialNumber            42033    /**<  The serial number of the body of the camera */
#define EXIFIFD_LensSpecification           42034    /**<  The specification of the lens */
#define EXIFIFD_LensMake                    42035    /**<  The lens manufacture */
#define EXIFIFD_LensModel                   42036    /**<  The lens model name and numeber */
#define EXIFIFD_LensSerialNumber            42037    /**<  The serial number of the interchangeble lens */
#define EXIFIFD_Gamma                       42240    /**<  The value coefficient gamma */

/**
 * GPSIFD is a set of tags for recording information such as GPS information,
 * and is pointed by the TIFF_GPSInfoIFDPointer(34853) tag in IFD0 or IFD1.
 */
#define GPSIFD_GPSVersionID             0    /**<  GPS tag version */
#define GPSIFD_GPSLatitudeRef           1    /**<  North or south latitude */
#define GPSIFD_GPSLatitude              2    /**<  Latitude */
#define GPSIFD_GPSLongitudeRef          3    /**<  East or west latitude */
#define GPSIFD_GPSLongitude             4    /**<  Longitude */
#define GPSIFD_GPSAltitudeRef           5    /**<  Altitude reference */
#define GPSIFD_GPSAltitude              6    /**<  Altitude */
#define GPSIFD_GPSTimeStamp             7    /**<  GPS time(atomic clock) */
#define GPSIFD_GPSSatellites            8    /**<  GPS satellites used for measurement */
#define GPSIFD_GPSStatus                9    /**<  GPS receiver status */
#define GPSIFD_GPSMeasureMode           10   /**<  GPS measurement mode */
#define GPSIFD_GPSDOP                   11   /**<  Measurement precision */
#define GPSIFD_GPSSpeedRef              12   /**<  Speed unit */
#define GPSIFD_GPSSpeed                 13   /**<  Speed of GPS receiver */
#define GPSIFD_GPSTrackRef              14   /**<  Reference for direction of movement */
#define GPSIFD_GPSTrack                 15   /**<  Direction of movement */
#define GPSIFD_GPSImgDirectionRef       16   /**<  Reference for direction of image */
#define GPSIFD_GPSImgDirection          17   /**<  Direction of image */
#define GPSIFD_GPSMapDatum              18   /**<  Geodetic survey data used */
#define GPSIFD_GPSDestLatitudeRef       19   /**<  Reference for latitude of destination */
#define GPSIFD_GPSDestLatitude          20   /**<  Latitude of destination */
#define GPSIFD_GPSDestLongitudeRef      21   /**<  Reference for longitude of destination */
#define GPSIFD_GPSDestLongitude         22   /**<  Longitude of destination */
#define GPSIFD_GPSDestBearingRef        23   /**<  Reference for bearing of destination */
#define GPSIFD_GPSDestBearing           24   /**<  Bearing of destination */
#define GPSIFD_GPSDestDistanceRef       25   /**<  Reference for distance of destination */
#define GPSIFD_GPSDestDistance          26   /**<  Distance to destination */
#define GPSIFD_GPSProcessingMethod      27   /**<  Name of GPS processing method */
#define GPSIFD_GPSAreaInformation       28   /**<  Name of GPS area */
#define GPSIFD_GPSDateStamp             29   /**<  GPS date */
#define GPSIFD_GPSDifferential          30   /**<  GPS differential correction */
#define GPSIFD_GPSHPositioningError     31   /**<  Horizontal position error */

/**
 * Interoperability IFD is composed tags which store the information to ensure the
 * interoperability and pointed by the EXIFIFD_InteroperabilityIFDPointer(40965) tag in EXIFIFD.
 */
#define IntIFD_InteroperabilityIndex        1   /**<  The identification of the interoperability rule */
#define IntIFD_InteroperabilityVersion      2   /**<  The interoperability version is supported */

/**
 * TODO: The below tags are other metadata information for image
 * IPTC tag list
 * MP Index IFD tag list
 * MP Attribute IFD tag list
 *
 */
#define IPTC_ObjectName                     0x0205
#define IPTC_Urgent                         0x020A
#define IPTC_Categories                     0x020F
#define IPTC_SupplementalCategories         0x0214
#define IPTC_Keywords                       0x0219
#define IPTC_SpecialInstructions            0x0228
#define IPTC_DateCreated                    0x0237
#define IPTC_Byline                         0x0250
#define IPTC_BylineTitle                    0x0255
#define IPTC_City                           0x025A
#define IPTC_Sublocation                    0x025C
#define IPTC_ProvinceState                  0x025F
#define IPTC_Country                        0x0265
#define IPTC_OriginalTransmissionReference  0x0267
#define IPTC_Headline                       0x0269
#define IPTC_Credits                        0x026E
#define IPTC_Source                         0x0273
#define IPTC_Copyright                      0x0274
#define IPTC_Caption                        0x0278
#define IPTC_CaptionWriter                  0x027A

#define MPIFD_MPFVersion                45056
#define MPIFD_NumberOfImages            45057
#define MPIFD_MPEntry                   45058
#define MPIFD_ImageUIDList              45059
#define MPIFD_TotalFrames               45060

#define MPIFD_MPIndividualNum           45313
#define MPIFD_PanOrientation            45569
#define MPIFD_PanOverlap_H              45570
#define MPIFD_PanOverlap_V              45571
#define MPIFD_BaseViewpointNum          45572
#define MPIFD_CovergenceAngle           45573
#define MPIFD_BaselineLength            45574
#define MPIFD_VerticalDivergence        45575
#define MPIFD_AxisDistance_X            45576
#define MPIFD_AxisDistance_Y            45577
#define MPIFD_AxisDistance_Z            45578
#define MPIFD_YawAngle                  45579
#define MPIFD_PitchAngle                45580
#define MPIFD_RollAngle                 45581

/**
 * Exif GPS information
 */
typedef struct {
    UINT32  VersionId;              /**<  GPSVersionID */
    UINT8 LatitudeRef[2];           /**<  GPSLatitudeRef */
    UINT64  Latitude[3];            /**<  GPSLatitude */
    UINT8 LongitudeRef[2];          /**<  GPSLongitudeRef */
    UINT64  Longitude[3];           /**<  GPSLongitude */
    UINT8   AltitudeRef;            /**<  GPSAltitudeRef */
    UINT64  Altitude;               /**<  GPSAltitude */
    UINT64  Timestamp[3];           /**<  GPSTimeStamp */
    UINT32  SatelliteOffset;        /**<  GPSSatellites tag offset*/
    UINT32  SatelliteCount;         /**<  GPSSatellites tag size */
    UINT8 Status[2];                /**<  GPSStatus */
    UINT8 MeasureMode[2];           /**<  GPSMeasureMode */
    UINT64  Dop;                    /**<  GPSDOP */
    UINT8 SpeedRef[2];              /**<  GPSSpeedRef */
    UINT64  Speed;                  /**<  GPSSpeed */
    UINT8 TrackRef[2];              /**<  GPSTrackRef */
    UINT64  Track;                  /**<  GPSTrack */
    UINT8 ImgdirectionRef[2];       /**<  GPSImgDirectionRef */
    UINT64  Imgdirection;           /**<  GPSImgDirection */
    UINT32  MapdatumOffset;         /**<  GPSMapDatum tag offset*/
    UINT32  MapdatumCount;          /**<  GPSMapDatum tag size */
    UINT8 DestlatitudeRef[2];       /**<  GPSDestLatitudeRef */
    UINT64  Destlatitude[3];        /**<  GPSDestLatitude */
    UINT8 DestlongitudeRef[2];      /**<  GPSDestLongitudeRef */
    UINT64  Destlongitude[3];       /**<  GPSDestLongitude */
    UINT8 DestbearingRef[2];        /**<  GPSDestBearingRef */
    UINT64  Destbearing;            /**<  GPSDestBearing */
    UINT8 DestdistanceRef[2];       /**<  GPSDestDistanceRef */
    UINT64  Destdistance;           /**<  GPSDestDistance */
    UINT32  ProcessingmethodOffset; /**<  GPSProcessingMethod tag offset */
    UINT32  ProcessingmethodCount;  /**<  GPSProcessingMethod tag size */
    UINT32  AreainformationOffset;  /**<  GPSAreaInformation tag offset */
    UINT32  AreainformationCount;   /**<  GPSAreaInformation tag size */
    UINT8 Datestamp[11];            /**<  GPSDateStamp */
    UINT16  Differential;           /**<  GPSDifferential */
    UINT64  HPositioningError;      /**<  GPSHPositioningError */
} GPS_INFO_s;


/** TYPE of the second picture(screennail, ...)*/
#define AMP_FORMAT_SCRAPP2  1
#define AMP_FORMAT_SCRMPF   2
#define AMP_FORMAT_SCRFPXR  3

/**
 * Exif private information
 */
typedef struct {
    UINT32 MakerNoteOff;  /**< The start offset of makernote in the file */
    UINT32 MakerNoteSize; /**< The size of makernote */
    UINT32 App1Off;       /**< The start offset of maker APP1 in the file */
    UINT32 App2Off;       /**< The start offset of maker APP2 in the file */
    UINT32 TiffBase;      /**< The start offset of Tiff header in the file */
    UINT32 Ifd0Off;       /**< The start offset of IFD0 structure in the file */
    UINT32 Ifd1Off;       /**< The start offset of IFD1 structure in the file */
    UINT32 ExifIfdOff;    /**< The start offset of ExifIfd structure in the file */
    UINT32 GpsIfdOff;     /**< The start offset of GpsIfd structure in the file */
    UINT32 SecPicType;    /**< The format type of the second picture (screennail) */
    BOOL8 Thumbnail;      /**< The flag of the thumbnail in the image */
    BOOL8 Screennail;     /**< The flag of the screennail in the image */
    BOOL8 Fullview;       /**< The flag of the fullview in the image */
} AMP_EXIF_PRIV_INFO_s;

#endif
