/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTUNE_HdlrManager.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella TUNE Handler
\*-------------------------------------------------------------------------------------------------------------------*/


#ifndef _AMBA_TUNE_HDLR_MANAGER_H_
#define _AMBA_TUNE_HDLR_MANAGER_H_
/**
* @defgroup Tune 
* @brief Ambarella DSP Image Kernel CfaNoiseFilter APIs
*                            
* 
*/
/*!
 * @addtogroup Tune
 * @{
 */
#include "AmbaImgCalibItuner.h" // Note: Ituner_Ext_File_Type_e Need
#include "AmbaKAL.h"

/*!@brief Specified Tuning Mode*/
typedef enum {
    TEXT_TUNE = 0,  //!< Text Tuning (Default)
    USB_TUNE,       //!< USB Tuning
    MAX_TUNE,//!< MAX_TUNE
} TuneHdlr_Mode_e;
/**
 * tune USB save parameter 
 */
typedef struct {
    UINT8 *Buffer; /**<Buffer*/
    UINT32 Offset; /**<Offset*/
    UINT32 LengthRequested; /**<Length Requested*/
    UINT32 *ActualLength; /**<Actual Length*/
} TUNE_USB_Save_Param_s;

/**
 * tune TEXT save parameter
 */
typedef struct {
    char *Filepath; //!< Dest. Path
} TUNE_TEXT_Save_Param_s;

/**
 * tune USB load parameter
 */
typedef struct {
    UINT8 *Buffer; /**<Buffer*/
    UINT32 Offset; /**<Offset*/
    UINT32 Length; /**<Length*/
} TUNE_USB_Load_Param_s;

/**
 * tune TEXT load parameter
 */
typedef struct {
    char *FilePath; //!< Source Path
} TUNE_TEXT_Load_Param_s;

/**
 * tune load parameter union
 */
typedef union {
    TUNE_TEXT_Load_Param_s Text; //!< The input param of AmbaTUNE_Load_IDSP(TEXT), detail please reference TUNE_TEXT_Load_Param_s
    TUNE_USB_Load_Param_s USB; //!< The input param of AmbaTUNE_Load_IDSP(USB), detail please reference TUNE_USB_Load_Param_s
} TUNE_Load_Param_s;

/**
 * tune save parameter union
 */
typedef union {
    TUNE_TEXT_Save_Param_s Text;//!< The input param of AmbaTUNE_Save_IDSP(TEXT), detail please reference TUNE_TEXT_Save_Param_s
    TUNE_USB_Save_Param_s USB; //!< The input param of AmbaTUNE_Save_IDSP(USB), detail please reference TUNE_USB_Save_Param_s
} TUNE_Save_Param_s;

/**
 * tune initial config
 */
typedef struct{
    AMBA_KAL_BYTE_POOL_t  *pBytePool; /**<pBytePool*/
    int (*StillTuningRawBuffer)(AMBA_DSP_RAW_BUF_s *rawBuf);
    int (*StillTuningPreLoadDone)(void);
    int (*StillItunerRawEncode)(void);
} TUNE_USB_Initial_Config_s;

typedef struct{
    AMBA_KAL_BYTE_POOL_t  *pBytePool; /**<pBytePool*/
} TUNE_Text_Initial_Config_s;


typedef union{
    TUNE_Text_Initial_Config_s Text;
    TUNE_USB_Initial_Config_s Usb;
} TUNE_Initial_Config_s;



/**
 * @brief AmbaTUNE Initial
 * If you want to use any AmbaTUNE_xxx(), please call this function first.(Except AmbaTUNE_Change_Parser_Mode())
 *
 * @param [in] pTuneInitialConfig
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Init(TUNE_Initial_Config_s *pTuneInitialConfig);

/**
 * @brief AmbaTUNE UnInitial
 * If you want to uninitial AmbaTUNE, please call this function.
 *
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_UnInit(void);


/**
 * @brief Change Parser Mode
 * Use this function to change the parser mode\n
 * Default case, system will use the TEXT_TUNE mode\n
 * Do avoid data lose, if u want to switch the parser mode, please make sure the previous tuning process work finished.
 *
 * @param[in] Parser_Mode: Target Parser_Mode, detail please reference TuneHdlr_Mode_e
 *
 * @return None
 */
extern void AmbaTUNE_Change_Parser_Mode(TuneHdlr_Mode_e Parser_Mode);

/**
 * @brief Save IDSP Settings
 * This function will save current idsp configure.
 *
 * @param[in] pMode: set Pipe and AlgoMode
 * @param[in] Save_Param: USB and Text has different save param, detail please reference TUNE_Save_Param_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param);

/**
 * @brief Load IDSP Settings
 * This function will load user's setting and transform to idsp configure.
 *
 * @param[in] Load_Param: USB and Text has different load param, detail please reference TUNE_Load_Param_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Load_IDSP(TUNE_Load_Param_s *Load_Param);

/**
 * @brief Save Data
 * This function can use to save any table or bit stream data to external file.
 *
 * @param[in] Ext_File_Type: Input save file type, detail please reference Ituner_Ext_File_Type_e
 * @param[in] Ext_File_Param: The parameters that save need, it related with your choice of Ext_File_Type.\n
 *          eg:if Ext_File_Type = EXT_FILE_FPN_MAP, then you should use Ext_File_Param->FPN_MAP_Ext_Sove_Param.\n
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param);

/**
 * @brief Load Data
 * This function can use to load any kind of external file.
 *
 * @param[in] Ext_File_Type: Input load file type, detail please reference Ituner_Ext_File_Type_e
 * @param[in] Ext_File_Param: The parameters that load need, it related with your choice of Ext_File_Type.\n
 *          eg:if Ext_File_Type = EXT_FILE_FPN_MAP, then you should use Ext_File_Param->FPN_MAP_Ext_Load_Param.\n
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param);

/**
 * @brief Apply idsp configure to ucode
 * This function will apply the idsp configure to code, please call AmbaTUNE_Load_IDSP() first
 *
 * @param[in] pMode: set Pipe and AlgoMode
 * @param[in] ProcInfo: set high iso batch id
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo);

/**
 * @brief Get system info(eg. Raw info)
 *
 * @param[out] System: Output System Info, detail please reference ITUNER_SYSTEM_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Get_SystemInfo(ITUNER_SYSTEM_s *System);

/**
 * @brief Set system info
 *
 * @param[in] System: System Settings, detail please reference ITUNER_SYSTEM_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Set_SystemInfo(ITUNER_SYSTEM_s *System);

/**
 * @brief Get ae info(eg. Raw info)
 *
 * @param[out] AeInfo: Output System Info, detail please reference ITUNER_AE_INFO_s 
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo);

/**
 * @brief Set system info
 *
 * @param[in] AeInfo: System Settings, detail please reference ITUNER_AE_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo);

/**
 * @brief Set system info
 *
 * @param[in] WbSimInfo: WbSimInfo Settings, detail please reference ITUNER_WB_SIM_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);

/**
 * @brief Get ae info(eg. Raw info)
 *
 * @param[out] WbSimInfo: Output System Info, detail please reference ITUNER_WB_SIM_INFO_s 
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);


/**
 * @brief Get ituner info(eg tuning mode)
 * Please call this function before AmbaTUNE_Load_IDSP()
 *
 * @param[out] ItunerInfo: Output Ituner Info, detail please reference ITUNER_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
extern int AmbaTUNE_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo);
#endif /* _AMBA_TUNE_HDLR_MANAGER_H_ */
/*!
 *
 * @} end of addtogroup Tune
 */

