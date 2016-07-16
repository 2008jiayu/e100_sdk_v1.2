 /**
  * @file inc/mw/vin/vin.h
  *
  * Amba vin header
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef __VIN_H__
#define __VIN_H__
/**
 * @defgroup VIN
 * @brief Vin manager implementation
 *
 * Implementation the VIN manager/handler, include below function implementation
 * 1. Initial Vin manager function.
 * 2. Create Vin manager function.
 * 3. Delete Vin manager function.
 * 4. Config Vin manager after Vin manager already created
 *
 */

/**
 * @addtogroup VIN
 *
 * @{
 */

#include <mw.h>
#include <AmbaDSP.h>
#include "AmbaDSP_WarpCore.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"


/**
 * VIN Events
 */
typedef enum AMP_VIN_EVENT_e_{
        AMP_VIN_EVENT_INVALID = AMP_VIN_EVENT_START_NUM, /**< VIN signal is going to be invalid */
        AMP_VIN_EVENT_FRAME_READY,
        AMP_VIN_EVENT_FRAME_DROPPED,
        AMP_VIN_EVENT_VALID,                            /**< VIN starts operation */
        AMP_VIN_EVENT_CHANGED_PRIOR,                    /**< VIN is about to be configure */
        AMP_VIN_EVENT_CHANGED_POST,                     /**< VIN finishes configure */
        AMP_VIN_EVENT_MAINVIEW_CHANGED_PRIOR,           /**< MainView is about to be configure */
        AMP_VIN_EVENT_MAINVIEW_CHANGED_POST,            /**< MainView finish configure */
        AMP_VIN_EVENT_TOTAL_NUM
} AMP_VIN_EVENT_e;

/**
 * VIN info type
 */
typedef enum _AMP_VIN_INTO_TYPE_e_ {
    AMP_VIN_INFO_CFG        /**< current configuration */
} AMP_VIN_INFO_TYPE_e;

/**
 * VIN SOURCE
 */
typedef enum _AMP_VIN_SOURCE_e_{
        AMP_VIN_EXTERNAL_CFA = 0,               /**< VIN signal comes from external CFA output, ie sensor */
        AMP_VIN_EXTERNAL_YUV,               /**< VIN signal comes from external CFA output, ie sensor */
        AMP_VIN_MEMORY_CFA,               /**< VIN signal comes from external CFA output, ie sensor */
        AMP_VIN_MEMORY_YUV,               /**< VIN signal comes from external CFA output, ie sensor */
        AMP_VIN_SOURCE_TOTAL_NUM
} AMP_VIN_SOURCE_e;


/**
 * VIN layout instance
 */
typedef struct _AMP_VIN_LAYOUT_HDLR_s_ {
    void *Ctx;       /**< private context of the vin layout */
} AMP_VIN_LAYOUT_HDLR_s;

/**
 * vin init config structure
 */
typedef struct _AMP_VIN_INIT_CFG_s_ {
    UINT32 NumMaxVin; /**< max vin number */
    UINT8* MemoryPoolAddr; /**< buffer start address for vin module */
    UINT32 MemoryPoolSize; /**< size of buffer */
} AMP_VIN_INIT_CFG_s;

/**
 * VIN instance
 */
typedef struct _AMP_VIN_HDLR_s_ {
    void *Ctx;       /**< private context of the vin */
} AMP_VIN_HDLR_s;

typedef struct _AMP_VIN_LAYOUT_CFG_s_{
    UINT8 EnableSourceArea:1;     /**< sourceArea is valid or not */
    UINT8 EnableOBArea:1;     /**< OBArea is valid or not */
    UINT8 AreaEnReserved:6;
    AMP_ROTATION_e Rotation;    /**< Rotate after crop */
    UINT16 Reserved;
    AMP_AREA_s SourceArea;      /**< Area cropped from the source plane */
    AMP_AREA_s ActiveArea;         /**< Area of Active cropped from the Vcap plane */
    AMP_AREA_s OBArea;          /**< Area of OB cropped from the Vcap plane */
    UINT16 Width;               /**< IDSP output width (main window), equal to codec's source plane  */
    UINT16 Height;              /**< IDSP output height (main window), equal to codec's source plane */
    int (*cfaOutputCallback)(AMBA_DSP_IMG_WARP_CALC_CFA_INFO_s *pCfaInfo); /**< customer's CFA output calculation callback */
    UINT32 DzoomFactorX;        /**< Initial horizontal digital zoom factor, 16.16 format */
    UINT32 DzoomFactorY;        /**< Initial vertical digital zoom factor, 16.16 format */
    INT32 DzoomOffsetX;         /**< Initial horizontal digital zoom offset, 16.16 format */
    INT32 DzoomOffsetY;         /**< Initial vertical digital zoom offset, 16.16 format */
    UINT8 MainviewReportRate;   /**< Mainview status report rate */
} AMP_VIN_LAYOUT_CFG_s;


