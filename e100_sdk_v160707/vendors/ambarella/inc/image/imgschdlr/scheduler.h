 /**
  * @file scheduler.h
  *
  * Ambarella image scheduler header
  *
  * Copyright (C) 2015, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef IMG_SCHEDULER_H_
#define IMG_SCHEDULER_H_

#include "AmbaDSP.h"
#include "AmbaDSP_ImgFilter.h"

/**
 * @defgroup ImageScheduler
 * @brief Scheduler for AE control, slow shutter
 *
 * Implementation the Scheduler module, include below function implementation
 *  1. Initial Scheduler module function.
 *  2. Create Scheduler module function.
 *  3. Delete Scheduler module function.
 *  4. Enable/Disable Scheduler module function.
 *  5. Set/Get desired AE information
 *  6. Request/Release/Query Channle setup information
 *  7. Get sensor Mapping table
 *
 */

/**
 * @addtogroup ImageScheduler
 * @{
 */

/*****************************
 *   Macro                   *
 *****************************/
#define MAX_SUBCHANNEL_NUM   (4)
#define MAX_BLENDING_NUM     (MAX_SUBCHANNEL_NUM-1)

/**
 * Error code
 */
typedef enum _AMBA_IMG_SCHDLR_ER_CODE_e_ {
    AMBA_IS_OK = 0,                              /**< Execution OK */
    AMBA_IS_ERR_GENERAL_ERR = -1,                /**< General error */
    AMBA_IS_ERR_CHANNEL_OCCUPIED = -2,           /**< certain channel is executing ae/awb/adj control */
    AMBA_IS_ERR_INVALID_PARAM_STRUCTURE = -3,    /**< Incorrect structure used */
    AMBA_IS_ERR_INVALID_PARAM_VALUE_RANGE = -4,  /**< Incorrect value range */
    AMBA_IS_ERR_ISOCFG_BUFFER_FULL = -5,         /**< No free iso cfg buffer to issue adj parameter */
} AMBA_IMG_SCHDLR_ER_CODE_e;

typedef int (*AMBA_IMGSCHDLR_CALLBACK_f)(void* hdlr, UINT32 event, void* info);

/**
 * ImgSchldr callback event
 */
typedef enum _AMBA_IMG_SCHDLR_EVENT_e_ {
    AMBA_IMG_SCHDLR_EVENT_CFA_STAT_READY,       /**< cfa statistics ready, app could read out the data from ds image */
    AMBA_IMG_SCHDLR_EVENT_RGB_STAT_READY,       /**< rgb statistics ready, app could read out the data from ds image */
    AMBA_IMG_SCHDLR_EVENT_MAIN_CFA_HIST_READY,  /**< Main histgram statistics ready, app could read out the data from ds image */
    AMBA_IMG_SCHDLR_EVENT_HDR_CFA_HIST_READY,   /**< PIP histgram statistics ready, app could read out the data from ds image */
    AMBA_IMG_SCHDLR_EVENT_UNKNOW                /**< UnKnow event */
} AMBA_IMG_SCHDLR_EVENT_e;

typedef struct _AMBA_IMG_SCHDLR_META_CFG_s_ {
    UINT32          AAAStatAddress; /* 3A(CFA/RGB/Hist) stat address */
    UINT8           LisoCtxId;      /* free LISO Context ID */
    UINT8           HisoCtxId;      /* free HISO Context ID */
    UINT32          UserDataAddr;   /* User Data address, valid in CACHE_MODE */
    UINT32          UserDataSize;   /* User Data Size, valid in CACHE_MODE */
} AMBA_IMG_SCHDLR_META_CFG_s;

/**
 * Module initialization config
 */
typedef struct _AMBA_IMG_SCHDLR_INIT_CFG_s_ {
    UINT8 *MemoryPoolAddr;      /**< scheduler module working pool address */
    UINT32 MemoryPoolSize;      /**< scheduler module working pool size */
    UINT8 MainViewNum;          /**< Number of MainView to be controlled */
    UINT8 SlowShutterEnable;    /**< Enable shlow shutter */
    UINT8 DigitalZoomEnable;    /**< Enable digital zoom */
    UINT16 IsoCfgNum;           /**< Number of IsoCfg resident in ImgKernel, use for HybridIsoMode */
    UINT32 MsgTaskPriority;     /**< MsgTask info */
    UINT32 MsgTaskStackSize;    /**< MagTask info */
    UINT32 MsgTaskCoreSelection;/**< Core selection bitmap. Bit[0] = 1 means core #0 is selected */
} AMBA_IMG_SCHDLR_INIT_CFG_s;

