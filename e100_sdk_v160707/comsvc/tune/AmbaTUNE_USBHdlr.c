/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: ApplibTune_USBHdlr.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella Image Tuning Tools USB Handler
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaTUNE_USBHdlr.h"
#include "AmbaAmageStruct.h"
#include "AmbaPrintk.h"
#include "AmbaImgCalibItuner.h" //Note: ITUNER_SYSTEM_s need
#include "AmbaDSP_ImgFilter.h" // Note: AMBA_DSP_IMG_NUM_EXPOSURE_CURVE need
#include "AmbaKAL.h"
#include "AmbaImg_Impl_Cmd.h" //3
#include "AmbaImg_Proc.h"
#include "AmbaTUNE_TextHdlr.h" // Note: Tune_Func_s Need
#include "AmbaTUNE_HdlrManager.h"
#include "AmbaImg_AaaDef.h"
#include "AmbaTUNE_Parser.h"
#include "AmbaTUNE_Rule.h"
#include "AmbaFS.h"
#include "AmbaUSB_API.h"
#include "AmbaCache.h"
#include "scheduler.h"
#include "AmbaTUNE_VideoHDRRule.h"

#include <util.h>
//#include <comsvc/ApplibComSvc_MemMgr.h>
//#include "AmbaSensor.h"

#define  SYNC_DIRECT (2 << 16)
#define  SYNC_SNAP   (3 << 16)

#define HDLR_DEBF(fmt, arg...) AmbaPrint("[USB HDLR][DEBUG]"fmt, ##arg)
#define HDLR_WARF(fmt, arg...) AmbaPrint("[USB HDLR][WARNING]"fmt, ##arg)
#define HDLR_ERRF(fmt, arg...) AmbaPrint("[USB HDLR][ERROR]"fmt, ##arg)

#define GRID_TABLE_SIZE (4*96*64)
#define THREED_TABLE_SIZE (4096)
#ifndef NULL
#define NULL (void*)0
#endif
#define 	StillRawEncode_THREAD_STACK_SIZE (1024*16)
#define     FLAG_PREPARSE_START                  0x01
#define 	MTP_EC_VENDOR_AMBA_TEST              0xC002


static UINT32 Item;
static UINT32 AmageSendDataSize = 0;
static char Data[MAX_AMGAGE_DATA_SIZE] = {0};

typedef struct {
    AMBA_KAL_BYTE_POOL_t *MemPool;
    Ituner_USB_Config_t CBFuncList;
} USB_Handler_s;

static USB_Handler_s USB_Handler = {0};

AMBA_KAL_EVENT_FLAG_t   SaveJpegEventFlag;
AMBA_KAL_TASK_t         StillRawEncodeThread = {0};
UINT8 StillRawEncodeThreadStack[StillRawEncode_THREAD_STACK_SIZE];

JPG_INFO_t JPG_INFO ={.addr=NULL, .size=0};
static UINT8 TuningMode = VIDEO;
static UINT8   AmbaTUNE_Init_Flg = 1;
static UINT8 StillRawEncodeThreadflg=1;
static UINT8   HDR_FUNC_MODE = 0;

AMBA_MEM_CTRL_s Still_Data_Buff ={0};  //FPNMap, hiso_text, hiso_three_d_table
AMBA_MEM_CTRL_s IQTable_Buff ={0};  //IQTable
AMBA_DSP_RAW_BUF_s RawBuf={0};

#define POSIX_FILE AMBA_FS_FILE



typedef struct{
    UINT8* addr;
    UINT8  isSet;
}ITUNER_TABLE_ADDR_s;

typedef struct {
    ITUNER_TABLE_ADDR_s FPNMap;
    ITUNER_TABLE_ADDR_s WarpGrid;
    ITUNER_TABLE_ADDR_s CaGrid;
    ITUNER_TABLE_ADDR_s VignetteTable;
    ITUNER_TABLE_ADDR_s CcReg;
    ITUNER_TABLE_ADDR_s CcThreeD;
    ITUNER_TABLE_ADDR_s AmpLinear0LookupTable;
    ITUNER_TABLE_ADDR_s AmpLinear1LookupTable;
}CalibTableAddr_s;

static CalibTableAddr_s calibTable;

typedef struct _TEXT_STREAM_s {
    char* text_addr;
    int size;
    int ptr;
}TEXT_STREAM;

static TEXT_STREAM hisoText;

typedef struct _ITUNER_OBJ_s_ {
    AMBA_DSP_IMG_MODE_CFG_s TuningAlgoMode;
    TUNING_MODE_EXT_e TuningModeExt;
    TUNE_Parser_Object_t* ParserObject;
    UINT8 Is_Idsp_Load_Finish;
} ITUNER_OBJ_s;

static ITUNER_OBJ_s _Ituner;
static ITUNER_SYSTEM_s _System;

static int _USBHdlr_Update_System_Info(AMBA_DSP_IMG_MODE_CFG_s *pMode);
extern int AmbaItuner_Get_Calib_Table(ITUNER_Calib_Table_s **Ituner_Calib_Table);
extern int AmbaItuner_Get_HDR_Table(ITUNER_VIDEO_HDR_TABLE_s *ITUNER_Hdr_Table);


static const char _str_tuning_mode[IMG_MODE_NUMBER][32] = {
    TUING_MODE_TO_STR(IMG_MODE_VIDEO),
    TUING_MODE_TO_STR(IMG_MODE_STILL),
    TUING_MODE_TO_STR(IMG_MODE_LOW_ISO_STILL),
    TUING_MODE_TO_STR(IMG_MODE_MID_ISO_STILL),
    TUING_MODE_TO_STR(IMG_MODE_HIGH_ISO_STILL),
    TUING_MODE_TO_STR(IMG_MODE_NIGHT_PORTRAIT_STILL),
    TUING_MODE_TO_STR(IMG_MODE_PREVIEW),
    TUING_MODE_TO_STR(IMG_MODE_HIGH_ISO_VIDEO),
};

static const char _str_tuning_mode_ext[TUNING_MODE_EXT_NUMBER][32] = {
    TUING_MODE_EXT_TO_STR(SINGLE_SHOT),
    TUING_MODE_EXT_TO_STR(MULTI_SHOT_HDR),
    TUING_MODE_EXT_TO_STR(MULTI_SHOT_MF_HISO),
    TUING_MODE_EXT_TO_STR(MULTI_SHOT_MA_HDR),
};

static TUNING_MODE_e _USBHdlr_Get_TuningMode(void)
{
    const char* str_tuning_mode_lut[IMG_MODE_NUMBER] = {
        TUING_MODE_TO_STR(IMG_MODE_VIDEO),
        TUING_MODE_TO_STR(IMG_MODE_STILL),
        TUING_MODE_TO_STR(IMG_MODE_LOW_ISO_STILL),
        TUING_MODE_TO_STR(IMG_MODE_MID_ISO_STILL),
        TUING_MODE_TO_STR(IMG_MODE_HIGH_ISO_STILL),
        TUING_MODE_TO_STR(IMG_MODE_NIGHT_PORTRAIT_STILL),
        TUING_MODE_TO_STR(IMG_MODE_PREVIEW),
        TUING_MODE_TO_STR(IMG_MODE_HIGH_ISO_PREVIEW),
        TUING_MODE_TO_STR(IMG_MODE_HIGH_ISO_VIDEO),
    };
    UINT8 i;
    for (i = 0; i < IMG_MODE_NUMBER; i++) {
        if (strcmp(_System.TuningMode, str_tuning_mode_lut[i]) == 0) {
            return (TUNING_MODE_e)i;
        }
    }
    HDLR_WARF("%s() %d, Invalid TuningMode: %s", __func__, __LINE__, _System.TuningMode);
    return IMG_MODE_NUMBER;
}

static TUNING_MODE_EXT_e _USBHdlr_Get_TuningModeExt(void)
{
    const char str_tuning_mode_ext_lut[TUNING_MODE_EXT_NUMBER][32] = {
        TUING_MODE_EXT_TO_STR(SINGLE_SHOT),
        TUING_MODE_EXT_TO_STR(MULTI_SHOT_HDR),
        TUING_MODE_EXT_TO_STR(MULTI_SHOT_MF_HISO),
        TUING_MODE_EXT_TO_STR(MULTI_SHOT_MA_HDR),
        TUING_MODE_EXT_TO_STR(SINGLE_SHOT_MULTI_EXPOSURE_HDR),
        TUING_MODE_EXT_TO_STR(DYNAMIC_OB),
    };
    UINT8 i;
    for (i = 0; i < TUNING_MODE_EXT_NUMBER; i++) {
        if (strcmp(_System.TuningModeExt, str_tuning_mode_ext_lut[i]) == 0) {
            return (TUNING_MODE_EXT_e)i;
        }
    }
    HDLR_WARF("%s() %d, Invalid TuningModeExt: %s", __func__, __LINE__, _System.TuningModeExt);
    return TUNING_MODE_EXT_NUMBER;
}

static void  _USBHdlr_Get_ImgMode(AMBA_DSP_IMG_MODE_CFG_s *pImgMode)
{
    TUNING_MODE_e TuningMode;
    TUNING_MODE_EXT_e TuningModeExt;
    TuningMode = _USBHdlr_Get_TuningMode();

    TuningModeExt = _USBHdlr_Get_TuningModeExt();
    do {
        if (TuningModeExt == SINGLE_SHOT_MULTI_EXPOSURE_HDR) {
            if (TuningMode == IMG_MODE_VIDEO || TuningMode == IMG_MODE_PREVIEW) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
                pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
                break;
            }
        } else if (TuningModeExt == SINGLE_SHOT) {
            if (TuningMode == IMG_MODE_VIDEO || TuningMode == IMG_MODE_PREVIEW) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
                pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                break;
            } else if (TuningMode == IMG_MODE_HIGH_ISO_VIDEO || TuningMode == IMG_MODE_HIGH_ISO_PREVIEW) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                break;
            } else if (TuningMode == IMG_MODE_LOW_ISO_STILL || TuningMode == IMG_MODE_MID_ISO_STILL) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_STILL;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
                //pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV; //PIV still not go with AMBA_DSP_IMG_FUNC_MODE_FV but AMBA_DSP_IMG_FUNC_MODE_PIV
                break;
            } else if (TuningMode == IMG_MODE_HIGH_ISO_STILL) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_STILL;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                break;
            }
        } else if (TuningModeExt == DYNAMIC_OB) {
            if (TuningMode == IMG_MODE_LOW_ISO_STILL) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_STILL;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
                pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_RAW2RAW;
                break;
            } else if (TuningMode == IMG_MODE_HIGH_ISO_STILL) {
                pImgMode->Pipe = AMBA_DSP_IMG_PIPE_STILL;
                pImgMode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                pImgMode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_RAW2RAW;
                break;
            }
        }
        HDLR_WARF("Unknown TuningMode: %s, TuningModeExt: %s", _System.TuningMode, _System.TuningModeExt);
    } while (0);
}

