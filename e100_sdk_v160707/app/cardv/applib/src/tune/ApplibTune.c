    /**
 * @file src/app/connected/applib/src/tune/ApplibTune.c
 *
 *  Implementation of vin Utility interface.
 *
 * History:
 *    2014/10/21 - [Martin Lai] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrintk.h>
#include <AmbaDSP_ImgDef.h>
#include "AmbaTUNE_HdlrManager.h"
#include "Applib_IK_Utility.h"
#include <AmbaCache.h>
#include <common/common.h>
#include <AmbaDSP_ImgUtility.h>
#include <AmbaFS.h>
#include <AmbaSensor.h>
#include <applib.h>
#include <AmbaTest.h>
#include "AmbaRTSL_MMU_Def.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_ImgFilter.h"
#include "imgproc/AmbaImg_Impl_Cmd.h"
#include "imgproc/AmbaImg_Proc.h"
#include "AmbaDSP_WarpCore.h"
#include "imgschdlr/scheduler.h" //ImgSchdlr_QueryLockedIsoCfg Need




#define AppLib_DEBF(fmt, arg...) AmbaPrint("[AppLib][DEBUG]"fmt, ##arg)
#define AppLib_WARF(fmt, arg...) AmbaPrint("[AppLib][WARNING]"fmt, ##arg)
#define AppLib_ERRF(fmt, arg...) AmbaPrint("[AppLib][ERROR]"fmt, ##arg)

#define TEST_CMD_TYPE "img"
#define MAX_SECTION_NUM (8)
#define MAX_SLICE_NUM (16)
#define MAX_IDSP_DUMP_SIZE (149824)

#define DEBUG_CALIB 0 // For Allen Test Only
typedef struct {
    char* Cmd_ID;
    AMP_ER_CODE_e(* Test_Func)(int argc, char **argv);
    char* Cmd_Description;
} TuneTestCmd_s;
static AMP_ER_CODE_e AppLibTune_Help(int argc, char ** argv);
static AMP_ER_CODE_e AppLibTune_RawEnc(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Lowiso(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Highiso(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Ituner(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_idsp(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Test(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Stat(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_StaticBlackLevel(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Resampler(int Argc, char** Argv);
static AMP_ER_CODE_e AppLibTune_VideoHdr(int Argc, char** Argv);
static AMP_ER_CODE_e AppLibTune_Profile(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_ShowCmds(int Argc, char **Argv);
extern char _TextHdlr_Get_Driver_Letter(void); // FIXME:
extern int AmbaDSP_ImgLowIsoDumpCfg(AMBA_DSP_IMG_CFG_INFO_s CfgInfo, char DriveLetter);
extern int AmbaDSP_ImgHighIsoDumpCfg(AMBA_DSP_IMG_CFG_INFO_s CfgInfo, char DriveLetter);
extern int AmbaDSP_CmdSetDebugLevel(UINT8 Module, UINT8 Level, UINT8 Mask);
extern int AmbaDSP_ImgSetWarpCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s *pWarpDzoomCorrByPass);
extern int AmbaDSP_ImgSetWarpCompensationReserve2(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT16 ReservedNum);
extern int AmbaDSP_WarpCore_SetWarpFlipEnb(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WARP_FLIP_INFO_s *WarpFlipEnb);
extern int AmbaDSP_WarpCore_GetWindowInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WARP_WIN_INFO_s *pWindowInfo);
extern int AmbaDSP_WarpCore_CalcWindowInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WARP_CALC_WIN_INFO_s *pCalcWinInfo);
extern int AmbaDSP_WarpCore_SetWarpRotateTheta(AMBA_DSP_IMG_MODE_CFG_s *pMode, double Theta);

#if 0
#include <AmbaDSP_WarpCore.h>
static AMP_ER_CODE_e AppLibTune_vnc(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_Warp(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_ca(int Argc, char **Argv);
static AMP_ER_CODE_e AppLibTune_fpn(int Argc, char **Argv);
#endif
static TuneTestCmd_s TuneTestCmd[] =
{
    {"help", AppLibTune_Help, "",},
    {"-blc", AppLibTune_StaticBlackLevel, "black level correction setup"},
    {"-ituner", AppLibTune_Ituner, ""},
//    {"-itunerload", AppLibTune_Ituner_Load, "",},
//    {"-itunersave", AppLibTune_Ituner_Save, "",},
    {"-rawenc", AppLibTune_RawEnc, "",},
    {"-highiso", AppLibTune_Highiso, "",},
    {"-lowiso", AppLibTune_Lowiso, "", },
    {"-idsp", AppLibTune_idsp,"",},
    {"-test", AppLibTune_Test, "",},
    {"-stat", AppLibTune_Stat, "",},
    {"-resampler", AppLibTune_Resampler, "",},
    {"-vhdr", AppLibTune_VideoHdr, "",},
    {"-profile", AppLibTune_Profile, "",},
    {"-showcmd", AppLibTune_ShowCmds, "",},
#if (DEBUG_CALIB == 1)
    {"-warp", AppLibTune_Warp, "",},
    {"-vnc", AppLibTune_vnc, "",},
    {"-ca", AppLibTune_ca, "", },
    {"-fpn", AppLibTune_fpn, "",},
    {"-dpc", AppLibTune_dpc, "",},
#endif
};

typedef enum _AMBA_ITUNER_MODE_e_{
    ITUNER_BASIC_TUNING_MODE = 0x00,
    ITUENR_PIV_VERIFIED_MODE,
} AMBA_ITUNER_MODE_e;
typedef struct {
    UINT8 Sec2Base[MAX_IDSP_DUMP_SIZE*MAX_SECTION_NUM];
    UINT8 Sec3Base[MAX_IDSP_DUMP_SIZE*MAX_SECTION_NUM];
    AMBA_DSP_EXPRESS_IDSP_CFG_INFO_s IdspDumpInfoOut[MAX_SLICE_NUM*MAX_SECTION_NUM];
    AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s DubugInfo;
} ITUNER_PIV_DUMP_s __attribute__((aligned (32))) ;

static AMBA_ITUNER_MODE_e G_ItunerMode = ITUNER_BASIC_TUNING_MODE;
static struct {
    #define PIV_TASK_STACK_SIZE (30000)
    AMBA_MEM_CTRL_s DumpStackMem;
    AMBA_KAL_TASK_t DumpTask;
} PIVDumpIdsp;
static struct {
    #define HDR_DUMP_TASK_STACK_SIZE (30000)
    AMBA_MEM_CTRL_s DumpStackMem;
    AMBA_KAL_TASK_t DumpTask;
} HDRDumpRaw = {0};

static int ishelp(char* Chars)
{
    if ((strcmp(Chars, "--help") == 0) || (strcmp(Chars, "help") == 0)) {
        return 1;
    } else {
        return 0;
    }
}
static int isnumber(char* Chars)
{

    char* Ptr;
    int AsciiCode;

    Ptr = &Chars[0];
    if (*Ptr == '-')
        Ptr++;
    while (*Ptr != '\0') {
        AsciiCode = (int) *Ptr;
        if ((AsciiCode < '0') || (AsciiCode > '9')) {
            return 0;
        }
        Ptr++;
    }
    return 1;
}

static AMP_ER_CODE_e AppLibTune_Help(int argc, char ** argv)
{
    int i;
    for (i = 0; i < sizeof(TuneTestCmd) / sizeof(TuneTestCmd[0]); i++) {
        AppLib_DEBF("%s: %s", TuneTestCmd[i].Cmd_ID, TuneTestCmd[i].Cmd_Description);
    }
    return AMP_OK;
}

static AMP_ER_CODE_e AppLibTune_RawEnc(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    extern int AppLibStillEnc_RawEncode(void)  __attribute__((weak));

    if (Argc == 3) {
        do {
            TUNE_Initial_Config_s InitialConfig;
            TUNE_Load_Param_s LoadParam;
            memset(&InitialConfig, 0x0, sizeof(TUNE_Initial_Config_s));

            InitialConfig.Text.pBytePool = APPLIB_G_MMPL;
            AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
            if (0 != AmbaTUNE_Init(&InitialConfig)) {
                AppLib_WARF("Call AmbaTUNE_Init() Fail");
                break;
            }

            memset(&LoadParam, 0x0, sizeof(TUNE_Load_Param_s));
            LoadParam.Text.FilePath = Argv[2];
            if (0 != AmbaTUNE_Load_IDSP(&LoadParam)) {
                AppLib_WARF("Call AmbaTUNE_Load_IDSP(%s) Fail", Argv[2]);
                break;
            }

            if (AppLibStillEnc_RawEncode) {
                AppLibStillEnc_RawEncode();
            }
        } while (0);

        Rval = AMP_OK;
    }
    if (Rval == -1)
        AmbaPrint("t "TEST_CMD_TYPE" -rawenc [filepath] : raw encode flow\n"
                  "\tfilepath = file path for configuration file\n");
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_Lowiso(int Argc, char **Argv)
{
    extern int AmbaDSP_ImgSetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgGetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgLowIsoPrintCfg(AMBA_DSP_IMG_CFG_INFO_s CfgInfo);
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
    AMBA_DSP_IMG_DEBUG_MODE_s DebugMode = { 0 };
    char Driver = 'c';
    Driver = _TextHdlr_Get_Driver_Letter();
    if (strcmp(Argv[2], "dump") == 0) {
        if (strcmp(Argv[3], "still") == 0) {
            CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            CfgInfo.CfgId = 0;
            Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
            Mode.ContextId = 0;
            Mode.ConfigId = CfgInfo.CfgId;
            AmbaDSP_ImgLowIsoPrintCfg(CfgInfo);
            AmbaDSP_ImgLowIsoDumpCfg(CfgInfo, Driver);
            Rval = AMP_OK;
        } else if (strcmp(Argv[3], "video") == 0) {
            CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
            if (Argc > 4) {
                CfgInfo.CfgId = atoi(Argv[4]);
            } else {
                int Ret = AmbaDSP_ImgGetDspWriteBackVideoIsoCfgId(&CfgInfo.CfgId);
                if (Ret != AMBA_DSP_IMG_RVAL_SUCCESS) {
                    AppLib_DEBF("Get IsoCfg Id from MW");
                    Ret = ImgSchdlr_QueryLockedIsoCfg(0, AMBA_DSP_IMG_ALGO_MODE_LISO, &CfgInfo.CfgId);
                    if (Ret != OK) {
                        AppLib_ERRF("Call ImgSchdlr_QueryLockedIsoCfg() Fail");
                        goto done;
                    }
                }
                AppLib_DEBF("Dump IsoCfg Id: %d", CfgInfo.CfgId);
            }
            Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
            Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
            Mode.ContextId = 0;
            //Mode.ConfigId = 0;
            Mode.ConfigId = CfgInfo.CfgId;
            AmbaDSP_ImgLowIsoPrintCfg(CfgInfo);
            AmbaDSP_ImgLowIsoDumpCfg(CfgInfo, Driver);
            Rval = AMP_OK;
        }
    } else if (strcmp(Argv[2], "debug") == 0) {
        if (Argc > 4) {
            memset(&DebugMode, 0x0, sizeof(DebugMode));
            DebugMode.Step = atoi(Argv[4]);
            DebugMode.Mode = atoi(Argv[5]);
            DebugMode.BreakMode = atoi(Argv[6]);
            DebugMode.TileX = atoi(Argv[7]);
            DebugMode.TileY = atoi(Argv[8]);
            Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
            Mode.ContextId = 0;
            Mode.ConfigId = 0;
            Mode.ChanId = DebugMode.ChannelID;
            if (strcmp(Argv[3], "still") == 0) {
                Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                if(Argc == 10){
                    DebugMode.ChannelID = atoi(Argv[9]);
                }
            } else if (strcmp(Argv[3], "video") == 0) {
                Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                if (Argc == 10) {
                    DebugMode.PicNum = (UINT32)atoi(Argv[9]);
                } else {
                    DebugMode.PicNum = 0xFFFFFFFF;
                }
            } else {
                goto done;
            }
            AmbaDSP_ImgSetDebugMode(&Mode, &DebugMode);
            Rval = AMP_OK;
        } else {
            if (strcmp(Argv[3], "still") == 0) {
                Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            } else if (strcmp(Argv[3], "video") == 0) {
                Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
            }
            Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
            Mode.ContextId = 0;
            Mode.ConfigId = 0;
            AmbaDSP_ImgGetDebugMode(&Mode, &DebugMode);
            AmbaPrint("Current Lowiso Video debug Step=%d Mode=%d", DebugMode.Step, DebugMode.Mode);
            Rval = AMP_OK;
        }
    }
    done:
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -lowiso [dump | debug]: lowISO test functions\n\r"
                  "       t %s -lowiso dump : dump current lowISO settings\n\r"
                  "       t %s -lowiso debug [step] [mode]: set lowISO debug mode",
                  Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}


static AMP_ER_CODE_e AppLibTune_Highiso(int Argc, char **Argv)
{
    extern int AmbaDSP_ImgGetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgSetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgHighIsoPrintCfg(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFG_INFO_s CfgInfo);
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_DSP_IMG_DEBUG_MODE_s DebugMode = { 0 };
    char Driver = 'c';
    Driver = _TextHdlr_Get_Driver_Letter();

        if (strcmp(Argv[2], "dump") == 0) {
            if (strcmp(Argv[3], "still") == 0) {
                CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                CfgInfo.CfgId = 0;
                Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                Mode.ContextId = 0;
                Mode.ConfigId = CfgInfo.CfgId;
                AmbaDSP_ImgHighIsoPrintCfg(&Mode, CfgInfo);
                AmbaDSP_ImgHighIsoDumpCfg(CfgInfo, Driver);
                Rval = AMP_OK;
            } else if (strcmp(Argv[3], "video") == 0) {
                CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                if (Argc > 4) {
                    CfgInfo.CfgId = atoi(Argv[4]);
                } else {
                    int Ret = AmbaDSP_ImgGetDspWriteBackVideoIsoCfgId(&CfgInfo.CfgId);
                    if (Ret != AMBA_DSP_IMG_RVAL_SUCCESS) {
                        AppLib_DEBF("Get IsoCfg Id from MW");
                        Ret = ImgSchdlr_QueryLockedIsoCfg(0, AMBA_DSP_IMG_ALGO_MODE_LISO, &CfgInfo.CfgId);
                        if (Ret != OK) {
                            AppLib_ERRF("Call ImgSchdlr_QueryLockedIsoCfg() Fail");
                            goto done;
                        }
                    }
                    AppLib_DEBF("Dump IsoCfg Id: %d", CfgInfo.CfgId);
                }
                Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                Mode.ContextId = 0;
                //Mode.ConfigId = 0;
                Mode.ConfigId = CfgInfo.CfgId;
                AmbaDSP_ImgHighIsoPrintCfg(&Mode, CfgInfo);
                AmbaDSP_ImgHighIsoDumpCfg(CfgInfo, Driver);
                Rval = AMP_OK;
            }
        } else if (strcmp(Argv[2], "debug") == 0) {
            if (Argc > 4) {
                memset(&DebugMode, 0x0, sizeof(DebugMode));
                DebugMode.Step = atoi(Argv[4]);
                DebugMode.Mode = atoi(Argv[5]);
                DebugMode.BreakMode = atoi(Argv[6]);
                DebugMode.TileX = atoi(Argv[7]);
                DebugMode.TileY = atoi(Argv[8]);
                Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;

                if (strcmp(Argv[3], "still") == 0) {
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                    Mode.ConfigId = 0;
                    Mode.ContextId = 0;
                } else if (strcmp(Argv[3], "video") == 0) {
                    DebugMode.PicNum = atoi(Argv[9]);
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                    Mode.ConfigId = 0;
                    Mode.ContextId = 1;
                } else {
                    goto done;
                }
                AmbaDSP_ImgSetDebugMode(&Mode, &DebugMode);
                Rval = AMP_OK;
            } else {
                if (strcmp(Argv[3], "still") == 0) {
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                    Mode.ContextId = 0;
                } else if (strcmp(Argv[3], "video") == 0) {
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                    Mode.ContextId = 1;
                }
                Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                Mode.ConfigId = 0;
                AmbaDSP_ImgGetDebugMode(&Mode, &DebugMode);
                AmbaPrint("Current Lowiso Video debug Step=%d Mode=%d", DebugMode.Step, DebugMode.Mode);
                Rval = AMP_OK;
            }
        }
done:
    if (Rval == -1) {
        AmbaPrint("\n\r"
                        "Usage: t %s -highiso [dump | debug] [video | still]: highISO test functions\n\r"
                        "       t %s -highiso dump [video | still]: dump current highISO settings\n\r"
                        "       t %s -highiso debug [video | still] [step] [mode]: set highISO debug mode",
                        Argv[0], Argv[0], Argv[0]);

        return Rval;
    }
    return Rval;
}
#if 1

static UINT8 IdspDump[MAX_IDSP_DUMP_SIZE + 32]  __attribute__((section(".bss.noinit")));;
static int _test_is2_get_idsp_debug_dump(UINT8 SecId, UINT8 *pdebugData)
{
    extern int AmbaDSP_CmdDumpIDspCfg(UINT32 BatchID, UINT32 *DramAddr, UINT32 *DramSize, UINT32 *Mode);
    int SecSize;
    UINT32 *pSecSize;
    UINT32 DramAddr;
    UINT32 DramSize;
    UINT32 SectionID;
    if(pdebugData == NULL) {
        AppLib_ERRF("Input parameter pdebugData = NULL");
        return -1;
    }
    *((int *)pdebugData) = 0xdeadbeef; // clear chip rev for evaluating if dsp is halted
    AmbaCache_Clean((void *)pdebugData, MAX_IDSP_DUMP_SIZE);
    AmbaKAL_TaskSleep(5);

    if (AmbaDSP_ImgIsSupportVirtualMemory()) {
        extern __attribute__((weak)) UINT32 AmbaRTSL_MmuVirtToPhys(UINT32 Virt);
        DramAddr = AmbaRTSL_MmuVirtToPhys((UINT32)(pdebugData) & 0x0FFFFFFF);
    } else {
        DramAddr = ((UINT32)(pdebugData) & 0x0FFFFFFF);
    }

    DramSize = MAX_IDSP_DUMP_SIZE;
    SectionID = (UINT32)SecId;
    AppLib_DEBF("idsp dump address is 0x%.8X, sec_id:%d", DramAddr, SecId);
    AmbaDSP_CmdDumpIDspCfg(0, &DramAddr, &DramSize, &SectionID);
    AmbaKAL_TaskSleep(100);
    AmbaCache_Flush((void *)pdebugData, MAX_IDSP_DUMP_SIZE);
    DramAddr = ((UINT32)0xFFFFFFFF);
    if (*((int *)pdebugData) == 0xdeadbeef) {
        AppLib_WARF("DSP is halted! Dumping post mortem section %d debug data", SecId);
    } else {
        pSecSize = (UINT32 *)(pdebugData + 8);
        SecSize = ((UINT32)*pSecSize) + 64; //64 for header,
    }
    return SecSize;
}

static AMBA_FS_FILE* _posix_fopen(const char *FileName, const char *Mode)
{
    AMBA_FS_FILE *Rval = NULL;

    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        K_ASSERT(0);
        /* Note: A12 not support Unicodeå**/
    } else {
        Rval = AmbaFS_fopen(FileName, Mode);
    }
    return Rval;
}

static int _posix_mkdir(const char *folder_name)
{
    int Rval = 0;
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        K_ASSERT(0);
        /* Note: A12 not support unicode*/
    } else {
        Rval = AmbaFS_Mkdir(folder_name);
    }
    return Rval;
}

static void AppLibTune_DumpPIVTask(UINT32 DubugInfoInputAddr)
{
    AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s* PIVdumpInfo;
    AMBA_DSP_EXPRESS_IDSP_CFG_INFO_s* PIVcfgInfo;
    AMBA_MEM_CTRL_s AmbaTunePIVBuffer;
    ITUNER_PIV_DUMP_s* pItunerPIV;
    int Rval;

    Rval = (AMP_ER_CODE_e)AmbaKAL_MemAllocate(APPLIB_G_MMPL, &AmbaTunePIVBuffer, sizeof(ITUNER_PIV_DUMP_s), 32);
    if (Rval != AMP_OK) {
        AppLib_ERRF("Call AmbaKAL_MemAllocate() Fail");
    }else{
        pItunerPIV = (ITUNER_PIV_DUMP_s*)AmbaTunePIVBuffer.pMemAlignedBase;
        pItunerPIV->DubugInfo.DumpMode = ((AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s*)DubugInfoInputAddr)->DumpMode;
        pItunerPIV->DubugInfo.IdspSec2Enable = ((AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s*)DubugInfoInputAddr)->IdspSec2Enable;
        pItunerPIV->DubugInfo.IdspSec3Enable = ((AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s*)DubugInfoInputAddr)->IdspSec3Enable;
        pItunerPIV->DubugInfo.Sec2CfgBase = (UINT32)( &(pItunerPIV->Sec2Base) );
        pItunerPIV->DubugInfo.Sec3CfgBase = (UINT32)( &(pItunerPIV->Sec3Base) );
        pItunerPIV->DubugInfo.Sec2CfgSize = sizeof(pItunerPIV->Sec2Base);
        pItunerPIV->DubugInfo.Sec3CfgSize = sizeof(pItunerPIV->Sec3Base);

        PIVdumpInfo = &(pItunerPIV->DubugInfo);
        PIVcfgInfo = pItunerPIV->IdspDumpInfoOut;

        AmbaCache_Clean((void*) PIVdumpInfo->Sec2CfgBase, sizeof(PIVdumpInfo->Sec2CfgBase));
        AmbaCache_Clean((void*) PIVdumpInfo->Sec3CfgBase, sizeof(PIVdumpInfo->Sec3CfgBase));

        Rval = AmbaDSP_ExpressIDSPDbg(PIVdumpInfo, PIVcfgInfo);
        if(Rval < 0){
            AppLib_ERRF("PIV dump from IDSP Fail!");
        }else{
            char FnFileName[256];
            AMBA_FS_FILE *Fid;
            UINT8 SliceIdx[MAX_SECTION_NUM] ={0};
            int i=0;
            const char* sec_lut[] = {"sec1","sec2","sec3","sec4","sec5"};//mapping mode to section

            AmbaCache_Flush((void *)PIVdumpInfo->Sec2CfgBase, sizeof(PIVdumpInfo->Sec2CfgBase));
            AmbaCache_Flush((void *)PIVdumpInfo->Sec3CfgBase, sizeof(PIVdumpInfo->Sec3CfgBase));

            for (i=0; i<Rval; i++) {
                AppLib_DEBF( "PIV dump No_%d--------------------", i );
                AppLib_DEBF( "IdspMode:%d", PIVcfgInfo[i].IdspMode );
                AppLib_DEBF( "IdspCfgSize:0x%X IdspCfgAddr:0x%X", PIVcfgInfo[i].IdspCfgSize, PIVcfgInfo[i].IdspCfgAddr );
            }
            _posix_mkdir("c:\\bins");
            for (i=0; i<Rval; i++) {
                SliceIdx[ PIVcfgInfo[i].IdspMode ]++;
                snprintf(FnFileName, sizeof(FnFileName), "c:\\bins\\VID_L_step1_%s_slice%d.bin", sec_lut[PIVcfgInfo[i].IdspMode] , SliceIdx[ PIVcfgInfo[i].IdspMode ]);
                Fid = _posix_fopen(FnFileName, "wb");
                if (Fid == NULL ) {
                    AppLib_ERRF("File open fail. Skip dumping debug data %s", FnFileName);
                    Rval = AMP_ERROR_GENERAL_ERROR;
                    break;
                }
                AmbaFS_fwrite((void const*) PIVcfgInfo[i].IdspCfgAddr, sizeof(UINT8), PIVdumpInfo->Sec2CfgSize, Fid);
                AmbaFS_fclose(Fid);
                AmbaPrint("PIV dump [mode:0] [%s] [slice:%d]", sec_lut[PIVcfgInfo[i].IdspMode] , SliceIdx[ PIVcfgInfo[i].IdspMode ]);
                AmbaPrint("Save Path: %s",FnFileName);
            }
            if(Rval==AMP_OK){
                AppLib_DEBF("PIV dump from IDSP successfully!");
            }
        }

        AmbaKAL_MemFree(&AmbaTunePIVBuffer);
        AmbaKAL_TaskTerminate(&PIVDumpIdsp.DumpTask);
    }
}

