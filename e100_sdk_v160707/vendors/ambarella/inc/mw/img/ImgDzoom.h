/**
 *  @file ImgDzoom.h
 *
 *  File stream handling
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

#ifndef IMG_DZOOM_H_
#define IMG_DZOOM_H_

#include <mw.h>

#define AMP_IMG_DZOOM_MAX_ACTIVE_DZOOM    4    /**< the hard limit of active dzoom count */

/**
 * @defgroup ImgDzoom
 * @brief Digital Zoom control module implementation
 *
 * Implementation the Digital Zoom control module, include below function implementation
 *  1. Init Digital Zoom control module function.
 *  2. Create Digital Zoom control module function.
 *  3. Delete Digital Zoom control module function.
 *  4. Reset Digital Zoom control module current contxt
 *  5. Callback of VIN ISR handler
 *  6. Digital Zoom control APIs
 *  7. Get Digital Zoom module information
 *
 */

/**
 * @addtogroup ImgDzoom
 * @{
 */


/**
 * ImgDzoom hdlr
 */
typedef struct _AMP_IMG_DZOOM_HDLR_s_ {
    void *Ctx; /**< private data of the dzoom */
} AMP_IMG_DZOOM_HDLR_s;

/**
 * amp ImgDzoom config structure
 */
typedef struct _AMP_IMG_DZOOM_INIT_CFG_s_ {
    int     NumOfMaxDzoom;
    //UINT8   SensorNum;            // Number of physical sensors
} AMP_IMG_DZOOM_INIT_CFG_s;

/**
 * ImgDzoom config
 */
typedef struct _AMP_IMG_DZOOM_CFG_s_ {
    AMP_TASK_INFO_s TaskInfo;   // Task info
    UINT32          MaxDzoomFactor;
    UINT32          ImgModeContextId;
    UINT32          ImgModeBatchId;
} AMP_IMG_DZOOM_CFG_s;

typedef struct _AMP_IMG_DZOOM_VIN_INVALID_INFO_s_ {
    UINT32  CapW;
    UINT32  CapH;
} AMP_IMG_DZOOM_VIN_INVALID_INFO_s;


/**
 * ImgDzoom Dzoom Table
 */
typedef struct _AMP_IMG_DZOOM_TABLE_s_ {
    UINT32  TotalStepNumber;
    UINT32  MaxDzoomFactor;     // 16.16
    int     (*GetDzoomFactor)(int Step, UINT32 *Factor);    // Get the dzoom factor under corresponding status
} AMP_IMG_DZOOM_TABLE_s;

#define    DZOOM_IN    0
#define    DZOOM_OUT    1
#define    DZOOM_SFT    3

#define DZOOM_SPEED_MAX 5

typedef struct _AMP_IMG_DZOOM_POSITION_s_ {
    UINT32  Direction;
    UINT32  Speed;
    UINT32  Distance;
    int     ShiftX;    // 16.16 format
    int     ShiftY;    // 16.16 format
} AMP_IMG_DZOOM_POSITION_s;

typedef struct _AMP_IMG_DZOOM_JUMP_s_ {
    UINT32  Step;
    int     ShiftX;    // 16.16 format
    int     ShiftY;    // 16.16 format
} AMP_IMG_DZOOM_JUMP_s;

typedef struct _AMP_IMG_DZOOM_INFO_s_ {
    UINT32  TotalStepNumber;
    UINT32  MaxDzoomFactor;     // 16.16
} AMP_IMG_DZOOM_INFO_s;

typedef struct _AMP_IMG_DZOOM_STATUS_s_ {
    UINT32  Region;
#define OPT_ZOOM_REGION        0
#define DIGI_ZOOM_REGION    1
    UINT32  IsProcessing;
#define DZOOM_NOTPROC    0
#define DZOOM_PROC    1
    UINT32  Step;
    UINT32  Factor;  // 16.16
    int     ShiftX;
    int     ShiftY;
} AMP_IMG_DZOOM_STATUS_s;