static TUNING_MODE_e _USBHdlr_Lookup_Tuning_Mode(const char* Key)
{
    int Mode = 0;
    for (Mode = 0; Mode < IMG_MODE_NUMBER; Mode++) {
        if (strcmp(Key, _str_tuning_mode[Mode]) == 0) {
            break;
        }
    }
    if (Mode == IMG_MODE_NUMBER) {
        HDLR_WARF("%s() %d, Unknown Tuning Mode: %s", __func__, __LINE__, Key);
        return IMG_MODE_VIDEO;
    }
    return (TUNING_MODE_e)Mode;
}

static TUNING_MODE_EXT_e _USBHdlr_Lookup_Tuning_Mode_Ext(const char* Key)
{
    int Mode = 0;
    for (Mode = 0; Mode < TUNING_MODE_EXT_NUMBER; Mode++) {
        if (strcmp(Key, _str_tuning_mode_ext[Mode]) == 0) {
            break;
        }
    }
    if (Mode == TUNING_MODE_EXT_NUMBER) {
        HDLR_WARF("%s() %d, Unknown Tuning Mode Ext: %s", __func__, __LINE__, Key);
        return SINGLE_SHOT;
    }
    return (TUNING_MODE_EXT_e)Mode;
}


static AMBA_DSP_IMG_PIPE_e _USBHdlr_Get_Imag_Pipe(TUNING_MODE_e TuningMode)
{
    AMBA_DSP_IMG_PIPE_e ImagePipe;

    switch(TuningMode)
    {
    case IMG_MODE_VIDEO:
    case IMG_MODE_PREVIEW:
    case IMG_MODE_HIGH_ISO_PREVIEW:
    case IMG_MODE_HIGH_ISO_VIDEO:
            ImagePipe = AMBA_DSP_IMG_PIPE_VIDEO;
            break;
    case IMG_MODE_STILL:
    case IMG_MODE_LOW_ISO_STILL:
    case IMG_MODE_MID_ISO_STILL:
    case IMG_MODE_HIGH_ISO_STILL:
    case IMG_MODE_NIGHT_PORTRAIT_STILL:
            ImagePipe = AMBA_DSP_IMG_PIPE_STILL;
            break;
        default:
            ImagePipe = AMBA_DSP_IMG_PIPE_VIDEO;
            break;
    }
    return ImagePipe;
}

static AMBA_DSP_IMG_ALGO_MODE_e _USBHdlr_Get_Image_Algo_Mode(TUNING_MODE_e TuningMode)
{
    AMBA_DSP_IMG_ALGO_MODE_e ImageAlgoMode;

    switch(TuningMode)
    {
        case IMG_MODE_LOW_ISO_STILL:
            ImageAlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            break;
        case IMG_MODE_HIGH_ISO_STILL:
        case IMG_MODE_HIGH_ISO_VIDEO:
        case IMG_MODE_HIGH_ISO_PREVIEW:
            ImageAlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
            break;
        case IMG_MODE_VIDEO:
        case IMG_MODE_PREVIEW:
            ImageAlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            break;
        case IMG_MODE_STILL:
        case IMG_MODE_MID_ISO_STILL:
        case IMG_MODE_NIGHT_PORTRAIT_STILL:

        default:
            ImageAlgoMode = AMBA_DSP_IMG_ALGO_MODE_FAST;
            break;
    }

    return ImageAlgoMode;
}

static int USB_Get_Calib_Table_Addr(void)
{
    ITUNER_Calib_Table_s* Ituner_Calib_Table;
    ITUNER_VIDEO_HDR_TABLE_s *ITUNER_Hdr_Table = NULL;
    
    AmbaItuner_Get_Calib_Table(&Ituner_Calib_Table);
    calibTable.FPNMap.addr = Ituner_Calib_Table->FPNMap;
    calibTable.WarpGrid.addr = Ituner_Calib_Table->WarpGrid;
    calibTable.CaGrid.addr = Ituner_Calib_Table->Ca_grid;
    calibTable.VignetteTable.addr = Ituner_Calib_Table->VignetteTable;
    calibTable.CcReg.addr = Ituner_Calib_Table->CcReg;
    calibTable.CcThreeD.addr =  Ituner_Calib_Table->Cc3d;

    calibTable.FPNMap.isSet = 0;
    calibTable.WarpGrid.isSet = 0;
    calibTable.CaGrid.isSet = 0;
    calibTable.VignetteTable.isSet= 0;
    calibTable.CcReg.isSet = 0;
    calibTable.CcThreeD.isSet = 0;

    AmbaItuner_Get_HDR_Table(ITUNER_Hdr_Table);
    calibTable.AmpLinear0LookupTable.addr = (UINT8 *)(&(ITUNER_Hdr_Table->AmpLinear0LookupTable[0][0]));
    calibTable.AmpLinear1LookupTable.addr = (UINT8 *)(&(ITUNER_Hdr_Table->AmpLinear1LookupTable[0][0]));

    calibTable.AmpLinear0LookupTable.isSet = 0;
    calibTable.AmpLinear1LookupTable.isSet = 0;
    return 0;
}

static int _USBHdlr_Update_ItunerInfo(void)
{
    ITUNER_SYSTEM_s System;
    TUNING_MODE_e TuningMode;
    AmbaItuner_Get_SystemInfo(&System);
    TuningMode = TUNE_Rule_Lookup_Tuning_Mode(System.TuningMode);
    _Ituner.TuningModeExt = TUNE_Rule_Lookup_Tuning_Mode_Ext(System.TuningModeExt);
    _Ituner.TuningAlgoMode.Pipe = _USBHdlr_Get_Imag_Pipe(TuningMode);
    _Ituner.TuningAlgoMode.AlgoMode = _USBHdlr_Get_Image_Algo_Mode(TuningMode);
    _Ituner.TuningAlgoMode.BatchId = (UINT32)((_Ituner.TuningAlgoMode.Pipe == AMBA_DSP_IMG_PIPE_VIDEO)? AMBA_DSP_VIDEO_FILTER : AMBA_DSP_STILL_LISO_FILTER);
    if (TuningMode == IMG_MODE_NIGHT_PORTRAIT_STILL) {
        System.InputPicCnt = 4;
    } else {
        System.InputPicCnt = 1;
    }
    AmbaItuner_Set_SystemInfo(&System);
    return 0;
}

int USBHdlr_Set_USBConfig(TUNE_Initial_Config_s *TuneInitialConfig)
{
    if (TuneInitialConfig == NULL) {
        return NG;
     }
    USB_Handler.MemPool=TuneInitialConfig->Usb.pBytePool;
    USB_Handler.CBFuncList.StillItunerRawEncode = TuneInitialConfig->Usb.StillItunerRawEncode;
    USB_Handler.CBFuncList.StillTuningPreLoadDone = TuneInitialConfig->Usb.StillTuningPreLoadDone;
    USB_Handler.CBFuncList.StillTuningRawBuffer = TuneInitialConfig->Usb.StillTuningRawBuffer;
    return OK;
}

int USB_CheckProcessingDown()
{
    int Ret;
    Ret=AmbaTUNE_Init_Flg;

    return Ret;
}

int USBHdlr_Init(Ituner_Initial_Config_t *pInitialConfig)
{
    int ReturnValue = 0;
    AmbaItuner_Config_t Ituner_Config;

    //memcpy(&USB_Handler.MemPool,pInitialConfig->pItunerBuffer, sizeof(AMBA_MEM_CTRL_s));

    Rule_Info_t pRuleInfo;
    if(TuningMode == VIDEO||TuningMode == STILL)
    {
        if (AmbaTUNE_Init_Flg == 0 )
            return OK;
        else
            AmbaTUNE_Init_Flg = 0 ;
    }
 	AmbaPrintColor(YELLOW, "USBHdlr_Init" );
        //AMBA_MEM_CTRL_s *pItunerBuffer = NULL;
	//pItunerBuffer = (AMBA_MEM_CTRL_s *)pInitialConfig->pItunerBuffer;

    memset(&_Ituner.TuningAlgoMode,0x0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    _Ituner.TuningAlgoMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    _Ituner.TuningAlgoMode.AlgoMode= AMBA_DSP_IMG_ALGO_MODE_FAST;
    _Ituner.TuningAlgoMode.BatchId= AMBA_DSP_VIDEO_FILTER;
    TUNE_Parser_Create(&_Ituner.ParserObject);
    _Ituner.Is_Idsp_Load_Finish = FALSE;

    TUNE_Rule_Get_Info(&pRuleInfo);


    for(int i = 0; i < pRuleInfo.RegCount; i++) {
        TUNE_Parser_Add_Reg(&pRuleInfo.RegList[i], _Ituner.ParserObject);
    }

	TUNE_Video_HDR_Rule_Get_Info(&pRuleInfo);
    for(int i = 0; i < pRuleInfo.RegCount; i++) {
        TUNE_Parser_Add_Reg(&pRuleInfo.RegList[i], _Ituner.ParserObject);
    }

    TUNE_Parser_Set_Reglist_Valid(0, _Ituner.ParserObject);
    Ituner_Config.Hook_Func.Load_Data = USBHdlr_Load_Data,
    Ituner_Config.Hook_Func.Save_Data = USBHdlr_Save_Data,
    Ituner_Config.pMemPool = (AMBA_KAL_BYTE_POOL_t *)pInitialConfig->pMemPool;
    ReturnValue = AmbaItuner_Init(&Ituner_Config);
    USB_Get_Calib_Table_Addr() ;
    return ReturnValue;
}

static UINT8 USB_READ_TEXT(void* destAddr, int size, int count, TEXT_STREAM* srcAddr)
{
    if( (srcAddr->ptr + size*count) >= srcAddr->size)
        return NG;
    else {
        int idx = srcAddr->ptr;
        memcpy(destAddr, &(srcAddr->text_addr[idx]), size*count);
        srcAddr->ptr = srcAddr->ptr + size*count;
    }
    return OK;
}

static int _read_line(TEXT_STREAM* text, char * Line_Buf, int Buf_Size, char **Ptr)
{
    int Ret = 0;

   *Ptr = Line_Buf;
    while(USB_READ_TEXT(*Ptr, 1, 1, text) == OK) {
        if ((**Ptr == '\r') || (**Ptr == '\n')) {
            **Ptr = 0;
            //AmbaPrintColor(YELLOW, "%s", Line_Buf);
            Ret = 1;
            break;
        }
        (*Ptr)++;
        if ((*Ptr - Line_Buf) >= Buf_Size) {
            HDLR_WARF("%s() %d, Input Text String is too long", __func__, __LINE__);
            Ret = 1;
            break;
        }
    }
    return Ret;
}

static int _USBHdlr_Load_Text(TEXT_STREAM* text)
{
    int Line = 0;
    char *Ptr;

    ITUNER_SYSTEM_s System;
    char *Buf = NULL;
    int BufSize = 0;

    TUNE_Parser_Set_Opmode(ITUNER_DEC);
    if (0 != TUNE_Parser_Set_Reglist_Valid(0, _Ituner.ParserObject)) {
        HDLR_WARF("%s() %d, call TUNE_PArser_Set_Reglist_Valid() Fail", __func__, __LINE__);
        return -1;
    }
    if (0 != TUNE_Parser_Get_LineBuf(_Ituner.ParserObject, &Buf, &BufSize)) {
        HDLR_WARF("%s() %d, call TUNE_Parser_Get_LineBuf() Fail", __func__, __LINE__);
        return -1;
    }
    while(_read_line(text, Buf, BufSize, &Ptr)) {
        TUNE_Parser_Parse_Line(_Ituner.ParserObject);
        Line++;
    }

    AmbaItuner_Get_SystemInfo(&System);

    // If multi shot mode, read in additional config files

    _Ituner.TuningModeExt = _USBHdlr_Lookup_Tuning_Mode_Ext(System.TuningModeExt);

    if (_Ituner.TuningModeExt) {
        HDLR_ERRF("%s() %d, Not Implement!", __func__, __LINE__);
    }
    return 0;
}

int USBHdlr_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_USB_Save_Param_s *USBbuffer)
{
    if (0 != _USBHdlr_Update_System_Info(pMode)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Update_System_Info() Fail", __func__, __LINE__);
        return -1;
    }

  USBHdlr_AmageDataGet(USBbuffer->Buffer, USBbuffer->Offset, USBbuffer->LengthRequested, USBbuffer->ActualLength);

  return 0;
}