static AMP_ER_CODE_e AppLibTune_idsp(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_OK;
    int SecId = 0;
    //char *Prefix;
    //char *Drive;
    UINT8 *pDumpAddress = 0;
    UINT32 Misalign = 0;
    memset(IdspDump, 0xff, (MAX_IDSP_DUMP_SIZE + 32));

    Misalign = ((UINT32) IdspDump) % 32;
    pDumpAddress = (UINT8*) ((UINT32) IdspDump + (32 - Misalign));

    if ((Argc >= 3) && !ishelp(Argv[2])) {
        if (strcmp(Argv[2], "dump") == 0) {

            AMBA_FS_FILE *Fid;
            char *FnIdspDump = "idsp";
            char *Fmode = "w";
            char FnFileName[64];
            if ((Argc > 3) && isnumber(Argv[3])) {
                SecId = (UINT8) atoi(Argv[3]);
            }
            if (((-1 < SecId) && (SecId < 8)) || (SecId == 100) || (SecId == 101) || (SecId == 102)) {
                // The reasonable SecId in video mode 0 are 0~7, 100:MCTF, 101:TBD, 102:TBD
                _test_is2_get_idsp_debug_dump(SecId, (UINT8 *) pDumpAddress);
                AppLib_DEBF("idsp_dump addr %0.8X", pDumpAddress);

                if (Argc > 4) {
                    sprintf(FnFileName, "%c:\\%s_sec%d.bin", _TextHdlr_Get_Driver_Letter(), Argv[4], SecId);
                } else {
                    sprintf(FnFileName, "%c:\\%s_sec%d.bin", _TextHdlr_Get_Driver_Letter(), FnIdspDump, SecId);
                }

                //ignore mctf header
                if ((SecId == 100) && (Argc == 6) && (strcmp(Argv[5], "ignoreheader") == 0)) {
                    memset(pDumpAddress, 0x0, 32);
                    AmbaPrint("ignore header");
                }

                Fid = _posix_fopen(FnFileName, Fmode);
                if (Fid == NULL ) {
                    AppLib_WARF("File open fail. Skip dumping debug data %s", FnFileName);
                    return Rval;
                }
                Rval = (AMP_ER_CODE_e)AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), MAX_IDSP_DUMP_SIZE, Fid);
                AmbaFS_fclose(Fid);
                Rval = AMP_OK;
            } else {
                Rval = AMP_ERROR_GENERAL_ERROR;
            }
        } else if (strcmp(Argv[2], "debug") == 0) {
            UINT8 Module;
            UINT8 Level;
            UINT8 Mask;
            Module = (UINT8)atoi(Argv[3]);
            Level = (UINT8)atoi(Argv[4]);
            Mask = (UINT8)atoi(Argv[5]);
            AmbaDSP_CmdSetDebugLevel(Module, Level, Mask);
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "pivdump") == 0) {
            AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s DubugInfo;
            DubugInfo.DumpMode = (UINT8)atoi(Argv[3]);
            DubugInfo.IdspSec2Enable = (UINT8)atoi(Argv[4]);
            DubugInfo.IdspSec3Enable = (UINT8)atoi(Argv[5]);
            {
                int ReturnValue = 0;

                if (PIVDumpIdsp.DumpStackMem.pMemBase != NULL) {
                    AmbaKAL_TaskDelete(&PIVDumpIdsp.DumpTask);
                    AmbaKAL_MemFree(&PIVDumpIdsp.DumpStackMem);
                    memset(&PIVDumpIdsp, 0x0, sizeof(PIVDumpIdsp));
                }
                AmbaKAL_MemAllocate(APPLIB_G_MMPL, &PIVDumpIdsp.DumpStackMem, PIV_TASK_STACK_SIZE, 32);
                if (PIVDumpIdsp.DumpStackMem.pMemAlignedBase == NULL) {
                    AppLib_WARF("call AmbaKAL_MemAllocate() Fail");
                    Rval = AMP_ERROR_GENERAL_ERROR;
                } else {
                    memset(PIVDumpIdsp.DumpStackMem.pMemAlignedBase, 0, PIV_TASK_STACK_SIZE);
                    ReturnValue = AmbaKAL_TaskCreate( &PIVDumpIdsp.DumpTask,  /* pTask */
                            "PIVdump",                              /* pTaskName */
                            160,                                    /* Priority */
                            (void *) AppLibTune_DumpPIVTask,             /* void (*EntryFunction)(UINT32) */
                            (UINT32) &DubugInfo,                    /* EntryArg */
                            (void *) PIVDumpIdsp.DumpStackMem.pMemAlignedBase,                  /* pStackBase */
                            PIV_TASK_STACK_SIZE,                    /* StackByteSize */
                            AMBA_KAL_AUTO_START);                   /* AutoStart */

                    if (ReturnValue != OK) {
                        AppLib_WARF("Create task fail = %d", ReturnValue);
                        Rval = AMP_ERROR_GENERAL_ERROR;
                    } else {
                        Rval = AMP_OK;
                    }
                }
            }
#endif
        }
    } else {
        Rval = AMP_ERROR_GENERAL_ERROR;
    }

    if (Rval == AMP_ERROR_GENERAL_ERROR) {
        AmbaPrint("Usage: t img -idsp dump [0, 1-7] [filename.bin]: dump idsp configuration");
        AmbaPrint("       t img -idsp dump [100] [filename.bin]: dump MCTF configuration");
        AmbaPrint("       t img -idsp debug [module(8 bits)] [level(8 bits)] [mask(8 bits): set dsp debug level");
        AmbaPrint("       t img -idsp pivdump [mode] [section2] [section3]: set dsp debug level");
    }
    return Rval;
}


int still3ahack = 0;
static AMP_ER_CODE_e AppLibTune_Test(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    if ((Argc >= 3) && !ishelp(Argv[2])) {
        //int ptn = atoi(Argv[2]);
        //OV4689_TestPattern(ptn);
        if (strcmp(Argv[2], "3a") == 0) {
            extern UINT8 ShowAAAHeader;
            ShowAAAHeader = atoi(Argv[3]);
            AmbaPrint("ShowAAAHeader = %d",ShowAAAHeader);
        }else if (strcmp(Argv[2], "hdr3a") == 0) {
            extern UINT8 ShowHdrAaa;
            ShowHdrAaa = atoi(Argv[3]);
        } else if (strcmp(Argv[2], "def_stat") == 0) {
            AMBA_DSP_IMG_MODE_CFG_s Mode;
            AMBA_DSP_IMG_AAA_STAT_INFO_s AAAStat;
            memset(&Mode, 0x0, sizeof(Mode));
            Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
            Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;

            AAAStat.AeTileNumCol = 12;
            AAAStat.AeTileNumRow = 8;
            AAAStat.AeTileColStart = 0;
            AAAStat.AeTileRowStart = 0;
            AAAStat.AeTileWidth = 340;
            AAAStat.AeTileHeight = 512;
            AAAStat.AePixMinValue = 0;
            AAAStat.AePixMaxValue = 16100;

            AAAStat.AfTileNumCol = 12;
            AAAStat.AfTileNumRow = 8;
            AAAStat.AfTileColStart = 0;
            AAAStat.AfTileRowStart = 0;
            AAAStat.AfTileWidth = 340;
            AAAStat.AfTileHeight = 512;
            AAAStat.AfTileActiveWidth = 340;
            AAAStat.AfTileActiveHeight = 512;


            AAAStat.AwbTileNumCol = 32;
            AAAStat.AwbTileNumRow = 32;
            AAAStat.AwbTileColStart = 0;
            AAAStat.AwbTileRowStart = 0;
            AAAStat.AwbTileWidth = 128;
            AAAStat.AwbTileHeight = 128;
            AAAStat.AwbTileActiveWidth = 128;
            AAAStat.AwbTileActiveHeight = 128;
            AAAStat.AwbPixMinValue = 0;
            AAAStat.AwbPixMaxValue = 16100;
            Rval = (AMP_ER_CODE_e)AmbaDSP_Img3aSetAaaStatInfo(&Mode,  &AAAStat);

            Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
            Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
            Rval = (AMP_ER_CODE_e)AmbaDSP_Img3aSetAaaStatInfo(&Mode,  &AAAStat);
        } else {
            Rval = AMP_ERROR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_Ituner(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    static UINT16 MainWidth = 0;
    static UINT16 MainHeight = 0;
    static UINT16 RawWidth = 0;
    static UINT16 RawHeight = 0;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    static ITUNER_SYSTEM_s System;

    memset(&Mode, 0x0, sizeof(Mode));

    Mode.FuncMode = 0;
    Mode.Pipe = 0;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;

    if (!ishelp(Argv[2])) {
        if ((Argc >= 5) && (strcmp(Argv[2], "init") == 0)) {
            MainWidth = atoi(Argv[3]);
            MainHeight = atoi(Argv[4]);
            AppLib_DEBF("Main Width = %d, Main Height = %d", MainWidth, MainHeight);
            if (Argc == 7) {
                RawWidth = atoi(Argv[5]);
                RawHeight = atoi(Argv[6]);
            }
            return AMP_OK;
        } else if ((Argc >= 4) && (strcmp(Argv[2], "load") == 0)) {
            do {
                TUNE_Initial_Config_s InitialConfig;
                ITUNER_INFO_s ItunerInfo;
                AMBA_ITUNER_PROC_INFO_s ProcInfo;
                TUNE_Load_Param_s LoadParam;
                memset(&InitialConfig, 0x0, sizeof(TUNE_Initial_Config_s));
                InitialConfig.Text.pBytePool = APPLIB_G_MMPL;
                AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);

                if (0 != AmbaTUNE_Init(&InitialConfig)) {
                    AppLib_WARF("Call AmbaTUNE_Init() Fail");
                    break;
                }
                memset(&LoadParam, 0x0, sizeof(TUNE_Load_Param_s));
                LoadParam.Text.FilePath = Argv[3];
                if (0 != AmbaTUNE_Load_IDSP(&LoadParam)) {
                    AppLib_WARF("Call AmbaTUNE_Load_IDSP(%s) Fail", Argv[3]);
                    break;
                }
                if (G_ItunerMode != ITUENR_PIV_VERIFIED_MODE) {
                    if (0 != AmbaTUNE_Get_ItunerInfo(&ItunerInfo)) {
                        AppLib_WARF("Call TextHdlr_Get_ItunerInfo() Fail");
                        break;
                    }
                    memcpy(&Mode, &ItunerInfo.TuningAlgoMode, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
                    if (Mode.AlgoMode == AMBA_DSP_IMG_ALGO_MODE_LISO) {
                        Mode.ContextId = 0;
                    } else if (Mode.AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
                        Mode.ContextId = 1;
                    }

                    memset(&ProcInfo, 0, sizeof(AMBA_ITUNER_PROC_INFO_s));
                    if (MainHeight != 0 && MainHeight != 0) {
                        AMBA_SENSOR_STATUS_INFO_s SsrStatus;
                        AMBA_DSP_CHANNEL_ID_u chan;
                        AppLib_DEBF("Main Width = %d, Main Height = %d", MainWidth, MainHeight);
                        AmbaTUNE_Get_SystemInfo(&System);
                        // Note: For Preview Tuning, RawWidth/High Info should get from current sensor, not from ituner file.
                        //AmbaSSPSystemInfo.SensorReadoutMode
                        chan.Bits.SensorID = 0;                        //
                        chan.Bits.VinID = 0; // TODO: Consider Dual Chan
                        memset(&SsrStatus, 0x0, sizeof(AMBA_SENSOR_STATUS_INFO_s));
                        AmbaSensor_GetStatus(chan, &SsrStatus);
                        System.RawStartX = 0;
                        System.RawStartY = 0;
                        if (RawWidth || RawHeight) {
                            System.RawWidth = RawWidth;
                            System.RawHeight = RawHeight;
                        } else {
                            System.RawWidth = SsrStatus.ModeInfo.OutputInfo.RecordingPixels.Width;
                            System.RawHeight = SsrStatus.ModeInfo.OutputInfo.RecordingPixels.Height;
                        }
                        System.MainWidth = MainWidth;
                        System.MainHeight = MainHeight;
                        System.HSubSampleFactorNum = SsrStatus.ModeInfo.InputInfo.HSubsample.FactorNum;
                        System.HSubSampleFactorDen = SsrStatus.ModeInfo.InputInfo.HSubsample.FactorDen;
                        System.VSubSampleFactorNum = SsrStatus.ModeInfo.InputInfo.VSubsample.FactorNum;
                        System.VSubSampleFactorDen = SsrStatus.ModeInfo.InputInfo.VSubsample.FactorDen;
                        AmbaTUNE_Set_SystemInfo(&System);

                        AppLib_DEBF("Execute RawW: %d, H: %d, MainW: %d, H: %d", System.RawWidth, System.RawHeight, System.MainWidth, System.MainHeight);
                        AppLib_DEBF("RawX: %d, Y:%d, H %d %d, V %d %d", System.RawStartX, System.RawStartY, System.HSubSampleFactorNum, System.HSubSampleFactorDen,
                                   System.VSubSampleFactorNum, System.VSubSampleFactorDen);

                    }

                    //ProcInfo.HisoBatchId = AMBA_DSP_STILL_HISO_FILTER;
                    if (0 != AmbaTUNE_Execute_IDSP(&Mode, &ProcInfo)) {
                        AppLib_WARF("Call TextHdlr_Execute_IDSP() Fail");
                        break;
                    }
                }
                Rval = AMP_OK;
            } while (0);
        } else if ((Argc >= 4) && (strcmp(Argv[2], "save") == 0)) {
            do {
                TUNE_Initial_Config_s InitialConfig;
                TUNE_Save_Param_s SaveParam;

                Mode.FuncMode = (UINT8)atoi(Argv[4]);

                memset(&InitialConfig, 0x0, sizeof(TUNE_Initial_Config_s));
                InitialConfig.Text.pBytePool = APPLIB_G_MMPL;
                AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
                if (0 != AmbaTUNE_Init(&InitialConfig)) {
                    AppLib_WARF("Call TextHdlr_Init() Fail");
                    break;
                }
                AppLib_WARF("FIXME: We need to update AE/ WB Info, Wait 3A Get AE Info Function Ready");

                AmbaTUNE_Get_SystemInfo(&System);
                if ((System.RawWidth == 0) || (System.RawHeight == 0)){
                    System.RawWidth = RawWidth;
                    System.RawHeight = RawHeight;
                    System.RawPitch = RawWidth * 2;
                }
                if (System.MainWidth == 0 || System.MainHeight == 0) {
                    System.MainWidth = MainWidth;
                    System.MainHeight = MainHeight;
                }
                if (System.HSubSampleFactorDen == 0 || System.HSubSampleFactorNum == 0 || System.VSubSampleFactorDen == 0 || System.VSubSampleFactorNum == 0) {
                    AMBA_DSP_CHANNEL_ID_u chan;
                    AMBA_SENSOR_STATUS_INFO_s SensorStatus;
                    chan.Bits.SensorID = 0;
                    chan.Bits.VinID = 0;
                    AmbaSensor_GetStatus(chan, &SensorStatus);
                    System.HSubSampleFactorDen = SensorStatus.ModeInfo.InputInfo.HSubsample.FactorDen;
                    System.HSubSampleFactorNum = SensorStatus.ModeInfo.InputInfo.HSubsample.FactorNum;
                    System.VSubSampleFactorDen = SensorStatus.ModeInfo.InputInfo.VSubsample.FactorDen;
                    System.VSubSampleFactorNum = SensorStatus.ModeInfo.InputInfo.HSubsample.FactorNum;
                    //System.RawBayer = SensorStatus.ModeInfo.OutputInfo.CfaPattern;
                    //System.RawResolution = SensorStatus.ModeInfo.OutputInfo.NumDataBits;
                }
                AmbaTUNE_Set_SystemInfo(&System);
                {
                    ITUNER_AE_INFO_s ItunerAeInfo;
                    AMBA_AE_INFO_s ImageAeInfo;
                    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, 0, IP_MODE_VIDEO, (UINT32)&ImageAeInfo);
                    ItunerAeInfo.EvIndex = ImageAeInfo.EvIndex;
                    ItunerAeInfo.NfIndex = ImageAeInfo.NfIndex;
                    ItunerAeInfo.ShutterIndex = ImageAeInfo.ShutterIndex;
                    ItunerAeInfo.AgcIndex = ImageAeInfo.AgcIndex;
                    ItunerAeInfo.IrisIndex = ImageAeInfo.IrisIndex;
                    ItunerAeInfo.Dgain = ImageAeInfo.Dgain;
                    ItunerAeInfo.IsoValue = ImageAeInfo.IsoValue;
                    ItunerAeInfo.Flash = ImageAeInfo.Flash;
                    ItunerAeInfo.Mode = ImageAeInfo.Mode;
                    ItunerAeInfo.ShutterTime = ImageAeInfo.ShutterTime;
                    ItunerAeInfo.AgcGain = ImageAeInfo.AgcGain;
                    ItunerAeInfo.Target = ImageAeInfo.Target;
                    ItunerAeInfo.LumaStat = ImageAeInfo.LumaStat;
                    ItunerAeInfo.LimitStatus = ImageAeInfo.LimitStatus;
                    ItunerAeInfo.Multiplier = 0; // FIXME:
                    AmbaTUNE_Set_AeInfo(&ItunerAeInfo);
                }
                memset(&SaveParam, 0x0, sizeof(TUNE_Save_Param_s));
                SaveParam.Text.Filepath = Argv[3];
                if (0 != AmbaTUNE_Save_IDSP(&Mode, &SaveParam)) {
                    AppLib_WARF("Call AmbaTUNE_Save_IDSP(FilePath: %s Fail", Argv[3]);
                    break;
                }
                Rval = AMP_OK;
            } while (0);
        } else if ((Argc >= 4) && (strcmp(Argv[2], "mode") == 0)) {

            if((strcmp(Argv[3], "0") == 0)){
                 AppLib_DEBF("Ituner Mode: BASIC");
                G_ItunerMode = ITUNER_BASIC_TUNING_MODE;
                Rval = AMP_OK;
            }
            else if((strcmp(Argv[3], "1") == 0)){
                 AppLib_DEBF("Ituner Mode: PIV");
                G_ItunerMode = ITUENR_PIV_VERIFIED_MODE;
                Rval = AMP_OK;
            }else{
                 AppLib_ERRF("Error Ituner Mode");
                Rval = AMP_ERROR_GENERAL_ERROR;
            }
        } else if ((Argc >= 4) && (strcmp(Argv[2], "savebin") == 0)) {
            Applib_IK_INPUTS_u input;
            input.SaveIsoCfgData.pInItunerFileName = Argv[3];
            input.SaveIsoCfgData.pOutBinFileName = Argv[4];
            Applib_IK_Test(Applib_IK_CMD_STILL_SAVE_ISOCONFIG, &input, APPLIB_G_MMPL);

            Rval = AMP_OK;
        } else if ((Argc >= 4) && (strcmp(Argv[2], "cmpbin") == 0)) {
            Applib_IK_INPUTS_u input;
            input.CmpIsoCfgData.pInItunerFileName = Argv[3];
            input.CmpIsoCfgData.pInBinFileName = Argv[4];
            Applib_IK_Test(Applib_IK_CMD_STILL_CMP_ISOCONFIG, &input, APPLIB_G_MMPL);
            Rval = AMP_OK;
        }else if ((Argc >= 5) &&(strcmp(Argv[2], "Applib_IK") == 0)) {
            UINT8 mode = atoi(Argv[3]);
            Applib_IK_INPUTS_u input;
            input.InitData.CompareNum =  atoi(Argv[4]);
            input.InitData.Mode =  atoi(Argv[5]);
            if( mode == 0){
                Applib_IK_Test(Applib_IK_CMD_INIT, &input, APPLIB_G_MMPL);
            }else if( mode == 1 ){
                Applib_IK_Test(Applib_IK_CMD_UNINIT, &input, APPLIB_G_MMPL);
            }else if( mode == 2 ){
                Applib_IK_Test(Applib_IK_CMD_PRINT_LOG, &input, APPLIB_G_MMPL);
            }
            Rval = AMP_OK;
        }
    }

    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "t %s -ituner load/save [filepath] : ituner flow\n\r"
                  "     -ituner mode [mode idx] \n\r"
                  "                        filepath = file path for configuration file",
                  Argv[0]);
    }
    return Rval;
}

