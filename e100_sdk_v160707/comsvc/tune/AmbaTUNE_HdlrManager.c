/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: ApplibTune_HdlrManager.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella TUNE Handler
\*-------------------------------------------------------------------------------------------------------------------*/


#include "AmbaTUNE_TextHdlr.h" // Note: Tune_Func_s Need
#define SDK_PROJECT

#include "AmbaKAL.h"// Note: AmbaKAL_MutexCreate Need
#include "AmbaPrintk.h" // Note: AmbaPrint Need
#include "AmbaTUNE_HdlrManager.h"

#ifdef SDK_PROJECT
#include "AmbaTUNE_USBHdlr.h"// Note: Tune_Func_s Need
//extern AMBA_KAL_BYTE_POOL_t G_NC_MMPL;
#endif


#define MNG_DEBF(fmt, arg...) //AmbaPrint("[TUNE MNG][DEBUG]"fmt, ##arg)
#define MNG_WARF(fmt, arg...) AmbaPrint("[TUNE MNG][WARNING]"fmt, ##arg)
#define MNG_ERRF(fmt, arg...) AmbaPrint("[TUNE MNG][ERROR]"fmt, ##arg)

#define OSAL_MUTEX_CREATE() do {AmbaKAL_MutexCreate(gTune_Hdlr.Parser_Mutex);} while (0)
#define OSAL_MUTEX_LOCK() do {AmbaKAL_MutexTake(gTune_Hdlr.Parser_Mutex, AMBA_KAL_WAIT_FOREVER);} while(0)
#define OSAL_MUTEX_UNLOCK() do {AmbaKAL_MutexGive(gTune_Hdlr.Parser_Mutex);} while(0)
#ifndef NULL
#define NULL (void*)0
#endif

typedef struct {
    int (*Init)(Ituner_Initial_Config_t *InitialConfig);
    int (*Save_IDSP)(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param);
    int (*Load_IDSP)(TUNE_Load_Param_s *Load_Param);
    int (*Save_Data)(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param);
    int (*Load_Data)(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param);
    int (*Execute_IDSP)(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo);
    int (*Get_SystemInfo)(ITUNER_SYSTEM_s *System);
    int (*Set_SystemInfo)(ITUNER_SYSTEM_s *System);
    int (*Get_AeInfo)(ITUNER_AE_INFO_s *AeInfo);
    int (*Set_AeInfo)(ITUNER_AE_INFO_s *AeInfo);
    int (*Get_WbSimInfo)(ITUNER_WB_SIM_INFO_s *WbSimInfo);
    int (*Set_WbSimInfo)(ITUNER_WB_SIM_INFO_s *WbSimInfo);
    int (*Get_ItunerInfo)(ITUNER_INFO_s *ItunerInfo);
	int (*Set_InitialConfig)(TUNE_Initial_Config_s  *TuneInitialConfig);
} Tune_Func_s;

#if 0//SDK_PROJECT
#else
static inline int _AmbaTUNE_Warpper_USB_Load_IDSP(TUNE_Load_Param_s *Load_Param);
static inline int _AmbaTUNE_Warpper_USB_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param);
#endif
static inline int _AmbaTUNE_Warpper_Text_Load_IDSP(TUNE_Load_Param_s *Load_Param);
static inline int _AmbaTUNE_Warpper_Text_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param);