int USBHdlr_Load_IDSP(TUNE_USB_Load_Param_s *USBbuffer)
{
    USBHdlr_AmageDataSend(USBbuffer->Buffer, USBbuffer->Offset, USBbuffer->Length);
    _USBHdlr_Update_ItunerInfo();
    return 0;
}

int USBHdlr_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo)
{
    AmbaPrint("USBHdlr_Execute_IDSP_AmbaItuner_Execute");
   	AmbaItuner_Execute(pMode, ProcInfo);

    return 0;
}

int USBHdlr_Get_SystemInfo(ITUNER_SYSTEM_s *System) 
{
    AmbaItuner_Get_SystemInfo(System);
    return 0;
}

int USBHdlr_Set_SystemInfo(ITUNER_SYSTEM_s *System) 
{
    AmbaItuner_Set_SystemInfo(System);
    return 0;
}

int USBHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo) 
{
    AmbaItuner_Get_AeInfo(AeInfo);
    return 0;
}

int USBHdlr_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo) 
{
    AmbaItuner_Set_AeInfo(AeInfo);
    return 0;
}

int USBHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo) 
{
    AmbaItuner_Get_WbSimInfo(WbSimInfo);
    return 0;
}

int USBHdlr_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo) 
{
    AmbaItuner_Set_WbSimInfo(WbSimInfo);
    return 0;
}

int USBHdlr_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo)
{
    ITUNER_SYSTEM_s System;
    TUNING_MODE_e TuningMode;
    AmbaItuner_Get_SystemInfo(&System);
    TuningMode = _USBHdlr_Lookup_Tuning_Mode(System.TuningMode);
    _Ituner.TuningModeExt = _USBHdlr_Lookup_Tuning_Mode_Ext(System.TuningModeExt);
    _Ituner.TuningAlgoMode.Pipe = _USBHdlr_Get_Imag_Pipe(TuningMode);
    _Ituner.TuningAlgoMode.AlgoMode = _USBHdlr_Get_Image_Algo_Mode(TuningMode);
    _Ituner.TuningAlgoMode.BatchId = (_Ituner.TuningAlgoMode.Pipe == AMBA_DSP_IMG_PIPE_VIDEO)? AMBA_DSP_VIDEO_FILTER : AMBA_DSP_STILL_LISO_FILTER;
    memcpy(&ItunerInfo->TuningAlgoMode, &_Ituner.TuningAlgoMode, sizeof(ItunerInfo->TuningAlgoMode));
    ItunerInfo->TuningModeExt = _Ituner.TuningModeExt;
    return 0;
}

static int _USBHdlr_Update_System_Info(AMBA_DSP_IMG_MODE_CFG_s *pMode)
{
    ITUNER_SYSTEM_s System;
    char const *Tuning_Mode_Str;
    AmbaItuner_Get_SystemInfo(&System);
    if (pMode->Pipe == AMBA_DSP_IMG_PIPE_VIDEO) {
        if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
            Tuning_Mode_Str = _str_tuning_mode[IMG_MODE_HIGH_ISO_VIDEO];
        } else {
            Tuning_Mode_Str = _str_tuning_mode[IMG_MODE_VIDEO];
        }
    } else { //AMBA_DSP_IMG_PIPE_STILL
        if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
            Tuning_Mode_Str = _str_tuning_mode[IMG_MODE_HIGH_ISO_STILL];
        } else {
            Tuning_Mode_Str = _str_tuning_mode[IMG_MODE_LOW_ISO_STILL];
        }
    }
    strncpy(System.TuningMode, Tuning_Mode_Str, sizeof(System.TuningMode));
    AmbaItuner_Set_SystemInfo(&System);
    return 0;
}

static int _USBHdlr_Load_Raw(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0; 
}