static AMBA_DSP_EVENT_RGB_3A_DATA_s gRgbStatData ;
static AMBA_DSP_EVENT_CFA_3A_DATA_s gCfaStatData ;
static AMP_ER_CODE_e AppLibTune_Stat(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_FS_FILE *Fid;
    AMBA_DSP_IMG_AAA_STAT_INFO_s AaaStatInfo;
    AMBA_DSP_IMG_AF_STAT_EX_INFO_s AfStatEx;
    char LineBuf[512];

    UINT32 pRgbData;
    UINT32 pCfaData;
    AMBA_DSP_IMG_MODE_CFG_s Mode;

    extern int AmbaImg_CtrlFunc_Get_Rgb_3a_Stat_Addr(UINT32 channelNo, UINT32 *addr);
    extern int AmbaImg_CtrlFunc_Get_Cfa_3a_Stat_Addr(UINT32 channelNo, UINT32 *addr);

    memset(&Mode, 0x0, sizeof(Mode));
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    AmbaImg_CtrlFunc_Get_Rgb_3a_Stat_Addr(0, &pRgbData);
    AmbaImg_CtrlFunc_Get_Cfa_3a_Stat_Addr(0, &pCfaData);
    if(((void*)&pRgbData!=NULL)&&((void*)&pCfaData != NULL)){
        memcpy(&gRgbStatData, (void *)pRgbData, sizeof(AMBA_DSP_EVENT_RGB_3A_DATA_s));
        memcpy(&gCfaStatData, (void *)pCfaData, sizeof(AMBA_DSP_EVENT_CFA_3A_DATA_s));
    }else{
        AmbaPrint("Test command get no data pRgbData = %p  pCfaData = %p", pRgbData, pCfaData);
        return Rval;
    }

    AmbaDSP_Img3aGetAaaStatInfo(&Mode, &AaaStatInfo);
    AmbaDSP_Img3aGetAfStatExInfo(&Mode, &AfStatEx);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            int i=0;
            AmbaPrint("current AWB statistics tile settings:");
            AmbaPrint("awb_tile_num_col:%d",AaaStatInfo.AwbTileNumCol);
            AmbaPrint("awb_tile_num_row:%d",AaaStatInfo.AwbTileNumRow);
            AmbaPrint("awb_tile_col_start:%d",AaaStatInfo.AwbTileColStart);
            AmbaPrint("awb_tile_row_start:%d",AaaStatInfo.AwbTileRowStart);
            AmbaPrint("awb_tile_width:%d",AaaStatInfo.AwbTileWidth);
            AmbaPrint("awb_tile_height:%d",AaaStatInfo.AwbTileHeight);
            AmbaPrint("awb_tile_active_width:%d",AaaStatInfo.AwbTileActiveWidth);
            AmbaPrint("awb_tile_active_height:%d",AaaStatInfo.AwbTileActiveHeight);
            AmbaPrint("awb_pix_min_value:%d",AaaStatInfo.AwbPixMinValue);
            AmbaPrint("awb_pix_max_value:%d",AaaStatInfo.AwbPixMaxValue);
            AmbaPrint("current AE statistics tile settings:");
            AmbaPrint("ae_tile_num_col:%d",AaaStatInfo.AeTileNumCol);
            AmbaPrint("ae_tile_num_row:%d",AaaStatInfo.AeTileNumRow);
            AmbaPrint("ae_tile_col_start:%d",AaaStatInfo.AeTileColStart);
            AmbaPrint("ae_tile_row_start:%d",AaaStatInfo.AeTileRowStart);
            AmbaPrint("ae_tile_width:%d",AaaStatInfo.AeTileWidth);
            AmbaPrint("ae_tile_height:%d",AaaStatInfo.AeTileHeight);
            AmbaPrint("ae_pix_min_value:%d",AaaStatInfo.AePixMinValue);
            AmbaPrint("ae_pix_max_value:%d",AaaStatInfo.AePixMaxValue);
            AmbaPrint("current AF statistics tile settings:");
            AmbaPrint("af_tile_num_col:%d",AaaStatInfo.AfTileNumCol);
            AmbaPrint("af_tile_num_row:%d",AaaStatInfo.AfTileNumRow);
            AmbaPrint("af_tile_col_start:%d",AaaStatInfo.AfTileColStart);
            AmbaPrint("af_tile_row_start:%d",AaaStatInfo.AfTileRowStart);
            AmbaPrint("af_tile_width:%d",AaaStatInfo.AfTileWidth);
            AmbaPrint("af_tile_height:%d",AaaStatInfo.AfTileHeight);
            AmbaPrint("af_tile_active_width:%d",AaaStatInfo.AfTileActiveWidth);
            AmbaPrint("af_tile_active_height:%d",AaaStatInfo.AfTileActiveHeight);
            AmbaPrint("current Af statistics extention settings:");
            AmbaPrint("AfHorizontalFilter1Mode:%d"     ,AfStatEx.AfHorizontalFilter1Mode);
            AmbaPrint("AfHorizontalFilter1Stage1Enb:%d",AfStatEx.AfHorizontalFilter1Stage1Enb);
            AmbaPrint("AfHorizontalFilter1Stage2Enb:%d",AfStatEx.AfHorizontalFilter1Stage2Enb);
            AmbaPrint("AfHorizontalFilter1Stage3Enb:%d",AfStatEx.AfHorizontalFilter1Stage3Enb);
            for(i=0; i<7; i++)
                AmbaPrint("AfHorizontalFilter1Gain[%d]:%d", i, AfStatEx.AfHorizontalFilter1Gain[i]);
            for(i=0; i<4; i++)
                AmbaPrint("AfHorizontalFilter1Shift[%d]:%d", i, AfStatEx.AfHorizontalFilter1Shift[i]);
            AmbaPrint("AfHorizontalFilter1BiasOff:%d"  ,AfStatEx.AfHorizontalFilter1BiasOff);
            AmbaPrint("AfHorizontalFilter1Thresh:%d"   ,AfStatEx.AfHorizontalFilter1Thresh);
            AmbaPrint("AfVerticalFilter1Thresh:%d"     ,AfStatEx.AfVerticalFilter1Thresh);
            AmbaPrint("AfHorizontalFilter2Mode:%d"     ,AfStatEx.AfHorizontalFilter2Mode);
            AmbaPrint("AfHorizontalFilter2Stage1Enb:%d",AfStatEx.AfHorizontalFilter2Stage1Enb);
            AmbaPrint("AfHorizontalFilter2Stage2Enb:%d",AfStatEx.AfHorizontalFilter2Stage2Enb);
            AmbaPrint("AfHorizontalFilter2Stage3Enb:%d",AfStatEx.AfHorizontalFilter2Stage3Enb);
            for(i=0; i<7; i++)
                AmbaPrint("AfHorizontalFilter2Gain[%d]:%d", i, AfStatEx.AfHorizontalFilter2Gain[i]);
            for(i=0; i<4; i++)
                AmbaPrint("AfHorizontalFilter2Shift[%d]:%d", i, AfStatEx.AfHorizontalFilter2Shift[i]);
            AmbaPrint("AfHorizontalFilter2BiasOff:%d"  ,AfStatEx.AfHorizontalFilter2BiasOff);
            AmbaPrint("AfHorizontalFilter2Thresh:%d"   ,AfStatEx.AfHorizontalFilter2Thresh);
            AmbaPrint("AfVerticalFilter2Thresh:%d"     ,AfStatEx.AfVerticalFilter2Thresh);
            AmbaPrint("AfTileFv1HorizontalShift:%d"    ,AfStatEx.AfTileFv1HorizontalShift);
            AmbaPrint("AfTileFv1VerticalShift:%d"      ,AfStatEx.AfTileFv1VerticalShift);
            AmbaPrint("AfTileFv1HorizontalWeight:%d"   ,AfStatEx.AfTileFv1HorizontalWeight);
            AmbaPrint("AfTileFv1VerticalWeight:%d"     ,AfStatEx.AfTileFv1VerticalWeight);
            AmbaPrint("AfTileFv2HorizontalShift:%d"    ,AfStatEx.AfTileFv2HorizontalShift);
            AmbaPrint("AfTileFv2VerticalShift:%d"      ,AfStatEx.AfTileFv2VerticalShift);
            AmbaPrint("AfTileFv2HorizontalWeight:%d"   ,AfStatEx.AfTileFv2HorizontalWeight);
            AmbaPrint("AfTileFv2VerticalWeight:%d"     ,AfStatEx.AfTileFv2VerticalWeight);

            Rval = AMP_OK;
        } else if (strcmp(Argv[2],"config") == 0) {
            if (strcmp(Argv[3],"ae") == 0) {
                AMBA_DSP_IMG_AE_STAT_INFO_s     AeStat;
                AeStat.AeTileNumCol = atoi(Argv[4]);
                AeStat.AeTileNumRow = atoi(Argv[5]);
                AeStat.AeTileColStart = atoi(Argv[6]);
                AeStat.AeTileRowStart = atoi(Argv[7]);
                AeStat.AeTileWidth = atoi(Argv[8]);
                AeStat.AeTileHeight = atoi(Argv[9]);
                AeStat.AePixMinValue = atoi(Argv[10]);
                AeStat.AePixMaxValue = atoi(Argv[11]);
                AmbaDSP_Img3aSetAeStatInfo(&Mode, &AeStat);
            } else if (strcmp(Argv[3],"awb") == 0) {
                AMBA_DSP_IMG_AWB_STAT_INFO_s    AwbStat;
                AwbStat.AwbTileNumCol = atoi(Argv[4]);
                AwbStat.AwbTileNumRow = atoi(Argv[5]);
                AwbStat.AwbTileColStart = atoi(Argv[6]);
                AwbStat.AwbTileRowStart = atoi(Argv[7]);
                AwbStat.AwbTileWidth = atoi(Argv[8]);
                AwbStat.AwbTileHeight = atoi(Argv[9]);
                AwbStat.AwbTileActiveWidth = atoi(Argv[10]);
                AwbStat.AwbTileActiveHeight = atoi(Argv[11]);
                AwbStat.AwbPixMinValue = atoi(Argv[12]);
                AwbStat.AwbPixMaxValue = atoi(Argv[13]);
                AmbaDSP_Img3aSetAwbStatInfo(&Mode, &AwbStat);
            } else if (strcmp(Argv[3],"af") == 0) {
                AMBA_DSP_IMG_AF_STAT_INFO_s     AfStat;
                AfStat.AfTileNumCol = atoi(Argv[4]);
                AfStat.AfTileNumRow = atoi(Argv[5]);
                AfStat.AfTileColStart = atoi(Argv[6]);
                AfStat.AfTileRowStart = atoi(Argv[7]);
                AfStat.AfTileWidth = atoi(Argv[8]);
                AfStat.AfTileHeight = atoi(Argv[9]);
                AfStat.AfTileActiveWidth = atoi(Argv[10]);
                AfStat.AfTileActiveHeight = atoi(Argv[11]);
                AmbaDSP_Img3aSetAfStatInfo(&Mode, &AfStat);
            } else if (strcmp(Argv[3],"all") == 0) {
                AMBA_DSP_IMG_AAA_STAT_INFO_s      AaaStat;
                AaaStat.AwbTileNumCol       = 32;
                AaaStat.AwbTileNumRow       = 32;
                AaaStat.AwbTileColStart     = 8;
                AaaStat.AwbTileRowStart     = 0;
                AaaStat.AwbTileWidth        = 170;
                AaaStat.AwbTileHeight       = 256;
                AaaStat.AwbTileActiveWidth  = 170;
                AaaStat.AwbTileActiveHeight = 256;
                AaaStat.AwbPixMinValue      = 0;
                AaaStat.AwbPixMaxValue      = 16383;

                AaaStat.AeTileNumCol   = 12;
                AaaStat.AeTileNumRow   = 8;
                AaaStat.AeTileColStart = 8;
                AaaStat.AeTileRowStart = 0;
                AaaStat.AeTileWidth    = 340;
                AaaStat.AeTileHeight   = 512;
                AaaStat.AePixMinValue  = 0;
                AaaStat.AePixMaxValue  = 16383;

                AaaStat.AfTileNumCol       = 8;
                AaaStat.AfTileNumRow       = 5;
                AaaStat.AfTileColStart     = 256;
                AaaStat.AfTileRowStart     = 10;
                AaaStat.AfTileWidth        = 448;
                AaaStat.AfTileHeight       = 816;
                AaaStat.AfTileActiveWidth  = 448;
                AaaStat.AfTileActiveHeight = 816;
                AmbaDSP_Img3aSetAaaStatInfo(&Mode, &AaaStat);
            }
            else if (strcmp(Argv[3],"test") == 0) {
                //extern INT32 flag_3a_test;
                //extern UINT32 Vcount;
                //flag_3a_test = 1;
                //Vcount=0;
                AmbaPrint("flag_3a_test = 1");
            }
            else
                goto AAA_RET_LABEL;
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "save") == 0) {
            int w,h,i,j,idx;
            AMBA_DSP_CFA_AE_s *pAe = &(gCfaStatData.Ae[0]);
            AMBA_DSP_CFA_AWB_s *pAwb = NULL;
            AMBA_DSP_CFA_AF_s *pAf = NULL;
            Fid = AmbaFS_fopen(Argv[3], "w");
            w = gCfaStatData.Header.AeTileNumCol;
            h = gCfaStatData.Header.AeTileNumRow;
            sprintf(LineBuf, "Current CFA AE statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAe[idx].LinY);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\nCurrent CFA AE Count_min\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAe[idx].CountMin);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\nCurrent CFA AE Count_max\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAe[idx].CountMax);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\nCurrent CFA AE tile col start: %d\n\r",gCfaStatData.Header.AeTileColStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AE tile row start: %d\n\r",gCfaStatData.Header.AeTileRowStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AE tile width: %d\n\r"    ,gCfaStatData.Header.AeTileWidth);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AE tile height: %d\n\r"   ,gCfaStatData.Header.AeTileHeight);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AE lin_Y shift: %d\n\n\r" ,gCfaStatData.Header.AeLinearYShift);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);

            sprintf(LineBuf, "Current RGB AE statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",gRgbStatData.Ae[idx].SumY);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\nCurrent RGB AE tile col start: %d\n\r",gRgbStatData.Header.AeTileColStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AE tile row start: %d\n\r",gRgbStatData.Header.AeTileRowStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AE tile width: %d\n\r"    ,gRgbStatData.Header.AeTileWidth);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AE tile height: %d\n\r"   ,gRgbStatData.Header.AeTileHeight);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AE Y shift: %d\n\n\r"     ,gRgbStatData.Header.AeYShift);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            w = gCfaStatData.Header.AwbTileNumCol;
            h = gCfaStatData.Header.AwbTileNumRow;
            pAwb = &(gCfaStatData.Awb[0]);
            sprintf(LineBuf, "Current CFA AWB statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "---  Red statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].SumR);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }



            sprintf(LineBuf, "\n---  Green statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].SumG);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }


            sprintf(LineBuf, "\n---  Blue statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].SumB);
                }

                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\n---  Count_min   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].CountMin);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "---  Count_max   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].CountMax);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "Current CFA AWB tile col start: %d\n\r" ,gCfaStatData.Header.AwbTileColStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AWB tile row start: %d\n\r" ,gCfaStatData.Header.AwbTileRowStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AWB tile act_width: %d\n\r" ,gCfaStatData.Header.AwbTileActiveWidth);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AWB tile act_height: %d\n\r",gCfaStatData.Header.AwbTileActiveHeight);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AWB RGB shift: %d\n\n\r"    ,gCfaStatData.Header.AwbRgbShift);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);

            w = gCfaStatData.Header.AfTileNumCol;
            h = gCfaStatData.Header.AfTileNumRow;

            sprintf(LineBuf, "Current CFA AF statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "---  Y statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumY);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }
            sprintf(LineBuf, "\n---  FV1 statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV1);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\n---  FV2 statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV2);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "Current CFA AF tile col start: %d\n\r" ,gCfaStatData.Header.AfTileColStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AF tile row start: %d\n\r" ,gCfaStatData.Header.AfTileRowStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AF tile act_width: %d\n\r" ,gCfaStatData.Header.AfTileActiveWidth);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AF tile act_height: %d\n\r",gCfaStatData.Header.AfTileActiveHeight);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current CFA AF Y shift: %d\n\n\r"      ,gCfaStatData.Header.AfCfaYShift);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AF statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "---  Y statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumY);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\n---  FV1 statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV1);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }

            sprintf(LineBuf, "\n---  FV2 statistics   ---\n\r");
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            for (j=0; j<h; j++) {
                for (i=0; i<w; i++) {
                    idx = j*w+ i;
                    sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV2);
                }
                sprintf(&LineBuf[6*w], "\n\r");
                AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
                AmbaKAL_TaskSleep(10);
            }
            sprintf(LineBuf, "\nCurrent RGB AF tile col start: %d\n\r" ,gRgbStatData.Header.AfTileColStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AF tile row start: %d\n\r" ,gRgbStatData.Header.AfTileRowStart);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AF tile act_width: %d\n\r" ,gRgbStatData.Header.AfTileActiveWidth);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AF tile act_height: %d\n\r",gRgbStatData.Header.AfTileActiveHeight);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            sprintf(LineBuf, "Current RGB AF Y shift: %d\n\n\r"      ,gRgbStatData.Header.AfYShift);
            AmbaFS_fwrite(LineBuf, strlen(LineBuf), 1, Fid);
            AmbaFS_fclose(Fid);
            AmbaPrint("%s() %d, Save Stat File to %s Finish!", __func__, __LINE__, Argv[3]);
            Rval = AMP_OK;
        } else if (strcmp(Argv[2],"show") == 0) {
            int w,h,i,j,idx;
            if (strcmp(Argv[3],"header") == 0) {
                int i=0;
                AMBA_DSP_3A_HEADER_s *pHeader;
            for(i =0; i<2 ;i++){
                if(i ==0){
                    AmbaPrint("RGB header");
                    pHeader =&gRgbStatData.Header;
                }else{
                    AmbaPrint("CFA header");
                    pHeader =&gCfaStatData.Header;
                }

                AmbaPrint("AwbTileColStart %d",pHeader->AwbTileColStart);
                AmbaPrint("AwbTileRowStart %d",pHeader->AwbTileRowStart);
                AmbaPrint("AwbTileWidth %d",pHeader->AwbTileWidth);
                AmbaPrint("AwbTileHeight %d",pHeader->AwbTileHeight);
                AmbaPrint("AwbTileActiveWidth %d",pHeader->AwbTileActiveWidth);
                AmbaPrint("AwbTileActiveHeight %d",pHeader->AwbTileActiveHeight);
                AmbaPrint("AwbRgbShift %d",pHeader->AwbRgbShift);
                AmbaPrint("AwbYShift %d",pHeader->AwbYShift);
                AmbaPrint("AwbMinMaxShift %d",pHeader->AwbMinMaxShift);
                AmbaPrint("AeTileColStart %d",pHeader->AeTileColStart);
                AmbaPrint("AeTileRowStart %d",pHeader->AeTileRowStart);
                AmbaPrint("AeTileWidth %d",pHeader->AeTileWidth);
                AmbaPrint("AeTileHeight %d",pHeader->AeTileHeight);
                AmbaPrint("AeYShift %d",pHeader->AeYShift);
                AmbaPrint("AeLinearYShift %d",pHeader->AeLinearYShift);
                AmbaPrint("AfTileColStart %d",pHeader->AfTileColStart);
                AmbaPrint("AfTileRowStart %d",pHeader->AfTileRowStart);
                AmbaPrint("AfTileWidth %d",pHeader->AfTileWidth);
                AmbaPrint("AfTileHeight %d",pHeader->AfTileHeight);
                AmbaPrint("AfTileActiveWidth %d",pHeader->AfTileActiveWidth);
                AmbaPrint("AfTileActiveHeight %d",pHeader->AfTileActiveHeight);
                AmbaPrint("AfYShift %d",pHeader->AfYShift);
                AmbaPrint("AfCfaYShift %d",pHeader->AfCfaYShift);
                AmbaPrint("AwbTileNumCol %d",pHeader->AwbTileNumCol);
                AmbaPrint("AwbTileNumRow %d",pHeader->AwbTileNumRow);
                AmbaPrint("AeTileNumCol %d",pHeader->AeTileNumCol);
                AmbaPrint("AeTileNumRow %d",pHeader->AeTileNumRow);
                AmbaPrint("AfTileNumCol %d",pHeader->AfTileNumCol);
                AmbaPrint("AfTileNumRow %d",pHeader->AfTileNumRow);
                /*
                AmbaPrint("TotalSlicesX %d",pHeader->TotalSlicesX);
                AmbaPrint("TotalSlicesY %d",pHeader->TotalSlicesY);
                AmbaPrint("SliceIndexX %d",pHeader->SliceIndexX);
                AmbaPrint("SliceIndexY %d",pHeader->SliceIndexY);
                AmbaPrint("SliceWidth %d",pHeader->SliceWidth);
                AmbaPrint("SliceHeight %d",pHeader->SliceHeight);
                AmbaPrint("SliceStartX %d",pHeader->SliceStartX);
                AmbaPrint("SliceStartY %d\n\r",pHeader->SliceStartY);
                */
            }
            Rval = AMP_OK;

            }
            else if (strcmp(Argv[3],"ae") == 0) {
                AMBA_DSP_CFA_AE_s *pAe = &(gCfaStatData.Ae[0]);
                w = gCfaStatData.Header.AeTileNumCol;
                h = gCfaStatData.Header.AeTileNumRow;

                AmbaPrint("Current CFA AE statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAe[idx].LinY);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("Current CFA AE Count_min\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAe[idx].CountMin);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("Current CFA AE Count_max\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAe[idx].CountMax);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("Current CFA AE tile col start: %d\n\r",gCfaStatData.Header.AeTileColStart);
                AmbaPrint("Current CFA AE tile row start: %d\n\r",gCfaStatData.Header.AeTileRowStart);
                AmbaPrint("Current CFA AE tile width: %d\n\r"    ,gCfaStatData.Header.AeTileWidth);
                AmbaPrint("Current CFA AE tile height: %d\n\r"   ,gCfaStatData.Header.AeTileHeight);
                AmbaPrint("Current CFA AE lin_Y shift: %d\n\n\r" ,gCfaStatData.Header.AeLinearYShift);

                AmbaPrint("Current RGB AE statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",gRgbStatData.Ae[idx].SumY);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }

                AmbaPrint("\n\r");
                AmbaPrint("Current RGB AE tile col start: %d\n\r",gRgbStatData.Header.AeTileColStart);
                AmbaPrint("Current RGB AE tile row start: %d\n\r",gRgbStatData.Header.AeTileRowStart);
                AmbaPrint("Current RGB AE tile width: %d\n\r"    ,gRgbStatData.Header.AeTileWidth);
                AmbaPrint("Current RGB AE tile height: %d\n\r"   ,gRgbStatData.Header.AeTileHeight);
                AmbaPrint("Current RGB AE Y shift: %d\n\n\r"     ,gRgbStatData.Header.AeYShift);
                Rval = AMP_OK;
            } else if (strcmp(Argv[3],"awb") == 0) {
                AMBA_DSP_CFA_AWB_s *pAwb = &(gCfaStatData.Awb[0]);
                w = gCfaStatData.Header.AwbTileNumCol;
                h = gCfaStatData.Header.AwbTileNumRow;

                AmbaPrint("Current CFA AWB statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
                AmbaPrint("---  Red statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].SumR);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  Green statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].SumG);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  Blue statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].SumB);
                    }

                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  Count_min   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].CountMin);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  Count_max   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAwb[idx].CountMax);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("Current CFA AWB tile col start: %d\n\r" ,gCfaStatData.Header.AwbTileColStart);
                AmbaPrint("Current CFA AWB tile row start: %d\n\r" ,gCfaStatData.Header.AwbTileRowStart);
                AmbaPrint("Current CFA AWB tile act_width: %d\n\r" ,gCfaStatData.Header.AwbTileActiveWidth);
                AmbaPrint("Current CFA AWB tile act_height: %d\n\r",gCfaStatData.Header.AwbTileActiveHeight);
                AmbaPrint("Current CFA AWB RGB shift: %d\n\n\r"    ,gCfaStatData.Header.AwbRgbShift);
                Rval = AMP_OK;
            } else if (strcmp(Argv[3],"af") == 0) {
                AMBA_DSP_CFA_AF_s *pAf = &(gCfaStatData.Af[0]);
                w = gCfaStatData.Header.AfTileNumCol;
                h = gCfaStatData.Header.AfTileNumRow;

                AmbaPrint("Current CFA AF statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
                AmbaPrint("---  Y statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumY);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  FV1 statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV1);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  FV2 statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV2);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("Current CFA AF tile col start: %d\n\r" ,gCfaStatData.Header.AfTileColStart);
                AmbaPrint("Current CFA AF tile row start: %d\n\r" ,gCfaStatData.Header.AfTileRowStart);
                AmbaPrint("Current CFA AF tile act_width: %d\n\r" ,gCfaStatData.Header.AfTileActiveWidth);
                AmbaPrint("Current CFA AF tile act_height: %d\n\r",gCfaStatData.Header.AfTileActiveHeight);
                AmbaPrint("Current CFA AF Y shift: %d\n\n\r"      ,gCfaStatData.Header.AfCfaYShift);
                AmbaPrint("Current RGB AF statistics (tile_num_col: %d, tile_num_row: %d)\n\r", w, h);
                AmbaPrint("---  Y statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumY);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  FV1 statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV1);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  FV2 statistics   ---\n\r");
                for (j=0; j<h; j++) {
                    for (i=0; i<w; i++) {
                        idx = j*w+ i;
                        sprintf(&LineBuf[6*i],"%5d ",pAf[idx].SumFV2);
                    }
                    sprintf(&LineBuf[6*w], "%c", '\0');

                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("Current RGB AF tile col start: %d\n\r" ,gRgbStatData.Header.AfTileColStart);
                AmbaPrint("Current RGB AF tile row start: %d\n\r" ,gRgbStatData.Header.AfTileRowStart);
                AmbaPrint("Current RGB AF tile act_width: %d\n\r" ,gRgbStatData.Header.AfTileActiveWidth);
                AmbaPrint("Current RGB AF tile act_height: %d\n\r",gRgbStatData.Header.AfTileActiveHeight);
                AmbaPrint("Current RGB AF Y shift: %d\n\n\r"      ,gRgbStatData.Header.AfYShift);
                Rval = AMP_OK;
            } else if (strcmp(Argv[3],"hist") == 0) {
                AmbaPrint("Current CFA Histogram statistics\n\r");
                AmbaPrint("---  R Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gCfaStatData.Histo.HisBinR[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  G Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gCfaStatData.Histo.HisBinG[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  B Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gCfaStatData.Histo.HisBinB[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  Y Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gCfaStatData.Histo.HisBinY[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("Current RGB Histogram statistics\n\r");
                AmbaPrint("---  Y Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gRgbStatData.Histo.HisBinY[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  R Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gRgbStatData.Histo.HisBinR[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  G Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gRgbStatData.Histo.HisBinG[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                AmbaPrint("---  B Histogram   ---\n\r");
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        idx = j*8+ i;
                        sprintf(&LineBuf[12*i],"%12d ",gRgbStatData.Histo.HisBinB[idx]);
                    }
                    sprintf(&LineBuf[12*8], "%c", '\0');
                    AmbaPrint("%s",LineBuf);
                    AmbaKAL_TaskSleep(10);
                }
                AmbaPrint("\n\r");
                Rval = AMP_OK;
            }
        } else if (strcmp(Argv[2],"debug") == 0) {
           extern int AmbaDSP_ImgSet3AStatDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT8 CfaEnable ,UINT8 RgbEnable);
           UINT8 cfaEnable;
           UINT8 RgbEnable;
           AMBA_DSP_IMG_MODE_CFG_s pMode;
           cfaEnable = atoi(Argv[3]);
           RgbEnable = atoi(Argv[4]);
           pMode.ContextId = 0;
           pMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
           AmbaDSP_ImgSet3AStatDebugMode(&pMode,cfaEnable,RgbEnable);
           Rval = AMP_OK;
        } else if (strcmp(Argv[2],"dump") == 0) {
            extern int AmbaDSP_ImgGet3AStatDebugCfaData(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_EVENT_CFA_3A_DATA_s *AaaCfaDta ,UINT32 *pFrameCount);
            extern int AmbaDSP_ImgGet3AStatDebugRgbData(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_EVENT_RGB_3A_DATA_s *AaaRgbDta ,UINT32 *pFrameCount);
            AMBA_DSP_EVENT_CFA_3A_DATA_s cfaData;
            AMBA_DSP_EVENT_RGB_3A_DATA_s rgbData;
            UINT32 cfaframeCnt;
            UINT32 rgbframeCnt;
            AMBA_DSP_IMG_MODE_CFG_s pMode;
            Ituner_Ext_File_Param_s Ext_File_Param;
            TUNE_Initial_Config_s TuneInitialConfig;
            char filePath[128];

            pMode.ContextId = 0;
            pMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
            AmbaDSP_ImgGet3AStatDebugCfaData(&pMode, &cfaData, &cfaframeCnt);
            AmbaPrint("CFA STAT frame counter = %d", cfaframeCnt);
            AmbaDSP_ImgGet3AStatDebugRgbData(&pMode, &rgbData, &rgbframeCnt);
            AmbaPrint("RGB STAT frame counter = %d", rgbframeCnt);

           AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
           TuneInitialConfig.Text.pBytePool = APPLIB_G_MMPL;
           if (0 != AmbaTUNE_Init(&TuneInitialConfig)) {
               AmbaPrint("Call AmbaTUNE_Init() Fail");
           }
           memset(&Ext_File_Param, 0x0, sizeof(Ituner_Ext_File_Param_s));
           Ext_File_Param.Stat_Save_Param.Address = &cfaData;
           Ext_File_Param.Stat_Save_Param.Size = sizeof(AMBA_DSP_EVENT_CFA_3A_DATA_s);
           Ext_File_Param.Stat_Save_Param.Target_File_Path = filePath; // Should provide Last file name,,
           sprintf(filePath, "C:\\dump_frm%d_CFA_STAT.txt", cfaframeCnt);
           AmbaTUNE_Save_Data(EXT_FILE_CFA_STAT, &Ext_File_Param);

           memset(&Ext_File_Param, 0x0, sizeof(Ituner_Ext_File_Param_s));
           Ext_File_Param.Stat_Save_Param.Address = &rgbData;
           Ext_File_Param.Stat_Save_Param.Size = sizeof(AMBA_DSP_EVENT_RGB_3A_DATA_s);
           Ext_File_Param.Stat_Save_Param.Target_File_Path = filePath; // Should provide Last file name,,
           sprintf(filePath, "C:\\dump_frm%d_RGB_STAT.txt", rgbframeCnt);
           AmbaTUNE_Save_Data(EXT_FILE_RGB_STAT, &Ext_File_Param);

           AmbaTUNE_UnInit();
            Rval = AMP_OK;
        } else {
            Rval = AMP_ERROR_GENERAL_ERROR;
        }
    }

