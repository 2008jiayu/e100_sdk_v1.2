#include "AmbaSSP.h" // Note: AmbaShell.h need
#include "AmbaTUNE_HdlrManager.h"
#include "AmbaFS.h"
#include "AmbaCache.h"
#include "Applib_IK_Utility.h"

#define Applib_IK_DEBF(fmt, arg...) AmbaPrint("[Applib][IK][DEBUG]%s() %d, "fmt, __func__, __LINE__, ##arg)
#define Applib_IK_WARF(fmt, arg...) AmbaPrint("[Applib][IK][WARNING]%s() %d, "fmt, __func__, __LINE__, ##arg)
#define Applib_IK_ERRF(fmt, arg...) do {AmbaPrint("[Applib][IK][ERROR]%s() %d, "fmt, __func__, __LINE__, ##arg); K_ASSERT(0);} while(0)
#define Applib_IK_SAVE_NG_ISOCFG_2_SD_CAR 1

static Applib_IK_ER_CODE_e Applib_IK_Init(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool);
static Applib_IK_ER_CODE_e Applib_IK_Uninit(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool);
static Applib_IK_ER_CODE_e Applib_IK_Print_Log(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool);
static Applib_IK_ER_CODE_e Applib_IK_Still_Save_IsoCfg(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool);
static Applib_IK_ER_CODE_e Applib_IK_Still_Cmp_IsoCfg(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool);
//TBD
typedef struct
{
    Applib_IK_CMD_e cmd;
    Applib_IK_ER_CODE_e (*funPtr)(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *MMPL);
} Applib_IK_Cmd_s;

typedef struct
{
    UINT32 dataNum;
    Applib_IK_DATA_CONTAINER_s *data;
    AMBA_MEM_CTRL_s ApplibIKbuffer;
    AMBA_MEM_CTRL_s TuneBuffer;
} Applib_IK_Data;

typedef UINT32 Applib_DATA_HDLR;

static const Applib_IK_Cmd_s Applib_IK_Cmd[Applib_IK_CMD_MAX] = {
  { Applib_IK_CMD_INIT, Applib_IK_Init },
  { Applib_IK_CMD_UNINIT, Applib_IK_Uninit },
  { Applib_IK_CMD_PRINT_LOG, Applib_IK_Print_Log },
  { Applib_IK_CMD_STILL_SAVE_ISOCONFIG, Applib_IK_Still_Save_IsoCfg },
  { Applib_IK_CMD_STILL_CMP_ISOCONFIG, Applib_IK_Still_Cmp_IsoCfg },
};

// GOLOBAL
static Applib_IK_Data G_Applib_IK_Data = { 0 };
static Applib_DATA_HDLR Active_DataHdlr = 0;
static int dataCounter = 0;

