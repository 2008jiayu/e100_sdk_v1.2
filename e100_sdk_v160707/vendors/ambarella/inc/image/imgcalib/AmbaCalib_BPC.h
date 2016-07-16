/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_BPC.h
 *
 * header file for Amba bad pixel calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#include "AmbaDSP_ImgDef.h"
#include "AmbaCalibInfo.h"

/**
* @defgroup AmbaCalib_BPC
* @brief Bad pixel calibration flows
*
* Bpc provide function to calculate bpc mapping \n
* and compress/decompresse for save in nand.
*/

/**
 * @addtogroup AmbaCalib_BPC
 * @ingroup Calibration
 * @{
 */

#define EE  (0)
#define EO  (1)
#define OE  (2)
#define OO  (3)
#define BPC_DETECT_BRIGHT_PIXEL (0x00)
#define BPC_DETECT_DARK_PIXEL   (0x01)
#define BPC_MAP_PITCH           (32)    // DMA's 2D mode needs row size to be multiple of 32-byte
#define BPC_MAP_BIT_PITCH       (BPC_MAP_PITCH*8)

#define BPC_BUFFER_HEADER_SIZE  (256)
#define BPC_2D_MODE     0
#define BPC_S3D_MODE    1

/**
*  Look Up Table definition
*/
typedef struct BPC_Mode_Lut_s_ {
    AMBA_DSP_IMG_SENSOR_SUBSAMPLING_s  HSubSample;  /**< horizontal SubSample factor*/
    AMBA_DSP_IMG_SENSOR_SUBSAMPLING_s  VSubSample;  /**< vertical SubSample factor */
    UINT32  BPCMode;                                /**< BPC function mode */
    UINT32  SensorCalMode;                          /**< Sensor mode */
} BPC_Mode_Lut_s;

#define FPN_DETECT_HOT_PIXEL                        (0x00)
#define FPN_DETECT_COLD_PIXEL                       (0x01)
#define BADPIXEL_THRESHOLD_RELATIVE_TO_AVERAGE      (0x00)
#define BADPIXEL_THRESHOLD_DIFFERENCE_TO_AVERAGE    (0x80)
#define BADPIXEL_THRESHOLD_RELATIVE_TO_AVERAGE_STD    (0x08)
#define BPC_TYPE_BIT (0x01)
#define BPC_MODE_BIT (0x88)

/**
*  Black level algorithm infomation
*/
typedef struct BPC_Detect_Algo_Info_s_ {
    UINT32              AlgoType;           /**< algorithm type. check bit 0, 0: hot pixel, 1: dark pixel */
    float               UpperThreshold;     /**< max threshold */
    float               LowerThreshold;     /**< min threshold */
    UINT32              ThresholdMode;      /**< Relative/Absoulte difference. check bit 7,0: relative, 1:difference */
    UINT32              BlockWidth;         /**< detect block width */
    UINT32              BlockHeight;        /**< detect block height */
    UINT32              RawPitch;           /**< Pitch >= width, Pitch is related to the internal implementation of IDSP */
    UINT32              OBPitch;           /**< OB Pitch >= width, Pitch is related to the internal implementation of IDSP */
    UINT32              RawWidth;           /**< Actual width of raw picture */
    UINT32              RawHeight;          /**< Actual height of raw picture */
    UINT32              OBWidth;           /**< OB width of the sensor mode */
    UINT32              OBHeight;          /**< OB height of the sensor mode */
    UINT32              OBOffsetX;           /**< optical black start offset in x direction */
    UINT32              OBOffsetY;           /**< optical black start offset in y direction */    
    UINT32              DebugFlag;          /**< debug flag */
    UINT32              OBEnable;               /**< enable flag to control OB, enable:1 disable:0 */
} BPC_Detect_Algo_Info_s;


