/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_VIG.h
 *
 * header file for Amba vignette calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#include "AmbaDSP_ImgDef.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaCalibInfo.h"

/**
* @defgroup AmbaCalib_VIG
* @brief Vignette calibration
*
* Vig provide function to calculate Vignette.
*/


/**
 * @addtogroup AmbaCalib_VIG
 * @ingroup Calibration
 * @{
 */


#define VIG_CH_ALL 0xFF
#define MAX_BLEND_VALUE 256
#define BLEND_SHIFT     8


#define VIGNETTE_FULL_VIEW_MAX_WIDTH 65
#define VIGNETTE_FULL_VIEW_MAX_HEIGHT 49
#define VIGNETTE_MAX_SIZE_INTERNAL ((((VIGNETTE_FULL_VIEW_MAX_WIDTH*VIGNETTE_FULL_VIEW_MAX_HEIGHT)+8)>>3)<<3)

#define VIGNETTE_CAL_SIZE (VIGNETTE_FULL_VIEW_MAX_WIDTH*VIGNETTE_FULL_VIEW_MAX_HEIGHT) //65*49
#define VIG_SEARCH_RANGE (15) //should be odd # and < Vig_half_search_range
#define VIG_HALF_SEARCH_RANGE ((VIG_SEARCH_RANGE-1)/2)

/*
 * Vignette Compensation Setup
 */
#define VIGNETTE_MAX_SIZE (2000) // 65*49*10/16+6 pad to multiple of 8
#define VIG_CAL_CUSTOM_INFO_SIZE (19)
//Total 16100 bytes

/**
*  Vignette data structure
*/
typedef struct Vignette_Pack_Storage_s_ {
    UINT8  Enable;                                  /**< enable for this table */
    UINT8  GainShift;                               /**< GainShift for this table */
    UINT8  Channel;                                 /**< channel id */
    UINT8  SiteId;                                  /**< SiteId */
    UINT32 GainR;                                   /**< GainR for this table */
    UINT32 GainB;                                   /**< GainB for this table */
    UINT32 WbIndex;                                 /**< WB index */
    UINT32 ZoomStep;                                /**< zoom step */
    UINT32 CalInfo[5];                              /**< Reserved for future */
    UINT32 Version;                                 /**< Version */
    int    TableWidth;                              /**< TableWidth */
    int    TableHeight;                             /**< TableHeight */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CalibVinSensorGeo;  /**<Calibration Vin sensor geometry */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  RawSensorGeo;  /**<Raw Vin sensor geometry */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  OBVinSensorGeo;  /**<OB Vin sensor geometry */
    UINT32 OBEnable;                                /**<  Enable flag for OB */
    UINT16 OBOffsetX;                               /**<  OB offset X */
    UINT16 OBOffsetY;                               /**<  OB offset Y */
    UINT16 VignetteRedGain[VIGNETTE_MAX_SIZE];      /**< vignette red color gain table */
    UINT16 VignetteGreenEvenGain[VIGNETTE_MAX_SIZE];/**< vignette green even color gain table */
    UINT16 VignetteGreenOddGain[VIGNETTE_MAX_SIZE]; /**< vignette green odd  color gain table */
    UINT16 VignetteBlueGain[VIGNETTE_MAX_SIZE];     /**< vignette blue color gain table */
    UINT32 Reserved2[66];                                /**<  Reserved for extention */
}  __attribute__((packed)) Vignette_Pack_Storage_s;

/**
*  Vignette data structure for computation
*/
typedef struct Vignette_Core_Storage_s_ {
    UINT8 Enable;                   /**< enable for this table */
    UINT8 GainShift;                /**< GainShift for this table */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s SwinInfo; /**< for A5S vignette geometry info */
    UINT16 *VignetteRedGain;        /**< Point to an UINT16 array of size "VIGNETTE_MAX_SIZE_INTERNAL" */
    UINT16 *VignetteGreenEvenGain;  /**< Point to an UINT16 array of size "VIGNETTE_MAX_SIZE_INTERNAL" */
    UINT16 *VignetteGreenOddGain;   /**< Point to an UINT16 array of size "VIGNETTE_MAX_SIZE_INTERNAL" */
    UINT16 *VignetteBlueGain;       /**< Point to an UINT16 array of size "VIGNETTE_MAX_SIZE_INTERNAL" */
} Vignette_Core_Storage_s;

/**
*  vignette infomation for idsp usage
*/
typedef struct Idsp_Vignette_Info_s_ {
    UINT8 Enable;                       /**< enable for this table */
    UINT8 GainShift;                    /**< GainShift for this table */
    UINT32 VignetteRedGainAddr;         /**< vignette red gain table array address */
    UINT32 VignetteGreenEvenGainAddr;   /**< vignette green even gain table array address */
    UINT32 VignetteGreenOddGainAddr;    /**< vignette green odd gain table array address */
    UINT32 VignetteBlueGainAddr;        /**< vignette blue gain table array address */
} Idsp_Vignette_Info_s;