static int Applib_IK_Data_Create(Applib_IK_MODE_e mode, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    memset(&G_Applib_IK_Data.data[dataCounter], 0x0, sizeof(Applib_IK_DATA_CONTAINER_s));
    G_Applib_IK_Data.data[dataCounter].Mode = mode;
    G_Applib_IK_Data.data[dataCounter].MemPool = memoryPool;
    switch(mode) {
    case Applib_IK_MODE_STILL:
        G_Applib_IK_Data.data[dataCounter].ResultInfo.Still.Result = Applib_IK_RESULT_UNKNOW;
        break;
    case Applib_IK_MODE_VIDEO:
    default:
        Applib_IK_ERRF("Not Support mode: %d", mode);
        break;
    }

    dataCounter = dataCounter + 1;
    return (dataCounter - 1);
}
static int Applib_IK_Data_Get(Applib_DATA_HDLR utDatahdlr, Applib_IK_DATA_CONTAINER_s *data)
{

    if (((UINT32) utDatahdlr) > G_Applib_IK_Data.dataNum) {
        Applib_IK_ERRF("Unexpected [Applib_DATA_HDLR = %d] > [maxIdx = %d]", utDatahdlr, G_Applib_IK_Data.dataNum);
    }
    if (data != NULL) {
        memcpy(data, &G_Applib_IK_Data.data[utDatahdlr], sizeof(Applib_IK_DATA_CONTAINER_s));
    } else {
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    return Applib_IK_OK;
}
static int Applib_IK_Data_Set(Applib_DATA_HDLR utDatahdlr, Applib_IK_DATA_CONTAINER_s *data)
{
    if (((UINT32) utDatahdlr) > G_Applib_IK_Data.dataNum) {
        Applib_IK_ERRF("Unexpected [Applib_DATA_HDLR = %d] > [maxIdx = %d]", utDatahdlr, G_Applib_IK_Data.dataNum);
    }
    if (data != NULL) {
        memcpy( (&(G_Applib_IK_Data.data[utDatahdlr])), data, sizeof(Applib_IK_DATA_CONTAINER_s));
    } else {
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    return Applib_IK_OK;
}

static int _posix_mkdir(const char *folder_name)
{
    int Rval = 0;
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        K_ASSERT(0);
        /* Note: A12 not support unicode
         static WCHAR w_folder_name[64]; // Note: Not allow multi instance, use static to avoid stack over flow
         w_asc2uni(w_folder_name, folder_name, sizeof(w_folder_name) / sizeof(w_folder_name[0]));
         Rval = AmbaFS_Mkdir((const char*)w_folder_name);
         */
    } else {
        Rval = AmbaFS_Mkdir(folder_name);
    }
    return Rval;
}

extern void img_dsp_core_probe_hook(UINT32 Contact_ID, UINT32 ProbeFunc, int *Result);
static Applib_IK_ER_CODE_e _Save2Bin(UINT32 IsoCfgAddr, UINT32 IsoCfgSize, char* filename)
{
    AMBA_FS_FILE *Fd;
    char* F_Mode = "wb";
    _posix_mkdir("c:\\bins");

    Fd = AmbaFS_fopen(filename, F_Mode);
    if (Fd == NULL) {
        Applib_IK_ERRF("Open File [%s] Error!", filename);
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    AmbaFS_fwrite((void *) IsoCfgAddr, 0x1, (UINT64) IsoCfgSize, Fd);
    AmbaFS_fclose(Fd);
    return Applib_IK_OK;
}
static Applib_IK_ER_CODE_e _ReadBin(UINT32 IsoCfgAddr, UINT32 IsoCfgSize, char* filename)
{
    AMBA_FS_FILE *Fd;
    char* F_Mode = "rb";

    Fd = AmbaFS_fopen(filename, F_Mode);
    if (Fd == NULL) {
        Applib_IK_ERRF("Open File [%s] Error!", filename);
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    AmbaFS_fread((void *) IsoCfgAddr, 0x1, (UINT64) IsoCfgSize, Fd);

    AmbaFS_fclose(Fd);
    return Applib_IK_OK;
}

extern int img_dsp_core_probe_set_liso_cfg_skip_info(UINT32 Addr, UINT32 Size, UINT32 shpSelectAddr);
extern int img_dsp_core_probe_set_hiso_cfg_skip_info(UINT32 Addr, UINT32 Size);
static int _IsoCfg_Dump(UINT32 IsoCfgAddr, UINT32 IsoCfgSize, UINT32 ModeAddr, UINT32 shpSelectAddr, UINT32 Reserved5)
{
    int Rval=0;
    UINT8 *pIsoCfgData;
    AMBA_DSP_IMG_MODE_CFG_s *pMode;
    Applib_IK_DATA_CONTAINER_s data;
    char *dumpFileName;
    AMBA_MEM_CTRL_s IsoCfgBuffer;
    Applib_IK_Data_Get(Active_DataHdlr, &data);
    Rval = AmbaKAL_MemAllocate(data.MemPool, &IsoCfgBuffer, IsoCfgSize, 32);
    if (Rval != 0) {
        Applib_IK_ERRF("Call AmbaKAL_MemAllocate() Fail");
        return Applib_IK_ERROR_GENERAL_ERROR;
    }

    pIsoCfgData = (UINT8*) IsoCfgBuffer.pMemAlignedBase;
    memcpy(pIsoCfgData, (void*) IsoCfgAddr, IsoCfgSize);
    dumpFileName = &data.ProcessInfo.Still.BinFileName[0];
    pMode = (AMBA_DSP_IMG_MODE_CFG_s*) ModeAddr;

    if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_LISO) {
        img_dsp_core_probe_set_liso_cfg_skip_info((UINT32) pIsoCfgData, IsoCfgSize, shpSelectAddr);
    } else if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
        img_dsp_core_probe_set_hiso_cfg_skip_info((UINT32) pIsoCfgData, IsoCfgSize);
    }
    Rval = _Save2Bin((UINT32) pIsoCfgData, IsoCfgSize, dumpFileName);
    Applib_IK_DEBF("Save IsoCfg to path: %s", dumpFileName);

    Rval = AmbaKAL_MemFree(&IsoCfgBuffer);
    if (Rval != 0) {
        Applib_IK_ERRF("Call AmbaKAL_MemFree() Fail");
        return Applib_IK_ERROR_GENERAL_ERROR;
    }

    return Rval;
}
void AmbaIKUnitTest_hook_IsoCfg_dump(void)
{
    img_dsp_core_probe_hook(0x0002, (UINT32)_IsoCfg_Dump, NULL);
}

static int _IsoCfg_Cmp(UINT32 IsoCfgAddr, UINT32 IsoCfgSize, UINT32 ModeAddr, UINT32 shpSelectAddr, UINT32 Reserved5)
{
    int Rval;
    UINT8 *pItunerIsoCfgData;
    UINT8 *pCmpTargetIsoCfgData;
    char NG_FileName[128];
    AMBA_DSP_IMG_MODE_CFG_s *pMode;
    Applib_IK_DATA_CONTAINER_s data;
    AMBA_MEM_CTRL_s IsoCfgBuffer;
    Applib_IK_CMP_RESULT_e result;
    Applib_IK_Data_Get(Active_DataHdlr, &data);

    Rval = AmbaKAL_MemAllocate(data.MemPool, &IsoCfgBuffer, IsoCfgSize * 2, 32);
    if (Rval != 0) {
        Applib_IK_ERRF("Call AmbaKAL_MemAllocate() Fail");
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    pItunerIsoCfgData = (UINT8*) IsoCfgBuffer.pMemAlignedBase;
    pCmpTargetIsoCfgData = pItunerIsoCfgData + IsoCfgSize;
    //1. get IsoCfg
    memcpy(pItunerIsoCfgData, (void*) IsoCfgAddr, IsoCfgSize);
    pMode = (AMBA_DSP_IMG_MODE_CFG_s*) ModeAddr;

    if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_LISO) {
        img_dsp_core_probe_set_liso_cfg_skip_info((UINT32) pItunerIsoCfgData, IsoCfgSize, shpSelectAddr);
    } else if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
        img_dsp_core_probe_set_hiso_cfg_skip_info((UINT32) pItunerIsoCfgData, IsoCfgSize);
    }
    //2. read golden bin file
    Rval = _ReadBin((UINT32) pCmpTargetIsoCfgData, (UINT32) IsoCfgSize, data.ProcessInfo.Still.BinFileName);

    //3. compare
    if (memcmp((void*) pCmpTargetIsoCfgData, (void*) pItunerIsoCfgData, (size_t) IsoCfgSize) == 0) {
        AmbaPrint("File: %s compare to %s => PASS ", data.ProcessInfo.Still.ItunerFileName, data.ProcessInfo.Still.BinFileName);
        result = Applib_IK_RESULT_PASS;
    } else {
        AmbaPrint("File: %s compare to %s => NG ", data.ProcessInfo.Still.ItunerFileName, data.ProcessInfo.Still.BinFileName);
        result = Applib_IK_RESULT_NG;
    }

    //4. Save NG Ituner IsoCfg
    if ( Applib_IK_SAVE_NG_ISOCFG_2_SD_CAR && (result == Applib_IK_RESULT_NG)) {
        sprintf(&NG_FileName[0], "%s_NG.bin", &data.ProcessInfo.Still.BinFileName[0]);
        _Save2Bin((UINT32) pItunerIsoCfgData, IsoCfgSize, NG_FileName);
        AmbaPrint("Save NG IsoCfg to Path:%s", NG_FileName);
    }

    Rval = AmbaKAL_MemFree(&IsoCfgBuffer);
    if (Rval != 0) {
        Applib_IK_ERRF("Call AmbaKAL_MemFree() Fail");
        return Applib_IK_ERROR_GENERAL_ERROR;
    }

    return (int)result;
}