/**
*  Badpixel error message
*/
typedef enum _BPC_CALIB_ERROR_MSG_ {
    BPC_CALIB_OK                           = 0,     /**< OK */
    BPC_CALIB_PAGE_CREATE_ERROR            = -1,    /**< Page overflow or overlapping */
    BPC_CALIB_LOAD_SCRIPT_ERROR            = -2,    /**< Argument number error */
    BPC_CALIB_PAGE_INVALID                 = -3,    /**< Bad pixel Page invalid */
    BPC_CALIB_BUFFER_INVALID               = -4,    /**< Bad pixel buffer invalid */
    BPC_CALIB_TYPE_INVALID                 = -5,    /**< Bad pixel type invalid, it should 0 or 1 */
    BPC_CALIB_NUM_INVALID                  = -6,    /**< Bad pixel number invalid, it should 0 to 60 */
    BPC_CALIB_THRESHOLD_INVALID            = -7,    /**< Bad pixel threshold invalid, it should 0 to 1000 */
    BPC_CALIB_AGC_INVALID                  = -8,    /**< Bad pixel Agc gain invalid, it should 0 to 1000 */
    BPC_CALIB_SHUTTER_INVALID              = -9,    /**< Bad pixel shutter_index invalid, it can't be 0 */
    BPC_CALIB_PAGE_TYPE_INVALID            = -10,   /**< Bad pixel Page invalid */
    BPC_CALIB_DECOMPRESS_ERROR             = -11,   /**< BpcMap decompressed error */
    BPC_CALIB_GET_GEO_ERROR                = -12,   /**< Sensor didn't report geometry information */
    BPC_CALIB_RAW_CAPTURE_ERROR            = -13,   /**< raw capture failed */
    BPC_CALIB_SAVE_ERROR                   = -14,   /**< save file failed */
    BPC_CALIB_PREDICT_ERROR                = -15,   /**< bad pixel pack predict error, plrase adjust the threshold */
    BPC_CALIB_EXCEED_THRESHOLD_ERROR       = -16,   /**< BadPixel number exceeds threshold */
    BPC_CALIB_ERROR_OPEN_RAW_ERROR         = -17,   /**< open raw data failed */
    BPC_CALIB_ERROR_CLUSTER_CHECK          = -18,   /**< bad pixel byte number error */
    BPC_CALIB_ERROR_SHUTTERTIME_TOO_SHORT  = -19,   /**< shuttertime is too short error */
} BPC_CALIB_ERROR_MSG;

/**
*  Sensor bpc mode
*/
typedef enum _SENSOR_CAL_BPC_MODE_ {
    SENSOR_IN_CALIBRATION = 0x80000000,        /**< Calibration sensor mode ID */
    SENSOR_CAL_VIGENETTE = 0x80000000,         /**< Vignette calibration sensor mode ID */
    SENSOR_CAL_BPC_MODE0 = 0x80000001,       /**< bad pixel calibration sensor mode0 ID */
} SENSOR_CAL_BPC_MODE;

/**
*  Bpc mode
*/
typedef enum _BPC_MODE_ {
    BPC_MODE0 = 0,           /**< bad pixel mode0 ID */
} BPC_MODE;

/**
*  Bpc page
*/
typedef enum _BPC_PAGE_ {
    BPC_PAGE0 = 0,            /**< bad pixel page0 ID */
} BPC_PAGE;

/**
*  Bpc buffer
*/
typedef enum _BPC_BUFFER_ {
    BPC_BUFFER0 = 0,         /**< bad pixe buffer0 ID */
} BPC_BUFFER;


// BPC buffer option
#define BPC_BUFFER_OPTION_NONE                  (0)
#define BPC_BUFFER_OPTION_3D_COMPLEX_BPC        (1)

#define MAGIC_BADPIXEL_BYTE (0xFF88FF88)

/**
*  Badpixel page structure
*/
typedef  struct BadPixel_Page_s_ {
    UINT32                              Version;        /**< current version */
    UINT32                              MagicNumber;    /**< bad pixel map bytes */
    UINT32                              RecordValid;    /**<  1 when the record is valid */
    UINT32                              MaskSize;       /**< size of unpacked size */
    UINT32                              PackedSize;     /**< packed size */
    UINT32                              BPCMode;        /**< bpc mode */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CalibVinSensorGeo;  /**< vin sensor geometry */
    UINT32                              NumBadPixels;   /**< number of bad pixels */
    UINT32                              Channel;        /**< channel id */
    UINT32                              OBEnable;        /**< Enable flag for OB */
    UINT32                              Reserved[6];    /**< for further usage */
    void                                *CompressedMap; /**< Compressed bad pixel map address in momory */
} BadPixel_Page_s;




