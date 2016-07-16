/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_VDsp_Handler.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Amba Image VDsp Handler Main.
 *
 \*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDSP_EventInfo.h"
#include "AmbaImg_AaaDef.h"
 
#ifndef __AMBA_IMG_VDSP_HANDLER_H__
#define __AMBA_IMG_VDSP_HANDLER_H__

/*-----------------------------------------------------------------------------------------------*\
 * Defined in ?
\*-----------------------------------------------------------------------------------------------*/
int  Amba_Img_VDspCfa_Handler(void *hdlr, UINT32 *pCfaData);
int  Amba_Img_VDspRgb_Handler(void *hdlr, UINT32 *pRgbData);
int  Amba_Img_VDspMainCfa_Hist_Handler(void *hdlr, UINT32 *pCfaHistData);
int  Amba_Img_VDspHdrCfa_Hist_Handler(void *hdlr, UINT32 *pCfaHistData);
void Amba_Img_Set_Video_Pipe_Ctrl_Params(UINT32 chNo, AMBA_DSP_IMG_MODE_CFG_s *mode);
void Amba_Img_Reset_Video_Pipe_Ctrl_Params(UINT32 chNo);
void Amba_Img_Reset_Video_Pipe_Ctrl_Flags(UINT32 chNo);
int  Amba_Img_Get_Video_Pipe_Ctrl_Addr(UINT32 chNo, UINT32 *TableAddr);
void Amba_Img_Set_Still_Pipe_Ctrl_Params(UINT32 chNo, AMBA_DSP_IMG_MODE_CFG_s *mode);
void Amba_Img_Reset_Still_Pipe_Ctrl_Params(UINT32 chNo);
int  Amba_Img_Get_Still_Pipe_Ctrl_Addr(UINT32 chNo, UINT32 *TableAddr);
int  Amba_Img_VDspCfa_Set_Counter(UINT32 Count);
int  Amba_Img_VDspCfa_Get_Counter(UINT32 *Count);
int  Amba_Img_Set_CcRegTable(UINT32 chNo, UINT8 *TableAddr);
int  AmbaImg_VDsp_Handler_MemMgr_Init(UINT32 channelCount);
void Amba_Img_Get_Video_Pipe_Ctrl_UpdateFlg(UINT32 chNo, UINT16 *updateflg);
int Amba_Img_Get_TaskEnableTable(UINT8 type, IMGPROC_TASK_ENABLE_s *pTaskEnableTable);
int Amba_Img_Set_TaskEnableTable(UINT8 type, IMGPROC_TASK_ENABLE_s *pTaskEnableTable);

#endif