/**
 * get ImgDzoom module default config for initializing
 *
 * @param [out] cfg ImgDzoom module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_GetInitDefaultCfg(AMP_IMG_DZOOM_INIT_CFG_s *DefaultCfg);

/**
 * Initialize ImgDzoom module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using ImgDzoom module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg ImgDzoom module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_Init(AMP_IMG_DZOOM_INIT_CFG_s *Cfg);

/**
 * get ImgDzoom default config
 *
 * @param [out] cfg ImgDzoom module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_GetDefaultCfg(AMP_IMG_DZOOM_CFG_s *DefaultCfg);

/**
 * Create a ImgDzoom instance.
 *
 * @param [in] cfg the config of the ImgDzoom
 * @param [out] imgDzoom the ImgDzoom handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 *
 * @see AMP_BITS_DESC_s
 */
extern int AmpImgDzoom_Create(AMP_IMG_DZOOM_CFG_s *Cfg,
                             AMP_IMG_DZOOM_HDLR_s **ImgDzoom);

/**
 * Reset the status in a ImgDzoom.
 *
 * @param [in] imgDzoom ImgDzoom to reset the status
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_ResetStatus(AMP_IMG_DZOOM_HDLR_s *ImgDzoom);

/**
 * Delete a ImgDzoom.
 *
 * @param [in] imgDzoom ImgDzoom to delete
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_Delete(AMP_IMG_DZOOM_HDLR_s *ImgDzoom);

/**
 * vsync handler for ImgDzoom.
 *
 * @param [in] imgDzoom ImgDzoom
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_VsyncHandler(AMP_IMG_DZOOM_HDLR_s *ImgDzoom);

/**
 * Change resolution for ImgDzoom.
 *
 * @param [in] imgDzoom ImgDzoom
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_ChangeResolutionHandler(AMP_IMG_DZOOM_HDLR_s *ImgDzoom, AMP_IMG_DZOOM_VIN_INVALID_INFO_s *Info);

/**
 * Register Dzoom table for ImgDzoom.
 *
 * @param [in] imgDzoom ImgDzoom
 * @param [in] *Table   Dzoom Table
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_RegDzoomTable(AMP_IMG_DZOOM_HDLR_s *ImgDzoom,
                                AMP_IMG_DZOOM_TABLE_s *Table);

/**
 * Set Dzoom Position for ImgDzoom.
 *
 * @param [in] imgDzoom     ImgDzoom
 * @param [in] *Position    Dzoom Position
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_SetDzoomPosition(AMP_IMG_DZOOM_HDLR_s *ImgDzoom,
                                AMP_IMG_DZOOM_POSITION_s *Position);

/**
 * Set Dzoom Jump for ImgDzoom.
 *
 * @param [in] imgDzoom     ImgDzoom
 * @param [in] *Jump        Dzoom Jump
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_SetDzoomJump(AMP_IMG_DZOOM_HDLR_s *ImgDzoom,
                                AMP_IMG_DZOOM_JUMP_s *Jump);

/**
 * Preset Dzoom Jump for ImgDzoom. Jump info will take effect after mode switch done
 *
 * @param [in] imgDzoom     ImgDzoom
 * @param [in] *Jump        Dzoom Jump
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_PresetDzoomJump(AMP_IMG_DZOOM_HDLR_s *ImgDzoom,
                                AMP_IMG_DZOOM_JUMP_s *Jump);

/**
 * Stop ImgDzoom. It will stop ImgDzoom during Moving or Jumping for ImgDzoom
 *
 * @param [in] imgDzoom     ImgDzoom
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_StopDzoom(AMP_IMG_DZOOM_HDLR_s *ImgDzoom);

/**
 * Get Dzoom information fo ImgDzoom
 *
 * @param [in] imgDzoom     ImgDzoom
 *
 * @param [out] *Info       Dzoom Information
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_GetDzoomInfo(AMP_IMG_DZOOM_HDLR_s *ImgDzoom,
                                AMP_IMG_DZOOM_INFO_s *Info);

/**
 * Get Dzoom Status fo ImgDzoom
 *
 * @param [in] imgDzoom     ImgDzoom
 *
 * @param [out] *Status     Dzoom Status
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpImgDzoom_GetDzoomStatus(AMP_IMG_DZOOM_HDLR_s *ImgDzoom,
                                AMP_IMG_DZOOM_STATUS_s *Status);

#endif /* IMG_DZOOM_H_ */