/**
 * Instance config
 */
typedef struct _AMBA_IMG_SCHDLR_CFG_s_ {
    UINT32  TaskPriority;               /**< Task info */
    UINT32  TaskStackSize;              /**< Task info */
    UINT32  CoreSelection;              /**< Core selection bitmap. Bit[0] = 1 means core #0 is selected */
    AMBA_DSP_CHANNEL_ID_u Channel;      /**< Channel info */
    UINT32 MainViewID;                  /**< this instance represent which MainView */
    void *Vin;                          /**< point to which Vin handler */
    AMBA_IMGSCHDLR_CALLBACK_f cbEvent;   /**< General event */
    UINT32  VideoProcMode;              /**< Video Process Mode :
                                             Bit[0]: 0=BatchMode 1=HybridISOMode
                                             Bit[4]: 0=NormalMode 1=HdrBlendingMode */
    UINT8 AAAStatSampleRate;            /**< AAA stat report interval */
} AMBA_IMG_SCHDLR_CFG_s;

/**
 * Instance update config
 * Only valid when Instance in Disable state
 */
typedef struct _AMBA_IMG_SCHDLR_UPDATE_CFG_s_ {
    UINT32  VideoProcMode;              /**< Video Process Mode :
                                             Bit[0]: 0=BatchMode 1=HybridISOMode
                                             Bit[4]: 0=NormalMode 1=HdrBlendingMode */
    UINT8 AAAStatSampleRate;            /**< AAA stat report interval */
} AMBA_IMG_SCHDLR_UPDATE_CFG_s;

/**
 * Instance body
 */
typedef struct _AMBA_IMG_SCHDLR_s_ {
    void *Ctx; /**< private context */
} AMBA_IMG_SCHDLR_HDLR_s;

/**
 * Scheduler control type
 */
typedef enum _AMBA_IMG_SCHDLR_SET_TYPE_e_{
    AMBA_IMG_SCHDLR_SET_TYPE_DIRECT = 2, /**< apply new setting directly */
    AMBA_IMG_SCHDLR_SET_TYPE_NORMAL,     /**< apply new setting by scheduler */
    AMBA_IMG_SCHDLR_SET_TYPE_ACK,        /**< forbid any 3A_STAT until new setting take effect */
    AMBA_IMG_SCHDLR_SET_TYPE_DIRECT_EXE, /**< apply new setting and execute directly, only valid in VideoTuning */
} AMBA_IMG_SCHDLR_SET_TYPE_e;

/**
 * Video HDR Filter update
 */
typedef union _AMBA_IMG_SCHDLR_VHDR_FILTER_u_ {
    UINT16   Data;

    struct {
        UINT16  BlendInfo:      1;      /* Blending Information */
        UINT16  AlphaCalc:      1;      /* Alpha table calculation config */
        UINT16  AlphaCalcThrd:  1;      /* Alpha tabel calculation threshold */
        UINT16  BlackCorr:      1;      /* black level correction */
        UINT16  AmpLinear:      1;      /* Amp linearization */
        UINT16  Reserved:       11;      /* Reserved */
    } Bits;
} AMBA_IMG_SCHDLR_VHDR_FILTER_u;

/**
 * Exosure information of each
 */
typedef struct _AMBA_IMG_SCHDLR_EXP_INFO_s_ {
    float AGC[MAX_SUBCHANNEL_NUM];     /**< Agc gain in db unit */
    float Shutter[MAX_SUBCHANNEL_NUM]; /**< shutter gain in millisecond unit */
    float GainR[MAX_SUBCHANNEL_NUM];   /**< Sensor R gain in db unit */
    float GainGr[MAX_SUBCHANNEL_NUM];  /**< Sensor Gr gain in db unit */
    float GainGb[MAX_SUBCHANNEL_NUM];  /**< Sensor Gb gain in db unit */
    float GainB[MAX_SUBCHANNEL_NUM];   /**< Sensor B gain in db unit */
    int IrisIndex;                     /**< iris setting */
    AMBA_DSP_IMG_MODE_CFG_s Mode;      /**< Dgain mode config */
    AMBA_DSP_IMG_WB_GAIN_s DGain;      /**< Dgain setting */
    UINT8 SensorWBUpdated;             /**< Using Sensor WB gain or not */
    UINT8 AdjUpdated;                  /**< Adj parameter is updated or not */
    UINT8 SubChannelNum;               /**< Sub channel number 0~4 */

    /* vHDR PreBlending */
    AMBA_IMG_SCHDLR_VHDR_FILTER_u HdrUpdated[MAX_BLENDING_NUM];         /**< Hdr parameter is updated or not */
    AMBA_DSP_IMG_HDR_BLENDING_INFO_s BlendInfo[MAX_BLENDING_NUM];       /**< blending info */
    AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s AlphaCalc[MAX_BLENDING_NUM];   /**< alpha table */
    AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s AlphaThresh[MAX_BLENDING_NUM]; /**< alpha table threshold */
    /* vHDR Blending */
    AMBA_DSP_IMG_BLACK_CORRECTION_s  BlackCorr[MAX_BLENDING_NUM];       /**< black level correction */
    AMBA_DSP_IMG_AMP_LINEARIZATION_s  AmpLinear[MAX_BLENDING_NUM];      /**< Linearization */
} AMBA_IMG_SCHDLR_EXP_INFO_s;