static int _USBHldr_Load_Vignette(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_VIGNETTE_s VignetteCompensation;
    AmbaItuner_Get_VignetteCompensation(&VignetteCompensation);
    if (VignetteCompensation.Enable) {
        HDLR_DEBF("%s() %d, Load Vignette Gain", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (Ext_File_Param->Vignette_Gain_Load_Param.Address == NULL || Ext_File_Param->Vignette_Gain_Load_Param.Size == 0) {
            HDLR_DEBF("%s() %d, Vignette_Gain_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, VignetteCompensation.CalibTablePath, Ext_File_Param->Vignette_Gain_Load_Param.Address, Ext_File_Param->Vignette_Gain_Load_Param.Size);
            return 0;
        }
        if(calibTable.VignetteTable.isSet == 0)
        {
            HDLR_ERRF("%s() %d, calibTable.Vignette_Flag = 0", __func__, __LINE__);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Vignette Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _USBHldr_Load_Bypass_Vignette(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Load_Warp_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    //char *warp_Data = (char *) (Ext_File_Param->Warp_Table_Load_Param.Address);
    //AmbaPrintColor(RED,"%d",warp_Data[0]);
    //AmbaPrintColor(RED,"%d",warp_Data[1]);
    //AmbaPrintColor(RED,"%d",warp_Data[2]);
    //AmbaPrintColor(RED,"%d",warp_Data[3]);
    ITUNER_WARP_s WarpCompensation;
    AmbaItuner_Get_WarpCompensation(&WarpCompensation);
    if (WarpCompensation.Enable) {
        HDLR_DEBF("%s() %d, Load Warp Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (Ext_File_Param->Warp_Table_Load_Param.Address == NULL || Ext_File_Param->Warp_Table_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, Warp_Table_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, WarpCompensation.WarpGridTablePath, Ext_File_Param->Warp_Table_Load_Param.Address, Ext_File_Param->Warp_Table_Load_Param.Size);
            return -1;
        }
        if(calibTable.WarpGrid.isSet ==0)
        {
            HDLR_ERRF("%s() %d, calibTable.Warp_Flag = 0", __func__, __LINE__);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Warp Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _USBHdlr_Load_Bypass_Horizontal_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Load_Bypass_Vertical_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Load_Ca_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_CHROMA_ABERRATION_s ChromaAberrationInfo;
    AmbaItuner_Get_ChromaAberrationInfo(&ChromaAberrationInfo);
    if (ChromaAberrationInfo.Enable) {
        HDLR_DEBF("%s() %d, Load Ca Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (Ext_File_Param->Ca_Table_Load_Param.Address == NULL || Ext_File_Param->Ca_Table_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, Ca_Table_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, ChromaAberrationInfo.CaGridTablePath, Ext_File_Param->Ca_Table_Load_Param.Address, Ext_File_Param->Ca_Table_Load_Param.Size);
            return -1;
        }
        if(calibTable.CaGrid.isSet ==0)
        {
            HDLR_ERRF("%s() %d, calibTable.Ca_Flag = 0", __func__, __LINE__);
            return -1;
        }
        else{
            memcpy(Ext_File_Param->Ca_Table_Load_Param.Address, calibTable.CaGrid.addr, GRID_TABLE_SIZE);
        }
    }
    else {
        HDLR_DEBF("%s() %d, Ca Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _USBHldr_Load_Bypass_Ca_Horizontal_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Load_Bypass_Ca_Vertical_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Load_FPN_Map(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_FPN_s StaticBadPixelCorrection;
    AmbaItuner_Get_StaticBadPixelCorrection(&StaticBadPixelCorrection);
    if (StaticBadPixelCorrection.Enable) {
        HDLR_DEBF("%s() %d, Load FPN Map", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (Ext_File_Param->FPN_MAP_Ext_Load_Param.Address == NULL || Ext_File_Param->FPN_MAP_Ext_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, FPN_Map_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, StaticBadPixelCorrection.MapPath, Ext_File_Param->FPN_MAP_Ext_Load_Param.Address, Ext_File_Param->FPN_MAP_Ext_Load_Param.Size);
            return -1;
        }
        if(calibTable.FPNMap.isSet ==0)
        {
            HDLR_ERRF("%s() %d, calibTable.FPN_Flag = 0", __func__, __LINE__);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, FPN Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _USBHdlr_Load_Bypass_FPN_Map(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Load_CC_Reg(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_COLOR_CORRECTION_s Color_Correction;
    AmbaItuner_Get_ColorCorrection(&Color_Correction);

    if (Ext_File_Param->CC_Reg_Load_Param.Address == NULL || Ext_File_Param->CC_Reg_Load_Param.Size == 0) {
        HDLR_ERRF("%s() %d, CC Reg Path = %s, Address = %p, Size = %d",
                __func__, __LINE__, Color_Correction.RegPath, Ext_File_Param->CC_Reg_Load_Param.Address, Ext_File_Param->CC_Reg_Load_Param.Size);
        return -1;
    }
    if(calibTable.CcReg.isSet ==0)
    {
         HDLR_ERRF("%s() %d, calibTable.CC_Flag = 0", __func__, __LINE__);
         return -1;
    }
    return 0;
}

static int _USBHdlr_Load_CC_ThreeD(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_COLOR_CORRECTION_s Color_Correction;
    AmbaItuner_Get_ColorCorrection(&Color_Correction);
    if (Ext_File_Param->CC_ThreeD_Load_Param.Address == NULL || Ext_File_Param->CC_ThreeD_Load_Param.Size == 0) {
        HDLR_ERRF("%s() %d, CC 3D Path = %s, Address = %p, Size = %d",
               __func__, __LINE__, Color_Correction.ThreeDPath, Ext_File_Param->CC_ThreeD_Load_Param.Address, Ext_File_Param->CC_ThreeD_Load_Param.Size);
        return -1;
    }
    if(calibTable.CcThreeD.isSet ==0)
    {
        HDLR_ERRF("%s() %d, calibTable.CC_Flag = 0", __func__, __LINE__);
        return -1;
    }
    return 0;
}

static int _USBHdlr_Load_FIRST_SHARPEN_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0; 
}

static int _USBHdlr_Load_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0; 
}

static int _USBHdlr_Load_HISO_LOW_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0; 
}

static int _USBHdlr_Load_HISO_CHROMA_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0; 
}

static int _USBHdlr_Load_LOW_ASF_COMBINE_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0; 
}

static int _USBHdlr_Save_FPN_Map(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_Vignette(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_Warp_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_Ca_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_CC_Reg(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_CC_ThreeD(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_FIRST_SHARPEN_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_HISO_LOW_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_Jpg(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    UINT32 actual_flags=0;

    AmbaPrintColor(YELLOW,"== %s(): Jpg Ready ==",__func__);
    AmbaPrintColor(YELLOW, "JPG_Save_Param addr/size : 0x%.8x/%d",  Ext_File_Param->JPG_Save_Param.Address,Ext_File_Param->JPG_Save_Param.Size);

    if (Ext_File_Param->JPG_Save_Param.Address == NULL || Ext_File_Param->JPG_Save_Param.Size == 0) {
       HDLR_WARF("%s() %d, Jpg Addr = %p, Size = %d",
            __func__, __LINE__, Ext_File_Param->JPG_Save_Param.Address, Ext_File_Param->JPG_Save_Param.Size);
        return -1;
    }

    JPG_INFO.addr = Ext_File_Param->JPG_Save_Param.Address;
    JPG_INFO.size = Ext_File_Param->JPG_Save_Param.Size;	

//********** Wait JPG Copy Done*************//

//        AMBA_KAL_TASK_t *pTask = NULL; 
//        pTask = AmbaKAL_TaskIdentify();
//        AmbaPrintColor(1,"pTask_SaveJpg=%s",pTask->tx_thread_name);

    AmbaKAL_TaskSleep(2000);
    for(int i=0;i<1;i++){
        if (Ext_File_Param->JPG_Save_Param.Address != NULL||Ext_File_Param->JPG_Save_Param.Size != 0){
            MTP_EVENT_s event = {0};

            AmbaPrintColor(YELLOW,"======USB add event======");

            event.MTP_EVENT_CODE = MTP_EC_VENDOR_AMBA_TEST;
            event.MTP_EVENT_PARAMETER_1 = 0x0003;
            event.MTP_EVENT_PARAMETER_2 = 0x0002;
            event.MTP_EVENT_PARAMETER_3 = 0x0001;
            if (AmbaUSB_Class_Mtp_AddEvent(&event) == 0) {
                AmbaPrint("add_event (0x%X) success", MTP_EC_VENDOR_AMBA_TEST);
            }
        }
    }

    if (OK!=AmbaKAL_EventFlagTake(&SaveJpegEventFlag,FLAG_PREPARSE_START, AMBA_KAL_AND_CLEAR,&actual_flags,AMBA_KAL_WAIT_FOREVER)){
        AmbaPrintColor(YELLOW,"%s() %d, Wait For Get Jpeg Timeout", __func__, __LINE__);
        AmbaPrintColor(YELLOW,"Get Jpeg Fail");
        return 0;
    }

    AmbaPrintColor(YELLOW,"%s() %d, Get Jpeg Success", __func__, __LINE__);

    return 0;
}

static int _USBHdlr_Save_Y(Save_Data_to_Path_Info_s *Y_Info)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_UV(Save_Data_to_Path_Info_s *UV_Info)
{
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_YUV(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    int Ret = 0;
    HDLR_WARF("%s() %d, Not yet Implement", __func__, __LINE__);
    Ret |= _USBHdlr_Save_Y(&Ext_File_Param->YUV_Save_Param.Y_Info);
    Ret |= _USBHdlr_Save_UV(&Ext_File_Param->YUV_Save_Param.UV_Info);
    return Ret;
}

static int _USBHdlr_Save_Raw(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not Yet Implement", __func__, __LINE__);
    return 0;
}

static int _USBHdlr_Save_CFA_STAT(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not Yet Implement", __func__, __LINE__);
    return 0;
}


static int _USBHdlr_Save_and_Load_Dummy(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not Yet Implement", __func__, __LINE__);
    return 0;
}

int USBHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s* Ext_File_Param)
{
    int (*Load_Data_Api_List[EXT_FILE_MAX])(Ituner_Ext_File_Param_s *Ext_File_Param) =
    {
        _USBHdlr_Load_FPN_Map, // EXT_FILE_FPN_MAP
        _USBHdlr_Load_Bypass_FPN_Map,
        _USBHldr_Load_Vignette,
        _USBHldr_Load_Bypass_Vignette,
        _USBHdlr_Load_Warp_Table,
        _USBHdlr_Load_Bypass_Horizontal_Table,
        _USBHdlr_Load_Bypass_Vertical_Table,
        _USBHdlr_Load_Ca_Table,
        _USBHldr_Load_Bypass_Ca_Horizontal_Table,
        _USBHdlr_Load_Bypass_Ca_Vertical_Table,
        _USBHdlr_Load_CC_Reg,
        _USBHdlr_Load_CC_ThreeD,
        _USBHdlr_Load_FIRST_SHARPEN_ThreeD_Table,
        _USBHdlr_Load_ASF_ThreeD_Table,
        _USBHdlr_Load_HISO_LOW_ASF_ThreeD_Table,
        _USBHdlr_Load_HISO_CHROMA_ASF_ThreeD_Table,
        _USBHdlr_Load_LOW_ASF_COMBINE_ThreeD_Table,
        _USBHdlr_Load_Raw,
        _USBHdlr_Save_and_Load_Dummy, //EXT_FILE_JPG
        _USBHdlr_Save_and_Load_Dummy, //EXT_FILE_YUV
        _USBHdlr_Save_and_Load_Dummy, //EXT_FILE_CFA_STAT

        //_USBHdlr_Load_FINAL_SHARPEN_ThreeD_Table,
        //_USBHdlr_Load_HISO_ASF_ThreeD_Table,
        //_USBHdlr_Load_HISO_HIGH_ASF_ThreeD_Table,
        //_USBHdlr_Load_HISO_MED1_ASF_ThreeD_Table,
        //_USBHdlr_Load_HISO_MED2_ASF_ThreeD_Table,
        //_USBHdlr_Load_HISO_LI2ND_ASF_ThreeD_Table,
        //_USBHdlr_Load_HISO_HIGH_SHARPEN_ThreeD_Table,
        //_USBHdlr_Load_HISO_MED_SHARPEN_ThreeD_Table,
        //_USBHdlr_Load_HISO_LISO1_SHARPEN_ThreeD_Table,
        //_USBHdlr_Load_HISO_LISO2_SHARPEN_ThreeD_Table,
        //_USBHdlr_Load_HISO_CHROMA_MED_COMBINE_ThreeD_Table,
        //_USBHdlr_Load_HISO_CHROMA_LOW_COMBINE_ThreeD_Table,
        //_USBHdlr_Load_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table,
        //_USBHdlr_Load_HISO_LUMA_NOISE_COMBINE_ThreeD_Table,
        //_USBHdlr_Load_HISO_COMBINE_ThreeD_Table,
    };

    if (Ext_File_Type >= EXT_FILE_MAX) {
        HDLR_ERRF("%s() %d, Invalid Ext_File_Type %d", __func__, __LINE__, Ext_File_Type);
        return -1;
    }
    return Load_Data_Api_List[Ext_File_Type](Ext_File_Param);
}

int USBHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s* Ext_File_Param)
{
    int (*Save_Data_Api_List[EXT_FILE_MAX])(Ituner_Ext_File_Param_s *Ext_File_Param) =
    {
        _USBHdlr_Save_FPN_Map, // EXT_FILE_FPN_MAP
        _USBHdlr_Save_and_Load_Dummy,//_USBHdlr_Save_Bypass_FPN_Map,
        _USBHdlr_Save_Vignette,
        _USBHdlr_Save_and_Load_Dummy,//_USBHldr_Save_Bypass_Vignette,
        _USBHdlr_Save_Warp_Table,
        _USBHdlr_Save_and_Load_Dummy,//_USBHdlr_Save_Bypass_Horizontal_Table,
        _USBHdlr_Save_and_Load_Dummy,//_USBHdlr_Save_Bypass_Vertical_Table,
        _USBHdlr_Save_Ca_Table,
        _USBHdlr_Save_and_Load_Dummy, //_USBHldr_Save_Bypass_Ca_Horizontal_Table,
        _USBHdlr_Save_and_Load_Dummy, //_USBHdlr_Save_Bypass_Ca_Vertical_Table,
        _USBHdlr_Save_CC_Reg,
        _USBHdlr_Save_CC_ThreeD,
        _USBHdlr_Save_FIRST_SHARPEN_ThreeD_Table,
        _USBHdlr_Save_ASF_ThreeD_Table,
        _USBHdlr_Save_HISO_LOW_ASF_ThreeD_Table,
        _USBHdlr_Save_and_Load_Dummy, //_USBHldr_Save_HISO_CHROMA_MED_COMBINE
        _USBHdlr_Save_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table,
        _USBHdlr_Save_Raw,
        _USBHdlr_Save_Jpg,
        _USBHdlr_Save_YUV,
        _USBHdlr_Save_CFA_STAT,

        //_USBHdlr_Save_FINAL_SHARPEN_ThreeD_Table,
        //_USBHdlr_Save_HISO_ASF_ThreeD_Table,
        //_USBHdlr_Save_HISO_HIGH_ASF_ThreeD_Table,

        //_USBHdlr_Save_HISO_MED1_ASF_ThreeD_Table,
        //_USBHdlr_Save_HISO_MED2_ASF_ThreeD_Table,
        //_USBHdlr_Save_HISO_LI2ND_ASF_ThreeD_Table,
        //_USBHdlr_Save_HISO_CHROMA_ASF_ThreeD_Table,

        //_USBHdlr_Save_HISO_HIGH_SHARPEN_ThreeD_Table,
        //_USBHdlr_Save_HISO_MED_SHARPEN_ThreeD_Table,
        //_USBHdlr_Save_HISO_LISO1_SHARPEN_ThreeD_Table,
        //_USBHdlr_Save_HISO_LISO2_SHARPEN_ThreeD_Table,

        //_USBHdlr_Save_HISO_CHROMA_MED_COMBINE_ThreeD_Table,
        //_USBHdlr_Save_HISO_CHROMA_LOW_COMBINE_ThreeD_Table,

        //_USBHdlr_Save_HISO_LUMA_NOISE_COMBINE_ThreeD_Table,
        //_USBHdlr_Save_LOW_ASF_COMBINE_ThreeD_Table,
        //_USBHdlr_Save_HISO_COMBINE_ThreeD_Table,

        //_USBHdlr_Save_and_Load_Dummy, //_USBHdlr_Save_Raw,
    };
    if (Ext_File_Type >= EXT_FILE_MAX) {
        HDLR_ERRF("%s() %d, Invalid Ext_File_Type %d", __func__, __LINE__, Ext_File_Type);
        return -1;
    }
    return Save_Data_Api_List[Ext_File_Type](Ext_File_Param);
}

static int _USBHdlr_Get_Color_Correction(void)
{
    AMBA_DSP_IMG_COLOR_CORRECTION_REG_s CcReg;
    AMBA_DSP_IMG_COLOR_CORRECTION_s CcThreed;
    AmbaPrint("ColorCorrection Get");

    AmbaItuner_Get_CcReg(&CcReg);
    memcpy(&Data[0], (UINT8*) CcReg.RegSettingAddr, AMBA_DSP_IMG_CC_REG_SIZE);
    AmbaItuner_Get_CcThreeD(&CcThreed);
    memcpy(&Data[AMBA_DSP_IMG_CC_REG_SIZE], (UINT8*) CcThreed.MatrixThreeDTableAddr, AMBA_DSP_IMG_CC_3D_SIZE);

    return OK;
}

static int _USBHdlr_Get_Exposure_Control(void)
{
    /* Dgain & Iris */
    AMBA_AE_INFO_s AeInfo;
    AmbaPrint("ExposureControl Get");
    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, 0, IP_MODE_VIDEO, (UINT32)&AeInfo);
    ((EC_INFO *)Data)->GainFactor = AeInfo.AgcGain;
    ((EC_INFO *)Data)->ShutterSpeed = AeInfo.ShutterTime;
    //((EC_INFO *)Data)->iris_index = AeInfo.IrisIndex;
    //((EC_INFO *)Data)->dgain =  AeInfo.Dgain;
    return OK;
}

static int _USBHdlr_Save_Text(void)
{
    //POSIX_FILE *Fd;
    int RegIndex;
    int ParamIndex;
    int txtdatasize = 0;
    char endofr='\r';
    char endchangelin='\n';
    char endoft='\t';
    REG_s *Reg;
    char *Buf = NULL;
    int BufSize = 0;
    AMBA_ITUNER_VALID_FILTER_t Filter_Status;
    ITUNER_INFO_s ItunerInfo;
    AmbaPrint("[AmbaTune_USBHdlr] _USBHdlr_Get_ItunerFile");

    //UINT8* pdata = (UINT8*) &Data;
    USBHdlr_Get_ItunerInfo(&ItunerInfo);
    if (0 != _USBHdlr_Update_System_Info(&ItunerInfo.TuningAlgoMode)) {
        HDLR_WARF("%s() %d, call _USBHdlr_Update_System_Info() Fail", __func__, __LINE__);
        return -1;
    }

	if (HDR_FUNC_MODE == 1){
		_Ituner.TuningAlgoMode.FuncMode=AMBA_DSP_IMG_FUNC_MODE_VHDR;
		HDR_FUNC_MODE = 0;
	}
	
    if (0 != AmbaItuner_Refresh(&_Ituner.TuningAlgoMode)) {
        HDLR_WARF("%s() %d, call AmbaItuner_Refresh() Fail", __func__, __LINE__);
        return -1;
    }
    TUNE_Parser_Set_Reglist_Valid(SKIP_VALID_CHECK, _Ituner.ParserObject);
    TUNER_Parser_Set_Reg_Valid("internal", 0, _Ituner.ParserObject);

    if (0 != TUNE_Parser_Get_LineBuf(_Ituner.ParserObject, &Buf, &BufSize)) {
        AmbaPrint("[AmbaTune_USBHdlr] USBHdlr_Get_ItunerFile_Get_LineBuf");
        HDLR_WARF("%s() %d, call TUNE_Parser_Get_LineBuf() Fail", __func__, __LINE__);
        return -1;
    }
    TUNE_Parser_Set_Opmode(ITUNER_ENC);
    AmbaItuner_Get_FilterStatus(&Filter_Status);
    for (RegIndex = 0; RegIndex < _Ituner.ParserObject->RegCount; RegIndex++) {
        Reg = TUNE_Parser_Get_Reg(RegIndex, _Ituner.ParserObject);
        if (Reg == NULL) {
            HDLR_WARF("%s() %d, Reg %d is NULL", __func__, __LINE__, RegIndex);
            continue;
        }
        if (Filter_Status[Reg->Index] == 0) {
              continue;
        }

        for (ParamIndex = 0; ParamIndex < Reg->ParamCount; ParamIndex++) {
            if (0 != TUNE_Parser_Generate_Line(RegIndex, ParamIndex, _Ituner.ParserObject)) {
                continue;
            }
            //AmbaPrintColor(YELLOW, "%s", Buf);
            memcpy((Data+txtdatasize),Buf,strlen(Buf));
            txtdatasize=txtdatasize+strlen(Buf);
            *(Data+txtdatasize-sizeof(char))=endofr;
            *(Data+txtdatasize)=endchangelin;
            txtdatasize=txtdatasize+sizeof(char);
        }
    }
    //****endmark of ituner file**************//
    *(Data+txtdatasize)=endoft;
    *(Data+txtdatasize+sizeof(char))=endoft;
    *(Data+txtdatasize+2*sizeof(char))=endoft;
    *(Data+txtdatasize+3*sizeof(char))=endofr;
    *(Data+txtdatasize+4*sizeof(char))=endchangelin;

    return 0;
}

static int _USBHdlr_Get_Item_Process(void)
{
    AmbaItuner_Refresh(&_Ituner.TuningAlgoMode);

    switch (Item) {
        case ITUNER_FILE:
            AmbaPrint("[AmbaTune_USBHdlr] _USBHdlr_Get_Item_Process Get_ITUNER_FILE");
            _USBHdlr_Save_Text();
            break;

        case ColorCorrection:
            AmbaPrint("[AmbaTune_USBHdlr] _USBHdlr_Get_Item_Process Get_ColorCorrection");
            _USBHdlr_Get_Color_Correction();
            break;

        case ConfigAAAControl:{
           AMBA_3A_OP_INFO_s   AaaOpInfo;
           UINT32 ChNo = 0;
           AmbaPrint("ConfigAAAControl Get");
           AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, ChNo, (UINT32)&AaaOpInfo, 0);
           ((AAA_CTRL *) Data)->ae_op = AaaOpInfo.AeOp;
           ((AAA_CTRL *) Data)->awb_op = AaaOpInfo.AwbOp;
           ((AAA_CTRL *) Data)->af_op =  AaaOpInfo.AfOp;
           ((AAA_CTRL *) Data)->adj_op = AaaOpInfo.AdjOp;}
           break;
        case ExposureControl:
            _USBHdlr_Get_Exposure_Control();
            break;

        case SAVE_JPG:
            AmbaPrint("JPG Get");
            //AmbaTUNE_Init_Flg = 1;
            break;

        default:
            AmbaPrint("default");
            break;
    }
    return OK;
}

static int _USBHdlr_Set_Color_Correction(void)
{
    ITUNER_COLOR_CORRECTION_s  Temp;
    AmbaPrint("ColorCorrection Send");

    strcpy(Temp.RegPath, "From USB");
    strcpy(Temp.ThreeDPath, "From USB");
    AmbaItuner_Set_ColorCorrection((ITUNER_COLOR_CORRECTION_s *) &Temp);
    memcpy(calibTable.CcReg.addr, &Data[0], AMBA_DSP_IMG_CC_REG_SIZE);
    memcpy(calibTable.CcThreeD.addr, &Data[AMBA_DSP_IMG_CC_REG_SIZE], AMBA_DSP_IMG_CC_3D_SIZE);
    AmbaPrintColor(YELLOW, "Color_Correction addr : 0x%.8x", calibTable.CcReg.addr);
    AmbaPrintColor(YELLOW, "Color_Correction addr : 0x%.8x", calibTable.CcThreeD.addr);
    calibTable.CcReg.isSet = 1;
    calibTable.CcThreeD.isSet = 1;

    return OK;
}

static int _USBHdlr_Set_Exposure_Control(void)
{
    AmbaPrint("ExposureControl Send");
    if(TuningMode == VIDEO){
        AMBA_AE_INFO_s AeInfo = {0};
        UINT32 Mode = 0;
        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, Mode, IP_MODE_VIDEO, (UINT32)&AeInfo);
        //AmbaPrintColor(YELLOW, "BLC: ShutterTime: %f AgcGain: %f Dgain: %d", AeInfo.ShutterTime, AeInfo.AgcGain, AeInfo.Dgain);

        AeInfo.AgcGain =  ((EC_INFO *)Data)->GainFactor;
        AeInfo.ShutterTime = ((EC_INFO *)Data)->ShutterSpeed;
        AeInfo.Flash = 0;
        AeInfo.IrisIndex = 0;
        AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, Mode, IP_MODE_VIDEO, (UINT32)&AeInfo);
        //AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO,Mode, IP_MODE_STILL, (UINT32)&AeInfo);
        AmbaPrint("AgcGain=%f",AeInfo.AgcGain);
        AmbaPrint("ShutterTime=%f",AeInfo.ShutterTime);
    }
	/*
    else{  // only for ae_info.dgain
        EC_INFO* pData = (EC_INFO*) &Data;
        ITUNER_AE_INFO_s AeInfo;
        AmbaItuner_Get_AeInfo(&AeInfo);
        AeInfo.Dgain = pData->dgain;
        AmbaItuner_Set_AeInfo(&AeInfo);
    }
    */

     return OK;
}

static int _USBHdlr_Set_Tuning_Mode(void)
{
    UINT8 *pData = (UINT8*) &Data;
    //void *Buf=NULL;
    //void *BufRaw=NULL;
    AmbaPrint("TUNING_MODE Send");

    TuningMode = *pData ;
    if(TuningMode == VIDEO)
    {
        int ReturnValue=-1;
        AmbaPrint("TuningMode: VIDEO Mode");
        AmbaTUNE_Init_Flg = 1;

        //***MWUT***
        AmbaKAL_MemFree(&Still_Data_Buff);	
        ReturnValue=AmbaKAL_MemAllocate(USB_Handler.MemPool, &Still_Data_Buff, sizeof(ITUNER_FPN_t), 1);

        if (ReturnValue != 0) {
            HDLR_DEBF("%s() %d, MemAllocate failed", __func__, __LINE__);
            return ReturnValue;
        }
    }
    else if(TuningMode == STILL)
    {
        int ReturnValue=0;
        AMBA_3A_OP_INFO_s  AAAOpInfo;
		UINT32 ChannelCount;
		UINT32 i;
		
        AmbaPrint("TuningMode: STILL MODE");
        AmbaTUNE_Init_Flg = 1;

        //***MWUT***
        AmbaKAL_MemFree(&Still_Data_Buff);	
        ReturnValue=AmbaKAL_MemAllocate(USB_Handler.MemPool, &Still_Data_Buff, sizeof(ITUNER_FPN_t), 1);

        if (ReturnValue != 0) {
            HDLR_DEBF("%s() %d, MemAllocate failed", __func__, __LINE__);
            return ReturnValue;
        }
        //***close 3a***
        AAAOpInfo.AeOp = 0;
        AAAOpInfo.AwbOp = 0;
        AAAOpInfo.AfOp = 0;
        AAAOpInfo.AdjOp = 0;
		AmbaImg_Proc_Cmd(MW_IP_GET_TOTAL_CH_COUNT, (UINT32)&ChannelCount,0, 0);
		for(i = 0; i < ChannelCount; i++){
			AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, i, (UINT32)&AAAOpInfo, 0);
		}
		
        AmbaPrint("=====3A disable=====");
    }
    else{
        AmbaPrint("TuningMode: IQTable MODE");
    }
    return OK;
}

void _USB_StillRawEncodeThreadEntry(UINT32 arg)
{
    ITUNER_SYSTEM_s System; 
    HDLR_DEBF("%s() %d ", __func__, __LINE__);
    USBHdlr_Get_SystemInfo(&System);
    AmbaPrintColor(YELLOW,"MainW=%u  MainH=%u",System.MainWidth,System.MainHeight);
    AmbaPrintColor(YELLOW,"RawW=%u RawW=%u RawPitch=%u RawResolution=%u",System.RawWidth,System.RawHeight,System.RawPitch,System.RawResolution);
    AmbaPrintColor(YELLOW,"RawStartX=%d RawStartY=%u",System.RawStartX,System.RawStartY);

    AmbaPrintColor(YELLOW,"%s() Still RawEncode Execute ",__func__);	
    //AmpUt_StillTuningPreLoadDone();
    //AmpUT_ItunerRawEncode();
    USB_Handler.CBFuncList.StillTuningPreLoadDone();
    USB_Handler.CBFuncList.StillItunerRawEncode();

    AmbaPrintColor(GREEN,"%s() Still RawEncode Done",__func__);	

    StillRawEncodeThreadflg=0;
    AmbaTUNE_Init_Flg = 1;

    while(1) {
        AmbaKAL_TaskSleep(5000);
    }
}

static int _USBHdlr_Ituner_Execute(void)
{
    if ((TuningMode == VIDEO) && (Item == ITUNER_FILE)){
        AMBA_ITUNER_PROC_INFO_s ProcInfo;
        AMBA_DSP_IMG_MODE_CFG_s Mode;
        USBHdlr_Get_SystemInfo(&_System);
        strncpy(_System.TuningMode, TUING_MODE_TO_STR(IMG_MODE_PREVIEW), sizeof(_System.TuningMode));
        USBHdlr_Set_SystemInfo(&_System);
        memcpy(&Mode, &_Ituner.TuningAlgoMode, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
        HDLR_DEBF("%s() %d ", __func__, __LINE__);
        AmbaItuner_Execute(&_Ituner.TuningAlgoMode, &ProcInfo);
        AmbaTUNE_Init_Flg = 1;

        // for Hybride Mode PostExecute
        {
            UINT32 MainViewId = 0; //0:single chan,1:dul chan
            UINT8 ImgPipe = 0;
            UINT8 ImgAlgo = 0;
            UINT8 ImgFunc = 0;
            UINT8 AdjUpdate = 0;
            UINT8 HdrUpdate = 0;

            AMBA_IMG_SCHDLR_EXP_INFO_s CurExpInfo = {0};
            AMBA_IMG_SCHDLR_EXP_s ExpInfo = {0};
            int CtxId = 0;

            _USBHdlr_Get_ImgMode(&Mode);
            if(strncmp (_System.TuningModeExt,"SINGLE_SHOT_MULTI_EXPOSURE_HDR",30) == 0){ 
                MainViewId = 0; //0:single chan,1:dul chan
                ImgPipe = Mode.Pipe;
                ImgAlgo = Mode.AlgoMode;
                ImgFunc = Mode.FuncMode;
                AdjUpdate = 1;
                HdrUpdate = 1;			 
            }
            else{
                MainViewId = 0; //0:single chan,1:dul chan
                ImgPipe = Mode.Pipe;
                ImgAlgo = Mode.AlgoMode;
                ImgFunc = Mode.FuncMode;
                AdjUpdate = 1;
                HdrUpdate = 0;
            }

            AmbaPrintColor(5,"t schdlr sys cfgupdate %d %d %d %d %d %d",MainViewId,ImgPipe,ImgAlgo,ImgFunc,AdjUpdate,HdrUpdate);	  
            AmbaImgSchdlr_GetExposureInfo(MainViewId, &CurExpInfo);
            memcpy(&ExpInfo.Info, &CurExpInfo, sizeof(AMBA_IMG_SCHDLR_EXP_INFO_s));
            ExpInfo.Type = AMBA_IMG_SCHDLR_SET_TYPE_DIRECT_EXE;
            ExpInfo.Info.AdjUpdated = AdjUpdate;
            ExpInfo.Info.HdrUpdated[0].Data = HdrUpdate;
            ExpInfo.Info.Mode.Pipe = (AMBA_DSP_IMG_PIPE_e)ImgPipe;
            ExpInfo.Info.Mode.AlgoMode = (AMBA_DSP_IMG_ALGO_MODE_e)ImgAlgo;
            ExpInfo.Info.Mode.FuncMode = (AMBA_DSP_IMG_FUNC_MODE_e)ImgFunc;
            CtxId = AmbaImgSchdlr_GetIsoCtxIndex(MainViewId, ExpInfo.Info.Mode.AlgoMode);
            if (CtxId >= 0) {
                ExpInfo.Info.Mode.ContextId = CtxId;
            }
            AmbaImgSchdlr_SetExposure(MainViewId, &ExpInfo);
        }
    }
    if((TuningMode == STILL) && (Item == RAWDATA)){
        UINT Threadstatus;
        if(StillRawEncodeThreadflg==0){
            StillRawEncodeThreadflg=1;
            AmbaKAL_TaskTerminate(&StillRawEncodeThread);			
            if(TX_SUCCESS!= AmbaKAL_TaskDelete(&StillRawEncodeThread)) {
                AmbaPrintColor(RED,"%s(): Fail to delete thread, Line %d\n", __func__, __LINE__);
                return NG;
            }
            if(TX_SUCCESS != AmbaKAL_EventFlagDelete(&SaveJpegEventFlag)) {
                AmbaPrintColor(RED,"%s(): Fail to delete SaveJpegEventFlag flag, Line %d\n", __func__, __LINE__);
                return NG;
            }
        }

        if(StillRawEncodeThreadflg==1){
            AmbaPrintColor(6,"StillRawEncodeThreadflg=%d",StillRawEncodeThreadflg);
		
            if(TX_SUCCESS != AmbaKAL_EventFlagCreate(&SaveJpegEventFlag)){
                AmbaPrintColor(RED,"%s(): Fail to create SaveJpegEventFlag, Line %d\n", __func__, __LINE__);
                return NG;
            }
            if(TX_SUCCESS != AmbaKAL_EventFlagClear(&SaveJpegEventFlag,FLAG_PREPARSE_START)) {
                AmbaPrintColor(RED,"%s(): Fail to clear SaveJpegEventFlag, Line %d\n", __func__, __LINE__);
                return NG;
            }				
            Threadstatus = AmbaKAL_TaskCreate(&StillRawEncodeThread,"USBHdlr_StillRawEncodeThread",50,_USB_StillRawEncodeThreadEntry,
                                                    0,StillRawEncodeThreadStack,StillRawEncode_THREAD_STACK_SIZE,AMBA_KAL_AUTO_START);
            if(Threadstatus != TX_SUCCESS) {
                AmbaPrintColor(RED,"%s(): Fail to create StillRawEncodeThread 0x%2x\n", __func__, Threadstatus);
                return NG;
            }
        }
    }
    //AMBA_KAL_TASK_t *pTask = NULL;
    //AmbaPrintColor(YELLOW,"return main thread");
    //pTask = AmbaKAL_TaskIdentify();
    //AmbaPrintColor(1,"pTask:_USBHdlr_Ituner_Execute=%s",pTask->tx_thread_name);
    return OK;
}

static int _USBHdlr_Set_Item_Process(void)
{
    HDLR_DEBF("%s() %d ", __func__, __LINE__);

    switch (Item) {
        case TUNING_MODE:
			AmbaPrintColor(YELLOW, "Compatiable with Amage A.1.1.0");
            _USBHdlr_Set_Tuning_Mode();
            break;
		case HDR_Mode:
			AmbaPrint("HDR_Mode Send");		
			HDR_FUNC_MODE = (UINT8) (Data[0]);
		break;
        case ITUNER_FILE:{
            AmbaPrint("ITUNER_FILE Send");
            hisoText.text_addr = Still_Data_Buff.pMemAlignedBase;
            hisoText.size = AmageSendDataSize;
            hisoText.ptr = 0;

            _USBHdlr_Load_Text(&hisoText);
            break;
        }

        case ColorCorrection:
            _USBHdlr_Set_Color_Correction();
            break;

        case ConfigAAAControl:{
            AMBA_3A_OP_INFO_s   AaaOpInfo;
			UINT32 ChannelCount;
			UINT32 i;
            AmbaPrint("ConfigAAAControl Send");
			
            AaaOpInfo.AeOp = ((AAA_CTRL *) Data)->ae_op;
            AaaOpInfo.AwbOp = ((AAA_CTRL *) Data)->awb_op;
            AaaOpInfo.AfOp = ((AAA_CTRL *) Data)->af_op;
            AaaOpInfo.AdjOp = ((AAA_CTRL *) Data)->adj_op;

            AmbaImg_Proc_Cmd(MW_IP_GET_TOTAL_CH_COUNT, (UINT32)&ChannelCount,0, 0);
			
			for(i = 0; i < ChannelCount; i++){
                 AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, i, (UINT32)&AaaOpInfo, 0);
            }
            AmbaPrint("--- AeOp : %d, AwbOp : %d, AdjOp : %d, AfOp : %d ---",
                 AaaOpInfo.AeOp,
                 AaaOpInfo.AwbOp,
                 AaaOpInfo.AfOp,
                 AaaOpInfo.AdjOp);
            }
        	break;

        case ExposureControl:
            _USBHdlr_Set_Exposure_Control();
            break;
        case Adj_VideoLiso:{
            UINT32 Addr = 0;
            UINT8* adj_videoliso = NULL;
            static ADJ_VIDEO_PARAM_s Adj_VideoTmp ;
            static COLOR_3D_s        Adj_ColorTmp ;
            static ADJ_VIDEO_PARAM_s *pAdjVideoTmp;
            AMBA_3A_OP_INFO_s  Aaa_OpInfo;
            AmbaPrint("Adj_VideoLiso send");	

            //***close 3a***

            Aaa_OpInfo.AeOp = 0;
            Aaa_OpInfo.AwbOp = 0;
            Aaa_OpInfo.AfOp = 0;
            Aaa_OpInfo.AdjOp = 0;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A disable=====");

            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_VIDEO_ADJ, (UINT32)&Addr);
            pAdjVideoTmp = (ADJ_VIDEO_PARAM_s *)Addr;	
            Adj_ColorTmp = pAdjVideoTmp->FilterParam.Def.Color;        
            adj_videoliso=(UINT8*) IQTable_Buff.pMemAlignedBase;

            memcpy((UINT8*)&Adj_VideoTmp,(UINT8*)adj_videoliso,sizeof(ADJ_VIDEO_PARAM_s));		

            for(int i = 0;i < 5;i++){
                Adj_VideoTmp.FilterParam.Def.Color.Table[i].MatrixThreeDTableAddr = Adj_ColorTmp.Table[i].MatrixThreeDTableAddr;
            }
            memcpy(pAdjVideoTmp,&Adj_VideoTmp,sizeof(Adj_VideoTmp));
            //AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_VIDEO_ADJ, (UINT32)&Adj_VideoTmp); ///New

            AmbaPrintColor(YELLOW, "Adj_VideoTmp : 0x%.8x", &Adj_VideoTmp);

            //***close 3a***
            Aaa_OpInfo.AeOp = 1;
            Aaa_OpInfo.AwbOp = 1;
            Aaa_OpInfo.AfOp = 1;
            Aaa_OpInfo.AdjOp = 1;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A Enable=====");
        }
        break;

        case Adj_Still_Liso:{
            UINT32 Addr = 0;
            UINT8* adj_Stillliso = NULL;
            static ADJ_STILL_FAST_LISO_PARAM_S Adj_StillLisoTmp ;
            static COLOR_3D_s        Adj_ColorTmp ;
            static ADJ_STILL_FAST_LISO_PARAM_S *pAdjStillLisoTmp;
            AMBA_3A_OP_INFO_s  Aaa_OpInfo;
            AmbaPrint("Adj_Still_Liso send");	

            //***close 3a***

            Aaa_OpInfo.AeOp = 0;
            Aaa_OpInfo.AwbOp = 0;
            Aaa_OpInfo.AfOp = 0;
            Aaa_OpInfo.AdjOp = 0;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A disable=====");

            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_STILL_LISO_ADJ, (UINT32)&Addr);
            pAdjStillLisoTmp = (ADJ_STILL_FAST_LISO_PARAM_S *)Addr;	
            Adj_ColorTmp = pAdjStillLisoTmp->Def.Color;        
            adj_Stillliso=(UINT8*) IQTable_Buff.pMemAlignedBase;

            memcpy((UINT8*)&Adj_StillLisoTmp,(UINT8*)adj_Stillliso,sizeof(ADJ_STILL_FAST_LISO_PARAM_S));		

            for(int i = 0;i < 5;i++){
                Adj_StillLisoTmp.Def.Color.Table[i].MatrixThreeDTableAddr = Adj_ColorTmp.Table[i].MatrixThreeDTableAddr;
            }
            AmbaPrintColor(YELLOW, "Adj_StillLisoTmp : 0x%.8x", &Adj_StillLisoTmp);
            memcpy(pAdjStillLisoTmp,&Adj_StillLisoTmp,sizeof(ADJ_STILL_FAST_LISO_PARAM_S));
            //AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_STILL_LISO_ADJ, (UINT32)&Adj_StillLisoTmp); ///New

            //***close 3a***
            Aaa_OpInfo.AeOp = 1;
            Aaa_OpInfo.AwbOp = 1;
            Aaa_OpInfo.AfOp = 1;
            Aaa_OpInfo.AdjOp = 1;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A Enable=====");
        }
        break;

        case Adj_Still_Hiso:{
            UINT32 Addr = 0;
            UINT8* adj_StillHiso = NULL;
            static ADJ_STILL_HISO_PARAM_s Adj_StillHisoTmp ;
            static COLOR_3D_s        Adj_ColorTmp ;
            static ADJ_STILL_HISO_PARAM_s *pAdjStillHisoTmp;
            AMBA_3A_OP_INFO_s  Aaa_OpInfo;
            AmbaPrint("*****Adj_Still_Hiso send*******");	

            //***close 3a***

            Aaa_OpInfo.AeOp = 0;
            Aaa_OpInfo.AwbOp = 0;
            Aaa_OpInfo.AfOp = 0;
            Aaa_OpInfo.AdjOp = 0;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A disable=====");

            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_STILL_HISO_ADJ, (UINT32)&Addr);
            pAdjStillHisoTmp = (ADJ_STILL_HISO_PARAM_s *)Addr;	
            Adj_ColorTmp = pAdjStillHisoTmp->Def.Color;        
            adj_StillHiso=(UINT8*) IQTable_Buff.pMemAlignedBase;

            memcpy((UINT8*)&Adj_StillHisoTmp,(UINT8*)adj_StillHiso,sizeof(ADJ_STILL_HISO_PARAM_s));		

            for(int i = 0;i < 5;i++){
                Adj_StillHisoTmp.Def.Color.Table[i].MatrixThreeDTableAddr = Adj_ColorTmp.Table[i].MatrixThreeDTableAddr;
            }
            AmbaPrintColor(RED, "Adj_StillHisoTmp : 0x%.8x", &Adj_StillHisoTmp);
            memcpy(pAdjStillHisoTmp,&Adj_StillHisoTmp,sizeof(ADJ_STILL_HISO_PARAM_s));
            //AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_STILL_LISO_ADJ, (UINT32)&Adj_StillLisoTmp); ///New

            //***close 3a***
            Aaa_OpInfo.AeOp = 1;
            Aaa_OpInfo.AwbOp = 1;
            Aaa_OpInfo.AfOp = 1;
            Aaa_OpInfo.AdjOp = 1;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A Enable=====");
        }
        break;

        case Adj_Photo:{
            UINT32 Addr = 0;
            UINT8* adj_photo = NULL;
            static ADJ_PHOTO_PARAM_s Adj_photoTmp ;
            static COLOR_3D_s        Adj_ColorTmp ;
            static ADJ_PHOTO_PARAM_s *pAdphotoTmp;
            AMBA_3A_OP_INFO_s  Aaa_OpInfo;
            AmbaPrint("Adj_VideoLiso send");	

            //***close 3a***

            Aaa_OpInfo.AeOp = 0;
            Aaa_OpInfo.AwbOp = 0;
            Aaa_OpInfo.AfOp = 0;
            Aaa_OpInfo.AdjOp = 0;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A disable=====");

            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)0, IQ_PARAMS_PHOTO_ADJ, (UINT32)&Addr);
            pAdphotoTmp = (ADJ_PHOTO_PARAM_s *)Addr;	
            Adj_ColorTmp = pAdphotoTmp->FilterParam.Def.Color;        
            adj_photo=(UINT8*) IQTable_Buff.pMemAlignedBase;

            memcpy((UINT8*)&Adj_photoTmp,(UINT8*)adj_photo,sizeof(ADJ_PHOTO_PARAM_s));		

            for(int i = 0;i < 5;i++){
                Adj_photoTmp.FilterParam.Def.Color.Table[i].MatrixThreeDTableAddr = Adj_ColorTmp.Table[i].MatrixThreeDTableAddr;
            }
            memcpy(pAdphotoTmp,&Adj_photoTmp,sizeof(ADJ_PHOTO_PARAM_s));
            AmbaPrintColor(YELLOW, "Adj_photoTmp : 0x%.8x", &Adj_photoTmp);

            //***close 3a***
            Aaa_OpInfo.AeOp = 1;
            Aaa_OpInfo.AwbOp = 1;
            Aaa_OpInfo.AfOp = 1;
            Aaa_OpInfo.AdjOp = 1;
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, 0, (UINT32)&Aaa_OpInfo, 0);
            AmbaPrint("=====3A Enable=====");
        }
        break;

        case RAWDATA:{
            AmbaPrint("RAW DATA Send");
            AmbaPrintColor(YELLOW, "RawBuf Window_Width: %d", RawBuf.Window.Width*2);
            AmbaPrintColor(YELLOW, "RawBuf Pitch= %d", RawBuf.Pitch);			

            if(RawBuf.Pitch!=2*RawBuf.Window.Width){
                UINT8* RawAddr=NULL;
                int TrueAddr,FalseAddr;				
                UINT8* temp_RawBuf = NULL;
                RawAddr=RawBuf.pBaseAddr;								

                temp_RawBuf=(UINT8*)Still_Data_Buff.pMemAlignedBase;

                AmbaPrintColor(YELLOW,"RAW need to alignment");	

                for (int j=(RawBuf.Window.Height-1);j>=0;j--){
                    TrueAddr=j*RawBuf.Pitch;
                    FalseAddr=j*(RawBuf.Window.Width*2);
                    memcpy(&temp_RawBuf[0],&RawAddr[FalseAddr],(RawBuf.Window.Width*2));	
                    memcpy(&RawAddr[TrueAddr],&temp_RawBuf[0],(RawBuf.Window.Width*2));
                }
                AmbaCache_Clean(RawBuf.pBaseAddr, RawBuf.Pitch*RawBuf.Window.Height);
            }
        }
        break;

        default:
            AmbaPrint("default");
            break;
    }

    return _USBHdlr_Ituner_Execute();
}