#define VigLumaGainOnly 0x80
#define VigNoKeepChromaticShading 0x40
#define VigKeepChromaticShading 0x00
//Total 64 bytes
/**
*  Vignette calibration control info
*/
typedef struct Vignette_Cal_Info_s_ {
    //Raw data info = 18 bytes
    UINT16 *RawAddr;                            /**< pointer to raw data address */
    AMBA_DSP_IMG_BLACK_CORRECTION_s BlackLevel; /**< black level */
    UINT16 Width;                               /**< raw image width */
    UINT16 Height;                              /**< raw image height */
    UINT8  Bayer;                               /**< raw image bayer pattern */
    UINT8  Padd1;                               /**< padding data */
    //Calibration configurations  = 6 bytes
    UINT8  GainShift;                           /**< 0 for 3.7, 1 for 2.8, 2 for 1.9, 3 for 0.10, 255 for auto select */
    UINT8  CompensateMethod;           /**< Method for compensating, 0x80: luma only, 0x40:4 channel compensate with the same gain */
    UINT16 Threshold;                           /**< if ((max/min)<<10 > threshold) => NG */
    UINT16 CompensateRatio;                     /**< 1024 for fully compensated , 0 for no compensation */
    //Outputs = 8 + 4*8 = 40 bytes
    UINT16 MaxMinRatio;                         /**< output max min ratio in .10 format */
    UINT16 Padd2;                               /**< padding data */
    UINT32 Reserved[7];                         /**< Reserved for future usage */
    Vignette_Pack_Storage_s *GainTable;         /**< pointer to vingette gain table address */
    UINT8  DarkCornerSizeX;                     /**< dark corner intervel size on x-axis  */
    UINT8  DarkCornerSizeY;                     /**< dark corner intervel size on y-axis */
    UINT32 VigGridsAddr;                        /**< vignette coarse grid table address */
} Vignette_Cal_Info_s;

#define VIGNETTE_VIDEO_ENABLE (1<<0)
#define VIGNETTE_STILL_ENABLE (1<<1)
#define VIGNETTE_RAW_ENCODE_GEN_ENABLE  (1<<2)

/*
 * Auto vignette configuration info
 */


#define VIG_SHIFT 16
#define MAX_VIG_RATIO (1<<VIG_SHIFT)



/**
*  Vignette error message
*/
typedef enum _VIG_CALIB_ERROR_MSG_ {
    VIG_CALIB_OK                                = 0,    /**< OK */
    VIG_CALIB_ERROR_LOAD_NUM                    = -1,   /**< calibration paramter number wrong */
    VIG_CALIB_ERROR_PARAM_MISMATCH              = -2,   /**< CalSubSiteId != CalStepStartOffset */
    VIG_CALIB_ERROR_PARAM_EXCEED                = -3,   /**< vignette table number exceed */
    VIG_CALIB_ERROR_PARAM_NG                    = -4,   /**< vignette calibration ozoom step must be increasing */
    VIG_CALIB_ERROR_MEMORY_ALOC                 = -5,   /**< calibration data require more memory than app allocated */
    VIG_CALIB_ERROR_FLICKER                     = -6,   /**< vignette calibration flicker mode must be 50 or 60 */
    VIG_CALIB_ERROR_TOO_BRIGHT                  = -7,   /**< raw image too bright */
    VIG_CALIB_ERROR_GAIN_SHIFT                  = -8,   /**< Unknown gain shift value,it should be 0/1/2/3/255 */
    VIG_CALIB_ERROR_SAVE_ITUNER_FILE            = -9,   /**< AmbaFS_fopen vig_gain_table fail */
    VIG_CALIB_ERROR_RAW_CAPTURE                 = -10,  /**< raw capture failed */
    VIG_CALIB_ERROR_EXCEED_THRESHOLD            = -11,  /**< Max shading exceed Threshold */
    VIG_CALIB_ERROR_OVEREXPOSURE                = -12,  /**< raw data avg max val too large */
    VIG_CALIB_ERROR_MAX_MIN_EXCEED_THRESHOLD    = -13,  /**< Max/Min ratio exceed Threshold */
    VIG_CALIB_ERROR_SENSOR_GEOMETRY_NG          = -14,  /**< read sensor geometry failed */
    VIG_CALIB_ERROR_LOAD_RAW_NG                 = -15,  /**< load raw image data failed */
    VIG_CALIB_ERROR_OPEN_RAW_ERROR              = -16,  /**< open raw image file error */
    VIG_CALIB_ERROR_SHUTTERTIME_TOO_SHORT       = -17,  /**< shuttertime is too short error */
} VIG_CALIB_ERROR_MSG;

/**
*  Doing vignette calibration from the raw image
*
*  @param [in] VignetteCalInfo the vignette calibration information
*  @param [out] OutputStr ountpt the debug message for this function
*
*  @return 0 - OK, others - VIG_CALIB_ERROR_MSG
*  @see VIG_CALIB_ERROR_MSG
*/
extern int AmpCalib_VigCalFromRaw(Vignette_Cal_Info_s *VignetteCalInfo,char *OutputStr);

/**
*  Set vignette decompress data address
*
*  @param [in] Channel channel id
*  @param [in] pVignetteCalcInfo address of vignette calibration buffer
*
*  @return none
*/
extern void AmpCalib_SetVigInfoAddr(UINT8 Channel,UINT32 *pVignetteCalcInfo);


/**
*   Decompress the vignette calibration data
*
*  @param [in] pointer of 5 UINT16
*  @param [out] pointer of 8 UINT10
*
*  @return none
*/
extern void AmpCalib_VigDecompress(UINT16 *P8U10,UINT16 *P5U16);

/**
*  Get vignette decompress data address
*
*  @param [in] Channel channel id
*  @param [in] pVignetteCalcInfo address of vignette calibration buffer
*
*  @return none
*/
//extern void AmpCalib_GetVigInfoAddr(UINT8 Channel,UINT32 *pVignetteCalcInfo);

/**
 * @}
 */