#if 0//SDK_PROJECT
static int _Fake_USBHdlr_Init(Ituner_Initial_Config_t *InitialConfig){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *Mode, TUNE_Save_Param_s *Save_Param){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Load_IDSP(TUNE_Load_Param_s *Load_Param){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *Mode, AMBA_ITUNER_PROC_INFO_s *ProcInfo){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Get_SystemInfo(ITUNER_SYSTEM_s *System){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Set_SystemInfo(ITUNER_SYSTEM_s *System){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _Fake_USBHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _Fake_USBHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
static int _Fake_USBHdlr_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}

static int _Fake_USBHdlr_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo){
    MNG_ERRF("%s() %d, Not yet Implement", __func__, __LINE__);
    return 0;
}
#endif

static const Tune_Func_s gUSBHdlr_Func = {
#if 0 //SDK_PROJECT

    .Init = _Fake_USBHdlr_Init,
    .Save_IDSP = _Fake_USBHdlr_Save_IDSP,
    .Load_IDSP = _Fake_USBHdlr_Load_IDSP,
    .Save_Data = _Fake_USBHdlr_Save_Data,
    .Load_Data = _Fake_USBHdlr_Load_Data,
    .Execute_IDSP = _Fake_USBHdlr_Execute_IDSP,
    .Get_SystemInfo = _Fake_USBHdlr_Get_SystemInfo,
    .Set_SystemInfo = _Fake_USBHdlr_Set_SystemInfo,
    .Get_AeInfo = _Fake_USBHdlr_Get_AeInfo,
    .Set_AeInfo = _Fake_USBHdlr_Set_AeInfo,
    .Get_WbSimInfo = _Fake_USBHdlr_Get_WbSimInfo,
    .Set_WbSimInfo = _Fake_USBHdlr_Set_WbSimInfo,
    .Get_ItunerInfo = _Fake_USBHdlr_Get_ItunerInfo,
    .Set_MemPool = _Fake_USBHdlr_Set_MemPool,
#else
    .Init = USBHdlr_Init,
    .Save_IDSP = _AmbaTUNE_Warpper_USB_Save_IDSP,
    .Load_Data = USBHdlr_Load_Data,
    .Save_Data = USBHdlr_Save_Data,
    .Load_IDSP = _AmbaTUNE_Warpper_USB_Load_IDSP,
    .Execute_IDSP = USBHdlr_Execute_IDSP,
    .Get_SystemInfo = USBHdlr_Get_SystemInfo,
    .Set_SystemInfo = USBHdlr_Set_SystemInfo,
    .Get_AeInfo = USBHdlr_Get_AeInfo,
    .Set_AeInfo = USBHdlr_Set_AeInfo,
    .Get_WbSimInfo = USBHdlr_Get_WbSimInfo,
    .Set_WbSimInfo = USBHdlr_Set_WbSimInfo,
    .Get_ItunerInfo = USBHdlr_Get_ItunerInfo,
    .Set_InitialConfig = USBHdlr_Set_USBConfig,
#endif
};


static const Tune_Func_s gTextHdlr_Func = {
    .Init = TextHdlr_Init,
    .Save_IDSP = _AmbaTUNE_Warpper_Text_Save_IDSP,
    .Load_Data = TextHdlr_Load_Data,
    .Save_Data = TextHdlr_Save_Data,
    .Load_IDSP = _AmbaTUNE_Warpper_Text_Load_IDSP,
    .Execute_IDSP = TextHdlr_Execute_IDSP,
    .Get_SystemInfo = TextHdlr_Get_SystemInfo,
    .Set_SystemInfo = TextHdlr_Set_SystemInfo,
    .Get_AeInfo = TextHdlr_Get_AeInfo,
    .Set_AeInfo = TextHdlr_Set_AeInfo,
    .Get_WbSimInfo = TextHdlr_Get_WbSimInfo,
    .Set_WbSimInfo = TextHdlr_Set_WbSimInfo,
    .Get_ItunerInfo = TextHdlr_Get_ItunerInfo,
    .Set_InitialConfig = NULL,
};

typedef enum {
    TUNE_VALID = 0,
    TUNE_INVALID,
} TuneHdlr_Status_e;
typedef struct {
    TuneHdlr_Mode_e Active_Parser_Mode;
    TuneHdlr_Status_e Parser_Status;
    AMBA_KAL_MUTEX_t *Parser_Mutex;
} TuneHdlr_s;
static const Tune_Func_s *gTune_Func[MAX_TUNE] = {
    &gTextHdlr_Func,
    &gUSBHdlr_Func,
};
static AMBA_KAL_MUTEX_t gTune_Mutex;
static TuneHdlr_s gTune_Hdlr = {
    .Active_Parser_Mode = TEXT_TUNE,
    .Parser_Status = TUNE_INVALID,
    .Parser_Mutex = NULL,
};

AMBA_MEM_CTRL_s AmbaTuneBuffer = {
    .pMemBase = NULL,
    .pMemAlignedBase = NULL,
};

AMBA_MEM_CTRL_s AmbaDualChanBuffer = {
    .pMemBase = NULL,
    .pMemAlignedBase = NULL,
};

//#ifdef SDK_PROJECT
#if 0
#else
static inline int _AmbaTUNE_Warpper_USB_Load_IDSP(TUNE_Load_Param_s *Load_Param)
{
    return 0;//USBHdlr_Load_IDSP((TUNE_USB_Load_Param_s*) &Load_Param->USB.Buffer);
}

static inline int _AmbaTUNE_Warpper_USB_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param)
{
    return 0;//USBHdlr_Save_IDSP(pMode, (TUNE_USB_Save_Param_s*) &Save_Param->USB.Buffer);
}
#endif
static inline int _AmbaTUNE_Warpper_Text_Load_IDSP(TUNE_Load_Param_s *Load_Param)
{
    return TextHdlr_Load_IDSP(Load_Param->Text.FilePath);
}

static inline int _AmbaTUNE_Warpper_Text_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param)
{
    return TextHdlr_Save_IDSP(pMode, Save_Param->Text.Filepath);
}