UINT32 USBHdlr_AmageCommunication (UINT32 Parameter1, UINT32 Parameter2, UINT32 Parameter3, UINT32 Parameter4, UINT32 Parameter5, UINT32* length, UINT32* dir)
{
    HDLR_DEBF("%s() %d ", __func__, __LINE__);
    *dir = Parameter1;
    *length = Parameter2;
    Item = Parameter3;
    AmageSendDataSize = Parameter2;
    AmbaPrint("P1 = 0x%x, P2 = 0x%x, P3 = 0x%x, P4 = 0x%x, P5 = 0x%x, length = 0x%x, dir = 0x%x", Parameter1, Parameter2, Parameter3, Parameter4, Parameter5, *length, *dir);

    if (*dir == AMAGE_ARM_TO_PC)
    {
        switch (Item){
            case ITUNER_FILE:
                AmbaPrint("[AmbaTune_USBHdlr] USBHdlr_AmageCommunication Get_Ituner_File");
                *length = MAX_AMGAGE_DATA_SIZE;
                AmbaPrint("ITUNER_FILE length");
                break;
            case ColorCorrection:
                AmbaPrint("[AmbaTune_USBHdlr] USBHdlr_AmageCommunication Get_ColorCorrection");
                *length = sizeof(AmageCC);
                break;
            case ConfigAAAControl:
                AmbaPrint("[AmbaTune_USBHdlr] USBHdlr_AmageCommunication ConfigAAAControl");
                *length = sizeof(AAA_CTRL);
                break;
            case ExposureControl:
                AmbaPrint("[AmbaTune_USBHdlr] USBHdlr_AmageCommunication ExposureControl");
                *length = sizeof(EC_INFO);
                break;
            case SAVE_JPG:
                *length = JPG_INFO.size;
                break;

            default:
                AmbaPrint("default");
                break;
        }
    }

    AmbaPrint("length : %d", *length);
    return(OK);
}