/**
 * Individual VIN instance configuration
 */
typedef struct _AMP_VIN_HDLR_CFG_s_ {
    AMBA_DSP_CHANNEL_ID_u Channel;  /**< HW device description */
    AMBA_SENSOR_MODE_ID_u Mode;     /**< mode id */
    AMBA_YUV_MODE_ID_u YuvMode;     /**< yuv mode id */
    UINT16 LayoutNumber;            /**< How many main windows will be produced from this vin */
    AMP_VIN_LAYOUT_CFG_s *Layout;   /**< main window */
    AMP_AREA_s HwCaptureWindow;     /**< HW capture window */
    AMP_CALLBACK_f cbSwitch;        /**< mode switch callback */
    AMP_CALLBACK_f cbEvent;         /**< general event callback */
    AMP_CALLBACK_f cbCfgUpdated;    /**< TBD */
    AMP_TASK_INFO_s TaskInfo;       /**< task info */
    AMP_VIN_SOURCE_e VinSource;    /**< Vin source */
    UINT8 reserve;
} AMP_VIN_HDLR_CFG_s;

/**
 * Runtime VIN configuration for a given handler (when liveview is running)
 */
typedef struct _AMP_VIN_RUNTIME_CFG_s_ {
    AMP_VIN_HDLR_s *Hdlr;
    AMP_AREA_s HwCaptureWindow;     /**< HW capture window */
    AMBA_SENSOR_MODE_ID_u Mode;     /**<  mode id */
    UINT32 LayoutNumber;            /**< How many main windows will be produced from this vin */
    AMP_VIN_LAYOUT_CFG_s *Layout;   /**< Layout list */
} AMP_VIN_RUNTIME_CFG_s;

/**
 * Offline VIN configuration for a given handler (when liveview is NOT running)
 */
typedef struct _AMP_VIN_CFG_s_ {
    AMBA_DSP_CHANNEL_ID_u Channel; /**< HW device description */
    AMBA_SENSOR_MODE_ID_u Mode;    /**< mode id */
    AMBA_YUV_MODE_ID_u YuvMode;    /**<yuv mode id */
    UINT16 LayoutNumber;           /**< How many main windows will be produced from this vin */
    AMP_VIN_LAYOUT_CFG_s *Layout;  /**< main window */
    AMP_AREA_s HwCaptureWindow;    /**< HW capture window */
    AMP_CALLBACK_f cbSwitch;       /**< mode switch callback */
    AMP_CALLBACK_f cbEvent;        /**< general event callback */
    AMP_CALLBACK_f cbCfgUpdated;   /**< TBD */
    AMP_VIN_SOURCE_e VinSource;    /**< Vin source */
    UINT8 reserve;
} AMP_VIN_CFG_s;


/**
 * Get vin module default configuration for initializing
 *
 * @param [out] cfg vin module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVin_GetInitDefaultCfg(AMP_VIN_INIT_CFG_s *defaultCfg);

/**
 * Initialize vin module.\n
 * The function should only be invoked once. \n
 * User MUST invoke this function before using vin module. \n
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg vin instance configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVin_Init(AMP_VIN_INIT_CFG_s *cfg);

/**
 * get vin default instance config
 *
 * @param [in,out] cfg default configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVin_GetDefaultCfg(AMP_VIN_HDLR_CFG_s *cfg);

/**
 * Crate and set initial configuration of a vin handler
 *
 * @param [in] cfg instance config
 * @param [out] vinHdlr Vin instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVin_Create(AMP_VIN_HDLR_CFG_s *cfg, AMP_VIN_HDLR_s **vinHdlr);

/**
 * Delete a vin handler
 *
 * @param [in] vinHdlr Vin instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVin_Delete(AMP_VIN_HDLR_s *vinHdlr);

/**
 * Change VIN setting of a vin handler when liveview is NOT running
 *
 * @param [in] vinHdlr Vin instance
 * @param [in] vinCfg Vin Config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVin_ConfigHandler(AMP_VIN_HDLR_s *vinHdlr, AMP_VIN_CFG_s *vinCfg);

/**
 * @}
 */

#endif