static void _AmbaTUNE_Check_Mutex_Created(void)
{
    if (gTune_Hdlr.Parser_Mutex == NULL) {
        MNG_DEBF("%s() %d, Create Mutex", __func__, __LINE__);
        gTune_Hdlr.Parser_Mutex = &gTune_Mutex;
        OSAL_MUTEX_CREATE();
    }
}
// FIXME:

int AmbaTUNE_Init(TUNE_Initial_Config_s *pTuneInitialConfig)
{
    int Ret = 0;
    Ituner_Initial_Config_t InitialConfig;
    AMBA_KAL_BYTE_POOL_t *pMemPool = NULL;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    gTune_Hdlr.Parser_Status = TUNE_VALID;

    if((AmbaTuneBuffer.pMemBase == NULL)||(AmbaTuneBuffer.pMemAlignedBase == NULL)){
        // TODO: Make Sure Pool ID is valid
        pMemPool = (gTune_Hdlr.Active_Parser_Mode == TEXT_TUNE) ? pTuneInitialConfig->Text.pBytePool : pTuneInitialConfig->Usb.pBytePool;
    }
    InitialConfig.pMemPool = pMemPool;

    if (gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_InitialConfig != NULL) {
	    gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_InitialConfig(pTuneInitialConfig);
    }
    Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Init(&InitialConfig);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_UnInit(void)
{
    int Ret = 0;
    OSAL_MUTEX_LOCK();
    gTune_Hdlr.Parser_Status = TUNE_INVALID;
    if((AmbaTuneBuffer.pMemBase != NULL)||(AmbaTuneBuffer.pMemAlignedBase != NULL)){
        Ret = AmbaKAL_MemFree(&AmbaTuneBuffer);
        AmbaTuneBuffer.pMemBase = NULL;
        AmbaTuneBuffer.pMemAlignedBase = NULL;
    }
    if((AmbaDualChanBuffer.pMemBase != NULL)||(AmbaDualChanBuffer.pMemAlignedBase != NULL)){
        Ret = AmbaKAL_MemFree(&AmbaDualChanBuffer);
        AmbaDualChanBuffer.pMemBase = NULL;
        AmbaDualChanBuffer.pMemAlignedBase = NULL;
    }
    
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

void AmbaTUNE_Change_Parser_Mode(TuneHdlr_Mode_e Parser_Mode)
{
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Active_Parser_Mode == Parser_Mode) {
            MNG_DEBF("%s() %d, Active Parser = %d Do not need to change parser mode", __func__, __LINE__, gTune_Hdlr.Active_Parser_Mode);
            break;
        }
        gTune_Hdlr.Parser_Status = TUNE_INVALID;
    } while (0);
    gTune_Hdlr.Active_Parser_Mode = Parser_Mode;
    OSAL_MUTEX_UNLOCK();

}
int AmbaTUNE_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, TUNE_Save_Param_s *Save_Param)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Save_IDSP(pMode, Save_Param);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Load_IDSP(TUNE_Load_Param_s *Load_Param)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Load_IDSP(Load_Param);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Save_Data(Ext_File_Type, Ext_File_Param);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s *Ext_File_Param)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Load_Data(Ext_File_Type, Ext_File_Param);
    } while(0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Execute_IDSP(pMode, ProcInfo);
    } while(0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Get_SystemInfo(ITUNER_SYSTEM_s *System)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Get_SystemInfo(System);
    } while(0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Set_SystemInfo(ITUNER_SYSTEM_s *System)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_SystemInfo(System);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Get_AeInfo(AeInfo);
    } while(0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_AeInfo(AeInfo);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}


int AmbaTUNE_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Get_WbSimInfo(WbSimInfo);
    } while(0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}

int AmbaTUNE_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Set_WbSimInfo(WbSimInfo);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}


int AmbaTUNE_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo)
{
    int Ret = 0;
    _AmbaTUNE_Check_Mutex_Created();
    OSAL_MUTEX_LOCK();
    do {
        if (gTune_Hdlr.Parser_Status == TUNE_INVALID) {
            MNG_WARF("%s() %d, Parser_Status Invalid", __func__, __LINE__);
            Ret = -1;
            break;
        }
        Ret = gTune_Func[gTune_Hdlr.Active_Parser_Mode]->Get_ItunerInfo(ItunerInfo);
    } while (0);
    OSAL_MUTEX_UNLOCK();
    return Ret;
}
