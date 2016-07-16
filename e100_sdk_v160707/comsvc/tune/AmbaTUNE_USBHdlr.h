/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: ApplibTune_USBHdlr.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella Image Tuning Tools USB Handler
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _APPLIB_USB_HDLR_H_
#define _APPLIB_USB_HDLR_H_

#include "AmbaDataType.h"
#include "AmbaDSP_ImgDef.h" // AMBA_DSP_IMG_MODE_CFG_s need
#include "AmbaImgCalibItuner.h"
//#include "AmageStruct.h"
#include "AmbaTUNE_HdlrManager.h"
#include "AmbaDSP.h" // AMBA_DSP_RAW_BUF_s need
typedef struct {
    int (*StillTuningRawBuffer)(AMBA_DSP_RAW_BUF_s *rawBuf);
    int (*StillTuningPreLoadDone)(void);
    int (*StillItunerRawEncode)(void);
} Ituner_USB_Config_t;

int USBHdlr_Init(Ituner_Initial_Config_t *pInitialConfig);
int USBHdlr_Set_USBConfig(TUNE_Initial_Config_s *TuneInitialConfig);
int USBHdlr_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_USB_Save_Param_s *USBbuffer);
int USBHdlr_Load_IDSP(TUNE_USB_Load_Param_s *USBbuffer);
int USBHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s* Ext_File_Param);
int USBHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s* Ext_File_Param);
int USBHdlr_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo);
int USBHdlr_Get_SystemInfo(ITUNER_SYSTEM_s *System);
int USBHdlr_Set_SystemInfo(ITUNER_SYSTEM_s *System);
int USBHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo);
int USBHdlr_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo);
int USBHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);
int USBHdlr_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);
int USBHdlr_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo);
int USB_CheckProcessingDown(void);

UINT32 USBHdlr_AmageCommunication (UINT32 Parameter1, UINT32 Parameter2, UINT32 Parameter3, UINT32 Parameter4, UINT32 Parameter5, UINT32* length, UINT32* dir);
UINT32 USBHdlr_AmageDataGet(UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength);
UINT32 USBHdlr_AmageDataSend(UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength);


#endif /* _APPLIB_USB_HDLR_H_ */