AAA_RET_LABEL:
    if (Rval != AMP_OK) {
        AmbaPrint("Usage: t %s -stat: get the current aaa statistics settings\n\r", Argv[0]);
        AmbaPrint("       t %s -stat config ae  [num_w] [num_h] [col_start] [row_start] [tile_w] [tile_h] [pix_min] [pix_max]: set AE statistics settings", Argv[0]);
        AmbaPrint("       t %s -stat config awb [num_w] [num_h] [col_start] [row_start] [tile_w] [tile_h] [act_w] [act_h] [pix_min] [pix_max]: set AWB statistics settings", Argv[0]);
        AmbaPrint("       t %s -stat config af  [num_w] [num_h] [col_start] [row_start] [tile_w] [tile_h] [act_w] [act_h]: set AF statistics settings", Argv[0]);
        AmbaPrint("       t %s -stat show [ae|awb|af|hist] : Show AE/AWB/AF/histogram statistics", Argv[0]);
        AmbaPrint("       t %s -stat dump [rgb_path] [cfa_path]: dump CFA + RGB statistics to storage", Argv[0]);
        AmbaPrint("       t %s -stat save [txt path]: dump statistics to text file", Argv[0]);
        AmbaPrint("       t %s -stat debug [cfa_enable] [rgb_enable]: set debug mode on(1)/off(0)", Argv[0]);
    }
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_StaticBlackLevel(int Argc, char** Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;

    AMBA_DSP_IMG_BLACK_CORRECTION_s BlackCorr;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    memset(&Mode, 0x0, sizeof(Mode));
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    if (!ishelp(Argv[2])) {
        if (Argc == 6) {
            BlackCorr.BlackR = atoi(Argv[2]);
            BlackCorr.BlackGr = atoi(Argv[3]);
            BlackCorr.BlackGb = atoi(Argv[4]);
            BlackCorr.BlackB = atoi(Argv[5]);
            AmbaDSP_ImgSetStaticBlackLevel(&Mode, &BlackCorr);
        } else {
            Rval = AmbaDSP_ImgGetStaticBlackLevel(&Mode, &BlackCorr);
            AmbaPrint("Static Black Level:");
            AmbaPrint("BlackR  : %d", (int)BlackCorr.BlackR);
            AmbaPrint("BlackGr : %d", (int)BlackCorr.BlackGr);
            AmbaPrint("BlackGb : %d", (int)BlackCorr.BlackGb);
            AmbaPrint("BlackB  : %d", (int)BlackCorr.BlackB);
        }
    }
    if (Rval != AMP_OK) {
        AmbaPrint("Usage: t %s -blc: get the current black level\n"
                  "       t %s -blc [r_black] [g_black] [b_black]: set the current black level\n"
                  "\n",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_Resampler(int Argc, char** Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_RESAMPLER_STR_s Resampler_Str;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    memset(&Mode, 0x0, sizeof(Mode));
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    if (!ishelp(Argv[2])) {
        if (Argc == 7) {
            UINT8 IsEnableCFA_H, IsEnableCFA_V, IsEnableMAIN_H, IsEnableMAIN_V;
            IsEnableCFA_H = atoi(Argv[2]);
            IsEnableCFA_V = atoi(Argv[3]);
            IsEnableMAIN_H = atoi(Argv[4]);
            IsEnableMAIN_V = atoi(Argv[5]);
            Resampler_Str.CutoffFreq = atoi(Argv[6]);
            Resampler_Str.Select = ((IsEnableCFA_H) ? RESMP_STR_SELECT_CFA_HORZ : 0);
            Resampler_Str.Select |= ((IsEnableCFA_V) ? RESMP_STR_SELECT_CFA_VERT : 0);
            Resampler_Str.Select |= ((IsEnableMAIN_H) ? RESMP_STR_SELECT_MAIN_HORZ : 0);
            Resampler_Str.Select |= ((IsEnableMAIN_V) ? RESMP_STR_SELECT_MAIN_VERT : 0);
            Rval = AmbaDSP_ImgSetResamplerStrength(&Mode, &Resampler_Str);
        } else {
            Rval = AmbaDSP_ImgGetResamplerStrength(&Mode, &Resampler_Str);
            AmbaPrint("Resampler Coef Adjust:");
            AmbaPrint("CFA_H  : %s", (Resampler_Str.Select & RESMP_STR_SELECT_CFA_HORZ) ? "Enable" : "Disable");
            AmbaPrint("CFA_V  : %s", (Resampler_Str.Select & RESMP_STR_SELECT_CFA_VERT) ? "Enable" : "Disable");
            AmbaPrint("MAIN_H : %s", (Resampler_Str.Select & RESMP_STR_SELECT_MAIN_HORZ) ? "Enable" : "Disable");
            AmbaPrint("MAIN_V : %s", (Resampler_Str.Select & RESMP_STR_SELECT_MAIN_VERT) ? "Enable" : "Disable");
            AmbaPrint("Cut off freq : %d", Resampler_Str.CutoffFreq);
        }
    }
    if (Rval != AMP_OK) {
        AmbaPrint("Usage: t %s -resampler: get resampler coef adjust settings", Argv[0]);
        AmbaPrint("       t %s -resampler [cfa_h] [cfa_v] [main_h] [main_v] [cut off freq]: set resampler coef adjust settings", Argv[0]);
    }
    return Rval;
}

static void AppLibTune_DumpHdrBlendingRaw(char *pFileNamePrefix)
{
    AMBA_MEM_CTRL_s BufAddr = {NULL, NULL};
    const UINT32 BufSize = 4416*3288*2;
    char FileNamePrefix[64];
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    int Rval;
    strncpy(FileNamePrefix, pFileNamePrefix, sizeof(FileNamePrefix));
    memset(&Mode, 0x0, sizeof(Mode));
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
    do {
        AmbaKAL_MemAllocate(APPLIB_G_MMPL, &BufAddr, BufSize, 32);
        if (BufAddr.pMemAlignedBase == NULL) {
            AppLib_WARF("call AmbaKAL_MemAllocate() Fail");
            break;
        }
        Rval = AmbaDSP_ImgDumpHdrDspBlendRaw(&Mode, (UINT32)BufAddr.pMemAlignedBase, BufSize, FileNamePrefix);
        if (Rval != AMBA_DSP_IMG_RVAL_SUCCESS) {
            AppLib_WARF("call AmbaDSP_ImgDumpHdrDspBlendRaw() Fail, Rval = 0x%x", Rval);
        }
        AmbaKAL_MemFree(&BufAddr);
    } while(0);
    AmbaKAL_TaskTerminate(&HDRDumpRaw.DumpTask);
}

static AMP_ER_CODE_e AppLibTune_VideoHdr(int Argc, char** Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo = {0};
    AMBA_DSP_IMG_HDR_BLENDING_INFO_s BlendInfo = {
        .CurrentBlendingIndex = 0,
        .MaxBlendingNumber = 1,
    };
    CfgInfo.CfgId = 0;
    CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    memset(&Mode, 0x0, sizeof(Mode));
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
    if (strcmp(Argv[2], "dump") == 0) {
        UINT8 CfgId = (UINT8)atoi(Argv[3]);
        CfgInfo.CfgId = CfgId;
        AmbaDSP_ImgVideoHdrPrintCfg(&Mode, &CfgInfo);
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "hdr_le_blc") == 0) {
        AMBA_DSP_IMG_BLACK_CORRECTION_s LeBlackCorr;
        AmbaDSP_ImgSetHdrBlendingIndex(&Mode, &BlendInfo);
        if (Argc == 7) {
            LeBlackCorr.BlackR = atoi(Argv[3]);
            LeBlackCorr.BlackGr = atoi(Argv[4]);
            LeBlackCorr.BlackGb = atoi(Argv[5]);
            LeBlackCorr.BlackB = atoi(Argv[6]);
            AmbaDSP_ImgSetStaticBlackLevel(&Mode, &LeBlackCorr);
        } else {
            Rval = AmbaDSP_ImgGetStaticBlackLevel(&Mode, &LeBlackCorr);
            AmbaPrint("vin_static_black_level[0] R: %d, Gr: %d, Gb: %d, B: %d", LeBlackCorr.BlackR, LeBlackCorr.BlackGr, LeBlackCorr.BlackGb, LeBlackCorr.BlackB);
        }
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "hdr_alpha_calc_cfg") == 0) {
        AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s AlphaCalcCfg;
        AmbaDSP_ImgSetHdrBlendingIndex(&Mode, &BlendInfo);
        if (Argc >= 10) {
            AlphaCalcCfg.AvgRadius = atoi(Argv[3]);
            AlphaCalcCfg.AvgMethod = atoi(Argv[4]);
            AlphaCalcCfg.BlendControl = atoi(Argv[5]);
            AlphaCalcCfg.LumaAvgWeightR = atoi(Argv[6]);
            AlphaCalcCfg.LumaAvgWeightGr = atoi(Argv[7]);
            AlphaCalcCfg.LumaAvgWeightGb = atoi(Argv[8]);
            AlphaCalcCfg.LumaAvgWeightB = atoi(Argv[9]);
            AmbaDSP_ImgSetHdrAlphaCalcConfig(&Mode, &AlphaCalcCfg);
        } else {
            AmbaDSP_ImgGetHdrAlphaCalcConfig(&Mode, &AlphaCalcCfg);
            AmbaPrint("hdr_alpha_calc_cfg[0] AvgRadius: %d, AvgMethod: %d, BlendControl: %d",
                      AlphaCalcCfg.AvgRadius, AlphaCalcCfg.AvgMethod, AlphaCalcCfg.BlendControl);
            AmbaPrint("LumaAvgWeight R: %d, Gr: %d, Gb: %d, B: %d",
                      AlphaCalcCfg.LumaAvgWeightR, AlphaCalcCfg.LumaAvgWeightGr, AlphaCalcCfg.LumaAvgWeightGb, AlphaCalcCfg.LumaAvgWeightB);
        }
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "hdr_alpha_calc_th") == 0) {
        AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s AlphaCalcThresh;
        AmbaDSP_ImgSetHdrBlendingIndex(&Mode, &BlendInfo);
        if (Argc >= 7) {
            AlphaCalcThresh.SaturationThresholdR = atoi(Argv[3]);
            AlphaCalcThresh.SaturationThresholdGr = atoi(Argv[4]);
            AlphaCalcThresh.SaturationThresholdGb = atoi(Argv[5]);
            AlphaCalcThresh.SaturationThresholdB = atoi(Argv[6]);
            AmbaDSP_ImgSetHdrAlphaCalcThreshold(&Mode, &AlphaCalcThresh);
        } else {
            AmbaDSP_ImgGetHdrAlphaCalcThreshold(&Mode, &AlphaCalcThresh);
            AmbaPrint("hdr_alpha_calc_thresh[0] R: %d, Gr: %d, Gb: %d, B: %d",
                      AlphaCalcThresh.SaturationThresholdR, AlphaCalcThresh.SaturationThresholdGr, AlphaCalcThresh.SaturationThresholdGb, AlphaCalcThresh.SaturationThresholdB);
        }
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "hdr_alpha_calc_blc") == 0) {
        AMBA_DSP_IMG_BLACK_CORRECTION_s BlackCorr;
        AmbaDSP_ImgSetHdrBlendingIndex(&Mode, &BlendInfo);
        if (Argc >= 7) {
            BlackCorr.BlackR = atoi(Argv[3]);
            BlackCorr.BlackGr = atoi(Argv[4]);
            BlackCorr.BlackGb = atoi(Argv[5]);
            BlackCorr.BlackB = atoi(Argv[6]);
            AmbaDSP_ImgSetHdrAlphaCalcBlackLevel(&Mode, &BlackCorr);
        } else {
            AmbaDSP_ImgGetHdrAlphaCalcBlackLevel(&Mode, &BlackCorr);
            AmbaPrint("hdr_alpha_calc_black_level[0] R: %d, Gr: %d, Gb: %d, B: %d", BlackCorr.BlackR, BlackCorr.BlackGr, BlackCorr.BlackGb, BlackCorr.BlackB);
        }
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "hack_flag") == 0) {
        extern UINT8 hack_vhdr_full_copy;
        hack_vhdr_full_copy = atoi(Argv[3]);
        AmbaPrint("%s", hack_vhdr_full_copy ? "always do full copy":"do run time update");
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "bypass") == 0) {
        AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s VignetteCalcInfo;
        AMBA_DSP_IMG_SBP_CORRECTION_s Sbp;
        AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s CfaLeakage;
        AMBA_DSP_IMG_ANTI_ALIASING_s AntiAliasing;
        AMBA_DSP_IMG_CAWARP_CALC_INFO_s Cawarp;
        AmbaPrint("Disable Vignette");
        AmbaDSP_ImgGetVignetteCompensation(&Mode, &VignetteCalcInfo);
        VignetteCalcInfo.Enb = 0;
        AmbaDSP_ImgCalcVignetteCompensation(&Mode, &VignetteCalcInfo);
        AmbaDSP_ImgSetVignetteCompensation(&Mode);
        AmbaPrint("Disable FPN");
        AmbaDSP_ImgGetStaticBadPixelCorrection(&Mode, &Sbp);
        Sbp.Enb = 0;
        AmbaDSP_ImgSetStaticBadPixelCorrection(&Mode, &Sbp);
        AmbaPrint("Disable Cfa Leakage filter");
        AmbaDSP_ImgGetCfaLeakageFilter(&Mode, &CfaLeakage);
        CfaLeakage.Enb = 0;
        AmbaDSP_ImgSetCfaLeakageFilter(&Mode, &CfaLeakage);
        AmbaPrint("Disable Antialiasing");
        AmbaDSP_ImgGetAntiAliasing(&Mode, &AntiAliasing);
        AntiAliasing.Enb = 0;
        AmbaDSP_ImgSetAntiAliasing(&Mode, &AntiAliasing);
        AmbaPrint("Disable Cawarp");
        AmbaDSP_ImgGetCawarpCompensation(&Mode, &Cawarp);
        Cawarp.CaWarpEnb = 0;
        AmbaDSP_ImgCalcCawarpCompensation(&Mode, &Cawarp);
        AmbaDSP_ImgSetCawarpCompensation(&Mode);
        Rval = AMP_OK;
    } else if (strcmp(Argv[2], "rawdump") == 0) {
        if (HDRDumpRaw.DumpStackMem.pMemBase != NULL) {
            AmbaKAL_TaskDelete(&HDRDumpRaw.DumpTask);
            AmbaKAL_MemFree(&HDRDumpRaw.DumpStackMem);
            memset(&HDRDumpRaw, 0x0, sizeof(HDRDumpRaw));
        }
        AmbaKAL_MemAllocate(APPLIB_G_MMPL, &HDRDumpRaw.DumpStackMem, HDR_DUMP_TASK_STACK_SIZE, 32);
        if (HDRDumpRaw.DumpStackMem.pMemAlignedBase == NULL) {
            AppLib_WARF("call AmbaKAL_MemAllocate() Fail");
            Rval = AMP_ERROR_GENERAL_ERROR;
        } else {
            memset(HDRDumpRaw.DumpStackMem.pMemAlignedBase, 0, HDR_DUMP_TASK_STACK_SIZE);
            if (OK != AmbaKAL_TaskCreate(&HDRDumpRaw.DumpTask,
                                           "HDRBlendingRawDump",
                                           160,
                                           (void*)AppLibTune_DumpHdrBlendingRaw/*FIXME*/,
                                           (UINT32)Argv[3],/*FIXME*/
                                           HDRDumpRaw.DumpStackMem.pMemAlignedBase,
                                           HDR_DUMP_TASK_STACK_SIZE,
                                           AMBA_KAL_AUTO_START)) {
                AppLib_WARF("call AmbaKAL_TaskCreate() Fail");
                Rval = AMP_ERROR_GENERAL_ERROR;
            } else {
                Rval = AMP_OK;
            }
        }
    } else if (strcmp(Argv[2], "atbl") == 0) {
        UINT8 blendingLevel;
        extern int AmbaDSP_ImgSetHdrAlphaCalcConfig(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s *pHdrAlphaConfig);
        extern int AmbaDSP_ImgGetHdrAlphaCalcConfig(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s *pHdrAlphaConfig);
        extern int AmbaDSP_ImgSetHdrAlphaCalcAlpha(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s *pHdrAlphaAlpha);
        extern int AmbaDSP_ImgGetHdrAlphaCalcAlpha(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s *pHdrAlphaAlpha);
        AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s hdr_alpha_calc_cfg;
        AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s hdr_alpha_calc_alpha;
        static UINT8 alphaTable[128];
        AmbaDSP_ImgGetHdrAlphaCalcConfig(&Mode, &hdr_alpha_calc_cfg);
        if(strcmp(Argv[3], "val") == 0){
            UINT8 val;
            val =(UINT8) atoi(Argv[4]);
            Rval = AMP_OK;
            memset(&alphaTable[0], val, 128);
            hdr_alpha_calc_cfg.BlendControl = -1;
            AmbaDSP_ImgSetHdrAlphaCalcConfig(&Mode, &hdr_alpha_calc_cfg);
            hdr_alpha_calc_alpha.AlphaTableAddr = (UINT32) &alphaTable[0];
            hdr_alpha_calc_alpha.PreAlphaMode = 0;
            hdr_alpha_calc_alpha.SaturationNumNei = 2;
            AmbaDSP_ImgSetHdrAlphaCalcAlpha(&Mode, &hdr_alpha_calc_alpha);

        }else if(Argc < 4){

            AmbaPrint("Blending Control = %d",hdr_alpha_calc_cfg.BlendControl);
            AmbaPrint("AvgMethod = %d",hdr_alpha_calc_cfg.AvgMethod);
            AmbaPrint("AvgRadius = %d",hdr_alpha_calc_cfg.AvgRadius);
            AmbaPrint("LumaAvgWeightB = %d",hdr_alpha_calc_cfg.LumaAvgWeightB);
            AmbaPrint("LumaAvgWeightGb= %d",hdr_alpha_calc_cfg.LumaAvgWeightGb);
            AmbaPrint("LumaAvgWeightGr = %d",hdr_alpha_calc_cfg.LumaAvgWeightGr);
            AmbaPrint("LumaAvgWeightR = %d",hdr_alpha_calc_cfg.LumaAvgWeightR);

            if(hdr_alpha_calc_cfg.BlendControl == (-1)){
                int x;
                UINT8 *AlphaTable;
                AmbaDSP_ImgGetHdrAlphaCalcAlpha(&Mode,&hdr_alpha_calc_alpha);
                AlphaTable = (UINT8*)hdr_alpha_calc_alpha.AlphaTableAddr;
                AmbaPrint("Alpha Table");

                for (x = 0; x < 8; x++) {
                    AmbaPrint("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                              AlphaTable[x * 16 + 0],
                              AlphaTable[x * 16 + 1],
                              AlphaTable[x * 16 + 2],
                              AlphaTable[x * 16 + 3],
                              AlphaTable[x * 16 + 4],
                              AlphaTable[x * 16 + 5],
                              AlphaTable[x * 16 + 6],
                              AlphaTable[x * 16 + 7],
                              AlphaTable[x * 16 + 8],
                              AlphaTable[x * 16 + 9],
                              AlphaTable[x * 16 + 10],
                              AlphaTable[x * 16 + 11],
                              AlphaTable[x * 16 + 12],
                              AlphaTable[x * 16 + 13],
                              AlphaTable[x * 16 + 14],
                              AlphaTable[x * 16 + 15]);
                }
            }

        }else{
            blendingLevel = atoi(Argv[3]);
            Rval = AMP_OK;
            if (blendingLevel < 8) {
                hdr_alpha_calc_cfg.BlendControl = blendingLevel;
                AmbaDSP_ImgSetHdrAlphaCalcConfig(&Mode, &hdr_alpha_calc_cfg);
            } else if (blendingLevel == 8) {
                memset(&alphaTable[0], 0x0, 128);
                hdr_alpha_calc_cfg.BlendControl = -1;
                AmbaDSP_ImgSetHdrAlphaCalcConfig(&Mode, &hdr_alpha_calc_cfg);

                //AmbaDSP_ImgGetHdrAlphaCalcAlpha(&Mode,&hdr_alpha_calc_alpha);
                hdr_alpha_calc_alpha.AlphaTableAddr = (UINT32) &alphaTable[0];
                hdr_alpha_calc_alpha.PreAlphaMode = 0;
                hdr_alpha_calc_alpha.SaturationNumNei = 2;
                AmbaDSP_ImgSetHdrAlphaCalcAlpha(&Mode, &hdr_alpha_calc_alpha);

            } else if (blendingLevel == 9) {
                memset(&alphaTable[0], 15, 128);
                hdr_alpha_calc_cfg.BlendControl = -1;
                AmbaDSP_ImgSetHdrAlphaCalcConfig(&Mode, &hdr_alpha_calc_cfg);

                hdr_alpha_calc_alpha.AlphaTableAddr = (UINT32) &alphaTable[0];
                hdr_alpha_calc_alpha.PreAlphaMode = 0;
                hdr_alpha_calc_alpha.SaturationNumNei = 2;
                AmbaDSP_ImgSetHdrAlphaCalcAlpha(&Mode, &hdr_alpha_calc_alpha);

            } else {
                AmbaPrint("Error! blendingLevel %d",blendingLevel);
                Rval = AMP_ERROR_GENERAL_ERROR;
            }
        }
    } else {
        Rval = AMP_ERROR_GENERAL_ERROR;
    }

    if (Rval != AMP_OK) {
        AmbaPrint("\n\r"
                  "Usage: t %s -vhdr dump [CfgId]: dump hdr config\n\r"
                  "       t %s -vhdr amp_linear_hdr: get hdr amp linear hdr setting\n\r"
                  "       t %s -vhdr amp_linear_hdr [AmpLinearBlcR] [Gr] [Gb] [B] [AmpHdrBlcR] [Gr] [Gb] [B] [HdrBlendBlcR] [Gr] [Gb] [B]\n\r"
                  "       t %s -vhdr hdr_le_blc: get hdr_long exposure black level setting\n\r"
                  "       t %s -vhdr hdr_le_blc [R] [Gr] [Gb] [B]\n\r"
                  "       t %s -vhdr hdr_se_blc: get hdr short exposure black level setting\n\r"
                  "       t %s -vhdr hdr_se_blc [R] [Gr] [Gb] [B]\n\r"
                  "       t %s -vhdr hdr_alpha_calc_cfg: get hdr_alpha_calc_config setting\n\r"
                  "       t %s -vhdr hdr_alpha_calc_cfg [AvgRadius] [AvgMethod] [BlendControl] [LumaAvgWeightR] [Gr] [Gb] [B]\n\r"
                  "       t %s -vhdr hdr_alpha_calc_th: get hdr_alpha_calc_thresh setting\n\r"
                  "       t %s -vhdr hdr_alpha_calc_th [SaturationThresholdR] [Gr] [Gb] [B]\n\r"
                  "       t %s -vhdr hdr_alpha_calc_blc: get hdr_alpha_calc_black setting\n\r"
                  "       t %s -vhdr hdr_alpha_calc_blc [R] [Gr] [Gb] B\n\r"
                  "       t %s -vhdr bypass: bypass vig, fpn, cfa_leakage_filter, antialiasing, cawarp\n\r"
                  "       t %s -vhdr rawdump [prefix]: dump blending raw file\n\r",
                  Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_Profile(int Argc, char** Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    //static void *profile_buffer = NULL;
    static AMBA_MEM_CTRL_s Mem = {NULL, NULL};
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "init") == 0) {
            extern int AmbaDSP_ImgProfilePoolInit(void* Buf, UINT32 Size);
            const UINT32 profile_buffer_size = 9 * 1024;
            if (Mem.pMemAlignedBase == NULL) {
                AmbaKAL_MemAllocate(APPLIB_G_MMPL, &Mem, profile_buffer_size, 32);
                if (Mem.pMemAlignedBase == NULL ) {
                    AppLib_ERRF("call AmbaKAL_MemAllocate() Fail");
                }
            }
            Rval = (AMP_ER_CODE_e)AmbaDSP_ImgProfilePoolInit(Mem.pMemAlignedBase, profile_buffer_size);
        } else if (strcmp(Argv[2], "uninit") == 0) {
            extern void AmbaDSP_ImgProfilePoolUnint(void);
            AmbaDSP_ImgProfilePoolUnint();
            AmbaKAL_MemFree(&Mem);
            Mem.pMemAlignedBase = NULL;
            Mem.pMemBase = NULL;
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "hist") == 0) {
            extern int AmbaDSP_ImgProfileDumpHistory(UINT32 History_Number);
            int History_Number = atoi(Argv[3]);
            Rval = (AMP_ER_CODE_e)AmbaDSP_ImgProfileDumpHistory(History_Number);
        }
    }

    if (Rval != AMP_OK) {
        AmbaPrint("\n\r"
                  "Usage: t %s -profile init: Init IK profile mode\n\r"
                  "       t %s -profile uninit: Uninit IK profile mode\n\r"
                  "       t %s -profile hist: List IK profile results",
                  Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_ShowCmds(int Argc, char **Argv)
{
    typedef struct AMBA_DSP_IMG_CTX_FILTERS_LOG_s_ {
        UINT32 Group[6];
    } AMBA_DSP_IMG_CTX_FILTERS_LOG_s;
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_CTX_FILTERS_LOG_s ImageKernelLogCtrl = { 0 };
    extern int AmbaDSP_ImgSetImageKernelControl(void *pLogCtrl);
    if (strcmp(Argv[2], "on") == 0) {
        AmbaPrint("Enable All of IK Log");
        memset(&ImageKernelLogCtrl, 0xF, sizeof(AMBA_DSP_IMG_CTX_FILTERS_LOG_s));
        Rval = (AMP_ER_CODE_e)AmbaDSP_ImgSetImageKernelControl((void*)&ImageKernelLogCtrl);
    } else if (strcmp(Argv[2], "off") == 0) {
        AmbaPrint("Disable All of IK Log");
        memset(&ImageKernelLogCtrl, 0x0, sizeof(AMBA_DSP_IMG_CTX_FILTERS_LOG_s));
        Rval = (AMP_ER_CODE_e)AmbaDSP_ImgSetImageKernelControl((void*)&ImageKernelLogCtrl);
    } else {
        if (Argc == 4) {
            extern void AmbaDSP_ImgSetImageKernelControlById(UINT32 Id, UINT8 Setting);
            UINT32 Id = (UINT32) atoi(Argv[2]);
            UINT8 Setting = (UINT8) atoi(Argv[3]);
            AmbaDSP_ImgSetImageKernelControlById(Id, Setting);
            Rval = AMP_OK;
        }
    }
    if (Rval != AMP_OK) {
        AmbaPrint("\n\r"
                  "Usage: t %s -showcmd [on|off]: turn on|off all of IK filter debug msg\n\r"
                  "       t %s -showcmd [id] [on|off]: turn on|off specified filter debug msg\n\r"
                  "                                    id:0 can list all of cmd id",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

#if (DEBUG_CALIB == 1)
#define MAX_CALIB_WARP_HOR_GRID_NUM (64)
#define MAX_CALIB_WARP_VER_GRID_NUM (64)
static AMBA_DSP_IMG_GRID_POINT_s CalibWarpTbl[MAX_CALIB_WARP_HOR_GRID_NUM*MAX_CALIB_WARP_VER_GRID_NUM];
static const INT16 INT16CalibWarpTblHor[MAX_CALIB_WARP_HOR_GRID_NUM*MAX_CALIB_WARP_VER_GRID_NUM] =
{
    578,  539,  500,  462,  423,  385,  346,  308,  269,  231,  192,  154,  115,   77,   38,    0,  -38,  -77, -115, -154, -192, -231, -269, -308, -346, -385, -423, -462, -500, -539, -578,
    512,  477,  443,  409,  375,  341,  307,  273,  238,  204,  170,  136,  102,   68,   34,    0,  -34,  -68, -102, -136, -170, -204, -238, -273, -307, -341, -375, -409, -443, -477, -512,
    450,  420,  390,  360,  330,  300,  270,  240,  210,  180,  150,  120,   90,   60,   30,    0,  -30,  -60,  -90, -120, -150, -180, -210, -240, -270, -300, -330, -360, -390, -420, -450,
    392,  365,  339,  313,  287,  261,  235,  209,  182,  156,  130,  104,   78,   52,   26,    0,  -26,  -52,  -78, -104, -130, -156, -182, -209, -235, -261, -287, -313, -339, -365, -392,
    338,  315,  292,  270,  247,  225,  202,  180,  157,  135,  112,   90,   67,   45,   22,    0,  -22,  -45,  -67,  -90, -112, -135, -157, -180, -202, -225, -247, -270, -292, -315, -338,
    288,  268,  249,  230,  211,  192,  172,  153,  134,  115,   96,   76,   57,   38,   19,    0,  -19,  -38,  -57,  -76,  -96, -115, -134, -153, -172, -192, -211, -230, -249, -268, -288,
    242,  225,  209,  193,  177,  161,  145,  129,  112,   96,   80,   64,   48,   32,   16,    0,  -16,  -32,  -48,  -64,  -80,  -96, -112, -129, -145, -161, -177, -193, -209, -225, -242,
    200,  186,  173,  160,  146,  133,  120,  106,   93,   80,   66,   53,   40,   26,   13,    0,  -13,  -26,  -40,  -53,  -66,  -80,  -93, -106, -120, -133, -146, -160, -173, -186, -200,
    162,  151,  140,  129,  118,  108,   97,   86,   75,   64,   54,   43,   32,   21,   10,    0,  -10,  -21,  -32,  -43,  -54,  -64,  -75,  -86,  -97, -108, -118, -129, -140, -151, -162,
    128,  119,  110,  102,   93,   85,   76,   68,   59,   51,   42,   34,   25,   17,    8,    0,   -8,  -17,  -25,  -34,  -42,  -51,  -59,  -68,  -76,  -85,  -93, -102, -110, -119, -128,
    98,   91,   84,   78,   71,   65,   58,   52,   45,   39,   32,   26,   19,   13,    6,    0,   -6,  -13,  -19,  -26,  -32,  -39,  -45,  -52,  -58,  -65,  -71,  -78,  -84,  -91,  -98,
    72,   67,   62,   57,   52,   48,   43,   38,   33,   28,   24,   19,   14,    9,    4,    0,   -4,   -9,  -14,  -19,  -24,  -28,  -33,  -38,  -43,  -48,  -52,  -57,  -62,  -67,  -72,
    50,   46,   43,   40,   36,   33,   30,   26,   23,   20,   16,   13,   10,    6,    3,    0,   -3,   -6,  -10,  -13,  -16,  -20,  -23,  -26,  -30,  -33,  -36,  -40,  -43,  -46,  -50,
    32,   29,   27,   25,   23,   21,   19,   17,   14,   12,   10,    8,    6,    4,    2,    0,   -2,   -4,   -6,   -8,  -10,  -12,  -14,  -17,  -19,  -21,  -23,  -25,  -27,  -29,  -32,
    18,   16,   15,   14,   13,   12,   10,    9,    8,    7,    6,    4,    3,    2,    1,    0,   -1,   -2,   -3,   -4,   -6,   -7,   -8,   -9,  -10,  -12,  -13,  -14,  -15,  -16,  -18,
    8,    7,    6,    6,    5,    5,    4,    4,    3,    3,    2,    2,    1,    1,    0,    0,    0,   -1,   -1,   -2,   -2,   -3,   -3,   -4,   -4,   -5,   -5,   -6,   -6,   -7,   -8,
    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -2,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -2,
    8,    7,    6,    6,    5,    5,    4,    4,    3,    3,    2,    2,    1,    1,    0,    0,    0,   -1,   -1,   -2,   -2,   -3,   -3,   -4,   -4,   -5,   -5,   -6,   -6,   -7,   -8,
    18,   16,   15,   14,   13,   12,   10,    9,    8,    7,    6,    4,    3,    2,    1,    0,   -1,   -2,   -3,   -4,   -6,   -7,   -8,   -9,  -10,  -12,  -13,  -14,  -15,  -16,  -18,
    32,   29,   27,   25,   23,   21,   19,   17,   14,   12,   10,    8,    6,    4,    2,    0,   -2,   -4,   -6,   -8,  -10,  -12,  -14,  -17,  -19,  -21,  -23,  -25,  -27,  -29,  -32,
    50,   46,   43,   40,   36,   33,   30,   26,   23,   20,   16,   13,   10,    6,    3,    0,   -3,   -6,  -10,  -13,  -16,  -20,  -23,  -26,  -30,  -33,  -36,  -40,  -43,  -46,  -50,
    72,   67,   62,   57,   52,   48,   43,   38,   33,   28,   24,   19,   14,    9,    4,    0,   -4,   -9,  -14,  -19,  -24,  -28,  -33,  -38,  -43,  -48,  -52,  -57,  -62,  -67,  -72,
    98,   91,   84,   78,   71,   65,   58,   52,   45,   39,   32,   26,   19,   13,    6,    0,   -6,  -13,  -19,  -26,  -32,  -39,  -45,  -52,  -58,  -65,  -71,  -78,  -84,  -91,  -98,
    128,  119,  110,  102,   93,   85,   76,   68,   59,   51,   42,   34,   25,   17,    8,    0,   -8,  -17,  -25,  -34,  -42,  -51,  -59,  -68,  -76,  -85,  -93, -102, -110, -119, -128,
    162,  151,  140,  129,  118,  108,   97,   86,   75,   64,   54,   43,   32,   21,   10,    0,  -10,  -21,  -32,  -43,  -54,  -64,  -75,  -86,  -97, -108, -118, -129, -140, -151, -162,
    200,  186,  173,  160,  146,  133,  120,  106,   93,   80,   66,   53,   40,   26,   13,    0,  -13,  -26,  -40,  -53,  -66,  -80,  -93, -106, -120, -133, -146, -160, -173, -186, -200,
    242,  225,  209,  193,  177,  161,  145,  129,  112,   96,   80,   64,   48,   32,   16,    0,  -16,  -32,  -48,  -64,  -80,  -96, -112, -129, -145, -161, -177, -193, -209, -225, -242,
    288,  268,  249,  230,  211,  192,  172,  153,  134,  115,   96,   76,   57,   38,   19,    0,  -19,  -38,  -57,  -76,  -96, -115, -134, -153, -172, -192, -211, -230, -249, -268, -288,
    338,  315,  292,  270,  247,  225,  202,  180,  157,  135,  112,   90,   67,   45,   22,    0,  -22,  -45,  -67,  -90, -112, -135, -157, -180, -202, -225, -247, -270, -292, -315, -338,
    392,  365,  339,  313,  287,  261,  235,  209,  182,  156,  130,  104,   78,   52,   26,    0,  -26,  -52,  -78, -104, -130, -156, -182, -209, -235, -261, -287, -313, -339, -365, -392,
    450,  420,  390,  360,  330,  300,  270,  240,  210,  180,  150,  120,   90,   60,   30,    0,  -30,  -60,  -90, -120, -150, -180, -210, -240, -270, -300, -330, -360, -390, -420, -450,
    512,  477,  443,  409,  375,  341,  307,  273,  238,  204,  170,  136,  102,   68,   34,    0,  -34,  -68, -102, -136, -170, -204, -238, -273, -307, -341, -375, -409, -443, -477, -512,
    578,  539,  500,  462,  423,  385,  346,  308,  269,  231,  192,  154,  115,   77,   38,    0,  -38,  -77, -115, -154, -192, -231, -269, -308, -346, -385, -423, -462, -500, -539, -578,
};
static const INT16 INT16CalibWarpTblVer[MAX_CALIB_WARP_HOR_GRID_NUM*MAX_CALIB_WARP_VER_GRID_NUM] =
{
   382,  333,  287,  244,  205,  170,  137,  108,   83,   61,   42,   27,   15,    6,    1,    0,    1,    6,   15,   27,   42,   61,   83,  108,  137,  170,  205,  244,  287,  333,  382,
   360,  313,  270,  230,  193,  160,  129,  102,   78,   57,   40,   25,   14,    6,    1,    0,    1,    6,   14,   25,   40,   57,   78,  102,  129,  160,  193,  230,  270,  313,  360,
   337,  294,  253,  216,  181,  150,  121,   96,   73,   54,   37,   24,   13,    6,    1,    0,    1,    6,   13,   24,   37,   54,   73,   96,  121,  150,  181,  216,  253,  294,  337,
   315,  274,  236,  201,  169,  140,  113,   89,   68,   50,   35,   22,   12,    5,    1,    0,    1,    5,   12,   22,   35,   50,   68,   89,  113,  140,  169,  201,  236,  274,  315,
   292,  254,  219,  187,  157,  130,  105,   83,   63,   46,   32,   20,   11,    5,    1,    0,    1,    5,   11,   20,   32,   46,   63,   83,  105,  130,  157,  187,  219,  254,  292,
   270,  235,  202,  172,  145,  120,   97,   76,   58,   43,   30,   19,   10,    4,    1,    0,    1,    4,   10,   19,   30,   43,   58,   76,   97,  120,  145,  172,  202,  235,  270,
   247,  215,  185,  158,  133,  110,   89,   70,   53,   39,   27,   17,    9,    4,    1,    0,    1,    4,    9,   17,   27,   39,   53,   70,   89,  110,  133,  158,  185,  215,  247,
   225,  196,  169,  144,  121,  100,   81,   64,   49,   36,   25,   16,    9,    4,    1,    0,    1,    4,    9,   16,   25,   36,   49,   64,   81,  100,  121,  144,  169,  196,  225,
   202,  176,  152,  129,  108,   90,   72,   57,   44,   32,   22,   14,    8,    3,    0,    0,    0,    3,    8,   14,   22,   32,   44,   57,   72,   90,  108,  129,  152,  176,  202,
   180,  156,  135,  115,   96,   80,   64,   51,   39,   28,   20,   12,    7,    3,    0,    0,    0,    3,    7,   12,   20,   28,   39,   51,   64,   80,   96,  115,  135,  156,  180,
   157,  137,  118,  100,   84,   70,   56,   44,   34,   25,   17,   11,    6,    2,    0,    0,    0,    2,    6,   11,   17,   25,   34,   44,   56,   70,   84,  100,  118,  137,  157,
   135,  117,  101,   86,   72,   60,   48,   38,   29,   21,   15,    9,    5,    2,    0,    0,    0,    2,    5,    9,   15,   21,   29,   38,   48,   60,   72,   86,  101,  117,  135,
   112,   98,   84,   72,   60,   50,   40,   32,   24,   18,   12,    8,    4,    2,    0,    0,    0,    2,    4,    8,   12,   18,   24,   32,   40,   50,   60,   72,   84,   98,  112,
    90,   78,   67,   57,   48,   40,   32,   25,   19,   14,   10,    6,    3,    1,    0,    0,    0,    1,    3,    6,   10,   14,   19,   25,   32,   40,   48,   57,   67,   78,   90,
    67,   58,   50,   43,   36,   30,   24,   19,   14,   10,    7,    4,    2,    1,    0,    0,    0,    1,    2,    4,    7,   10,   14,   19,   24,   30,   36,   43,   50,   58,   67,
    45,   39,   33,   28,   24,   20,   16,   12,    9,    7,    5,    3,    1,    0,    0,    0,    0,    0,    1,    3,    5,    7,    9,   12,   16,   20,   24,   28,   33,   39,   45,
    22,   19,   16,   14,   12,   10,    8,    6,    4,    3,    2,    1,    0,    0,    0,    0,    0,    0,    0,    1,    2,    3,    4,    6,    8,   10,   12,   14,   16,   19,   22,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   -22,  -19,  -16,  -14,  -12,  -10,   -8,   -6,   -4,   -3,   -2,   -1,    0,    0,    0,    0,    0,    0,    0,   -1,   -2,   -3,   -4,   -6,   -8,  -10,  -12,  -14,  -16,  -19,  -22,
   -45,  -39,  -33,  -28,  -24,  -20,  -16,  -12,   -9,   -7,   -5,   -3,   -1,    0,    0,    0,    0,    0,   -1,   -3,   -5,   -7,   -9,  -12,  -16,  -20,  -24,  -28,  -33,  -39,  -45,
   -67,  -58,  -50,  -43,  -36,  -30,  -24,  -19,  -14,  -10,   -7,   -4,   -2,   -1,    0,    0,    0,   -1,   -2,   -4,   -7,  -10,  -14,  -19,  -24,  -30,  -36,  -43,  -50,  -58,  -67,
   -90,  -78,  -67,  -57,  -48,  -40,  -32,  -25,  -19,  -14,  -10,   -6,   -3,   -1,    0,    0,    0,   -1,   -3,   -6,  -10,  -14,  -19,  -25,  -32,  -40,  -48,  -57,  -67,  -78,  -90,
  -112,  -98,  -84,  -72,  -60,  -50,  -40,  -32,  -24,  -18,  -12,   -8,   -4,   -2,    0,    0,    0,   -2,   -4,   -8,  -12,  -18,  -24,  -32,  -40,  -50,  -60,  -72,  -84,  -98, -112,
  -135, -117, -101,  -86,  -72,  -60,  -48,  -38,  -29,  -21,  -15,   -9,   -5,   -2,    0,    0,    0,   -2,   -5,   -9,  -15,  -21,  -29,  -38,  -48,  -60,  -72,  -86, -101, -117, -135,
  -157, -137, -118, -100,  -84,  -70,  -56,  -44,  -34,  -25,  -17,  -11,   -6,   -2,    0,    0,    0,   -2,   -6,  -11,  -17,  -25,  -34,  -44,  -56,  -70,  -84, -100, -118, -137, -157,
  -180, -156, -135, -115,  -96,  -80,  -64,  -51,  -39,  -28,  -20,  -12,   -7,   -3,    0,    0,    0,   -3,   -7,  -12,  -20,  -28,  -39,  -51,  -64,  -80,  -96, -115, -135, -156, -180,
  -202, -176, -152, -129, -108,  -90,  -72,  -57,  -44,  -32,  -22,  -14,   -8,   -3,    0,    0,    0,   -3,   -8,  -14,  -22,  -32,  -44,  -57,  -72,  -90, -108, -129, -152, -176, -202,
  -225, -196, -169, -144, -121, -100,  -81,  -64,  -49,  -36,  -25,  -16,   -9,   -4,   -1,    0,   -1,   -4,   -9,  -16,  -25,  -36,  -49,  -64,  -81, -100, -121, -144, -169, -196, -225,
  -247, -215, -185, -158, -133, -110,  -89,  -70,  -53,  -39,  -27,  -17,   -9,   -4,   -1,    0,   -1,   -4,   -9,  -17,  -27,  -39,  -53,  -70,  -89, -110, -133, -158, -185, -215, -247,
  -270, -235, -202, -172, -145, -120,  -97,  -76,  -58,  -43,  -30,  -19,  -10,   -4,   -1,    0,   -1,   -4,  -10,  -19,  -30,  -43,  -58,  -76,  -97, -120, -145, -172, -202, -235, -270,
  -292, -254, -219, -187, -157, -130, -105,  -83,  -63,  -46,  -32,  -20,  -11,   -5,   -1,    0,   -1,   -5,  -11,  -20,  -32,  -46,  -63,  -83, -105, -130, -157, -187, -219, -254, -292,
  -315, -274, -236, -201, -169, -140, -113,  -89,  -68,  -50,  -35,  -22,  -12,   -5,   -1,    0,   -1,   -5,  -12,  -22,  -35,  -50,  -68,  -89, -113, -140, -169, -201, -236, -274, -315,
  -337, -294, -253, -216, -181, -150, -121,  -96,  -73,  -54,  -37,  -24,  -13,   -6,   -1,    0,   -1,   -6,  -13,  -24,  -37,  -54,  -73,  -96, -121, -150, -181, -216, -253, -294, -337,
  -360, -313, -270, -230, -193, -160, -129, -102,  -78,  -57,  -40,  -25,  -14,   -6,   -1,    0,   -1,   -6,  -14,  -25,  -40,  -57,  -78, -102, -129, -160, -193, -230, -270, -313, -360,
  -382, -333, -287, -244, -205, -170, -137, -108,  -83,  -61,  -42,  -27,  -15,   -6,   -1,    0,   -1,   -6,  -15,  -27,  -42,  -61,  -83, -108, -137, -170, -205, -244, -287, -333, -382,
};
static int gNonStitching = 0;
#define VCAPWIDTH 1920
#define VCAPHEIGHT 1080
#define MAINWIDTH 1920
#define MAINHEIGHT 1080
static AMP_ER_CODE_e AppLibTune_Warp(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_WARP_CALC_INFO_s CalcWarp = { 0 };
    int Test;
    UINT16 VCapWidth, VCapHeight, MainWidth, MainHeight;
    AMBA_DSP_IMG_MODE_CFG_s Mode = {0};
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
    VCapWidth = VCAPWIDTH;
    VCapHeight = VCAPHEIGHT;
    MainWidth = MAINWIDTH;
    MainHeight = MAINHEIGHT;
    AmbaPrint("Warp size info VCapWidth= %d VCapHeight=%d MainWidth=%d MainHeight=%d", VCapWidth, VCapHeight, MainWidth, MainHeight);

    AmbaDSP_ImgGetWarpCompensation(&Mode, &CalcWarp);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current warp info: TBD");
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "wininfo") == 0) {
            AMBA_DSP_IMG_MODE_CFG_s ImgMode;
            AMBA_DSP_IMG_WARP_CALC_INFO_s WarpInfo;
            AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s WarpBypassInfo;

            extern int AmbaDSP_ImgGetWarpCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s *pWarpDzoomCorrByPass);

            memset(&WarpBypassInfo, 0, sizeof(AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s));
            memset(&WarpInfo, 0, sizeof(AMBA_DSP_IMG_WARP_CALC_INFO_s));

            memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
            ImgMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
            ImgMode.BatchId = AMBA_DSP_VIDEO_FILTER;
            AmbaDSP_ImgGetWarpCompensation(&ImgMode, &WarpInfo);
            AmbaDSP_ImgGetWarpCompensationByPass(&ImgMode, &WarpBypassInfo);

            AmbaPrint("VIN Win Dimension\n");
            AmbaPrint("===================\n");
            AmbaPrint("  StartX    = %d\n", WarpInfo.VinSensorGeo.StartX);
            AmbaPrint("  StartY    = %d\n", WarpInfo.VinSensorGeo.StartY);
            AmbaPrint("  Width     = %d\n", WarpInfo.VinSensorGeo.Width);
            AmbaPrint("  Height    = %d\n\n", WarpInfo.VinSensorGeo.Height);

            AmbaPrint("CFA Win Dimension\n");
            AmbaPrint("===================\n");
            AmbaPrint("  Width     = %d\n", WarpBypassInfo.CfaOutputWidth);
            AmbaPrint("  Height    = %d\n\n", WarpBypassInfo.CfaOutputHeight);

            AmbaPrint("Dummy Win Dimension\n");
            AmbaPrint("===================\n");
            AmbaPrint("  StartX    = %d\n", WarpBypassInfo.DummyWindowXLeft);
            AmbaPrint("  StartY    = %d\n", WarpBypassInfo.DummyWindowYTop);
            AmbaPrint("  Width     = %d\n", WarpBypassInfo.DummyWindowWidth);
            AmbaPrint("  Height    = %d\n\n", WarpBypassInfo.DummyWindowHeight);

            AmbaPrint("Active Win Dimension\n");
            AmbaPrint("===================\n");
            AmbaPrint("  LeftTopX  = %f\n", (WarpBypassInfo.ActualLeftTopX)/65536.0);
            AmbaPrint("  LeftTopY  = %f\n", (WarpBypassInfo.ActualLeftTopY)/65536.0);
            AmbaPrint("  RightBotX = %f\n", (WarpBypassInfo.ActualRightBotX)/65536.0);
            AmbaPrint("  RightBotY = %f\n\n", (WarpBypassInfo.ActualRightBotY)/65536.0);

            AmbaPrint("Main Win Dimension\n");
            AmbaPrint("===================\n");
            AmbaPrint("  Width     = %d\n", WarpInfo.MainWinDim.Width);
            AmbaPrint("  Height    = %d\n", WarpInfo.MainWinDim.Height);

            Rval = AMP_OK;
        }else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugWarpCompensation(UINT8 TmpWarpDebugMessageFlag);
            UINT8 WarpDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugWarpCompensation(WarpDebugFlag);
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "vert") == 0) {
            AMBA_DSP_IMG_WARP_FLIP_INFO_s WarpFlipInfo;
            UINT32 VertWarpFlipEnb = atoi(Argv[3]);
            WarpFlipInfo.VerticalEnable = VertWarpFlipEnb;
            AmbaDSP_WarpCore_SetWarpFlipEnb(&Mode, &WarpFlipInfo);
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "windowinfo") == 0) {
            AMBA_DSP_IMG_WARP_WIN_INFO_s WarpWinInfo;
            AmbaDSP_WarpCore_GetWindowInfo(&Mode, &WarpWinInfo);
            AmbaPrint("WarpWinInfo.DzoomInfo.ZoomX      = %d", WarpWinInfo.DzoomInfo.ZoomX);
            AmbaPrint("WarpWinInfo.DzoomInfo.ZoomY      = %d", WarpWinInfo.DzoomInfo.ZoomY);
            AmbaPrint("WarpWinInfo.DzoomInfo.ShiftX     = %d", WarpWinInfo.DzoomInfo.ShiftX);
            AmbaPrint("WarpWinInfo.DzoomInfo.ShiftY     = %d", WarpWinInfo.DzoomInfo.ShiftY);
            AmbaPrint("WarpWinInfo.ActWinCrop.LeftTopX  = %d", WarpWinInfo.ActWinCrop.LeftTopX);
            AmbaPrint("WarpWinInfo.ActWinCrop.LeftTopY  = %d", WarpWinInfo.ActWinCrop.LeftTopY);
            AmbaPrint("WarpWinInfo.ActWinCrop.RightBotX = %d", WarpWinInfo.ActWinCrop.RightBotX);
            AmbaPrint("WarpWinInfo.ActWinCrop.RightBotY = %d", WarpWinInfo.ActWinCrop.RightBotY);
            AmbaPrint("WarpWinInfo.DmyWinGeo.StartX     = %d", WarpWinInfo.DmyWinGeo.StartX);
            AmbaPrint("WarpWinInfo.DmyWinGeo.StartY     = %d", WarpWinInfo.DmyWinGeo.StartY);
            AmbaPrint("WarpWinInfo.DmyWinGeo.Width      = %d", WarpWinInfo.DmyWinGeo.Width);
            AmbaPrint("WarpWinInfo.DmyWinGeo.Height     = %d", WarpWinInfo.DmyWinGeo.Height);
            AmbaPrint("WarpWinInfo.CfaWinDim.Width      = %d", WarpWinInfo.CfaWinDim.Width);
            AmbaPrint("WarpWinInfo.CfaWinDim.Height     = %d", WarpWinInfo.CfaWinDim.Height);
            AmbaPrint("WarpWinInfo.MainWinDim.Width     = %d", WarpWinInfo.MainWinDim.Width);
            AmbaPrint("WarpWinInfo.MainWinDim.Height    = %d", WarpWinInfo.MainWinDim.Height);

            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "warpcoretrasnfer") == 0 || (strcmp(Argv[2], "wct") == 0)) {
            AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
            AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
            AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange;
            AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = { 0 };
            AMBA_DSP_IMG_EIS_INFO_s TmpEISInfo = { 0 };
            AMBA_DSP_IMG_WARP_CALC_WIN_INFO_s TmpWindowInfo;

            TmpVinSensorGeo.StartX = atoi(Argv[3]);
            TmpVinSensorGeo.StartY = atoi(Argv[4]);
            TmpVinSensorGeo.Width = atoi(Argv[5]);
            TmpVinSensorGeo.Height = atoi(Argv[6]);
            TmpVinSensorGeo.HSubSample.FactorNum = atoi(Argv[7]);
            TmpVinSensorGeo.HSubSample.FactorDen = atoi(Argv[8]);
            TmpVinSensorGeo.VSubSample.FactorNum = atoi(Argv[9]);
            TmpVinSensorGeo.VSubSample.FactorDen = atoi(Argv[10]);

            AmbaPrint("VinSensorGeo.StartX     = %d", TmpVinSensorGeo.StartX);
            AmbaPrint("VinSensorGeo.StartY     = %d", TmpVinSensorGeo.StartY);
            AmbaPrint("VinSensorGeo.Width      = %d", TmpVinSensorGeo.Width);
            AmbaPrint("VinSensorGeo.Height     = %d", TmpVinSensorGeo.Height);
            AmbaPrint("VinSensorGeo.HSubSample = %d/%d", TmpVinSensorGeo.HSubSample.FactorNum, TmpVinSensorGeo.HSubSample.FactorDen);
            AmbaPrint("VinSensorGeo.VSubSample = %d/%d", TmpVinSensorGeo.VSubSample.FactorNum, TmpVinSensorGeo.VSubSample.FactorDen);

            TmpDzoomInfo.ZoomX = atoi(Argv[11]);
            TmpDzoomInfo.ZoomY = atoi(Argv[12]);
            TmpDzoomInfo.ShiftX = atoi(Argv[13]);
            TmpDzoomInfo.ShiftY = atoi(Argv[14]);

            AmbaPrint("DzoomInfo.ZoomX   = %d", TmpDzoomInfo.ZoomX);
            AmbaPrint("DzoomInfo.ZoomY   = %d", TmpDzoomInfo.ZoomY);
            AmbaPrint("DzoomInfo.ShiftX  = %d", TmpDzoomInfo.ShiftX);
            AmbaPrint("DzoomInfo.ShiftY  = %d", TmpDzoomInfo.ShiftY);

            TmpDmyRange.Bottom = atoi(Argv[15]);
            TmpDmyRange.Top = atoi(Argv[16]);
            TmpDmyRange.Left = atoi(Argv[17]);
            TmpDmyRange.Right = atoi(Argv[18]);
            AmbaPrint("DmyRange.Bottom  = %d", TmpDmyRange.Bottom);
            AmbaPrint("DmyRange.Top     = %d", TmpDmyRange.Top);
            AmbaPrint("DmyRange.Left    = %d", TmpDmyRange.Left);
            AmbaPrint("DmyRange.Right   = %d", TmpDmyRange.Right);


            TmpOutWinInfo.MainWinDim.Width = atoi(Argv[19]);
            TmpOutWinInfo.MainWinDim.Height = atoi(Argv[20]);
            AmbaPrint("MainWinDim.Width  = %d", TmpOutWinInfo.MainWinDim.Width);
            AmbaPrint("MainWinDim.Height = %d", TmpOutWinInfo.MainWinDim.Height);


            TmpEISInfo.HorSkewPhaseInc = atoi(Argv[21]);
            TmpEISInfo.VerSkewPhaseInc = atoi(Argv[22]);
            AmbaPrint("EIS.HorSkewPhaseInc = %d", TmpEISInfo.HorSkewPhaseInc);
            AmbaPrint("EIS.VerSkewPhaseInc = %d", TmpEISInfo.VerSkewPhaseInc);

            AmbaDSP_WarpCore_Init();

            TmpWindowInfo.VinSensorGeo = TmpVinSensorGeo;
            TmpWindowInfo.DzoomInfo = TmpDzoomInfo;
            TmpWindowInfo.OutWinInfo = TmpOutWinInfo;
            TmpWindowInfo.DmyRange = TmpDmyRange;
            TmpWindowInfo.EisInfo = TmpEISInfo;

            AmbaDSP_WarpCore_CalcWindowInfo(&Mode, &TmpWindowInfo);
            AmbaPrint("==============Output===============");
            AmbaPrint("WindowInfo.ActWinCrop.LeftTopX  = %d", TmpWindowInfo.ActWinCrop.LeftTopX);
            AmbaPrint("WindowInfo.ActWinCrop.LeftTopY  = %d", TmpWindowInfo.ActWinCrop.LeftTopY);
            AmbaPrint("WindowInfo.ActWinCrop.RightBotX = %d", TmpWindowInfo.ActWinCrop.RightBotX);
            AmbaPrint("WindowInfo.ActWinCrop.RightBotY = %d", TmpWindowInfo.ActWinCrop.RightBotY);

            AmbaPrint("WindowInfo.DmyWinGeo.StartX  = %d", TmpWindowInfo.DmyWinGeo.StartX);
            AmbaPrint("WindowInfo.DmyWinGeo.StartY  = %d", TmpWindowInfo.DmyWinGeo.StartY);
            AmbaPrint("WindowInfo.DmyWinGeo.Width   = %d", TmpWindowInfo.DmyWinGeo.Width);
            AmbaPrint("WindowInfo.DmyWinGeo.Height  = %d", TmpWindowInfo.DmyWinGeo.Height);

            AmbaPrint("WindowInfo.CfaWinDim.Width  = %d", TmpWindowInfo.CfaWinDim.Width);
            AmbaPrint("WindowInfo.CfaWinDim.Height = %d", TmpWindowInfo.CfaWinDim.Height);

            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "bypasstest") == 0) {
            AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s TestWarpCorrByPass;
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
            extern int AmbaDSP_ImgSetWarpCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s *pWarpDzoomCorrByPass);

            memset(&TmpMode, 0x0, sizeof(TmpMode));

            // Dzoom part setting

            TestWarpCorrByPass.DummyWindowWidth = VCapWidth;
            TestWarpCorrByPass.DummyWindowHeight = VCapHeight;
            TestWarpCorrByPass.DummyWindowXLeft = (((VCapWidth - CalcWarp.DmyWinGeo.Width) >> 1) + 1) & 0xFFFFFFFE;
            TestWarpCorrByPass.DummyWindowYTop = (((VCapHeight - CalcWarp.DmyWinGeo.Height) >> 1) + 1) & 0xFFFFFFFE;

            TestWarpCorrByPass.CfaOutputWidth = TestWarpCorrByPass.DummyWindowWidth;
            TestWarpCorrByPass.CfaOutputHeight = TestWarpCorrByPass.DummyWindowHeight;

            TestWarpCorrByPass.ActualLeftTopX = 0;
            TestWarpCorrByPass.ActualLeftTopY = 0;
            TestWarpCorrByPass.ActualRightBotX = VCapWidth << 16;
            TestWarpCorrByPass.ActualRightBotY = VCapHeight << 16;

            TestWarpCorrByPass.ForceV4tapDisable = 0;
            TestWarpCorrByPass.HorSkewPhaseInc = 0;

            TestWarpCorrByPass.XCenterOffset = 0;
            TestWarpCorrByPass.YCenterOffset = 0;

            TestWarpCorrByPass.ZoomX = 65536;
            TestWarpCorrByPass.ZoomY = 65536;

            // Warp part setting
            TestWarpCorrByPass.WarpControl = 1;
            TestWarpCorrByPass.GridArrayWidth = 30;
            TestWarpCorrByPass.GridArrayHeight = 34;
            TestWarpCorrByPass.HorzGridSpacingExponent = 3;
            TestWarpCorrByPass.VertGridSpacingExponent = 2;
            TestWarpCorrByPass.VertWarpEnable = 1;
            TestWarpCorrByPass.VertWarpGridArrayWidth = 30;
            TestWarpCorrByPass.VertWarpGridArrayHeight = 34;
            TestWarpCorrByPass.VertWarpHorzGridSpacingExponent = 3;
            TestWarpCorrByPass.VertWarpVertGridSpacingExponent = 2;
            TestWarpCorrByPass.pWarpHorizontalTable = (INT16 *) INT16CalibWarpTblHor;
            TestWarpCorrByPass.pWarpVerticalTable = (INT16 *) INT16CalibWarpTblVer;

            /*{
             // Print warp table value
             INT32 x, y;
             char outtext[32 * 6];
             int VerGridNum = TestWarpCorrByPass.GridArrayHeight+ 1;
             int HorGridNum = TestWarpCorrByPass.GridArrayWidth + 1;
             for (y=0; y<VerGridNum; y++) {
             for (x=0; x<HorGridNum; x++) {
             sprintf(&outtext[6*x], "%6d",INT16CalibWarpTblHor[y*HorGridNum+x]);
             }
             sprintf(&outtext[6*HorGridNum], "\0");
             AmbaPrint("%s", outtext);
             }
             }*/


            AmbaDSP_ImgSetWarpCompensationByPass(&TmpMode, &TestWarpCorrByPass);

            Rval = AMP_OK;
        } else if (isnumber(Argv[2])) {
            Test = atoi(Argv[2]);
            if (Test == 11) {
                gNonStitching = 1;
                AmbaPrint("Force to NonStitching = %d ", gNonStitching);
                Rval = AMP_OK;
            } else if (Test == 10) {
                gNonStitching = 0;
                AmbaPrint("Force to NonStitching = %d ", gNonStitching);
                Rval = AMP_OK;
            }

            if ((Test == 0) || (Test == 1)) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                // dzoom param
//                VCapWidth = 640;
//                VCapHeight = 480;
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                CalcWarp.VinSensorGeo.HSubSample.FactorNum = 1;
                CalcWarp.VinSensorGeo.HSubSample.FactorDen = 1;
                CalcWarp.VinSensorGeo.VSubSample.FactorNum = 1;
                CalcWarp.VinSensorGeo.VSubSample.FactorDen = 1;
                CalcWarp.DmyWinGeo.Width = VCapWidth;
                CalcWarp.DmyWinGeo.Height = VCapHeight;
                CalcWarp.DmyWinGeo.StartX = (((VCapWidth - CalcWarp.DmyWinGeo.Width) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartY = (((VCapHeight - CalcWarp.DmyWinGeo.Height) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                CalcWarp.ActWinCrop.LeftTopX = 0;
                CalcWarp.ActWinCrop.LeftTopY = 0;
                CalcWarp.ActWinCrop.RightBotX = VCapWidth << 16;
                CalcWarp.ActWinCrop.RightBotY = VCapHeight << 16;
                CalcWarp.MainWinDim.Width = MainWidth;
                CalcWarp.MainWinDim.Height = MainHeight;
                if (gNonStitching == 1)
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width > 2716 ? 2716 : CalcWarp.DmyWinGeo.Width;

                // warp param
                if (Test == 0) {
                    CalcWarp.WarpEnb = 0;
                    memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                } else if (Test == 1) {
                    CalcWarp.WarpEnb = 1;
                    CalcWarp.CalibWarpInfo.Version = 0x20130101;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.Width = W = CalcWarp.VinSensorGeo.Width;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.Height = H = CalcWarp.VinSensorGeo.Height;

                    CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                    CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;
                    CalcWarp.CalibWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                    CalcWarp.CalibWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                    CalcWarp.CalibWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                    CalcWarp.CalibWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;

                    if (HGN * VGN > MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM) {
                        AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM);
                        Rval = AMP_ERROR_GENERAL_ERROR;
                        goto done;
                    }

                    memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                    AmbaPrint("Test hor & ver warp table");
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibWarpTbl[y * HGN + x].X = -1 * (x - (HGN >> 1)) * ((y - (VGN >> 1)) * (y - (VGN >> 1))) / 15;
                            CalibWarpTbl[y * HGN + x].Y = -1 * (y - (VGN >> 1)) * ((x - (HGN >> 1)) * (x - (HGN >> 1))) / 40;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                    CalcWarp.CalibWarpInfo.pWarp = CalibWarpTbl;
                }
                {
                    AMBA_FS_FILE *Fid;
                    char *Fnca = "d:\\warp_table_3904x2604.bin";
                    char *Fmode = "wb";

                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.StartX = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.StartX);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.StartY = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.StartY);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.Width = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.Width);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.Height = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.Height);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = %d", CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen);
                    AmbaPrint("CalcWarp.CalibWarpInfo.TileWidthExp = %d", CalcWarp.CalibWarpInfo.TileWidthExp);
                    AmbaPrint("CalcWarp.CalibWarpInfo.TileHeightExp = %d", CalcWarp.CalibWarpInfo.TileHeightExp);
                    AmbaPrint("CalcWarp.CalibWarpInfo.HorGridNum = %d", CalcWarp.CalibWarpInfo.HorGridNum);
                    AmbaPrint("CalcWarp.CalibWarpInfo.VerGridNum = %d", CalcWarp.CalibWarpInfo.VerGridNum);

                    Fid = _posix_fopen(Fnca, Fmode);
                    if (Fid == NULL) {
                    AmbaPrint("file open error");
                    return Rval;
                    }
                    Rval = AmbaFS_fwrite((void const*)CalibWarpTbl, sizeof(AMBA_DSP_IMG_GRID_POINT_s),(MAX_CALIB_WARP_HOR_GRID_NUM*MAX_CALIB_WARP_VER_GRID_NUM) , Fid);
                    AmbaFS_fclose(Fid);

                }
                AmbaDSP_ImgCalcWarpCompensation(&Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&Mode);
            } else if (Test == 2) {
                AmbaPrint("Test 2x dzoom");
                // dzoom param
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                CalcWarp.DmyWinGeo.Width = VCapWidth >> 1;
                CalcWarp.DmyWinGeo.Height = ((VCapHeight >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartX = (((VCapWidth - CalcWarp.DmyWinGeo.Width) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartY = (((VCapHeight - CalcWarp.DmyWinGeo.Height) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                CalcWarp.ActWinCrop.LeftTopX = 0;
                CalcWarp.ActWinCrop.LeftTopY = 0;
                CalcWarp.ActWinCrop.RightBotX = (VCapWidth << 15);
                CalcWarp.ActWinCrop.RightBotY = (VCapHeight << 15);
                CalcWarp.MainWinDim.Width = MainWidth;
                CalcWarp.MainWinDim.Height = MainHeight;
                if (gNonStitching == 1)
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width > 2716 ? 2716 : CalcWarp.DmyWinGeo.Width;

                AmbaDSP_ImgCalcWarpCompensation(&Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&Mode);
            } else if (Test == 21) {
                AmbaPrint("Test 2x dzoom with large dummy range");
                // dzoom param
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                CalcWarp.DmyWinGeo.Width = VCapWidth >> 1;
                CalcWarp.DmyWinGeo.Height = ((VCapHeight >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartX = (((VCapWidth - CalcWarp.DmyWinGeo.Width) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartY = (((VCapHeight - CalcWarp.DmyWinGeo.Height) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                CalcWarp.ActWinCrop.LeftTopX = 0;
                CalcWarp.ActWinCrop.LeftTopY = 0;
                CalcWarp.ActWinCrop.RightBotX = (VCapWidth << 15);
                CalcWarp.ActWinCrop.RightBotY = (VCapHeight << 15);
                CalcWarp.MainWinDim.Width = MainWidth;
                CalcWarp.MainWinDim.Height = MainHeight;
                {
                    CalcWarp.ActWinCrop.LeftTopX += CalcWarp.DmyWinGeo.StartX << 16;
                    CalcWarp.ActWinCrop.LeftTopY += CalcWarp.DmyWinGeo.StartY << 16;
                    CalcWarp.ActWinCrop.RightBotX += CalcWarp.DmyWinGeo.StartX << 16;
                    CalcWarp.ActWinCrop.RightBotY += CalcWarp.DmyWinGeo.StartY << 16;
                    CalcWarp.DmyWinGeo.Width = VCapWidth;
                    CalcWarp.DmyWinGeo.Height = VCapHeight;
                    CalcWarp.DmyWinGeo.StartX = 0; //(((VCapWidth-CalcWarp.DmyWinGeo.Width)>>1)+ 1)&0xFFFFFFFE;
                    CalcWarp.DmyWinGeo.StartY = 0; //(((VCapHeight-CalcWarp.DmyWinGeo.Height)>>1)+ 1)&0xFFFFFFFE;
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                    CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                }
                if (gNonStitching == 1)
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width > 2716 ? 2716 : CalcWarp.DmyWinGeo.Width;

                AmbaDSP_ImgCalcWarpCompensation(&Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&Mode);
            } else if (Test == 3) {
                AmbaPrint("Test 1x dzoom");
                // dzoom param
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                CalcWarp.DmyWinGeo.Width = VCapWidth;
                CalcWarp.DmyWinGeo.Height = (VCapHeight);
                CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                CalcWarp.ActWinCrop.LeftTopX = 0;
                CalcWarp.ActWinCrop.LeftTopY = 0;
                CalcWarp.ActWinCrop.RightBotX = (VCapWidth << 16);
                CalcWarp.ActWinCrop.RightBotY = (VCapHeight << 16);
                CalcWarp.MainWinDim.Width = MainWidth;
                CalcWarp.MainWinDim.Height = MainHeight;
                if (gNonStitching == 1)
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width > 2716 ? 2716 : CalcWarp.DmyWinGeo.Width;

                AmbaDSP_ImgCalcWarpCompensation(&Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&Mode);
            } else if (Test == 4) {
                AmbaPrint("Test only 2x dzoom with warp effect");
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                // dzoom param
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                CalcWarp.DmyWinGeo.Width = VCapWidth >> 1;
                CalcWarp.DmyWinGeo.Height = ((VCapHeight >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartX = (((VCapWidth - CalcWarp.DmyWinGeo.Width) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartY = (((VCapHeight - CalcWarp.DmyWinGeo.Height) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                CalcWarp.ActWinCrop.LeftTopX = 0;
                CalcWarp.ActWinCrop.LeftTopY = 0;
                CalcWarp.ActWinCrop.RightBotX = (VCapWidth << 15);
                CalcWarp.ActWinCrop.RightBotY = (VCapHeight << 15);
                CalcWarp.MainWinDim.Width = MainWidth;
                CalcWarp.MainWinDim.Height = MainHeight;
                if (gNonStitching == 1)
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width > 2716 ? 2716 : CalcWarp.DmyWinGeo.Width;

                // Tutn off warp
                /*
                 CalcWarp.WarpEnb = 0;
                 memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                 */

                // Test 2x warp table.
                CalcWarp.WarpEnb = 1;
                CalcWarp.CalibWarpInfo.Version = 0x20130101;
                CalcWarp.CalibWarpInfo.VinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                CalcWarp.CalibWarpInfo.VinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                CalcWarp.CalibWarpInfo.VinSensorGeo.Width = W = CalcWarp.VinSensorGeo.Width;
                CalcWarp.CalibWarpInfo.VinSensorGeo.Height = H = CalcWarp.VinSensorGeo.Height;

                CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;
                CalcWarp.CalibWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                CalcWarp.CalibWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                CalcWarp.CalibWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                CalcWarp.CalibWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;

                if (HGN * VGN > MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM) {
                    AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM);
                    Rval = AMP_ERROR_GENERAL_ERROR;
                    goto done;
                }

                memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                AmbaPrint("Test hor & ver warp table");
                for (y = 0; y < VGN; y++) {
                    for (x = 0; x < HGN; x++) {
                        //CalibWarpTbl[y*HGN+x].X = -300*(x-(HGN>>1))*((y-(VGN>>1))*(y-(VGN>>1)))/15;
                        if (x < (HGN / 2)) {
                            CalibWarpTbl[y * HGN + x].X = (-20) << 4;
                        } else {
                            CalibWarpTbl[y * HGN + x].X = 20 << 4;
                        }
                        CalibWarpTbl[y * HGN + x].Y = 0;
                        if (y == 0)
                            AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                    }
                }
                CalcWarp.CalibWarpInfo.pWarp = CalibWarpTbl;

                AmbaDSP_ImgCalcWarpCompensation(&Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&Mode);
            } else if (Test == 5) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                AMBA_DSP_IMG_MODE_CFG_s TmpMode ;
                AMBA_DSP_IMG_WIN_DIMENSION_s TmpR2rWin = {0};
                AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
                AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
                AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange;
                AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = {0};
                AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s TmpWarpRefferenceDmyMarginPixel;
                AMBA_DSP_IMG_CALIB_WARP_INFO_s TmpCalcWarp;
                double Theta = 0.0;
                #define PI  3.1415926535897932384626433832795

                AmbaPrint("Extra effect rotate");

                memset(&TmpMode, 0x0, sizeof(TmpMode));


                TmpVinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                TmpVinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                TmpVinSensorGeo.Width = CalcWarp.VinSensorGeo.Width;
                TmpVinSensorGeo.Height = CalcWarp.VinSensorGeo.Height;
                TmpVinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                TmpVinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                TmpVinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                TmpVinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;

                TmpDzoomInfo.ZoomX = atoi(Argv[3]) ;
                TmpDzoomInfo.ZoomY = atoi(Argv[4]) ;
                TmpDzoomInfo.ShiftX = atoi(Argv[5]) ;
                TmpDzoomInfo.ShiftY = atoi(Argv[6]) ;

                TmpDmyRange.Top = atoi(Argv[7]) ;
                TmpDmyRange.Bottom = atoi(Argv[8]) ;
                TmpDmyRange.Left = atoi(Argv[9]) ;
                TmpDmyRange.Right= atoi(Argv[10]) ;

                TmpOutWinInfo.MainWinDim.Width = MainWidth;
                TmpOutWinInfo.MainWinDim.Height = MainHeight;

                TmpWarpRefferenceDmyMarginPixel.Enable = 1;

                CalcWarp.WarpEnb = 1;

                CalcWarp.CalibWarpInfo.Version = 0x20130101;
                CalcWarp.CalibWarpInfo.VinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                CalcWarp.CalibWarpInfo.VinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                CalcWarp.CalibWarpInfo.VinSensorGeo.Width = W = CalcWarp.VinSensorGeo.Width;
                CalcWarp.CalibWarpInfo.VinSensorGeo.Height = H = CalcWarp.VinSensorGeo.Height;

                CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;
                CalcWarp.CalibWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                CalcWarp.CalibWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                CalcWarp.CalibWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                CalcWarp.CalibWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;

                if (HGN * VGN > MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM) {
                    AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM);
                    Rval = AMP_ERROR_GENERAL_ERROR;

                }


                CalcWarp.CalibWarpInfo.pWarp = CalibWarpTbl;
                CalcWarp.CalibWarpInfo.Enb2StageCompensation = 1;
                TmpCalcWarp = CalcWarp.CalibWarpInfo;

                AmbaDSP_WarpCore_Init();
                AmbaDSP_WarpCore_SetVinSensorGeo(&Mode, &TmpVinSensorGeo);
                AmbaDSP_WarpCore_SetR2rOutWin(&Mode, &TmpR2rWin);
                AmbaDSP_WarpCore_SetDummyWinMarginRange(&Mode, &TmpDmyRange);
                AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(&Mode, &TmpWarpRefferenceDmyMarginPixel);
                AmbaDSP_WarpCore_SetDzoomFactor(&Mode, &TmpDzoomInfo);
                AmbaDSP_WarpCore_SetOutputWin(&Mode, &TmpOutWinInfo);
                AmbaDSP_WarpCore_SetDspVideoMode(&Mode, 5);
                AmbaDSP_WarpCore_SetCalibWarpInfo(&Mode, &TmpCalcWarp);
                Theta= atoi(Argv[11]) * PI / 180.0;
                AmbaDSP_WarpCore_SetWarpRotateTheta(&Mode, Theta);
                AmbaDSP_WarpCore_CalcDspWarp(&Mode, 0);
                AmbaDSP_WarpCore_SetDspWarp(&Mode);
            } else if (Test == 6) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                AmbaPrint("Test only 2x dzoom with warp effect");
                // dzoom param
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                CalcWarp.DmyWinGeo.Width = VCapWidth >> 2;
                CalcWarp.DmyWinGeo.Height = ((VCapHeight >> 2) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartX = (((VCapWidth - CalcWarp.DmyWinGeo.Width) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.DmyWinGeo.StartY = (((VCapHeight - CalcWarp.DmyWinGeo.Height) >> 1) + 1) & 0xFFFFFFFE;
                CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width;
                CalcWarp.CfaWinDim.Height = CalcWarp.DmyWinGeo.Height;
                CalcWarp.ActWinCrop.LeftTopX = 0;
                CalcWarp.ActWinCrop.LeftTopY = 0;
                CalcWarp.ActWinCrop.RightBotX = (VCapWidth << 14);
                CalcWarp.ActWinCrop.RightBotY = (VCapHeight << 14);
                CalcWarp.MainWinDim.Width = MainWidth;
                CalcWarp.MainWinDim.Height = MainHeight;
                if (gNonStitching == 1)
                    CalcWarp.CfaWinDim.Width = CalcWarp.DmyWinGeo.Width > 2716 ? 2716 : CalcWarp.DmyWinGeo.Width;

                // Tutn off warp
                /*
                 CalcWarp.WarpEnb = 0;
                 memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                 */

                // Test 2x warp table.
                CalcWarp.WarpEnb = 1;
                CalcWarp.CalibWarpInfo.Version = 0x20130101;
                CalcWarp.CalibWarpInfo.VinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                CalcWarp.CalibWarpInfo.VinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                CalcWarp.CalibWarpInfo.VinSensorGeo.Width = W = CalcWarp.VinSensorGeo.Width;
                CalcWarp.CalibWarpInfo.VinSensorGeo.Height = H = CalcWarp.VinSensorGeo.Height;

                CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                CalcWarp.CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                CalcWarp.CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;
                CalcWarp.CalibWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                CalcWarp.CalibWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                CalcWarp.CalibWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                CalcWarp.CalibWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;

                if (HGN * VGN > MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM) {
                    AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM);
                    Rval = AMP_ERROR_GENERAL_ERROR;
                    goto done;
                }

                memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                AmbaPrint("Test hor & ver warp table");
                for (y = 0; y < VGN; y++) {
                    for (x = 0; x < HGN; x++) {
                        CalibWarpTbl[y * HGN + x].X = 0;
                        if (y < (VGN / 2)) {
                            CalibWarpTbl[y * HGN + x].Y = (-20) << 4;
                        } else {
                            CalibWarpTbl[y * HGN + x].Y = 20 << 4;
                        }
                        if (y == 0)
                            AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                    }
                }
                CalcWarp.CalibWarpInfo.pWarp = CalibWarpTbl;

                AmbaDSP_ImgCalcWarpCompensation(&Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&Mode);
            }
            Rval = AMP_OK;
        } else {
            Rval = AMP_ERROR_GENERAL_ERROR;
        }
    }
    done: if (Rval == -1)
        AmbaPrint("\n\r"
                  "Usage: t %s -warp: get warp info\n\r"
                  "       t %s -warp 0: warp off\n\r"
                  "       t %s -warp 1: hor and ver warp on\n\r"
                  "       t %s -warp 2: dzoom 2x test\n\r"
                  "       t %s -warp 3: dzoom 1x test\n\r"
                  "       t %s -warp bypasstest: Only used in video mode, bypass function test with the the same pattern to -warp 1\n\r"
                  "       t %s -warp vert [0|1]: enable/disable vertical warp flip\n\r",
                  Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0], Argv[0]);
    return Rval;
}
#define MAX_CALIB_CAWARP_HOR_GRID_NUM (64)
#define MAX_CALIB_CAWARP_VER_GRID_NUM (64)
static INT16 ByPassCaWarpHorzTbl[MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM];
static INT16 ByPassCaWarpVertTbl[MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM];
static AMBA_DSP_IMG_GRID_POINT_s CalibCaWarpTbl[MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM];


static AMP_ER_CODE_e AppLibTune_ca(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    AMBA_DSP_IMG_CAWARP_CALC_INFO_s CalcCaWarp = { 0 };
    int Test;
    AMBA_DSP_IMG_MODE_CFG_s Mode = {0};
    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
    UINT16 VCapWidth, VCapHeight, MainWidth, MainHeight;
    VCapWidth = VCAPWIDTH;
    VCapHeight = VCAPHEIGHT;
    MainWidth = MAINWIDTH;
    MainHeight = MAINHEIGHT;
    AmbaPrint("CaWarp size info VCapWidth= %d VCapHeight=%d MainWidth=%d MainHeight=%d", VCapWidth, VCapHeight, MainWidth, MainHeight);
    AmbaDSP_ImgGetCawarpCompensation(&Mode, &CalcCaWarp);

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current cawarp info: TBD");
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugCawarpCompensation(UINT8 TmpCawarpDebugMessageFlag);
            UINT8 CawarpDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugCawarpCompensation(CawarpDebugFlag);
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "bypasstest") == 0) {

            int MaxByPassTableSize = MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM;
            int x;
            AMBA_DSP_IMG_BYPASS_CAWARP_INFO_s TestCAWarpCorrByPass;

            TestCAWarpCorrByPass.HorzWarpEnable = 1;
            TestCAWarpCorrByPass.VertWarpEnable = 1;
            TestCAWarpCorrByPass.HorzPassGridArrayWidth = 21;
            TestCAWarpCorrByPass.HorzPassGridArrayHeight = 31;
            TestCAWarpCorrByPass.HorzPassHorzGridSpacingExponent = 2;
            TestCAWarpCorrByPass.HorzPassVertGridSpacingExponent = 1;
            TestCAWarpCorrByPass.VertPassGridArrayWidth = 21;
            TestCAWarpCorrByPass.VertPassGridArrayHeight = 31;
            TestCAWarpCorrByPass.VertPassHorzGridSpacingExponent = 2;
            TestCAWarpCorrByPass.VertPassVertGridSpacingExponent = 1;
            TestCAWarpCorrByPass.RedScaleFactor = 0x80;
            TestCAWarpCorrByPass.BlueScaleFactor = 0x80;
            TestCAWarpCorrByPass.pWarpHorzTable = ByPassCaWarpHorzTbl;
            TestCAWarpCorrByPass.pWarpVertTable = ByPassCaWarpVertTbl;

            for (x = 0; x < MaxByPassTableSize; x++) {
                ByPassCaWarpHorzTbl[x] = 511;    // s4.5 format
                ByPassCaWarpVertTbl[x] = 127;    // s4.5 format
            }

            extern int AmbaDSP_ImgSetCawarpCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s* Mode, AMBA_DSP_IMG_BYPASS_CAWARP_INFO_s *pCAWarpCorrByPass);

            AmbaDSP_ImgSetCawarpCompensationByPass(&Mode, &TestCAWarpCorrByPass);
            Rval = AMP_OK;

        } else if (isnumber(Argv[2])) {
            Test = atoi(Argv[2]);
            if ((Test == 0) || (Test == 1) || (Test == 2)) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                // vin param
                CalcCaWarp.VinSensorGeo.Width = VCapWidth;
                CalcCaWarp.VinSensorGeo.Height = VCapHeight;
                CalcCaWarp.VinSensorGeo.HSubSample.FactorNum = 1;
                CalcCaWarp.VinSensorGeo.HSubSample.FactorDen = 1;
                CalcCaWarp.VinSensorGeo.VSubSample.FactorNum = 1;
                CalcCaWarp.VinSensorGeo.VSubSample.FactorDen = 1;
                CalcCaWarp.DmyWinGeo.Width = VCapWidth;
                CalcCaWarp.DmyWinGeo.Height = VCapHeight;
                CalcCaWarp.CfaWinDim.Width = CalcCaWarp.DmyWinGeo.Width;
                CalcCaWarp.CfaWinDim.Height = CalcCaWarp.DmyWinGeo.Height;

                if (gNonStitching == 1)
                    CalcCaWarp.CfaWinDim.Width = CalcCaWarp.CfaWinDim.Width > 2716 ? 2716 : CalcCaWarp.CfaWinDim.Width;

                // ca warp param
                if (Test == 0) {
                    CalcCaWarp.CaWarpEnb = 0;
                    memset(CalibCaWarpTbl, 0, sizeof(CalibCaWarpTbl));
                } else if (Test == 1) {
                    CalcCaWarp.CaWarpEnb = 1;
                    CalcCaWarp.CalibCaWarpInfo.Version = 0x20130125;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.StartX = CalcCaWarp.VinSensorGeo.StartX;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.StartY = CalcCaWarp.VinSensorGeo.StartY;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.Width = W = CalcCaWarp.VinSensorGeo.Width;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.Height = H = CalcCaWarp.VinSensorGeo.Height;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcCaWarp.VinSensorGeo.HSubSample.FactorDen;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcCaWarp.VinSensorGeo.HSubSample.FactorNum;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcCaWarp.VinSensorGeo.VSubSample.FactorDen;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcCaWarp.VinSensorGeo.VSubSample.FactorNum;
                    CalcCaWarp.CalibCaWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                    CalcCaWarp.CalibCaWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                    CalcCaWarp.CalibCaWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                    CalcCaWarp.CalibCaWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;
                    CalcCaWarp.CalibCaWarpInfo.RedScaleFactor = 128;
                    CalcCaWarp.CalibCaWarpInfo.BlueScaleFactor = 128;

                    if (HGN * VGN > MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM) {
                        AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM);
                        Rval = AMP_ERROR_GENERAL_ERROR;
                        goto done;
                    }

                    memset(CalibCaWarpTbl, 0, sizeof(CalibCaWarpTbl));
                    AmbaPrint("Test hor & ver cawarp table");
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibCaWarpTbl[y * HGN + x].X = 511;    // s4.5 format
                            CalibCaWarpTbl[y * HGN + x].Y = 127;    // s4.5 format
                        }
                    }
                    CalcCaWarp.CalibCaWarpInfo.pCaWarp = CalibCaWarpTbl;
                } else if (Test == 2) {
                    CalcCaWarp.CaWarpEnb = 1;
                    CalcCaWarp.CalibCaWarpInfo.Version = 0x20130125;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.StartX = CalcCaWarp.VinSensorGeo.StartX;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.StartY = CalcCaWarp.VinSensorGeo.StartY;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.Width = W = CalcCaWarp.VinSensorGeo.Width;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.Height = H = CalcCaWarp.VinSensorGeo.Height;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcCaWarp.VinSensorGeo.HSubSample.FactorDen;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcCaWarp.VinSensorGeo.HSubSample.FactorNum;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcCaWarp.VinSensorGeo.VSubSample.FactorDen;
                    CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcCaWarp.VinSensorGeo.VSubSample.FactorNum;
                    CalcCaWarp.CalibCaWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                    CalcCaWarp.CalibCaWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                    CalcCaWarp.CalibCaWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                    CalcCaWarp.CalibCaWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;
                    CalcCaWarp.CalibCaWarpInfo.RedScaleFactor = 128;
                    CalcCaWarp.CalibCaWarpInfo.BlueScaleFactor = 128;

                    if (HGN * VGN > MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM) {
                        AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_CAWARP_HOR_GRID_NUM * MAX_CALIB_CAWARP_VER_GRID_NUM);
                        Rval = AMP_ERROR_GENERAL_ERROR;
                        goto done;
                    }

                    memset(CalibCaWarpTbl, 0, sizeof(CalibCaWarpTbl));
                    AmbaPrint("Test hor & ver cawarp table");
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibCaWarpTbl[y * HGN + x].X = (511 * (x + 1)) / HGN;    // s4.5 format
                            CalibCaWarpTbl[y * HGN + x].Y = (127 * (y + 1)) / VGN;    // s4.5 format
                        }
                    }
                    CalcCaWarp.CalibCaWarpInfo.pCaWarp = CalibCaWarpTbl;
                }
                /*{
                 AMBA_FS_FILE *Fid;
                 char *Fnca = "C:\\ca_table_3904x3604.bin";
                 char *Fmode = "w";

                 Fid = _posix_fopen(Fnca, Fmode);
                 if (Fid == NULL) {
                 AmbaPrint("file open error");
                 return Rval;
                 }
                 Rval = AmbaFS_fwrite((void const*)CalibCaWarpTbl, sizeof(AMBA_DSP_IMG_GRID_POINT_s),(MAX_CALIB_CAWARP_HOR_GRID_NUM*MAX_CALIB_CAWARP_VER_GRID_NUM) , Fid);
                 AmbaFS_fclose(Fid);

                 }*/
                AmbaDSP_ImgCalcCawarpCompensation(&Mode, &CalcCaWarp);
                AmbaDSP_ImgSetCawarpCompensation(&Mode);
            } else if (Test == 8) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
                AMBA_DSP_IMG_MODE_CFG_s TmpMode;
                AMBA_DSP_IMG_WIN_DIMENSION_s TmpR2rWin = {0};
                AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange = {0};
                AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
                AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = {0};
                AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s TmpWarpRefferenceDmyMarginPixel;
                AMBA_DSP_IMG_CALIB_CAWARP_INFO_s TmpCalcCaWarp;

                AmbaPrint("Extra effect");

                memset(&TmpMode, 0x0, sizeof(TmpMode));
                TmpVinSensorGeo.StartX = CalcCaWarp.VinSensorGeo.StartX;
                TmpVinSensorGeo.StartY = CalcCaWarp.VinSensorGeo.StartY;
                TmpVinSensorGeo.Width = CalcCaWarp.VinSensorGeo.Width;
                TmpVinSensorGeo.Height = CalcCaWarp.VinSensorGeo.Height;
                TmpVinSensorGeo.HSubSample.FactorNum = CalcCaWarp.VinSensorGeo.HSubSample.FactorNum;
                TmpVinSensorGeo.HSubSample.FactorDen = CalcCaWarp.VinSensorGeo.HSubSample.FactorDen;
                TmpVinSensorGeo.VSubSample.FactorNum = CalcCaWarp.VinSensorGeo.VSubSample.FactorNum;
                TmpVinSensorGeo.VSubSample.FactorDen = CalcCaWarp.VinSensorGeo.VSubSample.FactorDen;

                TmpDzoomInfo.ZoomX = 1 << 16;
                TmpDzoomInfo.ZoomY = 1 << 16;
                TmpDzoomInfo.ShiftX = 0;
                TmpDzoomInfo.ShiftY = 0;

                TmpOutWinInfo.MainWinDim.Width = MainWidth;
                TmpOutWinInfo.MainWinDim.Height= MainHeight;

                TmpWarpRefferenceDmyMarginPixel.Enable = 0;

                CalcCaWarp.CaWarpEnb = 1;

                CalcCaWarp.CalibCaWarpInfo.Version = 0x20130125;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.StartX = CalcCaWarp.VinSensorGeo.StartX;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.StartY = CalcCaWarp.VinSensorGeo.StartY;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.Width = W = CalcCaWarp.VinSensorGeo.Width;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.Height = H = CalcCaWarp.VinSensorGeo.Height;

                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorNum = CalcCaWarp.VinSensorGeo.HSubSample.FactorNum;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorDen = CalcCaWarp.VinSensorGeo.HSubSample.FactorDen;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorNum = CalcCaWarp.VinSensorGeo.VSubSample.FactorNum;
                CalcCaWarp.CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorDen = CalcCaWarp.VinSensorGeo.VSubSample.FactorDen;
                CalcCaWarp.CalibCaWarpInfo.TileWidthExp = TWE = 6; //2^6 = 64;
                CalcCaWarp.CalibCaWarpInfo.TileHeightExp = THE = 6; //2^6 = 64;
                CalcCaWarp.CalibCaWarpInfo.HorGridNum = HGN = ((W + ((1 << TWE) - 1)) >> TWE) + 1;
                CalcCaWarp.CalibCaWarpInfo.VerGridNum = VGN = ((H + ((1 << THE) - 1)) >> THE) + 1;

                if (HGN * VGN > MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM) {
                    AmbaPrint("Error; Calc grid number %d * %d > max available number %d", HGN, VGN, MAX_CALIB_WARP_HOR_GRID_NUM * MAX_CALIB_WARP_VER_GRID_NUM);
                    Rval = AMP_ERROR_GENERAL_ERROR;

                }

                memset(CalibCaWarpTbl, 0, sizeof(CalibCaWarpTbl));
                AmbaPrint("Test hor & ver warp table");
                for (y = 0; y < VGN; y++) {
                    for (x = 0; x < HGN; x++) {
                        if (y < VGN / 2) {
                            CalibCaWarpTbl[y * HGN + x].X = 511;    // s4.5 format
                            CalibCaWarpTbl[y * HGN + x].Y = 127;    // s4.5 format
                        }
                        if (x == 0) {
                            AmbaPrint("%d       %d", x, CalibCaWarpTbl[y * HGN + x].X);
                        }
                    }
                }
                CalcCaWarp.CalibCaWarpInfo.pCaWarp = CalibCaWarpTbl;
                CalcCaWarp.CalibCaWarpInfo.RedScaleFactor = 128;
                CalcCaWarp.CalibCaWarpInfo.BlueScaleFactor = 128;
                TmpCalcCaWarp = CalcCaWarp.CalibCaWarpInfo;

                AmbaDSP_WarpCore_Init();
                AmbaDSP_WarpCore_SetVinSensorGeo(&Mode, &TmpVinSensorGeo);
                AmbaDSP_WarpCore_SetR2rOutWin(&Mode, &TmpR2rWin);
                AmbaDSP_WarpCore_SetDummyWinMarginRange(&Mode, &TmpDmyRange);
                AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(&Mode, &TmpWarpRefferenceDmyMarginPixel);
                AmbaDSP_WarpCore_SetDzoomFactor(&Mode, &TmpDzoomInfo);
                AmbaDSP_WarpCore_SetOutputWin(&Mode, &TmpOutWinInfo);
                AmbaDSP_WarpCore_SetDspVideoMode(&Mode, 5);
                AmbaDSP_WarpCore_SetCalibCawarpInfo(&Mode, &TmpCalcCaWarp);

                AmbaDSP_WarpCore_CalcDspCawarp(&Mode, 0);
                AmbaDSP_WarpCore_SetDspCawarp(&Mode);
            }
            Rval = AMP_OK;
        } else {
            Rval = AMP_ERROR_GENERAL_ERROR;
        }
    }
    done: if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -ca: get cawarp info\n\r"
                  "       t %s -ca 0: cawarp off\n\r"
                  "       t %s -ca 1: hor and ver cawarp on. Constant values.\n\r"
                  "       t %s -ca 2: hor and ver cawarp on. Running numbers.\n\r"
                  "       t %s -ca bypasstest: Only used in capture mode, bypass function test with the the same pattern to -ca 1",
                  Argv[0], Argv[0], Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

#define CALIB_FPN_WIDTH (3840)//*/1312
#define CALIB_FPN_HEIGHT (2160)//*/998
static int filled_fpn_diagonal_pattern(UINT8 *MapStartAddress, INT16 MapWidth, INT16 MapHeight, UINT8 IntervalByte)
{
    /*
                                 MapWidth
              |--------------------------------------------|

                IntervalByte
              |----|
            - *    *    *    *    *    *    *    *    *    * -
            |  *    *    *    *    *    *    *    *    *     |
IntervalByte|   *    *    *    *    *    *    *    *    *    |
            |    *    *    *    *    *    *    *    *    *   |
            |     *    *    *    *    *    *    *    *    *  | MapHeight
            - *    *    *    *    *    *    *    *    *    * |
               *    *    *    *    *    *    *    *    *     |
                *    *    *    *    *    *    *    *    *    |
                 *    *    *    *    *    *    *    *    *   -

     */
    INT16 HeightIndex = 0;
    INT16 WidthIndex = 0;

    INT16 FirstPointInLine = 0; // the first static bad pixel index in width by line
    UINT8 TmpBlockPattern = 0; // 00000001, 00000010, 00000100, 00001000, ...

    // 1 Byte with 8-bit data in width
    MapWidth = MapWidth / 8;

    // Init memeroy, resetting the whole map
    memset(MapStartAddress, 0, MapWidth * MapHeight);

    AmbaPrint(" MapStartAddress: %x", MapStartAddress);

    // Fill the FPN table with diagonal pattern
    for (HeightIndex = 0; HeightIndex < MapHeight; HeightIndex++) {

        //Create pattern line by line
        //Pattern: 10000000, 01000000, 00100000, 00010000, ...
        TmpBlockPattern = 1 << (HeightIndex % 8);

        //Caculate the first bad static pixel postion in line.
        FirstPointInLine = (HeightIndex % (IntervalByte * 8)) / 8;
        AmbaPrint(" HeightIndex: %d", HeightIndex);
        AmbaPrint(" IntervalByte: %d", IntervalByte);
        AmbaPrint(" FirstPointInLine: %d", FirstPointInLine);

        // Fill pattern from the first postion, and then fill data by internal in a line.
        for (WidthIndex = FirstPointInLine; WidthIndex < MapWidth; WidthIndex += IntervalByte) {
            MapStartAddress[HeightIndex * MapWidth + WidthIndex] = TmpBlockPattern;
        }
    }

    AmbaPrint("The Map:\n");
    for (HeightIndex = 0; HeightIndex < 200; HeightIndex++) {
        AmbaPrint("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",
                  MapStartAddress[HeightIndex * MapWidth],
                  MapStartAddress[HeightIndex * MapWidth + 1],
                  MapStartAddress[HeightIndex * MapWidth + 2],
                  MapStartAddress[HeightIndex * MapWidth + 3],
                  MapStartAddress[HeightIndex * MapWidth + 4],
                  MapStartAddress[HeightIndex * MapWidth + 5],
                  MapStartAddress[HeightIndex * MapWidth + 6],
                  MapStartAddress[HeightIndex * MapWidth + 7],
                  MapStartAddress[HeightIndex * MapWidth + 8],
                  MapStartAddress[HeightIndex * MapWidth + 9],
                  MapStartAddress[HeightIndex * MapWidth + 10],
                  MapStartAddress[HeightIndex * MapWidth + 11],
                  MapStartAddress[HeightIndex * MapWidth + 12],
                  MapStartAddress[HeightIndex * MapWidth + 13],
                  MapStartAddress[HeightIndex * MapWidth + 14],
                  MapStartAddress[HeightIndex * MapWidth + 15],
                  MapStartAddress[HeightIndex * MapWidth + 16],
                  MapStartAddress[HeightIndex * MapWidth + 17],
                  MapStartAddress[HeightIndex * MapWidth + 18],
                  MapStartAddress[HeightIndex * MapWidth + 19]);
    }

    return 0;
}

static AMP_ER_CODE_e AppLibTune_fpn(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    INT16 FPNTestWidth, FPNTestHeight;
    UINT16 VCapWidth, VCapHeight;
    VCapWidth = VCAPWIDTH;
    VCapHeight = VCAPHEIGHT;

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            //AmbaPrint();
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "highlight") == 0) {
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
            UINT8 Highligh;
            extern int AmbaDSP_ImgSetStaticBadPixelCorrectionHighlightMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT8 *pSbpHighligh);
            AmbaPrint("FPN Highlight mode");

            memset(&TmpMode, 0x0, sizeof(TmpMode));
            Highligh = atoi(Argv[3]);
            AmbaDSP_ImgSetStaticBadPixelCorrectionHighlightMode(&TmpMode, &Highligh);

            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugStaticBadPixelCorrection(UINT8 TmpFPNDebugMessageFlag);
            UINT8 FPNDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugStaticBadPixelCorrection(FPNDebugFlag);
            Rval = AMP_OK;
        } else if (isnumber(Argv[2])) {
            extern int AmbaItuner_Get_Calib_Table(ITUNER_Calib_Table_s **Ituner_Calib_Table);
            ITUNER_Calib_Table_s *Ituner_Calib_Table;
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
            AMBA_DSP_IMG_SBP_CORRECTION_s SbpInfo;
            memset(&TmpMode, 0x0, sizeof(TmpMode));

            //Initial FPN table whole buffer, and fill FPN with diagonal pattern
            AmbaItuner_Get_Calib_Table(&Ituner_Calib_Table);
            memset(Ituner_Calib_Table->FPNMap, 0x0, ITUNER_MAX_FPN_MAP_SIZE);
            filled_fpn_diagonal_pattern(Ituner_Calib_Table->FPNMap, CALIB_FPN_WIDTH, CALIB_FPN_HEIGHT, 10);

            // Set parameter from I/O
            SbpInfo.Enb = atoi(Argv[2]);
            FPNTestWidth = VCapWidth;
            FPNTestHeight = VCapHeight;
            AmbaPrint("Fpn size info FPNTestWidth= %d FPNTestHeight=%d", FPNTestWidth, FPNTestHeight);

            SbpInfo.CurrentVinSensorGeo.StartX = 0;
            SbpInfo.CurrentVinSensorGeo.StartY = 0;
            SbpInfo.CurrentVinSensorGeo.Width = FPNTestWidth;
            SbpInfo.CurrentVinSensorGeo.Height = FPNTestHeight;
            SbpInfo.CurrentVinSensorGeo.HSubSample.FactorNum = 1;
            SbpInfo.CurrentVinSensorGeo.HSubSample.FactorDen = 1;
            SbpInfo.CurrentVinSensorGeo.VSubSample.FactorNum = 1;
            SbpInfo.CurrentVinSensorGeo.VSubSample.FactorDen = 1;

            SbpInfo.CalibSbpInfo.Version = AMBA_DSP_IMG_SBP_VER_1_0;
            SbpInfo.CalibSbpInfo.SbpBuffer = Ituner_Calib_Table->FPNMap;

            SbpInfo.CalibSbpInfo.VinSensorGeo = SbpInfo.CurrentVinSensorGeo;
            SbpInfo.CalibSbpInfo.VinSensorGeo.Width = CALIB_FPN_WIDTH;
            SbpInfo.CalibSbpInfo.VinSensorGeo.Height = CALIB_FPN_HEIGHT;

            if (atoi(Argv[3]) == 0) {
                AmbaDSP_ImgSetStaticBadPixelCorrection(&TmpMode, &SbpInfo);
            } else if (atoi(Argv[3]) == 1) {
                AmbaPrint("FPN Bypass mode");
                AMBA_DSP_IMG_BYPASS_SBP_INFO_s TmpSbpByPassCorr;
                extern int AmbaDSP_ImgSetStaticBadPixelCorrectionByPass(AMBA_DSP_IMG_MODE_CFG_s Mode, AMBA_DSP_IMG_BYPASS_SBP_INFO_s *pSbpCorrByPass);

                TmpSbpByPassCorr.Enable = atoi(Argv[2]);

                TmpSbpByPassCorr.PixelMapHeight = 0x3D8;
                TmpSbpByPassCorr.PixelMapWidth = 0x520;
                TmpSbpByPassCorr.PixelMapPitch = 0x1E0;
                TmpSbpByPassCorr.pMap = (UINT8*) 0x2484020;

                 AmbaDSP_ImgSetStaticBadPixelCorrectionByPass(TmpMode, &TmpSbpByPassCorr);
            }
            /*{
             AMBA_FS_FILE *Fid;
             char *Fn = "C:\\fpn_table_4608x3600.bin";
             char *Fmode = "w";

             Fid = _posix_fopen(Fn, Fmode);
             if (Fid == NULL) {
             AmbaPrint("file open error");
             return Rval;
             }
             Rval = AmbaFS_fwrite((void const*)FPNMap, sizeof(UINT8),(CALIB_FPN_WIDTH*CALIB_FPN_HEIGHT/8) , Fid);
             AmbaFS_fclose(Fid);
             }*/

            Rval = AMP_OK;
        } else {
            Rval = AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -fpn: get the current FPN setting\n\r"
                  "       t %s -fpn [enable] [struct mode]\n\r"
                  "         enable = [0|1], 0:disable, 1:enable\n\r"
                  "         struct mode = [0|1], 0:A9, 1:A7l\n\r"
                  "         without struct mode is A9 mode\n\r"
                  "       t %s -fpn load [enable] [w] [h]\n\r"
                  "       t %s -fpn highlight [enable]\n\r"
                  "         enable = [0|1], 0:disable, 1:enable",
                  Argv[0], Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

static AMP_ER_CODE_e AppLibTune_dpc(int Argc, char **Argv)
{
    AMBA_DSP_IMG_MODE_CFG_s TmpMode;
    AMBA_DSP_IMG_DBP_CORRECTION_s DpcInfo;
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    memset(&TmpMode, 0x0, sizeof(TmpMode));
    AmbaDSP_ImgGetDynamicBadPixelCorrection(&TmpMode, &DpcInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current dynamic bad pixel correction setting: enable:%d hot_str:%d dark_str:%d cor_method: %d\n",
                      (int) DpcInfo.Enb,
                      (int) DpcInfo.HotPixelStrength,
                      (int) DpcInfo.DarkPixelStrength,
                      (int) DpcInfo.CorrectionMethod);
            Rval = AMP_OK;
        } else if (isnumber(Argv[2])) {
            DpcInfo.Enb = atoi(Argv[2]);
            DpcInfo.HotPixelStrength = atoi(Argv[3]);
            DpcInfo.DarkPixelStrength = atoi(Argv[4]);
            DpcInfo.CorrectionMethod = atoi(Argv[5]);
            AmbaDSP_ImgSetDynamicBadPixelCorrection(&TmpMode, &DpcInfo);
            Rval = AMP_OK;
        } else {

        }
    }
    if (Rval == AMP_ERROR_GENERAL_ERROR) {
        AmbaPrint("\n\r"
                  "Usage: t %s -dpc: get the current dynamic bad pixel correction setting\n\r"
                  "       t %s -dpc [enable] [hot_str] [dark_str] [cor_method]: set the current dpc setting\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

#define CALIB_VNC_WIDTH  (3840)//*/1312
#define CALIB_VNC_HEIGHT (2160)//*/998
#define VIGNETTE_TABLE_WIDTH (65)
#define VIGNETTE_TABLE_HEIGTH (49)
#define VIGNETTE_VIG_STRENGTH_UNIT_SHIFT (16)
static UINT16 VignetteRedGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];
static UINT16 VignetteGreenEvenGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];
static UINT16 VignetteGreenOddGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];
static UINT16 VignetteBlueGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];