// ituner save

UINT32 USBHdlr_AmageDataGet(UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength)
{
    //HDLR_DEBF("%s() %d Offset: %d, LengthRequest: %d", __func__, __LINE__,ObjectOffset,ObjectLengthRequested);
    if (ObjectOffset == 0){
        _USBHdlr_Get_Item_Process();
    }
    if(Item == SAVE_JPG){
        UINT8* tmpAddr = (UINT8*) (JPG_INFO.addr);

        tmpAddr = tmpAddr + ObjectOffset;
        memcpy(ObjectBuffer, tmpAddr, ObjectLengthRequested);
        *ObjectActualLength = ObjectLengthRequested;

        if((ObjectOffset+ObjectLengthRequested) >= JPG_INFO.size){
            AmbaPrintColor(YELLOW, "%s() %d, SaveJpegEventFlag give", __func__, __LINE__);

            if(AmbaKAL_EventFlagGive(&SaveJpegEventFlag, FLAG_PREPARSE_START)== OK){
                AmbaPrintColor(YELLOW,"Get Jpg EventFlag Give Success");
            }
            else{
                AmbaPrintColor(YELLOW,"Get Jpg EventFlag Give Fail");
            }
        }
    }
    else{
        AmbaPrintColor(YELLOW,"Data get %s,%d", __func__, __LINE__);
        memcpy(ObjectBuffer, &Data[ObjectOffset], ObjectLengthRequested);
        *ObjectActualLength = ObjectLengthRequested;
    }
    return(OK);
}