#define BPC_BUFFER_MAGIC_NUMBER          (0x20130531)
#define BPC_BUFFER_MAGIC_DATA_VALID      (0x20130531)
#define BPC_BUFFER_1_BIT                 (0x01)
#define BPC_BUFFER_4_BIT                 (0x02)

/**
*  Badpixel map head structure
*/
#if 1
typedef  struct MW_BPC_Buffer_Header_s_ {
    UINT32                             MagicNumber;     /**< bad pixel map bytes */
    UINT32                             MagicDataValid;  /**< data valid */
    UINT32                             Size;            /**< Hidden size saved by memory allocate function */
    UINT32                             BufType;         /**< buffer type */
    UINT32                          BPCMode;            /**< bpc mode */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s GeometryCal;     /**< in sensor geometry */
    BadPixel_Page_s                 *BadPixelPage;      /**< pointer to bad pixel page */
    void                            *Buffer;            /**< pointer to bpc map */
} __attribute__((packed)) MW_BPC_Buffer_Header_s;
#else
typedef  struct MW_BPC_Buffer_Header_s_ {
    UINT32                             MagicNumber;     /**< bad pixel map bytes */
    UINT32                             MagicDataValid;  /**< data valid */
    UINT32                             Size;            /**< Hidden size saved by memory allocate function */
    UINT32                             BufType;         /**< buffer type */
    UINT32                          BPCMode;            /**< bpc mode */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s GeometryCal;     /**< in sensor geometry */
    BadPixel_Page_s                 *BadPixelPage;      /**< pointer to bad pixel page */
    void                            *Buffer;            /**< pointer to bpc map */
} __attribute__((packed)) MW_BPC_Buffer_Header_s;
#endif

extern UINT32 AmpCalib_BPCWidthToPitchExt(UINT32 Width);

/**
*  Detect the bad pixel from the raw image, and it will return the FPN MAP.
*
*  @param [out] BPCMapAddr the address of the bpc map
*  @param [in] RawAddr the address of the raw image
*  @param [in] AlgoInfo the input parameters for the bpc
*
*  @return bad pixel number
*/
extern int AmpCalib_BPCDetectFromRaw(UINT8 *BPCMapAddr, UINT16 *RawAddr, BPC_Detect_Algo_Info_s *AlgoInfo);

/**
*  Compress the bad pixel map to save the memory
*
*  @param [in] BPCBufAddr the address of the bpc map
*  @param [in] BPCBufSize the size of the bpc map
*  @param [in] PackedMapAddr the address of the packed bpc map
*  @param [out] PackedMapSize the size of the packed bpc map
*  @param [out] BadPixelCount total number of the bad pixel
*
*  @return 0 - OK, others - BPC_CALIB_ERROR_MSG
*  @see BPC_CALIB_ERROR_MSG
*/
extern int AmpCalib_BPCPackByte(UINT8 *BPCBufAddr, UINT32 BPCBufSize, UINT8 *PackedMapAddr, UINT32 *PackedMapSize, UINT32 *BadPixelCount);

/**
*  Decompress the bad pixel map
*
*  @param [in] PackedAddr the address of the packed bpc map
*  @param [in] PackedSize the size of the packed bpc map
*  @param [in] BPCBufAddr the address of the bpc map
*  @param [out] BPCBufSize the size of the bpc map
*  @param [out] BadPixelCount total number of the bad pixel
*
*  @return none
*/
extern void AmpCalib_BPCUnPackByte(UINT8 *PackedAddr, UINT32 PackedSize, UINT8 *BPCBufAddr, UINT32 *BPCBufSize, UINT32 *BadPixelCount);

/**
*  Decompress bad pixel calibration data to the buffer
*
*  @param [in] Buffer the address of bpc map
*  @param [in] BadPixelPage the page information for bad pixel
*  @param [in] BPCModeLut the bad pixel mode lookup table
*
*  @return 0 - OK, -1 - Failed
*/
extern int AmpCalib_HWFormatConv(UINT8 *Buffer, BadPixel_Page_s *BadPixelPage,BPC_Mode_Lut_s *BPCModeLut);

/**
 * @}
 */
