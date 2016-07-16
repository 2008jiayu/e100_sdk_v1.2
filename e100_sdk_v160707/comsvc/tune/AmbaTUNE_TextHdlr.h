/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTUNE_TextHdlr.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella Image Tuning Tools Text Handler
\*-------------------------------------------------------------------------------------------------------------------*/
/*!
 * @addtogroup ParserHdlr
 * @{
 *      @addtogroup TextHdlr
 *      @{
 */
#ifndef _AMBA_TEXT_HDLR_H_
#define _AMBA_TEXT_HDLR_H_
/*******************************************************************************
 * Header include
 ******************************************************************************/
#include "AmbaDataType.h"
#include "AmbaDSP_ImgDef.h" // AMBA_DSP_IMG_MODE_CFG_s need
#include "AmbaImgCalibItuner.h"

/*******************************************************************************
* Program
******************************************************************************/
/**
 * @brief TextHdlr Initial
 * If you want to use any TextHdlr_xxx(), please call this function first.
 *
 * @param None
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Init(Ituner_Initial_Config_t *pInitialConfig);

/**
 * @brief Save IDSP Settings
 * This function will save current idsp configure to the text file.
 *
 * @param[in] AMBA_DSP_IMG_MODE_CFG_s Mode: set Pipe and AlgoMode
 * @param[in] char *Filepath: set output filepath, eg. c:\xxx.txt
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, char *Filepath);

/**
 * @brief Load IDSP Settings
 * This function will load the text file and transform to idsp configure.
 *
 * @param[in] char *Filepath: set the source filepath, eg. c:\xxx.txt
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Load_IDSP(char *Filepath);

/**
 * @brief Load Data
 * This function can use to load any kind of external file.
 *
 * @param[in] Ituner_Ext_File_Type_e Ext_File_Type: Input load file type, detail please reference Ituner_Ext_File_Type_e
 * @param[in] Ituner_Ext_File_Param_s *Ext_File_Param: The parameters that load need, it related with your choice of Ext_File_Type.\n
 *          eg:if Ext_File_Type = EXT_FILE_FPN_MAP, then you should use Ext_File_Param->FPN_MAP_Ext_Load_Param.\n
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param);

/**
 * @brief Save Data
 * This function can use to save any table or bit stream data to external file.
 *
 * @param[in] Ituner_Ext_File_Type_e Ext_File_Type: Input save file type, detail please reference Ituner_Ext_File_Type_e
 * @param[in] Ituner_Ext_File_Param_s *Ext_File_Param: The parameters that save need, it related with your choice of Ext_File_Type.\n
 *          eg:if Ext_File_Type = EXT_FILE_FPN_MAP, then you should use Ext_File_Param->FPN_MAP_Ext_Sove_Param.\n
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param);

/**
 * @brief Apply idsp configure to ucode
 * This function will apply the idsp configure to code, please call TextHdlr_Load_IDSP() first
 *
 * @param[in] AMBA_DSP_IMG_MODE_CFG_s Mode: set Pipe and AlgoMode
 * @param[in] AMBA_ITUNER_PROC_INFO_s *ProcInfo: set high iso batch id
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo);

/**
 * @brief Get system info(eg. Raw info)
 *
 * @param[out] ITUNER_SYSTEM_s *System: Output System Info, detail please reference ITUNER_SYSTEM_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Get_SystemInfo(ITUNER_SYSTEM_s *System);

/**
 * @brief Set system info
 *
 * @param[in] ITUNER_AE_INFO_s *AeInfo: AeInfo Settings, detail please reference ITUNER_AE_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo);

/**
 * @brief Get ae info(eg. Raw info)
 *
 * @param[out] ITUNER_AE_INFO_s *AeInfo: Output Ae Info, detail please reference ITUNER_AE_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo);


/**
 * @brief Set system info
 *
 * @param[in] ITUNER_WB_SIM_INFO_s *WbSimInfo: WbSimInfo Settings, detail please reference ITUNER_WB_SIM_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);

/**
 * @brief Get ae info(eg. Raw info)
 *
 * @param[out] ITUNER_WB_SIM_INFO_s *WbSimInfo: Output WbSim Info, detail please reference ITUNER_WB_SIM_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);


/**
 * @brief Set system info
 *
 * @param[in] ITUNER_AE_INFO_s *AeInfo: System Settings, detail please reference ITUNER_AE_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Set_SystemInfo(ITUNER_SYSTEM_s *System);

/**
 * @brief Get ituner info(eg tuning mode)
 * Please call this function before TextHdlr_Load_IDSP()
 *
 * @param[out] ITUNER_INFO_s *ItunerInfo: Output Ituner Info, detail please reference ITUNER_INFO_s
 *
 * @return 0 (Success) / -1 (Failure)
 */
int TextHdlr_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo);
#endif /* _AMBA_TEXT_HDLR_H_ */
/*!
 *      @} end of addtogroup TextHdlr
 * @} end of addtogroup ParserHdlr
 */