void Applib_IK_hook_IsoCfg_cmp(Applib_IK_CMP_RESULT_e *result)
{
    img_dsp_core_probe_hook(0x0002, (UINT32) _IsoCfg_Cmp, (int*)result);
}
int Applib_IK_Test(Applib_IK_CMD_e cmd, Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    int i = 0, Rval;
    for (i = 0; i < sizeof(Applib_IK_Cmd) / sizeof(Applib_IK_Cmd[0]); i++) {
        if (cmd == Applib_IK_Cmd[i].cmd) {
            Rval = Applib_IK_Cmd[i].funPtr(input, memoryPool);
            return Rval;
        }
    }
    return Applib_IK_ERROR_GENERAL_ERROR;
}

static Applib_IK_ER_CODE_e Applib_IK_Init(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    int Rval, i;
    dataCounter = 0;
    G_Applib_IK_Data.dataNum = input->InitData.CompareNum;
    if (G_Applib_IK_Data.ApplibIKbuffer.pMemAlignedBase != NULL) {
        Applib_IK_WARF("Already Init, please uninit!");
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    Rval = AmbaKAL_MemAllocate(memoryPool, &G_Applib_IK_Data.ApplibIKbuffer, sizeof(Applib_IK_DATA_CONTAINER_s) * (G_Applib_IK_Data.dataNum), 32);
    if (Rval != 0) {
        Applib_IK_ERRF("Call AmbaKAL_MemAllocate() Fail");
        return Applib_IK_ERROR_GENERAL_ERROR;
    }
    G_Applib_IK_Data.data = (Applib_IK_DATA_CONTAINER_s*) G_Applib_IK_Data.ApplibIKbuffer.pMemAlignedBase;
    for (i = 0; i < input->InitData.CompareNum; i++) {
//        G_Applib_IK_Data.data[i].ResultInfo.Still.Result = Applib_IK_RESULT_UNKNOW;
        G_Applib_IK_Data.data[i].Mode = Applib_IK_MODE_UNKNOW;
    }
    Applib_IK_DEBF("IK Unit Test init done!");
    return Applib_IK_OK;
}
static Applib_IK_ER_CODE_e Applib_IK_Uninit(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    if (G_Applib_IK_Data.ApplibIKbuffer.pMemAlignedBase != NULL) {
        AmbaKAL_MemFree(&(G_Applib_IK_Data.ApplibIKbuffer));
        G_Applib_IK_Data.ApplibIKbuffer.pMemAlignedBase = NULL;
    }
    Applib_IK_DEBF("IK Unit Test uninit done!");
    return Applib_IK_OK;
}

static Applib_IK_ER_CODE_e Applib_IK_Print_Log(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    int i;
    Applib_DATA_HDLR utDataHdlr;
    Applib_IK_DATA_CONTAINER_s data;
    for (i = 0; i < input->InitData.CompareNum; i++) {
        utDataHdlr = i;
        Applib_IK_Data_Get(utDataHdlr, &data);
        switch (data.Mode) {
        case Applib_IK_MODE_STILL:
            if (data.ResultInfo.Still.Result == Applib_IK_RESULT_PASS) {
                AmbaPrint("[PASS]%50s:%50s", data.ProcessInfo.Still.ItunerFileName, data.ProcessInfo.Still.BinFileName);
            } else if (data.ResultInfo.Still.Result == Applib_IK_RESULT_NG) {
                AmbaPrint("[ NG ]%50s:%50s", data.ProcessInfo.Still.ItunerFileName, data.ProcessInfo.Still.BinFileName);
            }
            break;
        case Applib_IK_MODE_VIDEO:
            //TBD
            AmbaPrint("Video Print Log haven't implement yet!");
            break;
        default:
            break;
        }
    }
    return Applib_IK_OK;
}

static Applib_IK_ER_CODE_e Applib_IK_Still_Save_IsoCfg(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    int Rval = Applib_IK_OK;
    TUNE_Initial_Config_s InitialConfig;
    TUNE_Load_Param_s LoadParam;
    ITUNER_INFO_s ItunerInfo;
    AMBA_ITUNER_PROC_INFO_s ProcInfo;
    AMBA_DSP_IMG_CTX_INFO_s DestCtx;
    AMBA_DSP_IMG_CTX_INFO_s SrcCtx;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
    Applib_IK_DATA_CONTAINER_s data;
    memset(&SrcCtx, 0x0, sizeof(AMBA_DSP_IMG_CTX_INFO_s));
    memset(&DestCtx, 0x0, sizeof(AMBA_DSP_IMG_CTX_INFO_s));
    memset(&CfgInfo, 0x0, sizeof(CfgInfo));

    Active_DataHdlr = Applib_IK_Data_Create(Applib_IK_MODE_STILL, memoryPool);
    Applib_IK_Data_Get(Active_DataHdlr, &data);
    strncpy(&data.ProcessInfo.Still.ItunerFileName[0], input->SaveIsoCfgData.pInItunerFileName, sizeof(data.ProcessInfo.Still.ItunerFileName) -1);
    strncpy(&data.ProcessInfo.Still.BinFileName[0], input->SaveIsoCfgData.pOutBinFileName, sizeof(data.ProcessInfo.Still.BinFileName) - 1);
    Applib_IK_Data_Set(Active_DataHdlr, &data);

    AmbaIKUnitTest_hook_IsoCfg_dump();

    do {
        AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
        memset(&InitialConfig, 0x0, sizeof(TUNE_Initial_Config_s));
        InitialConfig.Text.pBytePool = memoryPool;
        if (0 != AmbaTUNE_Init(&InitialConfig)) {
            Applib_IK_WARF("Call AmbaTUNE_Init() Fail");
            break;
        }
        memset(&LoadParam, 0x0, sizeof(TUNE_Load_Param_s));
        LoadParam.Text.FilePath = input->SaveIsoCfgData.pInItunerFileName;
        if (0 != AmbaTUNE_Load_IDSP(&LoadParam)) {
            Applib_IK_WARF("Call TextHdlr_Load_IDSP(%s) Fail", input->SaveIsoCfgData.pInItunerFileName);
            break;
        }
        if (0 != AmbaTUNE_Get_ItunerInfo(&ItunerInfo)) {
            Applib_IK_WARF("Call TextHdlr_Get_ItunerInfo() Fail");
            break;
        }
        {
            DestCtx.Pipe = ItunerInfo.TuningAlgoMode.Pipe;
            DestCtx.CtxId = 0;
            Rval = AmbaDSP_ImgInitCtx(0, 0, &DestCtx, &SrcCtx);
            if (Rval != 0) {
                Applib_IK_ERRF("AmbaDSP_ImgInitCtx Fail Line = %d", __LINE__);
                break;
            }
        }
        if (0 != AmbaTUNE_Execute_IDSP(&ItunerInfo.TuningAlgoMode, &ProcInfo)) {
            Applib_IK_ERRF("Call TextHdlr_Execute_IDSP() Fail");
            break;
        }
        {
            AMBA_DSP_IMG_SIZE_INFO_s sizeInfo;
            ITUNER_SYSTEM_s System_info;
            memset(&sizeInfo, 0, sizeof(sizeInfo));
            CfgInfo.Pipe = ItunerInfo.TuningAlgoMode.Pipe;
            CfgInfo.CfgId = 0;
            Rval = AmbaDSP_ImgInitCfg(&CfgInfo, ItunerInfo.TuningAlgoMode.AlgoMode);
            if (Rval != 0) {
                break;
            }
            AmbaItuner_Get_SystemInfo(&System_info);
            sizeInfo.WidthIn = System_info.RawWidth;
            sizeInfo.HeightIn = System_info.RawHeight;
            sizeInfo.WidthMain = System_info.MainWidth;
            sizeInfo.HeightMain = System_info.MainHeight;
            AmbaDSP_ImgSetSizeInfo(&ItunerInfo.TuningAlgoMode, &sizeInfo);
            AmbaDSP_ImgPostExeCfg(&ItunerInfo.TuningAlgoMode, AMBA_DSP_IMG_CFG_EXE_FULLCOPY);
        }
    } while (0);

    return Rval;
}

static Applib_IK_ER_CODE_e Applib_IK_Still_Cmp_IsoCfg(Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool)
{
    int Rval = 0;
    TUNE_Initial_Config_s InitialConfig;
    TUNE_Load_Param_s LoadParam;
    ITUNER_INFO_s ItunerInfo;
    AMBA_ITUNER_PROC_INFO_s ProcInfo;
    AMBA_DSP_IMG_CTX_INFO_s DestCtx;
    AMBA_DSP_IMG_CTX_INFO_s SrcCtx;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
    Applib_IK_DATA_CONTAINER_s data;
    Applib_IK_CMP_RESULT_e cmpResult;

    memset(&SrcCtx, 0x0, sizeof(AMBA_DSP_IMG_CTX_INFO_s));
    memset(&DestCtx, 0x0, sizeof(AMBA_DSP_IMG_CTX_INFO_s));
    memset(&CfgInfo, 0x0, sizeof(CfgInfo));

    //1. Create UT data
    Active_DataHdlr = Applib_IK_Data_Create(Applib_IK_MODE_STILL, memoryPool);
    Applib_IK_Data_Get(Active_DataHdlr, &data);
    strncpy(&data.ProcessInfo.Still.ItunerFileName[0], input->SaveIsoCfgData.pInItunerFileName, sizeof(data.ProcessInfo.Still.ItunerFileName) -1);
    strncpy(&data.ProcessInfo.Still.BinFileName[0], input->SaveIsoCfgData.pOutBinFileName, sizeof(data.ProcessInfo.Still.BinFileName) - 1);
    Applib_IK_Data_Set(Active_DataHdlr, &data);

    //2. hook dump function
    Applib_IK_hook_IsoCfg_cmp(&cmpResult);

    //3. load and process ituner
    do {
        AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
        memset(&InitialConfig, 0x0, sizeof(TUNE_Initial_Config_s));
        InitialConfig.Text.pBytePool = memoryPool;
        if (0 != AmbaTUNE_Init(&InitialConfig)) {
           Applib_IK_WARF("Call AmbaTUNE_Init() Fail");
           break;
        }
        memset(&LoadParam, 0x0, sizeof(TUNE_Load_Param_s));
        LoadParam.Text.FilePath = input->SaveIsoCfgData.pInItunerFileName;
        if (0 != AmbaTUNE_Load_IDSP(&LoadParam)) {
           Applib_IK_WARF("Call TextHdlr_Load_IDSP(%s) Fail", input->SaveIsoCfgData.pInItunerFileName);
           break;
        }
        if (0 != AmbaTUNE_Get_ItunerInfo(&ItunerInfo)) {
           Applib_IK_WARF("Call TextHdlr_Get_ItunerInfo() Fail");
           break;
        }
        {
           DestCtx.Pipe = ItunerInfo.TuningAlgoMode.Pipe;
           DestCtx.CtxId = 0;
           Rval = AmbaDSP_ImgInitCtx(0, 0, &DestCtx, &SrcCtx);
           if (Rval != 0) {
               Applib_IK_ERRF("AmbaDSP_ImgInitCtx Fail Line = %d", __LINE__);
               break;
           }
        }
        if (0 != AmbaTUNE_Execute_IDSP(&ItunerInfo.TuningAlgoMode, &ProcInfo)) {
           Applib_IK_ERRF("Call TextHdlr_Execute_IDSP() Fail");
           break;
        }
        {
            AMBA_DSP_IMG_SIZE_INFO_s sizeInfo;
            ITUNER_SYSTEM_s System_info;
            memset(&sizeInfo, 0, sizeof(sizeInfo));
            CfgInfo.Pipe = ItunerInfo.TuningAlgoMode.Pipe;
            CfgInfo.CfgId = 0;
            Rval = AmbaDSP_ImgInitCfg(&CfgInfo, ItunerInfo.TuningAlgoMode.AlgoMode);
            if (Rval != 0) {
                break;
            }
            AmbaItuner_Get_SystemInfo(&System_info);
            sizeInfo.WidthIn = System_info.RawWidth;
            sizeInfo.HeightIn = System_info.RawHeight;
            sizeInfo.WidthMain = System_info.MainWidth;
            sizeInfo.HeightMain = System_info.MainHeight;
            AmbaDSP_ImgSetSizeInfo(&ItunerInfo.TuningAlgoMode, &sizeInfo);
            AmbaDSP_ImgPostExeCfg(&ItunerInfo.TuningAlgoMode, AMBA_DSP_IMG_CFG_EXE_FULLCOPY);
        }
    } while (0);

    //4. Save reults
    Applib_IK_Data_Get(Active_DataHdlr, &data);
    data.ResultInfo.Still.Result = cmpResult;
    Applib_IK_Data_Get(Active_DataHdlr, &data);

    return Rval;
}

// VIDEO CMP ARCHITECTURE
//  Create_UTData();
//  GetUTData(Active_DataHdlr, Data);
//  Data.Video.TotalFrame = 10;
//  Data.Video.CurrentFrame = 0;
//  Data.Video.BinFile[];
//  Data.Video.ItunerFile[];
//  Data.Video.Script = ""
//  Script2Bin(Script, BinFile, ItunerFile);
//
//
//  Active_DataHdlr = Create_UTData();
//  GetUTData(Active_DataHdlr, Data);
//  Data.Still.BinFile = xxx;
//  SetUTData(Active_DataHdlr, Data);

// Step1 Hook
//  sprintf(G_FileName,"%s",filename);
//  G_Applib_IK_Data.resultMsg[i].cmpData.Still.BinFileName = "";
//  G_Applib_IK_Data.resultMsg[i].cmpData.Still.ItunerFileName = "";
//  img_dsp_core_probe_hook(0x0002,(UINT32)_IsoCfg_Cmp,&G_Applib_IK_Data.resultMsg[i].cmpData.Still.Result);

// Do Test Process

// Update Result