static AMP_ER_CODE_e AppLibTune_vnc(int Argc, char **Argv)
{
    AMP_ER_CODE_e Rval = AMP_ERROR_GENERAL_ERROR;
    INT16 VncTestWidth, VncTestHeight;
    UINT16 VCapWidth, VCapHeight;
    VCapWidth = VCAPWIDTH;
    VCapHeight = VCAPHEIGHT;

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            Rval = AMP_OK;
        } else if ((strcmp(Argv[2], "load") == 0) && (Argc == 5)) {
            AppLib_DEBF("FIXME: Not Yet Implemented");
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "test") == 0) {
            AppLib_DEBF("FIXME: Not Yet Implemented");
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugVignetteCompensation(UINT8 TmpVignetteDebugMessageFlag);
            UINT8 VignetteDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugVignetteCompensation(VignetteDebugFlag);
            Rval = AMP_OK;
        } else if (strcmp(Argv[2], "bypasstest") == 0) {
            UINT16 *pGain[4], UnitGain;
            int i, j, idx0, idx1;
            AMBA_DSP_IMG_BYPASS_VIGNETTE_INFO_s TestVigCorrByPass;
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
            extern int AmbaDSP_ImgSetVignetteCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s* Mode, AMBA_DSP_IMG_BYPASS_VIGNETTE_INFO_s *pVigCorrByPass);

            memset(&TmpMode, 0x0, sizeof(TmpMode));
            TestVigCorrByPass.Enable = atoi(Argv[3]);
            TestVigCorrByPass.GainShift = 0;
            TestVigCorrByPass.pRedGain = VignetteRedGainTbl;
            TestVigCorrByPass.pGreenEvenGain = VignetteGreenEvenGainTbl;
            TestVigCorrByPass.pGreenOddGain = VignetteGreenOddGainTbl;
            TestVigCorrByPass.pBlueGain = VignetteBlueGainTbl;
            UnitGain = 1 << (TestVigCorrByPass.GainShift + 7);
            pGain[0] = TestVigCorrByPass.pRedGain;
            pGain[1] = TestVigCorrByPass.pGreenEvenGain;
            pGain[2] = TestVigCorrByPass.pGreenOddGain;
            pGain[3] = TestVigCorrByPass.pBlueGain;
            for (j = 0; j < 4; j++) {
                for (i = 0; i < 33 * 33; i++) {
                    pGain[j][i] = UnitGain;
                    //gain[j][i] = 0;
                }
            }
            for (i = 0; i < 33; i++) {
                idx1 = 33 * i + 33 / 2;
                for (j = 0; j < 4; j++) {
                    pGain[j][idx1] = 0x0;
                    //AmbaPrint("idx1: %d, gain[%d][idx1] addr: %d", idx1, j, &gain[j][idx1]);
                }
            }
            for (i = 0; i < 33; i++) {
                idx0 = 33 * (33 / 2) + i;
                for (j = 0; j < 4; j++) {
                    pGain[j][idx0] = 0x0;
                }
            }

            AmbaDSP_ImgSetVignetteCompensationByPass(&TmpMode, &TestVigCorrByPass);

            Rval = AMP_OK;
        } else if (isnumber(Argv[2])) {

            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
            AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s VncIfo;

            UINT8 Pattern = 0;
            UINT16 *pGain[4], UnitGain, TestGain;
            int i, j, idx0, idx1, c;

            memset(&TmpMode, 0x0, sizeof(TmpMode));

            VncIfo.Enb = atoi(Argv[2]);

            VncIfo.GainShift = atoi(Argv[3]);

            // Pattern type
            Pattern = atoi(Argv[4]);

            VncIfo.VigStrengthEffectMode = 0;
            VncIfo.ChromaRatio = 0;
            VncIfo.VigStrength = 1 << VIGNETTE_VIG_STRENGTH_UNIT_SHIFT;
            VncTestWidth = VCapWidth;
            VncTestHeight = VCapHeight;
            VncIfo.CurrentVinSensorGeo.StartX = (CALIB_VNC_WIDTH - VncTestWidth) / 2;
            VncIfo.CurrentVinSensorGeo.StartY = (CALIB_VNC_HEIGHT - VncTestHeight) / 2;
            VncIfo.CurrentVinSensorGeo.Width = VncTestWidth;
            VncIfo.CurrentVinSensorGeo.Height = VncTestHeight;
            VncIfo.CurrentVinSensorGeo.HSubSample.FactorNum = 1;
            VncIfo.CurrentVinSensorGeo.HSubSample.FactorDen = 1;
            VncIfo.CurrentVinSensorGeo.VSubSample.FactorNum = 1;
            VncIfo.CurrentVinSensorGeo.VSubSample.FactorDen = 1;
            VncIfo.CalibVignetteInfo.Version = AMBA_DSP_IMG_SBP_VER_1_0;
            VncIfo.CalibVignetteInfo.CalibVinSensorGeo = VncIfo.CurrentVinSensorGeo;
            VncIfo.CalibVignetteInfo.CalibVinSensorGeo.StartX = 0;
            VncIfo.CalibVignetteInfo.CalibVinSensorGeo.StartY = 0;
            VncIfo.CalibVignetteInfo.CalibVinSensorGeo.Width = CALIB_VNC_WIDTH;
            VncIfo.CalibVignetteInfo.CalibVinSensorGeo.Height = CALIB_VNC_HEIGHT;
            VncIfo.CalibVignetteInfo.TableWidth = VIGNETTE_TABLE_WIDTH;
            VncIfo.CalibVignetteInfo.TableHeight = VIGNETTE_TABLE_HEIGTH;
            VncIfo.CalibVignetteInfo.pVignetteRedGain = VignetteRedGainTbl;
            VncIfo.CalibVignetteInfo.pVignetteGreenEvenGain = VignetteGreenEvenGainTbl;
            VncIfo.CalibVignetteInfo.pVignetteGreenOddGain = VignetteGreenOddGainTbl;
            VncIfo.CalibVignetteInfo.pVignetteBlueGain = VignetteBlueGainTbl;
            UnitGain = 1 << VncIfo.GainShift;
            pGain[0] = VncIfo.CalibVignetteInfo.pVignetteRedGain;
            pGain[1] = VncIfo.CalibVignetteInfo.pVignetteGreenEvenGain;
            pGain[2] = VncIfo.CalibVignetteInfo.pVignetteGreenOddGain;
            pGain[3] = VncIfo.CalibVignetteInfo.pVignetteBlueGain;

            for (j = 0; j < 4; j++) {
                for (i = 0; i < VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH; i++) {
                    pGain[j][i] = UnitGain;
                    //gain[j][i] = 0;
                }
            }

            if (Pattern == 0) { // Pattern "+"
                for (i = 0; i < VIGNETTE_TABLE_HEIGTH; i++) {
                    idx1 = VIGNETTE_TABLE_WIDTH * i + VIGNETTE_TABLE_WIDTH / 2;
                    for (j = 0; j < 4; j++) {
                        pGain[j][idx1] = 0x0;
                        //AmbaPrint("idx1: %d, gain[%d][idx1] addr: %d", idx1, j, &gain[j][idx1]);
                    }
                }
                for (i = 0; i < VIGNETTE_TABLE_WIDTH; i++) {
                    idx0 = VIGNETTE_TABLE_WIDTH * (VIGNETTE_TABLE_HEIGTH / 2) + i;
                    for (j = 0; j < 4; j++) {
                        pGain[j][idx0] = 0x0;
                    }
                }
            } else if (Pattern == 1) {
                for (i = 0; i < VIGNETTE_TABLE_WIDTH; i++) {
                    //TestGain = (1023 * i + 16) >> 5;
                    TestGain = (1023 * i * VIGNETTE_TABLE_WIDTH);
                    AmbaPrint("column %d gain = %d", i, TestGain);
                    for (c = 0; c < 4; c++) {
                        for (j = 0; j < VIGNETTE_TABLE_HEIGTH; j++) {
                            idx0 = j * VIGNETTE_TABLE_WIDTH + i;
                            pGain[c][idx0] = TestGain;
                        }
                    }
                }
            } else if (Pattern == 2) {
                for (i = 0; i < VIGNETTE_TABLE_HEIGTH; i++) {
                    //TestGain = (1023 * i + 16) >> 5;
                    TestGain = (1023 * i * VIGNETTE_TABLE_HEIGTH);
                    AmbaPrint("row %d gain = %d", i, TestGain);
                    for (c = 0; c < 4; c++) {
                        for (j = 0; j < VIGNETTE_TABLE_WIDTH; j++) {
                            idx0 = i * VIGNETTE_TABLE_WIDTH + j;
                            pGain[c][idx0] = TestGain;
                        }
                    }
                }
            } else if (Pattern == 3) { // Pattern black screen
                for (c = 0; c < 4; c++)
                    memset(pGain[c], 0, VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH * sizeof(UINT16));
            } else if (Pattern == 4) { // Pattern horizontal black lines
                for (c = 0; c < 4; c++) {
                    for (i = 0; i < VIGNETTE_TABLE_HEIGTH; i++) {
                        if (i % 2 == 0) {
                            memset(&pGain[c][i * VIGNETTE_TABLE_WIDTH], 0x0, VIGNETTE_TABLE_WIDTH * sizeof(UINT16));
                        }
                        if (i % 2 == 1) {
                            memset(&pGain[c][i * VIGNETTE_TABLE_WIDTH], UnitGain, VIGNETTE_TABLE_WIDTH * sizeof(UINT16));
                        }
                    }
                }
            } else if (Pattern == 5) { // Pattern half black screen
                for (c = 0; c < 4; c++) {
                    memset(pGain[c], 0, VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH / 2 * sizeof(UINT16));
                }
            } else if (Pattern == 6) {
                UINT8 VigStrengthMode;
                UINT32 ChromaRatio;
                UINT32 VigStrength;

                for (i = 0; i < VIGNETTE_TABLE_HEIGTH; i++) {
                    for (c = 0; c < 4; c++) {
                        for (j = 0; j < VIGNETTE_TABLE_WIDTH; j++) {
                            idx0 = i * VIGNETTE_TABLE_WIDTH + j;
                            pGain[c][idx0] = UnitGain * (1 + c) * 2;
                        }
                    }
                }
                for (c = 0; c < 4; c++) {
                    AmbaPrint("pGain[%d] = %d", c, UnitGain * (1 + c) * 2);
                }
                VigStrengthMode = atoi(Argv[5]);
                ChromaRatio = atoi(Argv[6]);
                VigStrength = atoi(Argv[7]);
                VncIfo.VigStrengthEffectMode = VigStrengthMode;
                VncIfo.VigStrength = VigStrength;
                VncIfo.ChromaRatio = ChromaRatio;
            }
            /*{
             AMBA_FS_FILE *Fid;
             char *FnAll = "C:\\vig_table_4table.bin";
             char *Fmode = "w";
             Fid = _posix_fopen(FnAll, Fmode);
             if (Fid == NULL) {
             AmbaPrint("file open error");
             return Rval;
             }
             Rval = AmbaFS_fwrite((void const*)VignetteRedGainTbl, sizeof(UINT16),(VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH) , Fid);
             Rval = AmbaFS_fwrite((void const*)VignetteGreenEvenGainTbl, sizeof(UINT16),(VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH) , Fid);
             Rval = AmbaFS_fwrite((void const*)VignetteGreenOddGainTbl, sizeof(UINT16),(VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH) , Fid);
             Rval = AmbaFS_fwrite((void const*)VignetteBlueGainTbl, sizeof(UINT16),(VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH) , Fid);
             AmbaFS_fclose(Fid);

             }*/
            AmbaDSP_ImgCalcVignetteCompensation(&TmpMode, &VncIfo);
            AmbaDSP_ImgSetVignetteCompensation(&TmpMode);

            Rval = AMP_OK;
        } else {
            Rval = AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (Rval == -1)
        AmbaPrint("\n\r"
                  "Usage: t %s -vnc: get the current vignette settings\n\r"
                  "       t %s -vnc [enable] [gain_shift] [pattern]\n\r"
                  "         enable = [0|1], 0: disable, 1: enable\n\r"
                  "         gain_shift = [0-9], 0:10.0, 1:9.1, 2:8.2, 9:1.9\n\r"
                  "         pattern = [0-6],  0: black line test pattern\n\r"
                  "                   1: horizontal test pattern\n\r"
                  "                   2: vertical test pattern\n\r"
                  "                   3: all black test pattern\n\r"
                  "                   4: horizontal black lines\n\r"
                  "                   5: half black screen\n\r"
                  "       t %s -vnc [enable] [gain_shift] [6] [VigStrengthMode] [ChromaRatio] [VigStrength]\n\r"
                  "         VigStrengthMode = [0|1], 0: default mode, 1: Keep ratio mode\n\r"
                  "         ChromaRatio = [0-65535], Tune chroma strength\n\r"
                  "         VigStrength = [0-65535], 0: smallest, 65535:strongest\n\r"
                  "       t %s -vnc load [file] [gain_shift]: load vignette gain table from file\n\r"
                  "       t %s -vnc bypasstest [enable]: bypass function test with the cross pattern\n\r"
                  "         enable = [0|1], 0: disable, 1: enable",
                  Argv[0], Argv[0], Argv[0], Argv[0], Argv[0]);

    return Rval;
}
#endif

static int AppLib_TuneTest(struct _AMBA_SHELL_ENV_s_ *env, int argc, char **argv)
{
    int i = 0;
    for (i = 0; i < sizeof(TuneTestCmd) / sizeof(TuneTestCmd[0]); i++) {
        if (0 == strcmp(argv[1], TuneTestCmd[i].Cmd_ID)) {
            TuneTestCmd[i].Test_Func(argc, argv);
            return 0;
        }
    }
    AppLibTune_Help(argc, argv);
    return 0;
}

int AppLib_TuneTestAdd(void)
{
    AmbaPrint("Adding AppLib_Tune");
    AmbaTest_RegisterCommand(TEST_CMD_TYPE, AppLib_TuneTest);
    return AMP_OK;
}