/**
 * Exposure information to go
 */
typedef struct _AMBA_IMG_SCHDLR_EXP_s_ {
    AMBA_IMG_SCHDLR_SET_TYPE_e Type; /**< apply type */
    AMBA_IMG_SCHDLR_EXP_INFO_s Info; /**< apply setting */
} AMBA_IMG_SCHDLR_EXP_s;

/**
 * Sensor information
 */
typedef struct _AMBA_IMG_SCHDLR_SENSOR_SHUTTER_INFO_s_ {
    float ShutterTime[MAX_SUBCHANNEL_NUM];     /**< Shutter time in sec unit */
    UINT32 ShutterLine[MAX_SUBCHANNEL_NUM];    /**< Shutter time exposure line unit */
} AMBA_IMG_SCHDLR_SENSOR_SHUTTER_INFO_s;

typedef struct _AMBA_IMG_SCHDLR_SENSOR_GAIN_INFO_s_ {
    float AGC[MAX_SUBCHANNEL_NUM];     /**< Agc gain in db unit */
    float GainR[MAX_SUBCHANNEL_NUM];   /**< Sensor R gain in db unit */
    float GainGr[MAX_SUBCHANNEL_NUM];  /**< Sensor Gr gain in db unit */
    float GainGb[MAX_SUBCHANNEL_NUM];  /**< Sensor Gb gain in db unit */
    float GainB[MAX_SUBCHANNEL_NUM];   /**< Sensor B gain in db unit */
} AMBA_IMG_SCHDLR_SENSOR_GAIN_INFO_s;


/**
 * get image scheduler default config for initializing
 *
 * @param [out] cfg image scheduler module config
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_GetInitDefaultCfg(AMBA_IMG_SCHDLR_INIT_CFG_s *defaultInitCfg);

/**
 * get image scheduler memory usage by given mainView number
 *
 * @param [in] mainViewNum number of mainview
 * @param [out] memSize total memery size of given mainview number
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_QueryMemsize(UINT32 mainViewNum, UINT32 *memSize);

/**
 * Initialize image scheduler module.
 * The function should only be invoked once.
 * User MUST invoke this function before using image scheduler module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg image scheduler module config
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_Init(AMBA_IMG_SCHDLR_INIT_CFG_s *cfg);

/**
 * Get image scheduler default config for an instance
 *
 * @param [out] cfg image scheduler module config
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_GetDefaultCfg(AMBA_IMG_SCHDLR_CFG_s *defaultCfg);

/**
 * Create an image scheduler instance.
 * @note Only one instance is needed
 *
 * @param [in] cfg the config of the image scheduler
 * @param [out] instance the image scheduler handler
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 *
 */
extern int AmbaImgSchdlr_Create(AMBA_IMG_SCHDLR_CFG_s *cfg,
                             AMBA_IMG_SCHDLR_HDLR_s **instance);

/**
 * Get Config of image scheduler instance.
 * @note Only when instance be disabled
 *
 * @param [in] cfg the config of the image scheduler
 * @param [in] instance the image scheduler handler
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 *
 */
extern int AmbaImgSchdlr_GetConfig(AMBA_IMG_SCHDLR_HDLR_s *instance,
                             AMBA_IMG_SCHDLR_UPDATE_CFG_s *cfg);

/**
 * Config an image scheduler instance.
 * @note Only when instance be disabled
 *
 * @param [in] cfg the config of the image scheduler
 * @param [in] instance the image scheduler handler
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 *
 */