UINT32 USBHdlr_AmageDataSend(UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength)
{
    //HDLR_DEBF("%s() %d ", __func__, __LINE__);

    if (Item == ITUNER_FILE ){
        UINT8* stillDataBuff = NULL;
        AmbaPrint("ITUNER_FILE Send");
        stillDataBuff = (UINT8*) Still_Data_Buff.pMemAlignedBase;
        memcpy(&stillDataBuff[ObjectOffset], ObjectBuffer, ObjectLength);
        //AmbaPrint("%s",stillDataBuff);
    }
    else if (Item == CA){
        AmbaPrint("CHROMA ABERRATION Send");
        memcpy(&calibTable.CaGrid.addr[ObjectOffset],ObjectBuffer , ObjectLength);
        calibTable.CaGrid.isSet = 1;
        AmbaPrintColor(YELLOW, "CA table addr : 0x%.8x", calibTable.CaGrid.addr);
        //AmbaItuner_Set_ChromaAberrationInfo(( ITUNER_CHROMA_ABERRATION_s*)&calibTable.CaGrid.addr);
    }
    else if (Item == WARP){
        AmbaPrint("WARP Send");
        memcpy(&calibTable.WarpGrid.addr[ObjectOffset],ObjectBuffer, ObjectLength);
        calibTable.WarpGrid.isSet = 1;
        AmbaPrintColor(YELLOW, "WAP table addr : 0x%.8x", calibTable.WarpGrid.addr);
        //AmbaItuner_Set_WarpCompensation((ITUNER_WARP_s*) &calibTable.WarpGrid.addr);
	}
    else if (Item == VIGNETTE){
        AmbaPrint("VIGNETTE Send");
        memcpy(&calibTable.VignetteTable.addr[ObjectOffset],ObjectBuffer, ObjectLength);
        calibTable.VignetteTable.isSet = 1;
        AmbaPrintColor(YELLOW, "Vignette table addr : 0x%.8x", calibTable.VignetteTable.addr);
        //AmbaItuner_Set_VignetteCompensation( (ITUNER_VIGNETTE_s*) &calibTable.VignetteTable.addr);
    }
    else if (Item == FPN){
        AmbaPrint("FPN Send");
        memcpy(&calibTable.FPNMap.addr[ObjectOffset],ObjectBuffer, ObjectLength);
        calibTable.FPNMap.isSet = 1;
        AmbaPrintColor(YELLOW, "FPN map addr : 0x%.8x", calibTable.FPNMap.addr);
        //AmbaItuner_Set_StaticBadPixelCorrection((ITUNER_FPN_s *) &calibTable.FPNMap.addr);
    }
    else if (Item ==Adj_Still_Liso||Item==Adj_Photo||Item==Adj_VideoLiso||Item==Adj_Still_Hiso){
        int ReturnValue=-1;
        UINT8* adj_videoliso = NULL;
        if (ObjectOffset==0){
            AmbaKAL_MemFree(&IQTable_Buff);	
            ReturnValue=AmbaKAL_MemAllocate(USB_Handler.MemPool, &IQTable_Buff, sizeof(ADJ_STILL_HISO_PARAM_s), 1);
            //AmbaPrintColor(YELLOW, "AmbaKAL_MemAllocate addr : 0x%.8x", &IQTable_Buff.pMemAlignedBase);
            if (ReturnValue != 0) {
                HDLR_DEBF("%s() %d, MemAllocate failed", __func__, __LINE__);
                return ReturnValue;
            }
        }
        adj_videoliso=(UINT8*) IQTable_Buff.pMemAlignedBase;	
        memcpy(&adj_videoliso[ObjectOffset],ObjectBuffer , ObjectLength);		
    }
    else if (Item == RAWDATA ){

                UINT8* RawAddr=NULL;
                ITUNER_SYSTEM_s System;
                static int RawBufferSize;				
                USBHdlr_Get_SystemInfo(&System);

                if(ObjectOffset==0){
                        USB_Handler.CBFuncList.StillTuningRawBuffer(&RawBuf);
                        AmbaPrintColor(5,"StillTuningRawBuffer 0x%.8x",RawBuf.pBaseAddr);
                        RawBufferSize=System.RawPitch*System.RawHeight;
                        AmbaPrintColor(5,"RawBufferSize:%d",RawBufferSize);
                }
                RawAddr =  RawBuf.pBaseAddr;
                if (RawAddr==NULL) {
                        HDLR_DEBF("%s() %d, StillTuningRawBuffer 0x%.8x", __func__, __LINE__,RawBuf.pBaseAddr);
                        return NG;
                }
                
                if ((ObjectOffset+ObjectLength)<= RawBufferSize){
                        memcpy(&RawAddr[ObjectOffset], ObjectBuffer, ObjectLength);
                } 
                else if(ObjectOffset>RawBufferSize){
                }
                else {
                        memcpy(&RawAddr[ObjectOffset], ObjectBuffer, (RawBufferSize-ObjectOffset));
                        AmbaPrintColor(YELLOW,"AmageSendDataSize>RawBufferSize");
                        //AmbaPrintColor(5,"ObjectOffset:%d, ObjectLength:%d, Remainsize:%d",ObjectOffset,ObjectLength,(RawBufferSize-ObjectOffset));
                }
    }

    else{
        memcpy((char *) (Data+ObjectOffset), (char *) ObjectBuffer, ObjectLength);
    }

    if ((ObjectOffset+ObjectLength) >= AmageSendDataSize){
        if (Item == RAWDATA ){
            AmbaCache_Clean(RawBuf.pBaseAddr, RawBuf.Pitch*RawBuf.Window.Height);
        }
        return _USBHdlr_Set_Item_Process();
    }
    return(OK);
}