extern int AmbaImgSchdlr_SetConfig(AMBA_IMG_SCHDLR_HDLR_s *instance,
                             AMBA_IMG_SCHDLR_UPDATE_CFG_s *cfg);

/**
 * close a image scheduler.
 *
 * @param [in] instance Image scheduler instance to close
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_Delete(AMBA_IMG_SCHDLR_HDLR_s *instance);

/**
 * Enable/Disable Scheduler
 *
 * @param [in] instance Image scheduler instance
 * @param [in] enable Enable
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_Enable(AMBA_IMG_SCHDLR_HDLR_s *instance, UINT32 enable);

/**
 * Setup exposure info
 *
 * @param [in] mainViewID mainview ID
 * @param [in] exposure Exposure information
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_SetExposure(UINT32 mainViewID, AMBA_IMG_SCHDLR_EXP_s *exposure);

/**
 * Get current exposure info
 *
 * @param [in] mainViewID mainview ID
 * @param [out] info Current exposure information
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_GetExposureInfo(UINT32 mainViewID, AMBA_IMG_SCHDLR_EXP_INFO_s *info);

/**
 * Get useable context index when fill idsp filter in HybridISO mode
 *
 * @param [in] mainViewID mainview ID
 * @param [in] algo mode
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaImgSchdlr_GetIsoCtxIndex(UINT32 mainViewID, AMBA_DSP_IMG_ALGO_MODE_e algoMode);

/**
 * Get iso config information in HybridISO mode
 *
 * @param [in]  mainViewID mainview ID
 * @param [in]  algoMode algo mode
 * @param [out] CfgNum total iso config number
 * @param [out] CfgStartIndex starting iso config index
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int ImgSchdlr_QueryIsoCfgInfo(UINT32 MainViewID, AMBA_DSP_IMG_ALGO_MODE_e algoMode, UINT8 *CfgNum, UINT8* CfgStartIndex);

/**
 * Get iso config locked bufferID in HybridISO mode
 *
 * @param [in]  mainViewID mainview ID
 * @param [in]  algoMode algo mode
 * @param [out] CfgIdx current dsp using iso config index
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int ImgSchdlr_QueryLockedIsoCfg(UINT32 MainViewID, AMBA_DSP_IMG_ALGO_MODE_e algoMode, UINT8 *CfgIdx);

/**
 * Get Hdr config information in HybridISO mode
 *
 * @param [in]  mainViewID mainview ID
 * @param [in]  algoMode algo mode
 * @param [out] CfgNum total iso config number
 * @param [out] CfgStartIndex starting Hdr config index
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int ImgSchdlr_QueryHdrCfgInfo(UINT32 MainViewID, AMBA_DSP_IMG_ALGO_MODE_e algoMode, UINT8 *CfgNum, UINT8* CfgStartIndex);

/**
 * Get Hdr config locked bufferID in HybridISO mode
 *
 * @param [in]  mainViewID mainview ID
 * @param [in]  algoMode algo mode
 * @param [out] CfgIdx current dsp using Hdr config index
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int ImgSchdlr_QueryLockedHdrCfg(UINT32 MainViewID, AMBA_DSP_IMG_ALGO_MODE_e algoMode, UINT8 *CfgIdx);

/**
 * Get Actual shutter time will take effect in sensor driver later
 *
 * @param [in]  mainViewID mainview ID
 * @param [in]  ExposureFrame total exposure frame
 * @param [in]  DesiredShutter desired shutter time
 * @param [out] ActualShutter actual shutter time
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int ImgSchdlr_QueryActualShutterTime(UINT32 MainViewID, UINT32 ExposureFrame, float *DesiredShutter, AMBA_IMG_SCHDLR_SENSOR_SHUTTER_INFO_s *ActualShutter);

/**
 * Get Actual gain factor will take effect in sensor driver later
 *
 * @param [in]  mainViewID mainview ID
 * @param [in]  ExposureFrame total exposure frame
 * @param [in]  DesiredGain desired factor
 * @param [out] ActualGain actual factor
 *
 * @return >=0 - Index, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int ImgSchdlr_QueryActualGainFactor(UINT32 MainViewID, UINT32 ExposureFrame,
                                            AMBA_IMG_SCHDLR_SENSOR_GAIN_INFO_s *DesiredGain,
                                            AMBA_IMG_SCHDLR_SENSOR_GAIN_INFO_s *ActualGain);

/**
 * @}
 */

#endif /* IMG_SCHEDULER_H_ */
