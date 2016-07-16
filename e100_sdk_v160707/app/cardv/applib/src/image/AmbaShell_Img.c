#include <AmbaDataType.h>
#include "AmbaPrint.h" // Note: AmbaPrint() need
#include <AmbaPrintk.h> // Note: K_ASSERT need
#include <AmbaKAL.h> // Note: AmbaShell.h need
#include "AmbaShell.h" // Note: AMBA_SHELL_ENV_s need
#include "AmbaDSP_ImgUtility.h" // Note: AMBA_DSP_IMG_PIPE_INFO_s need
#include "AmbaDSP_ImgFilter.h"
#include "AmbaTUNE_HdlrManager.h"
#include <AmbaFS.h> // Note: AMBA_FS_FILE need
#include "AmbaTest.h"
#include "AmbaDSP_ImgHighIsoFilter.h"
#include "AmbaDSP_ImgLowIsoFilter.h"
#include "AmbaDSP_WarpCore.h"
#include <AmbaCache.h>
#include <stdio.h>

#define UT_IK_DEBF(fmt, arg...) AmbaPrint("[IK][UT][DEBUG]%s() %d, "fmt, __func__, __LINE__, ##arg)
#define UT_IK_WARF(fmt, arg...) AmbaPrint("[IK][UT][WARNING]%s() %d, "fmt, __func__, __LINE__, ##arg)
#define UT_IK_ERRF(fmt, arg...) do {AmbaPrint("[IK][UT][ERROR]%s() %d, "fmt, __func__, __LINE__, ##arg); K_ASSERT(0);} while (0)


#define STACK_PUSH(FileInfo) \
    do {\
        if (Rash_Stack->Currect_Stack < Rash_Stack->Max_Depth) {\
            memcpy(&Rash_Stack->File[Rash_Stack->Currect_Stack], &FileInfo, sizeof(File_Info_s));\
            Rash_Stack->Currect_Stack++;\
        } else {\
            UT_IK_ERRF("Stack Overflow");\
        }\
    } while (0)

#define STACK_POP(FileInfo) \
    do {\
        if (Rash_Stack->Currect_Stack > 0) {\
            Rash_Stack->Currect_Stack--;\
            memcpy(&FileInfo, &Rash_Stack->File[Rash_Stack->Currect_Stack], sizeof(File_Info_s));\
            memset(&Rash_Stack->File[Rash_Stack->Currect_Stack], 0x0, sizeof(File_Info_s));\
        } else {\
            UT_IK_ERRF("Stack Underflow");\
        }\
    } while (0)

#define IS_STACK_EMPTY (Rash_Stack->Currect_Stack == 0)

#ifndef GET_ARRAY_NUMBER
#define GET_ARRAY_NUMBER(x) (sizeof(x) / sizeof(x[0]))
#endif

#define WAIT_APP_INFO_RAW_STARTX 0
#define WAIT_APP_INFO_RAW_STARTY 0
#define WAIT_APP_INFO_RAW_WIDTH 0
#define WAIT_APP_INFO_RAW_HEIGHT 0
#define WAIT_APP_INFO_MAIN_WIDTH 0
#define WAIT_APP_INFO_MAIN_HEIGHT 0
#define WAIT_APP_INFO_HSUBSAMPLEFACTORNUM 0
#define WAIT_APP_INFO_HSUBSAMPLEFACTORDEN 0
#define WAIT_APP_INFO_VSUBSAMPLEFACTORNUM 0
#define WAIT_APP_INFO_VSUBSAMPLEFACTORDEN 0

typedef struct
{
    AMBA_DSP_IMG_MODE_CFG_s Mode;
} AMBA_DSP_IMG_TEST_IS2_s;
typedef enum {
    CMD_VISIBLE = 0,
    CMD_HIDDEN,
} AMBA_DSP_IMG_TEST_CMD_Visibility_e;
typedef struct _AMBA_DSP_IMG_TEST_FUNC_s_ {
    int (*Fp)(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv);
    AMBA_DSP_IMG_TEST_CMD_Visibility_e Visibility ;
    char* Command;
    char* Description;
} AMBA_DSP_IMG_TEST_FUNC_s;

typedef struct {
    char FilePath[64];
    int Offset;
} File_Info_s;

typedef struct {
    int Currect_Stack;
    int Max_Depth;
    File_Info_s File[0];
} Rash_Stack_s;

typedef struct {
    char Cmd[255];
    void *NextNode;
} CmdNode_s;

typedef struct {
    int Cmd_Num;
    CmdNode_s *List;
} CmdList_s;

static CmdList_s Cmd_List = {
    .Cmd_Num = 0,
    .List = NULL,
};
static UINT8 IkWorkingBuf[(int)(947016 + 32)];
static AMBA_DSP_IMG_TEST_IS2_s gTestIS2;
extern char _TextHdlr_Get_Driver_Letter(void);
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

static void _remove_windows_text_eof(char *str)
{
    int len;
    len = strlen(str);
    if (len >= 1 && (str[len - 1] == 0xa || str[len - 1] == 0xd))
        str[len - 1] = '\0';
    if (len > 2 && (str[len - 2] == 0xa || str[len - 2] == 0xd))
        str[len - 2] = '\0';
}

static int _string_to_arg(char* cmdline, int* Argc, char *Argv[MAX_CMD_ARGS])
{
    *Argc = 0;
    char* pcmdline;
    pcmdline = strtok(cmdline, " ");
    while (pcmdline && *Argc < MAX_CMD_ARGS) {
        Argv[*Argc] = pcmdline;
        *Argc = *Argc + 1;
        pcmdline = strtok(0, " ");
    }
    return 0;
}
/*
void gen_warp(int w, int h)
{
    static INT16 Warp[12*10*2] = {//  0     1     2     3     4     5     6     7     8     9    10    11
                           -3,-3,-2,-2,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,-1, 2,-2, 3,-3, //0
                           -2,-2,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,-1, 2,-2, //1
                           -1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,-1, //2
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //3
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //4
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //5
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //5
                           -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, //6
                           -2, 2,-1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, //7
                           -3, 3,-2, 2,-1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, //8
    };
//    int i, j;
//    INT16 value1;
//    INT16 value2;
    AMBA_FS_FILE *Fid;
    Fid = AmbaFS_fopen("C:\\gen_warp.bin", "wb");
    AmbaFS_fwrite((void*)Warp, sizeof(Warp), 1, Fid);
    AmbaFS_fclose(Fid);
}
*/
static int test_is2_init(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    INT32 Rval = 0, TotalSize = 0;
    AMBA_DSP_IMG_PIPE_INFO_s PipeInfo_V;
    AMBA_DSP_IMG_PIPE_INFO_s PipeInfo_S;
    AMBA_DSP_IMG_ARCH_INFO_s ArchInfo;
    AMBA_DSP_IMG_CTX_INFO_s DestCtx;
    AMBA_DSP_IMG_CTX_INFO_s SrcCtx;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;

    memset(&PipeInfo_V, 0x0, sizeof(PipeInfo_V));
    PipeInfo_V.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    PipeInfo_V.CtxBufNum = 1;
    PipeInfo_V.CfgBufNum = 1;
    memset(&PipeInfo_S, 0x0, sizeof(PipeInfo_S));
    PipeInfo_S.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    PipeInfo_S.CtxBufNum = 1;
    PipeInfo_S.CfgBufNum = 1;
    memset(&ArchInfo, 0x0, sizeof(ArchInfo));
    ArchInfo.pWorkBuf = &IkWorkingBuf[((UINT32) IkWorkingBuf & (32 - 1)) ? (32 - ((UINT32) IkWorkingBuf & (32 - 1))) : 0];

    ArchInfo.BufSize = sizeof(IkWorkingBuf) - 31;
    ArchInfo.PipeNum = 2;
    ArchInfo.pPipeInfo[0] = &PipeInfo_V;
    ArchInfo.pPipeInfo[1] = &PipeInfo_S;

    TotalSize = AmbaDSP_ImgQueryArchMemSize(&ArchInfo);
    UT_IK_DEBF("Total size needed %d Bytes", TotalSize);
    if (TotalSize > sizeof(IkWorkingBuf)) {
        UT_IK_ERRF("Working Buffer size not enough");
        return -1;
    }
    AmbaDSP_ImgInitArch(&ArchInfo);
    memset(&DestCtx, 0x0, sizeof(DestCtx));
    memset(&SrcCtx, 0x0, sizeof(SrcCtx));
    DestCtx.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    DestCtx.CtxId = 0;
    AmbaDSP_ImgInitCtx(0, 0, &DestCtx, &SrcCtx);
    DestCtx.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    DestCtx.CtxId = 0;

    AmbaDSP_ImgInitCtx(0, 0, &DestCtx, &SrcCtx);
    memset(&CfgInfo, 0x0, sizeof(CfgInfo));
    CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    CfgInfo.CfgId = 0;
    AmbaDSP_ImgInitCfg(&CfgInfo, AMBA_DSP_IMG_ALGO_MODE_LISO);

    AmbaDSP_WarpCore_Init();
    if (1) {   // vin sensor info.
        AMBA_DSP_IMG_SENSOR_INFO_s VinSensorInfo = { 0 };

        VinSensorInfo.SensorID = 0;
        VinSensorInfo.NumFieldsPerFormat = 1;
        VinSensorInfo.SensorResolution = 12;
        VinSensorInfo.SensorPattern = 0;
        AmbaDSP_ImgSetVinSensorInfo(&gTestIS2.Mode, &VinSensorInfo);
    }

    return Rval;
}

static int test_is2_demosaic(AMBA_SHELL_ENV_s *Env, int Argc, char**Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_DEMOSAIC_s Demosaic;
    AmbaDSP_ImgGetDemosaic(&gTestIS2.Mode, &Demosaic);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current demosaic ActivityDifferenceThresh: %d, ActivityThresh: %d, GradClipThresh: %d, GradNoiseThresh: %d",
                      Demosaic.ActivityDifferenceThresh, Demosaic.ActivityThresh, Demosaic.GradClipThresh, Demosaic.GradNoiseThresh);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            Demosaic.ActivityDifferenceThresh = atoi(Argv[2]);
            Demosaic.ActivityThresh = atoi(Argv[3]);
            Demosaic.GradClipThresh = atoi(Argv[4]);
            Demosaic.GradNoiseThresh = atoi(Argv[5]);
            Rval = AmbaDSP_ImgSetDemosaic(&gTestIS2.Mode, &Demosaic);
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -demosaic: get the current demosaic setting\n\r"
                  "       t %s -demosaic [ActivityDifferenceThresh] [ActivityThresh] [GradClipThresh] [GradNoiseThresh]: set the demosaic setting\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_blc(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_BLACK_CORRECTION_s BlackCorr;

    AmbaDSP_ImgGetStaticBlackLevel(&gTestIS2.Mode, &BlackCorr);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current black level: %d %d %d %d\n", (int) BlackCorr.BlackR, (int) BlackCorr.BlackGr, (int) BlackCorr.BlackGb, (int) BlackCorr.BlackB);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            BlackCorr.BlackR = atoi(Argv[2]);
            BlackCorr.BlackGr = atoi(Argv[3]);
            BlackCorr.BlackGb = atoi(Argv[3]);
            BlackCorr.BlackB = atoi(Argv[4]);
            AmbaDSP_ImgSetStaticBlackLevel(&gTestIS2.Mode, &BlackCorr);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1)
        AmbaPrint("\n\r"
                  "Usage: t %s -blc: get the current black level\n\r"
                  "       t %s -blc [r_black] [g_black] [b_black]: set the current black level\n\r",
                  Argv[0], Argv[0]);
    return Rval;
}

static int test_is2_wb(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_WB_GAIN_s WbGain;

    AmbaDSP_ImgGetWbGain(&gTestIS2.Mode, &WbGain);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current wb gain: R:%d G:%d B:%d\n", (int) WbGain.GainR, (int) WbGain.GainG, (int) WbGain.GainB);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            WbGain.GainR = atoi(Argv[2]);
            WbGain.GainG = atoi(Argv[3]);
            WbGain.GainB = atoi(Argv[4]);
            AmbaDSP_ImgSetWbGain(&gTestIS2.Mode, &WbGain);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1)
        AmbaPrint("\n\r"
                  "Usage: t %s -wb: get the current wb gain\n\r"
                  "       t %s -wb [r_gain] [g_gain] [b_gain]:set the current wb gain\n\r",
                  Argv[0], Argv[0]);
    return Rval;
}

static int test_is2_r2y(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_RGB_TO_YUV_s R2yInfo;

    AmbaDSP_ImgGetRgbToYuvMatrix(&gTestIS2.Mode, &R2yInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current R2Y Matrix: %4d %4d %4d", (int) R2yInfo.MatrixValues[0], (int) R2yInfo.MatrixValues[1], (int) R2yInfo.MatrixValues[2]);
            AmbaPrint("                    %4d %4d %4d", (int) R2yInfo.MatrixValues[3], (int) R2yInfo.MatrixValues[4], (int) R2yInfo.MatrixValues[5]);
            AmbaPrint("                    %4d %4d %4d", (int) R2yInfo.MatrixValues[6], (int) R2yInfo.MatrixValues[7], (int) R2yInfo.MatrixValues[8]);
            AmbaPrint("current YUV offset: Y:%4d U:%4d V:%4d", (int) R2yInfo.YOffset, (int) R2yInfo.UOffset, (int) R2yInfo.VOffset);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            int i;
            for (i = 0; i < 9; i++) {
                R2yInfo.MatrixValues[i] = atoi(Argv[i + 2]);
            }
            R2yInfo.YOffset = atoi(Argv[11]);
            R2yInfo.UOffset = atoi(Argv[12]);
            R2yInfo.VOffset = atoi(Argv[13]);
            AmbaDSP_ImgSetRgbToYuvMatrix(&gTestIS2.Mode, &R2yInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -r2y: get the current R2Y setting\n\r"
                  "       t %s -r2y [r2y_mat_0]...[r2y_mat_8] [y_off] [u_off] [v_off]: set the current R2Y setting\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_cc(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_COLOR_CORRECTION_s CcInfo;
    AMBA_FS_FILE *Fid;

    AmbaDSP_ImgGetColorCorrection(&gTestIS2.Mode, &CcInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current cc 3d table addr:0x%8x \n", CcInfo.MatrixThreeDTableAddr);
            Rval = 0;
        } else if (strcmp(Argv[2], "load") == 0) {
            const int cc_3d_index = 3;
            Fid = AmbaFS_fopen(Argv[cc_3d_index], "r");
            AmbaFS_fread((void *) CcInfo.MatrixThreeDTableAddr, 1, AMBA_DSP_IMG_CC_3D_SIZE, Fid);
            AmbaFS_fclose(Fid);
            AmbaDSP_ImgSetColorCorrection(&gTestIS2.Mode, &CcInfo);
            Rval = 0;
        } else {
            Rval = -1;
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -cc: get the current cc table addresses\n\r"
                  "       t %s -cc load [cc_3d_path]: set the current cc table\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_tone(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_TONE_CURVE_s ToneInfo;
    AMBA_FS_FILE *Fid;

    AmbaDSP_ImgGetToneCurve(&gTestIS2.Mode, &ToneInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            int i, j, c;
            char ColorName[3][6] = { "RED  \0", "GREEN\0", "BLUE \0" };
            UINT16 *TonePtr[3];
            TonePtr[0] = ToneInfo.ToneCurveRed;
            TonePtr[1] = ToneInfo.ToneCurveGreen;
            TonePtr[2] = ToneInfo.ToneCurveBlue;
            for (c = 0; c < 3; c++) {
                AmbaPrint("Current tone curve %s\n", ColorName[c]);
                for (i = 0; i < 16; i++) {
                    for (j = 0; j < 16; j++) {
                        AmbaPrint("%4d ", TonePtr[c][i * 16 + j]);
                    }
                    AmbaPrint("\n");
                }
            }
            Rval = 0;
        } else if (strcmp(Argv[2], "load") == 0) {
            Fid = AmbaFS_fopen(Argv[3], "r");
            AmbaFS_fread((void *) &ToneInfo, 1, sizeof(AMBA_DSP_IMG_TONE_CURVE_s), Fid);
            AmbaFS_fclose(Fid);
            AmbaDSP_ImgSetToneCurve(&gTestIS2.Mode, &ToneInfo);
            Rval = 0;
        } else if (strcmp(Argv[2], "set_linear") == 0) {
            int i;
            for (i = 0; i < 256; i++) {
                ToneInfo.ToneCurveRed[i] = ToneInfo.ToneCurveGreen[i] = ToneInfo.ToneCurveBlue[i] = i * 4 + 2;
            }
            AmbaDSP_ImgSetToneCurve(&gTestIS2.Mode, &ToneInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -tone: get the current tone curve\n\r"
                  "       t %s -tone load [tone_path]: set the tone curve\n\r"
                  "       t %s -tone set_linear: set linear tone curve\n\r",
                  Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_dpc(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_DBP_CORRECTION_s DpcInfo;

    AmbaDSP_ImgGetDynamicBadPixelCorrection(&gTestIS2.Mode, &DpcInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current dynamic bad pixel correction setting: enable:%d hot_str:%d dark_str:%d cor_method: %d\n",
                      (int) DpcInfo.Enb,
                      (int) DpcInfo.HotPixelStrength,
                      (int) DpcInfo.DarkPixelStrength,
                      (int) DpcInfo.CorrectionMethod);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            DpcInfo.Enb = atoi(Argv[2]);
            DpcInfo.HotPixelStrength = atoi(Argv[3]);
            DpcInfo.DarkPixelStrength = atoi(Argv[4]);
            DpcInfo.CorrectionMethod = atoi(Argv[5]);
            AmbaDSP_ImgSetDynamicBadPixelCorrection(&gTestIS2.Mode, &DpcInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -dpc: get the current dynamic bad pixel correction setting\n\r"
                  "       t %s -dpc [enable] [hot_str] [dark_str] [cor_method]: set the current dpc setting\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_antialias(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    UINT8 AaStr;

    AmbaDSP_ImgGetAntiAliasing(&gTestIS2.Mode, &AaStr);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current antialiasing strength: %d\n", AaStr);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            AaStr = atoi(Argv[2]);
            AmbaDSP_ImgSetAntiAliasing(&gTestIS2.Mode, AaStr);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -aa: get the current antialiasing strength\n\r"
                  "       t %s -aa [str]: set the current antialiasing strength\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_leakage(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s LeakageInfo;

    AmbaDSP_ImgGetCfaLeakageFilter(&gTestIS2.Mode, &LeakageInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current leakage filter settings: enable:%d alpha_rr:%d alpha_rb:%d alpha_br:%d alpha_bb:%d sat_level:%d\n",
                      LeakageInfo.Enb,
                      LeakageInfo.AlphaRR,
                      LeakageInfo.AlphaRB,
                      LeakageInfo.AlphaBR,
                      LeakageInfo.AlphaBB,
                      LeakageInfo.SaturationLevel);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            LeakageInfo.Enb = atoi(Argv[2]);
            LeakageInfo.AlphaRR = atoi(Argv[3]);
            LeakageInfo.AlphaRB = atoi(Argv[4]);
            LeakageInfo.AlphaBR = atoi(Argv[5]);
            LeakageInfo.AlphaBB = atoi(Argv[6]);
            LeakageInfo.SaturationLevel = atoi(Argv[7]);
            AmbaDSP_ImgSetCfaLeakageFilter(&gTestIS2.Mode, &LeakageInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -lk: get the current leakage filter settings\n\r"
                  "       t %s -lk [enable] [rr] [rb] [br] [bb] [sat_lvl]: set the current leakage filter settings\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_cnf(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s CnfInfo;

    AmbaDSP_ImgGetCfaNoiseFilter(&gTestIS2.Mode, &CnfInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current CFA noise filter settings:\nenable:%d ", CnfInfo.Enb);
            AmbaPrint("noise_level R:%d G:%d B:%d ", CnfInfo.NoiseLevel[0], CnfInfo.NoiseLevel[1], CnfInfo.NoiseLevel[2]);
            AmbaPrint("original_blend_str R:%d G:%d B:%d", CnfInfo.OriginalBlendStr[0], CnfInfo.OriginalBlendStr[1], CnfInfo.OriginalBlendStr[2]);
            AmbaPrint("extent_regular R:%d G:%d B:%d ", CnfInfo.ExtentRegular[0], CnfInfo.ExtentRegular[1], CnfInfo.ExtentRegular[2]);
            AmbaPrint("extent_fine R:%d G:%d B:%d ", CnfInfo.ExtentFine[0], CnfInfo.ExtentFine[1], CnfInfo.ExtentFine[2]);
            AmbaPrint("strength_fine R:%d G:%d B:%d ", CnfInfo.StrengthFine[0], CnfInfo.StrengthFine[1], CnfInfo.StrengthFine[2]);
            AmbaPrint("selectivity_regular:%d selectivity_fine:%d", CnfInfo.SelectivityRegular, CnfInfo.SelectivityFine);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            CnfInfo.Enb = atoi(Argv[2]);
            if (strcmp(Argv[3], "default") == 0) {
                CnfInfo.NoiseLevel[0] = 400;
                CnfInfo.NoiseLevel[1] = 400;
                CnfInfo.NoiseLevel[2] = 400;
                CnfInfo.OriginalBlendStr[0] = 0;
                CnfInfo.OriginalBlendStr[1] = 0;
                CnfInfo.OriginalBlendStr[2] = 0;
                CnfInfo.ExtentRegular[0] = 100;
                CnfInfo.ExtentRegular[1] = 100;
                CnfInfo.ExtentRegular[2] = 100;
                CnfInfo.ExtentFine[0] = 20;
                CnfInfo.ExtentFine[1] = 20;
                CnfInfo.ExtentFine[2] = 20;
                CnfInfo.StrengthFine[0] = 120;
                CnfInfo.StrengthFine[1] = 120;
                CnfInfo.StrengthFine[2] = 120;
                CnfInfo.SelectivityRegular = 0;
                CnfInfo.SelectivityFine = 0;
            }
            AmbaDSP_ImgSetCfaNoiseFilter(&gTestIS2.Mode, &CnfInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -cnf: get the current CFA noise filter settings\n\r"
                  "       t %s -cnf [enable] default : set the CFA noise filter strength to weak,mid or strong settings\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_le(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_LOCAL_EXPOSURE_s LeInfo;
    AMBA_FS_FILE *Fid;

    AmbaDSP_ImgGetLocalExposure(&gTestIS2.Mode, &LeInfo);

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            int i, j;
            AmbaPrint("current local exposure settings: enable:%d radius:%d\n"
                      "luma_weight_red:%d\n"
                      "luma_weight_green:%d\n"
                      "luma_weight_blue:%d\n"
                      "luma_weight_shift:%d\n",
                      LeInfo.Enb, LeInfo.Radius, LeInfo.LumaWeightRed, LeInfo.LumaWeightGreen, LeInfo.LumaWeightBlue, LeInfo.LumaWeightShift);
            AmbaPrint("Current gain_curve_table\n");
            for (i = 0; i < 16; i++) {
                for (j = 0; j < 16; j++) {
                    AmbaPrint("%4d ", LeInfo.GainCurveTable[i * 16 + j]);
                }
                AmbaPrint("\n");
            }
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            LeInfo.Enb = atoi(Argv[2]);
            LeInfo.Radius = atoi(Argv[3]);
            LeInfo.LumaWeightRed = atoi(Argv[4]);
            LeInfo.LumaWeightGreen = atoi(Argv[5]);
            LeInfo.LumaWeightBlue = atoi(Argv[6]);
            LeInfo.LumaWeightShift = atoi(Argv[7]);
            if (Argc > 8) {
                Fid = AmbaFS_fopen(Argv[8], "r");
                AmbaFS_fread((void *) LeInfo.GainCurveTable, 2, AMBA_DSP_IMG_NUM_EXPOSURE_CURVE, Fid);
                AmbaFS_fclose(Fid);
            } else {
                AmbaPrint("Using default LE table");
                const UINT16 DefLeTable[AMBA_DSP_IMG_NUM_EXPOSURE_CURVE] = \
                {
                  982,  1576,  1552,  1484,  1405,  1326,  1250,  1181,  1116,  1058,  1006,   958,   918,   881,   848,   820,
                  795,   774,   756,   740,   727,   717,   707,   701,   696,   691,   689,   688,   687,   688,   689,   691,
                  694,   697,   700,   704,   708,   711,   715,   719,   722,   726,   730,   732,   736,   738,   742,   745,
                  747,   750,   752,   756,   758,   761,   765,   767,   770,   772,   776,   780,   783,   785,   789,   793,
                  797,   800,   804,   808,   810,   814,   818,   821,   824,   828,   831,   834,   838,   841,   844,   847,
                  849,   852,   854,   856,   858,   861,   863,   866,   868,   870,   873,   875,   878,   881,   883,   886,
                  889,   891,   895,   898,   902,   905,   909,   913,   917,   921,   926,   930,   934,   939,   943,   948,
                  952,   957,   962,   967,   971,   976,   981,   986,   991,   995,  1000,  1005,  1010,  1014,  1019,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,
                 1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024
                 };

                memcpy(LeInfo.GainCurveTable, DefLeTable, sizeof(UINT16) * AMBA_DSP_IMG_NUM_EXPOSURE_CURVE);
            }
            AmbaDSP_ImgSetLocalExposure(&gTestIS2.Mode, &LeInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -le: get the current local exposure setting\n\r"
                  "       t %s -le [enable] [rad] [lum_wgt_r] [lum_wgt_g] [lum_wgt_b] [lum_wgt_shift] [gain_curve_path]\n\r"
                  "        : set the current local exposure setting\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_cs(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_CHROMA_SCALE_s CsInfo;
    AMBA_FS_FILE *Fid;

    AmbaDSP_ImgGetChromaScale(&gTestIS2.Mode, &CsInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            int i, j;
            AmbaPrint("current chroma scale settings: enable:%d\n", CsInfo.Enb);
            AmbaPrint("Current gain_curve_table\n");
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 16; j++) {
                    AmbaPrint("%4d ", CsInfo.GainCurve[i * 16 + j]);
                }
                AmbaPrint("\n");
            }
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            CsInfo.Enb = atoi(Argv[2]);
            if (Argc > 3) {
                Fid = AmbaFS_fopen(Argv[3], "r");
                AmbaFS_fread(CsInfo.GainCurve, 2, AMBA_DSP_IMG_NUM_CHROMA_GAIN_CURVE, Fid);
                AmbaFS_fclose(Fid);
            } else {
                AmbaPrint("Using default chroma scale table");
                const UINT16 DefCsTable[AMBA_DSP_IMG_NUM_EXPOSURE_CURVE] = {2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
                                                                            2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048
                                                                            };
                memcpy((void *) CsInfo.GainCurve, DefCsTable, sizeof(UINT16) * AMBA_DSP_IMG_NUM_CHROMA_GAIN_CURVE);
            }
            AmbaDSP_ImgSetChromaScale(&gTestIS2.Mode, &CsInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -cs: get the current cc table addresses\n\r"
                  "       t %s -cs [enable] [gain_curve_path]\n\r"
                  "        : set the current chroma scale setting\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_cmf(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s CmfInfo;

    AmbaDSP_ImgGetChromaMedianFilter(&gTestIS2.Mode, &CmfInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current chroma median filter settings: Enable:%d \
                    CbAdaptiveStrength:%d CrAdaptiveStrength:%d \
                    CbNonAdaptiveStrength:%d CrNonAdaptiveStrength:%d \
                    CbAdaptiveAmount:%d CrAdaptiveAmount:%d \n",
                      CmfInfo.Enable,
                      CmfInfo.CbAdaptiveStrength,
                      CmfInfo.CrAdaptiveStrength,
                      CmfInfo.CbNonAdaptiveStrength,
                      CmfInfo.CrNonAdaptiveStrength,
                      CmfInfo.CbAdaptiveAmount,
                      CmfInfo.CrAdaptiveAmount);
            Rval = 0;
        } else if (isnumber(Argv[2])) {

            CmfInfo.Enable = atoi(Argv[2]);
            CmfInfo.CbAdaptiveStrength = atoi(Argv[3]);
            CmfInfo.CrAdaptiveStrength = atoi(Argv[4]);
            CmfInfo.CbNonAdaptiveStrength = atoi(Argv[5]);
            CmfInfo.CrNonAdaptiveStrength = atoi(Argv[6]);
            CmfInfo.CbAdaptiveAmount = atoi(Argv[7]);
            CmfInfo.CrAdaptiveAmount = atoi(Argv[8]);
            AmbaDSP_ImgSetChromaMedianFilter(&gTestIS2.Mode, &CmfInfo);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -cmf: get the current chroma median filter settings\n\r"
                  "       t %s -cmf [enable] [cb_adp_str] [cr_adp_str] [cb_non_adp_str] [cr_non_adp_str] [cb_adp_amount] [cr_adp_amount]:\n\r"
                  "       set chroma median fitler\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_cc_reg(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_COLOR_CORRECTION_REG_s CcReg;
    AMBA_FS_FILE *Fid;

    AmbaDSP_ImgGetColorCorrectionReg(&gTestIS2.Mode, &CcReg);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current cc reg table addr:0x%8x\n", CcReg.RegSettingAddr);
            Rval = 0;
        } else if (strcmp(Argv[2], "load") == 0) {
            Fid = AmbaFS_fopen(Argv[3], "r");
            AmbaFS_fread((void *) CcReg.RegSettingAddr, 1, AMBA_DSP_IMG_CC_REG_SIZE, Fid);
            AmbaFS_fclose(Fid);
            AmbaDSP_ImgSetColorCorrectionReg(&gTestIS2.Mode, &CcReg);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -cc_reg: get the current cc reg table addresses\n"
                  "       t %s -cc_reg load [cc_reg_path] : set the current cc reg table\n"
                  "\n",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_defblc(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_DEF_BLC_s DefBlc;

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current def blc setting : Not yet implemented\n");
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            DefBlc.Enb = atoi(Argv[2]);
            AmbaDSP_ImgSetDeferredBlackLevel(&gTestIS2.Mode, &DefBlc);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -defblc: get the current defblc setting\n\r"
                  "       t %s -defblc [enable]: set defblc enable/disable\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_stat(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_AAA_STAT_INFO_s AaaStatInfo;
    AMBA_DSP_IMG_AF_STAT_EX_INFO_s AfStatEx;
    AmbaDSP_Img3aGetAaaStatInfo(&gTestIS2.Mode, &AaaStatInfo);
    AmbaDSP_Img3aGetAfStatExInfo(&gTestIS2.Mode, &AfStatEx);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            int i = 0;
            AmbaPrint("current AWB statistics tile settings:");
            AmbaPrint("awb_tile_num_col:%d", AaaStatInfo.AwbTileNumCol);
            AmbaPrint("awb_tile_num_row:%d", AaaStatInfo.AwbTileNumRow);
            AmbaPrint("awb_tile_col_start:%d", AaaStatInfo.AwbTileColStart);
            AmbaPrint("awb_tile_row_start:%d", AaaStatInfo.AwbTileRowStart);
            AmbaPrint("awb_tile_width:%d", AaaStatInfo.AwbTileWidth);
            AmbaPrint("awb_tile_height:%d", AaaStatInfo.AwbTileHeight);
            AmbaPrint("awb_tile_active_width:%d", AaaStatInfo.AwbTileActiveWidth);
            AmbaPrint("awb_tile_active_height:%d", AaaStatInfo.AwbTileActiveHeight);
            AmbaPrint("awb_pix_min_value:%d", AaaStatInfo.AwbPixMinValue);
            AmbaPrint("awb_pix_max_value:%d", AaaStatInfo.AwbPixMaxValue);
            AmbaPrint("current AE statistics tile settings:");
            AmbaPrint("ae_tile_num_col:%d", AaaStatInfo.AeTileNumCol);
            AmbaPrint("ae_tile_num_row:%d", AaaStatInfo.AeTileNumRow);
            AmbaPrint("ae_tile_col_start:%d", AaaStatInfo.AeTileColStart);
            AmbaPrint("ae_tile_row_start:%d", AaaStatInfo.AeTileRowStart);
            AmbaPrint("ae_tile_width:%d", AaaStatInfo.AeTileWidth);
            AmbaPrint("ae_tile_height:%d", AaaStatInfo.AeTileHeight);
            AmbaPrint("ae_pix_min_value:%d", AaaStatInfo.AePixMinValue);
            AmbaPrint("ae_pix_max_value:%d", AaaStatInfo.AePixMaxValue);
            AmbaPrint("current AF statistics tile settings:");
            AmbaPrint("af_tile_num_col:%d", AaaStatInfo.AfTileNumCol);
            AmbaPrint("af_tile_num_row:%d", AaaStatInfo.AfTileNumRow);
            AmbaPrint("af_tile_col_start:%d", AaaStatInfo.AfTileColStart);
            AmbaPrint("af_tile_row_start:%d", AaaStatInfo.AfTileRowStart);
            AmbaPrint("af_tile_width:%d", AaaStatInfo.AfTileWidth);
            AmbaPrint("af_tile_height:%d", AaaStatInfo.AfTileHeight);
            AmbaPrint("af_tile_active_width:%d", AaaStatInfo.AfTileActiveWidth);
            AmbaPrint("af_tile_active_height:%d", AaaStatInfo.AfTileActiveHeight);
            AmbaPrint("current Af statistics extention settings:");
            AmbaPrint("AfHorizontalFilter1Mode:%d", AfStatEx.AfHorizontalFilter1Mode);
            AmbaPrint("AfHorizontalFilter1Stage1Enb:%d", AfStatEx.AfHorizontalFilter1Stage1Enb);
            AmbaPrint("AfHorizontalFilter1Stage2Enb:%d", AfStatEx.AfHorizontalFilter1Stage2Enb);
            AmbaPrint("AfHorizontalFilter1Stage3Enb:%d", AfStatEx.AfHorizontalFilter1Stage3Enb);
            for (i = 0; i < 7; i++)
                AmbaPrint("AfHorizontalFilter1Gain[%d]:%d", i, AfStatEx.AfHorizontalFilter1Gain[i]);
            for (i = 0; i < 4; i++)
                AmbaPrint("AfHorizontalFilter1Shift[%d]:%d", i, AfStatEx.AfHorizontalFilter1Shift[i]);
            AmbaPrint("AfHorizontalFilter1BiasOff:%d", AfStatEx.AfHorizontalFilter1BiasOff);
            AmbaPrint("AfHorizontalFilter1Thresh:%d", AfStatEx.AfHorizontalFilter1Thresh);
            AmbaPrint("AfVerticalFilter1Thresh:%d", AfStatEx.AfVerticalFilter1Thresh);
            AmbaPrint("AfHorizontalFilter2Mode:%d", AfStatEx.AfHorizontalFilter2Mode);
            AmbaPrint("AfHorizontalFilter2Stage1Enb:%d", AfStatEx.AfHorizontalFilter2Stage1Enb);
            AmbaPrint("AfHorizontalFilter2Stage2Enb:%d", AfStatEx.AfHorizontalFilter2Stage2Enb);
            AmbaPrint("AfHorizontalFilter2Stage3Enb:%d", AfStatEx.AfHorizontalFilter2Stage3Enb);
            for (i = 0; i < 7; i++)
                AmbaPrint("AfHorizontalFilter2Gain[%d]:%d", i, AfStatEx.AfHorizontalFilter2Gain[i]);
            for (i = 0; i < 4; i++)
                AmbaPrint("AfHorizontalFilter2Shift[%d]:%d", i, AfStatEx.AfHorizontalFilter2Shift[i]);
            AmbaPrint("AfHorizontalFilter2BiasOff:%d", AfStatEx.AfHorizontalFilter2BiasOff);
            AmbaPrint("AfHorizontalFilter2Thresh:%d", AfStatEx.AfHorizontalFilter2Thresh);
            AmbaPrint("AfVerticalFilter2Thresh:%d", AfStatEx.AfVerticalFilter2Thresh);
            AmbaPrint("AfTileFv1HorizontalShift:%d", AfStatEx.AfTileFv1HorizontalShift);
            AmbaPrint("AfTileFv1VerticalShift:%d", AfStatEx.AfTileFv1VerticalShift);
            AmbaPrint("AfTileFv1HorizontalWeight:%d", AfStatEx.AfTileFv1HorizontalWeight);
            AmbaPrint("AfTileFv1VerticalWeight:%d", AfStatEx.AfTileFv1VerticalWeight);
            AmbaPrint("AfTileFv2HorizontalShift:%d", AfStatEx.AfTileFv2HorizontalShift);
            AmbaPrint("AfTileFv2VerticalShift:%d", AfStatEx.AfTileFv2VerticalShift);
            AmbaPrint("AfTileFv2HorizontalWeight:%d", AfStatEx.AfTileFv2HorizontalWeight);
            AmbaPrint("AfTileFv2VerticalWeight:%d", AfStatEx.AfTileFv2VerticalWeight);
            Rval = 0;
        } else if (strcmp(Argv[2], "config") == 0) {
            if (strcmp(Argv[3], "ae") == 0) {
                AMBA_DSP_IMG_AE_STAT_INFO_s AeStat;
                AeStat.AeTileNumCol = atoi(Argv[4]);
                AeStat.AeTileNumRow = atoi(Argv[5]);
                AeStat.AeTileColStart = atoi(Argv[6]);
                AeStat.AeTileRowStart = atoi(Argv[7]);
                AeStat.AeTileWidth = atoi(Argv[8]);
                AeStat.AeTileHeight = atoi(Argv[9]);
                AeStat.AePixMinValue = atoi(Argv[10]);
                AeStat.AePixMaxValue = atoi(Argv[11]);
                AmbaDSP_Img3aSetAeStatInfo(&gTestIS2.Mode, &AeStat);
            } else if (strcmp(Argv[3], "awb") == 0) {
                AMBA_DSP_IMG_AWB_STAT_INFO_s AwbStat;
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
                AmbaDSP_Img3aSetAwbStatInfo(&gTestIS2.Mode, &AwbStat);
            } else if (strcmp(Argv[3], "af") == 0) {
                AMBA_DSP_IMG_AF_STAT_INFO_s AfStat;
                AfStat.AfTileNumCol = atoi(Argv[4]);
                AfStat.AfTileNumRow = atoi(Argv[5]);
                AfStat.AfTileColStart = atoi(Argv[6]);
                AfStat.AfTileRowStart = atoi(Argv[7]);
                AfStat.AfTileWidth = atoi(Argv[8]);
                AfStat.AfTileHeight = atoi(Argv[9]);
                AfStat.AfTileActiveWidth = atoi(Argv[10]);
                AfStat.AfTileActiveHeight = atoi(Argv[11]);
                AmbaDSP_Img3aSetAfStatInfo(&gTestIS2.Mode, &AfStat);
            } else if (strcmp(Argv[3], "all") == 0) {
                AMBA_DSP_IMG_AAA_STAT_INFO_s AaaStat;
                AaaStat.AwbTileNumCol = 24;
                AaaStat.AwbTileNumRow = 16;
                AaaStat.AwbTileColStart = 8;
                AaaStat.AwbTileRowStart = 0;
                AaaStat.AwbTileWidth = 170;
                AaaStat.AwbTileHeight = 256;
                AaaStat.AwbTileActiveWidth = 170;
                AaaStat.AwbTileActiveHeight = 256;
                AaaStat.AwbPixMinValue = 0;
                AaaStat.AwbPixMaxValue = 16383;

                AaaStat.AeTileNumCol = 12;
                AaaStat.AeTileNumRow = 8;
                AaaStat.AeTileColStart = 8;
                AaaStat.AeTileRowStart = 0;
                AaaStat.AeTileWidth = 340;
                AaaStat.AeTileHeight = 512;
                AaaStat.AePixMinValue = 0;
                AaaStat.AePixMaxValue = 16383;

                AaaStat.AfTileNumCol = 8;
                AaaStat.AfTileNumRow = 5;
                AaaStat.AfTileColStart = 256;
                AaaStat.AfTileRowStart = 10;
                AaaStat.AfTileWidth = 448;
                AaaStat.AfTileHeight = 816;
                AaaStat.AfTileActiveWidth = 448;
                AaaStat.AfTileActiveHeight = 816;
                AmbaDSP_Img3aSetAaaStatInfo(&gTestIS2.Mode, &AaaStat);
            }
            Rval = 0;
        }
    }

    if (Rval == -1) {
        AmbaPrint("Usage: t %s -stat: get the current aaa statistics settings\n", Argv[0]);
        AmbaPrint("       t %s -stat config ae  [num_w] [num_h] [col_start] [row_start] [tile_w] [tile_h] [pix_min] [pix_max]: set AE statistics settings", Argv[0]);
        AmbaPrint("       t %s -stat config awb [num_w] [num_h] [col_start] [row_start] [tile_w] [tile_h] [act_w] [act_h] [pix_min] [pix_max]: set AWB statistics settings", Argv[0]);
        AmbaPrint("       t %s -stat config af  [num_w] [num_h] [col_start] [row_start] [tile_w] [tile_h] [act_w] [act_h]: set AF statistics settings", Argv[0]);
//        AmbaPrint("       t %s -stat show [ae|awb|af|hist] : Show AE/AWB/AF/histogram statistics", Argv[0]);
//        AmbaPrint("       t %s -stat dump [rgb_path] [cfa_path]: dump CFA + RGB statistics to storage", Argv[0]);
//        AmbaPrint("       t %s -stat save [txt path]: dump statistics to text file", Argv[0]);
    }
    return Rval;
}

static int test_is2_float(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1, i;
    AMBA_DSP_IMG_AAA_FLOAT_TILE_INFO_s FloatAaaInfo;
    AmbaDSP_Img3aGetAaaFloatTileInfo(&gTestIS2.Mode, &FloatAaaInfo);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            Rval = 0;
            AmbaPrint("float_aaa_info.frame_sync_id %d", FloatAaaInfo.FrameSyncId);
            AmbaPrint("float_aaa_info.number_of_tiles %d", FloatAaaInfo.NumberOfTiles);
            for (i = 0; i < FloatAaaInfo.NumberOfTiles; i++) {
                AmbaPrint("[%d].float_tile_col_start %d", i, FloatAaaInfo.FloatTileConfig[i].FloatTileColStart);
                AmbaPrint("[%d].float_tile_row_start %d", i, FloatAaaInfo.FloatTileConfig[i].FloatTileRowStart);
                AmbaPrint("[%d].float_tile_width %d", i, FloatAaaInfo.FloatTileConfig[i].FloatTileWidth);
                AmbaPrint("[%d].float_tile_height %d", i, FloatAaaInfo.FloatTileConfig[i].FloatTileHeight);
                AmbaPrint("[%d].float_tile_shift %d", i, FloatAaaInfo.FloatTileConfig[i].FloatTileShift);
            }
        } else if (strcmp(Argv[2], "show") == 0) {
            if (strcmp(Argv[3], "ae") == 0) {
                UT_IK_DEBF("Not impl in A12 yet");
            } else if (strcmp(Argv[3], "awb") == 0) {
                UT_IK_DEBF("Not impl in A12 yet");
            } else if (strcmp(Argv[3], "af") == 0) {
                UT_IK_DEBF("Not impl in A12 yet");
            }
        } else {
            int FrameId = atoi(Argv[2]);
            int TileNum = atoi(Argv[3]);
            FloatAaaInfo.FrameSyncId = FrameId;
            FloatAaaInfo.NumberOfTiles = TileNum;
            for (i = 0; i < TileNum; i++) {
                FloatAaaInfo.FloatTileConfig[i].FloatTileColStart = atoi(Argv[4 + i * 5]);
                FloatAaaInfo.FloatTileConfig[i].FloatTileRowStart = atoi(Argv[5 + i * 5]);
                FloatAaaInfo.FloatTileConfig[i].FloatTileWidth = atoi(Argv[6 + i * 5]);
                FloatAaaInfo.FloatTileConfig[i].FloatTileHeight = atoi(Argv[7 + i * 5]);
                FloatAaaInfo.FloatTileConfig[i].FloatTileShift = atoi(Argv[8 + i * 5]);
            }
            Rval = AmbaDSP_Img3aSetAaaFloatTileInfo(&gTestIS2.Mode, &FloatAaaInfo);
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -float: get the current float aaa statistics settings\n\r"
                  "       t %s -float [frame_sync_id] [number_of_tiles] {[col_start] [row_start] [width] [height] [shift]}\n\r"
                  "                 repeat items in {} for [number_of_tiles] times\n\r"
                  "       t %s -float show [ae|awb|af] : Show float tile AE/AWB/AF statistics\n\r",
                  Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

static int _send_test_cmd(AMBA_SHELL_ENV_s *Env, char *cmd)
{
    int Cmd_Argc;
    char *Cmd_Argv[10];
    AMBA_SHELL_PROC_f Proc;
    _string_to_arg(cmd, &Cmd_Argc, Cmd_Argv);
    Proc = AmbaShell_FindProcImage(Env, Cmd_Argv[0]);
    if (Proc != NULL ) {
        Proc(Env, Cmd_Argc, Cmd_Argv);
    } else {
        UT_IK_WARF("%s: command not found!\n", Cmd_Argv[0]);
        return -1;
    }
    return 0;
}
/* Note: It's a tempal function, after MW flow ready, this function should be removed*/

extern AMBA_KAL_BYTE_POOL_t G_MMPL;
static int test_is2_rawenc(AMBA_SHELL_ENV_s *Env, int Argc, char *Argv[])
{
    int Rval = -1;
    char Filename[APP_MAX_FN_SIZE];
    if (!ishelp(Argv[2])) {
        if (Argc == 3) {
            do {
                TUNE_Load_Param_s Load_Param;
                memset(&Load_Param, 0x0, sizeof(Load_Param));
                memcpy(Filename, Argv[2], APP_MAX_FN_SIZE);
                AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
                TUNE_Initial_Config_s TuneInitialConfig;
                TuneInitialConfig.pBytePool = &G_MMPL;
                if (0 != AmbaTUNE_Init(&TuneInitialConfig)) {
                    UT_IK_ERRF("Call AmbaTUNE_Init() Fail");
                    break;
                }
                Load_Param.Text.FilePath = Filename;
                if (0 != AmbaTUNE_Load_IDSP(&Load_Param)) {
                    UT_IK_WARF("Call AmbaTUNE_Load_IDSP() Fail");
                    break;
                }
                // TODO: AmpUT_ItunerRawEncode();

                UT_IK_ERRF("Wait App Api");

                UT_IK_DEBF("FIXME: Wait MW Function link");
            } while (0);
            Rval = 0;
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "t %s -rawenc [filepath] : raw encode flow\n\r"
                  "             filepath = file path for configuration file",
                  Argv[0]);
    }
    return Rval;
}

static int test_is2_raw(AMBA_SHELL_ENV_s *Env, int Argc, char *Argv[])
{
    int Rval = 0;
    if (!ishelp(Argv[2])) {
        if (Argc >= 4) {
            int i;
            ITUNER_SYSTEM_s System;
            struct TuningMode_Cfg_s
            {
                char *TuningMode;
                char *TuningModeExt;
            };
            UT_IK_DEBF("FIXME: Remove mid iso");
            const struct TuningMode_Cfg_s TuningMode_Cfg[] = { { TUING_MODE_TO_STR(IMG_MODE_VIDEO), TUING_MODE_EXT_TO_STR(SINGLE_SHOT) }, { TUING_MODE_TO_STR(IMG_MODE_LOW_ISO_STILL),
                                                                                                                                            TUING_MODE_EXT_TO_STR(SINGLE_SHOT) },
                                                               { TUING_MODE_TO_STR(IMG_MODE_HIGH_ISO_STILL), TUING_MODE_EXT_TO_STR(SINGLE_SHOT) } };
            int mode = atoi(Argv[2]);
            int count = atoi(Argv[3]);
            AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
            TUNE_Initial_Config_s TuneInitialConfig;
            TuneInitialConfig.pBytePool = &G_MMPL;
            for (i = 0; i < count; i++) {
                if (0 != AmbaTUNE_Init(&TuneInitialConfig)) {
                    UT_IK_ERRF("call AmbaTUNE_Init() Fail");
                    Rval = -1;
                    break;
                }
                // Note: Update System Info
                AmbaTUNE_Get_SystemInfo(&System);
                if (mode >= 0 && mode < GET_ARRAY_NUMBER(TuningMode_Cfg)) {
                    if ((TuningMode_Cfg[mode].TuningMode == NULL )|| (TuningMode_Cfg[mode].TuningModeExt == NULL)) {
                    Rval = -1;
                    break;
                }
                strncpy(System.TuningMode, TuningMode_Cfg[mode].TuningMode, sizeof(System.TuningMode));
                strncpy(System.TuningModeExt, TuningMode_Cfg[mode].TuningModeExt, sizeof(System.TuningModeExt));
            } else {
                Rval = -1;
                break;
            }
                AmbaTUNE_Set_SystemInfo(&System);
                // TODO: Trigger to Capture

                // TODO: Wait to Preview Capture Finish
                UT_IK_DEBF("FIXME: Wait MW Function link");
            }
        } else {
            Rval = -1;
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "t %s -raw [mode] [count] [compress]: raw capture\n\r"
                  "          mode = [0..8], 0:VIDEO\n\r"
                  "                         1:LOW-ISO STILL\n\r"
                  "                         2:HIGH-ISO STILL\n\r"
                  "          count = number of pictures to capture\n\r"
                  "          compress = [0, 1] optional, capture and dump the compressed raw",
                  Argv[0]);
    }
    return Rval;
}

static int test_is2_lowiso(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    extern int AmbaDSP_ImgSetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgGetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgLowIsoPrintCfg(AMBA_DSP_IMG_CFG_INFO_s CfgInfo);
    int Rval = -1;
    char Driver = 'c';
    Driver = _TextHdlr_Get_Driver_Letter();
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "dump") == 0) {
            AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
            if (strcmp(Argv[3], "still") == 0) {
                CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                CfgInfo.CfgId = 0;
                Rval = 0;
            } else if (strcmp(Argv[3], "video") == 0) {
                CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                CfgInfo.CfgId = 0;
                Rval = 0;
            } else {

            }
            AmbaDSP_ImgLowIsoPrintCfg(CfgInfo);
            AmbaDSP_ImgLowIsoDumpCfg(CfgInfo, Driver);
            Rval = 0;
        } else if (strcmp(Argv[2], "debug") == 0) {
            AMBA_DSP_IMG_DEBUG_MODE_s DebugMode = { 0 };
            if (Argc > 3) {
                DebugMode.Step = atoi(Argv[3]);
                DebugMode.Mode = atoi(Argv[4]);
                DebugMode.TileX = atoi(Argv[5]);
                DebugMode.TileY = atoi(Argv[6]);
                DebugMode.ChannelID = 0;
                DebugMode.PicNum = 0;
                gTestIS2.Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                gTestIS2.Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
                gTestIS2.Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                gTestIS2.Mode.ContextId = 0;
                gTestIS2.Mode.ConfigId = 0;
                AmbaDSP_ImgSetDebugMode(&gTestIS2.Mode, &DebugMode);
                Rval = 0;
            } else {
                gTestIS2.Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                gTestIS2.Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
                gTestIS2.Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                gTestIS2.Mode.ContextId = 0;
                gTestIS2.Mode.ConfigId = 0;
                AmbaDSP_ImgGetDebugMode(&gTestIS2.Mode, &DebugMode);
                AmbaPrint("Current Lowiso debug Step=%d Mode=%d TileX=%d TileY=%d", DebugMode.Step, DebugMode.Mode, DebugMode.TileX, DebugMode.TileY);
                Rval = 0;
            }
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -lowiso [dump | debug]: lowISO test functions\n\r"
                  "       t %s -lowiso dump : dump current lowISO settings\n\r"
                  "       t %s -lowiso debug [step] [mode] [tileX] [tileY]: set lowISO debug mode",
                  Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_highiso(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    extern int AmbaDSP_ImgGetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgSetDebugMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEBUG_MODE_s *pDebugMode);
    extern int AmbaDSP_ImgHighIsoPrintCfg(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFG_INFO_s CfgInfo);
    int Rval = -1;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_DSP_IMG_DEBUG_MODE_s DebugMode = { 0 };
    char Driver = 'c';
    Driver = _TextHdlr_Get_Driver_Letter();
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "dump") == 0) {
            if (strcmp(Argv[3], "still") == 0) {
                CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                CfgInfo.CfgId = 0;
                Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                Mode.ContextId = 0;
                Mode.ConfigId = 0;
                AmbaDSP_ImgHighIsoPrintCfg(&Mode, CfgInfo);
                AmbaDSP_ImgHighIsoDumpCfg(CfgInfo, Driver);
                Rval = 0;
            } else if (strcmp(Argv[3], "video") == 0) {
                CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                CfgInfo.CfgId = 0;
                Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                Mode.ContextId = 0;
                Mode.ConfigId = 0;
                AmbaDSP_ImgHighIsoPrintCfg(&Mode, CfgInfo);
                AmbaDSP_ImgHighIsoDumpCfg(CfgInfo, Driver);
                Rval = 0;
            } else {

            }
        } else if (strcmp(Argv[2], "debug") == 0) {
            if (strcmp(Argv[3], "still") == 0) {
                if (Argc > 4) {

                    DebugMode.Step = atoi(Argv[4]);
                    DebugMode.Mode = atoi(Argv[5]);
                    DebugMode.TileX = atoi(Argv[6]);
                    DebugMode.TileY = atoi(Argv[7]);
//                    if (Argc > 8) {
//                        extern UINT8 HisoCaseID;
//                        HisoCaseID = atoi(Argv[8]);
//                    }
                    DebugMode.ChannelID = 0;
                    DebugMode.PicNum = 0;

                    Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                    Mode.ContextId = 0;
                    Mode.ConfigId = 0;
                    AmbaDSP_ImgSetDebugMode(&Mode, &DebugMode);
                    Rval = 0;
                } else {
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
                    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                    Mode.ContextId = 0;
                    Mode.ConfigId = 0;
                    AmbaDSP_ImgGetDebugMode(&Mode, &DebugMode);
                    AmbaPrint("Current Highiso Still debug Step=%d Mode=%d TileX=%d TileY=%d", DebugMode.Step, DebugMode.Mode, DebugMode.TileX, DebugMode.TileY);
                    Rval = 0;
                }
            } else if (strcmp(Argv[3], "video") == 0) {
                if (Argc > 4) {
                    DebugMode.Step = atoi(Argv[4]);
                    DebugMode.Mode = atoi(Argv[5]);
                    DebugMode.TileX = atoi(Argv[6]);
                    DebugMode.TileY = atoi(Argv[7]);
                    DebugMode.ChannelID = 0;
                    DebugMode.PicNum = 0;
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                    Mode.ContextId = 0;
                    Mode.ConfigId = 0;
                    AmbaDSP_ImgSetDebugMode(&Mode, &DebugMode);
                    Rval = 0;
                } else {
                    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                    Mode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
                    Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
                    Mode.ContextId = 0;
                    Mode.ConfigId = 0;
                    AmbaDSP_ImgGetDebugMode(&Mode, &DebugMode);
                    AmbaPrint("Current Highiso Video debug Step=%d Mode=%d TileX=%d TileY=%d", DebugMode.Step, DebugMode.Mode, DebugMode.TileX, DebugMode.TileY);
                    Rval = 0;
                }
            } else {

            }
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -highiso [dump | debug] [video | still]: highISO test functions\n\r"
                  "       t %s -highiso dump [video | still]: dump current highISO settings\n\r"
                  "       t %s -highiso debug [video | still] [step] [mode] [tileX] [tileY]: set highISO debug mode",
                  Argv[0], Argv[0], Argv[0]);
        return Rval;
    }
    return Rval;
}

static int test_is2_ituner(AMBA_SHELL_ENV_s *Env, int Argc, char *Argv[])
{
    int Rval = -1;
    if (!ishelp(Argv[2])) {
        if (Argc == 4) {
            if ((strcmp(Argv[2], "load") == 0)) {
                do {
                    TUNE_Load_Param_s Load_Param;
                    AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
                    TUNE_Initial_Config_s TuneInitialConfig;
                    TuneInitialConfig.pBytePool = &G_MMPL;
                    if (0 != AmbaTUNE_Init(&TuneInitialConfig)) {
                        UT_IK_ERRF("Call AmbaTUNE_Init() Fail");
                        break;
                    }
                    AmbaItuner_Refresh(&gTestIS2.Mode);
                    memset(&Load_Param, 0x0, sizeof(Load_Param));
                    Load_Param.Text.FilePath = Argv[3];
                    if (0 != AmbaTUNE_Load_IDSP(&Load_Param)) {
                        UT_IK_WARF("Call AmbaTUNE_Load_IDSP(FilePath: %s) Fail", Argv[3]);
                        break;
                    }
                    ITUNER_INFO_s ItunerInfo;
                    if (0 != AmbaTUNE_Get_ItunerInfo(&ItunerInfo)) {
                        UT_IK_ERRF("Call AmbaTUNE_Get_ItunerInfo() Fail");
                        break;
                    }
                    memcpy(&gTestIS2.Mode, &ItunerInfo.TuningAlgoMode, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
                    //ItunerInfo.TuningAlgoMode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_FAST;
                    AMBA_ITUNER_PROC_INFO_s ProcInfo;
                    memset(&ProcInfo, 0, sizeof(AMBA_ITUNER_PROC_INFO_s));

                    static ITUNER_SYSTEM_s System;
                    AmbaTUNE_Get_SystemInfo(&System);
                    // Note: For Preview Tuning, RawWidth/High Info should get from current sensor, not from ituner file
                    System.RawStartX = WAIT_APP_INFO_RAW_STARTX;
                    System.RawStartY = WAIT_APP_INFO_RAW_STARTY;
                    System.RawWidth = WAIT_APP_INFO_RAW_WIDTH;
                    System.RawHeight = WAIT_APP_INFO_RAW_HEIGHT;
                    System.MainWidth = WAIT_APP_INFO_MAIN_WIDTH;
                    System.MainHeight = WAIT_APP_INFO_MAIN_HEIGHT;
                    System.HSubSampleFactorNum = WAIT_APP_INFO_HSUBSAMPLEFACTORNUM;
                    System.HSubSampleFactorDen = WAIT_APP_INFO_HSUBSAMPLEFACTORDEN;
                    System.VSubSampleFactorNum = WAIT_APP_INFO_VSUBSAMPLEFACTORNUM;
                    System.VSubSampleFactorDen = WAIT_APP_INFO_VSUBSAMPLEFACTORDEN;
                    AmbaTUNE_Set_SystemInfo(&System);
                    //ProcInfo.HisoBatchId = AMBA_DSP_STILL_HISO_FILTER;
                    UT_IK_DEBF("Execute RawW: %d, H: %d, MainW: %d, H: %d", System.RawWidth, System.RawHeight, System.MainWidth, System.MainHeight);
                    UT_IK_DEBF("RawX: %d, Y:%d, H %d %d, V %d %d", System.RawStartX, System.RawStartY, System.HSubSampleFactorNum, System.HSubSampleFactorDen,
                               System.VSubSampleFactorNum, System.VSubSampleFactorDen);
                    if (0 != AmbaTUNE_Execute_IDSP(&gTestIS2.Mode, &ProcInfo)) {
                        UT_IK_WARF("Call AmbaTUNE_Execute_IDSP() Fail");
                        break;
                    }
                    Rval = 0;
                } while (0);
            } else if ((strcmp(Argv[2], "save") == 0)) {
                do {
                    TUNE_Save_Param_s Save_Param;

                    memset(&Save_Param, 0x0, sizeof(Save_Param));
                    AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
                    TUNE_Initial_Config_s TuneInitialConfig;
                    TuneInitialConfig.pBytePool = &G_MMPL;
                    if (0 != AmbaTUNE_Init(&TuneInitialConfig)) {
                        AmbaPrint("%s() %d, call AmbaTUNE_Init() Fail", __func__, __LINE__);
                        break;
                    }
                    UT_IK_DEBF("FIXME: We need to update AE/ WB Info, Wait 3A Get AE Info Function Ready");
#if 0
                    {
                        AMBA_AE_INFO_s VideoAeInfo;
                        ITUNER_AE_INFO_s AeInfo;
                        WB_SIM_INFO_s VideoWbSimInfo;
                        ITUNER_WB_SIM_INFO_s WbSimInfo;
                        UINT32 ChNo = 0;

                        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)ChNo, IP_MODE_VIDEO, (UINT32)&VideoAeInfo);
                        AeInfo.Multiplier = 1024*1024;
                        AeInfo.EvIndex = VideoAeInfo.EvIndex;
                        AeInfo.NfIndex = VideoAeInfo.NfIndex;
                        AeInfo.ShutterIndex = VideoAeInfo.ShutterIndex;
                        AeInfo.AgcIndex = VideoAeInfo.AgcIndex;
                        AeInfo.IrisIndex = VideoAeInfo.IrisIndex;
                        AeInfo.Dgain = VideoAeInfo.Dgain;
                        AeInfo.IsoValue = VideoAeInfo.IsoValue;
                        AeInfo.Flash = VideoAeInfo.Flash;
                        AeInfo.Mode = VideoAeInfo.Mode;
                        AeInfo.ShutterTime = (INT32)(VideoAeInfo.ShutterTime * AeInfo.Multiplier);
                        AeInfo.AgcGain = (INT32)(VideoAeInfo.AgcGain * AeInfo.Multiplier);
                        AeInfo.Target = VideoAeInfo.Target;
                        AeInfo.LumaStat = VideoAeInfo.LumaStat;
                        AeInfo.LimitStatus = VideoAeInfo.LimitStatus;
                        AmbaTUNE_Set_AeInfo(&AeInfo);

                        AmbaImg_Proc_Cmd(MW_IP_GET_WB_SIM_INFO, ChNo, (UINT32)&VideoWbSimInfo, 0);
                        WbSimInfo.LumaIdx = VideoWbSimInfo.LumaIdx;
                        WbSimInfo.OutDoorIdx = VideoWbSimInfo.OutDoorIdx;
                        WbSimInfo.HighLightIdx = VideoWbSimInfo.HighLightIdx;
                        WbSimInfo.LowLightIdx = VideoWbSimInfo.LowLightIdx;
                        WbSimInfo.AwbRatio[0] = VideoWbSimInfo.AwbRatio[0];
                        WbSimInfo.AwbRatio[1] = VideoWbSimInfo.AwbRatio[1];
                        AmbaTUNE_Set_WbSimInfo(&WbSimInfo);
                    }
#endif
                    Save_Param.Text.Filepath = Argv[3];
                    if (0 != AmbaTUNE_Save_IDSP(&gTestIS2.Mode, &Save_Param)) {
                        UT_IK_WARF("Call AmbaTUNE_Save_IDSP(FilePath: %s Fail", Argv[3]);
                        break;
                    }
                    Rval = 0;
                } while (0);
            }
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "t %s -ituner load [filepath] : ituner flow\n\r"
                  "                  filepath = file path for configuration file",
                  Argv[0]);
    }
    return Rval;
}

static int test_is2_cf(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_CHROMA_FILTER_s ChromaFilter;

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaDSP_ImgGetChromaFilter(&gTestIS2.Mode, &ChromaFilter);
            AmbaPrint("Current chroma filter setting:");
            AmbaPrint("Enable: %d", ChromaFilter.Enable);
            AmbaPrint("NoiseLevel_Cb: %d             NoiseLevel_Cr: %d", ChromaFilter.NoiseLevelCb, ChromaFilter.NoiseLevelCr);
            AmbaPrint("OriginalBlendStrength_Cb: %d  OriginalBlendStrength_Cr: %d", ChromaFilter.OriginalBlendStrengthCb, ChromaFilter.OriginalBlendStrengthCr);
            AmbaPrint("Radius: %d", ChromaFilter.Radius);
            Rval = 0;
        } else {
            ChromaFilter.Enable = atoi(Argv[2]);
            ChromaFilter.NoiseLevelCb = atoi(Argv[3]);
            ChromaFilter.NoiseLevelCr = atoi(Argv[4]);
            ChromaFilter.OriginalBlendStrengthCb = atoi(Argv[5]);
            ChromaFilter.OriginalBlendStrengthCr = atoi(Argv[6]);
            ChromaFilter.Radius = atoi(Argv[7]);
            Rval = AmbaDSP_ImgSetChromaFilter(&gTestIS2.Mode, &ChromaFilter);
        }
    }

    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -cf: get the current lowISO chroma filter setting\n\r"
                  "       t %s -cf [enable] [nl_cb] [nl_cr] [ori_blnd_str_cb] [ori_blnd_str_cr] [radius] : set the chroma filter\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;
}

static int test_is2_mode(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("Current Mode: Pipe %d, AlgoMode %d, FuncMode %d, ContextId %d, BatchId %d, ConfigId %d\n",
                      (int) gTestIS2.Mode.Pipe,
                      (int) gTestIS2.Mode.AlgoMode,
                      (int) gTestIS2.Mode.FuncMode,
                      (int) gTestIS2.Mode.ContextId,
                      (int) gTestIS2.Mode.BatchId,
                      (int) gTestIS2.Mode.ConfigId);
            Rval = 0;
        } else if (isnumber(Argv[2])) {
            gTestIS2.Mode.Pipe = (AMBA_DSP_IMG_PIPE_e) atoi(Argv[2]);
            gTestIS2.Mode.AlgoMode = (AMBA_DSP_IMG_ALGO_MODE_e) atoi(Argv[3]);
            gTestIS2.Mode.FuncMode = (AMBA_DSP_IMG_FUNC_MODE_e) atoi(Argv[4]);
            gTestIS2.Mode.ContextId = (UINT8) atoi(Argv[5]);
            gTestIS2.Mode.BatchId = (UINT32) atoi(Argv[6]);
            gTestIS2.Mode.ConfigId = (UINT8) atoi(Argv[7]);
            AmbaPrint("Current Mode: Pipe %d, AlgoMode %d, FuncMode %d, ContextId %d, BatchId %d, ConfigId %d\n",
                      (int) gTestIS2.Mode.Pipe,
                      (int) gTestIS2.Mode.AlgoMode,
                      (int) gTestIS2.Mode.FuncMode,
                      (int) gTestIS2.Mode.ContextId,
                      (int) gTestIS2.Mode.BatchId,
                      (int) gTestIS2.Mode.ConfigId);
            Rval = 0;
        } else {

        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -mode: get the current mode\n\r"
                  "       t %s -mode [Pipe] [AlgoMode] [FuncMode] [ContextId] [BatchId] [ConfigId]: set the current mode\n\r",
                  Argv[0], Argv[0]);
    }
    return Rval;

}

static int test_is2_profile(AMBA_SHELL_ENV_s *Env, int Argc, char** Argv)
{
    int Rval = -1;
    static void *profile_buffer = NULL;
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "init") == 0) {
            extern int AmbaDSP_ImgProfilePoolInit(void* Buf, UINT32 Size);
            const UINT32 profile_buffer_size = 9 * 1024;
            AmbaKAL_BytePoolAllocate(&G_MMPL, (void **) &profile_buffer, profile_buffer_size, 100);
            if (profile_buffer == NULL ) {
                UT_IK_ERRF("call AmbaKAL_BytePoolAllocate() Fail");
            }
            Rval = AmbaDSP_ImgProfilePoolInit(profile_buffer, profile_buffer_size);
        } else if (strcmp(Argv[2], "uninit") == 0) {
            extern void AmbaDSP_ImgProfilePoolUnint(void);
            AmbaDSP_ImgProfilePoolUnint();
            AmbaKAL_BytePoolFree(profile_buffer);
            profile_buffer = NULL;
            Rval = 0;
        } else if (strcmp(Argv[2], "hist") == 0) {
            extern int AmbaDSP_ImgProfileDumpHistory(UINT32 History_Number);
            int History_Number = atoi(Argv[3]);
            Rval = AmbaDSP_ImgProfileDumpHistory(History_Number);
        }
    }
    if (Rval == -1) {
        AmbaPrint("\n\r"
                  "Usage: t %s -profile init: Init IK profile mode\n\r"
                  "       t %s -profile uninit: Uninit IK profile mode\n\r"
                  "       t %s -profile hist: List IK profile results",
                  Argv[0], Argv[0], Argv[0]);
    }
    return Rval;
}
/* Add Rash Cmd to Cmd_List
 * */
static int _Rash_Cmd_Enqueue(char* Cmd)
{
    CmdNode_s* New_Node = NULL;
    CmdNode_s *Current_Node;
    do {
        if (OK != AmbaShell_MemAlloc((void**) &New_Node, sizeof(CmdNode_s))) {
            UT_IK_WARF("call AmbaShell_MemAlloc() Fail");
            break;
        }
        strncpy(New_Node->Cmd, Cmd, sizeof(New_Node->Cmd));
        New_Node->NextNode = NULL;
        if (Cmd_List.List == NULL ) {
            Cmd_List.List = New_Node;
        } else {
            Current_Node = Cmd_List.List;
            while (Current_Node->NextNode != NULL ) {
                Current_Node = (CmdNode_s*) Current_Node->NextNode;
            }
            Current_Node->NextNode = New_Node;
        }
        Cmd_List.Cmd_Num++;
        return 0;

    } while (0);
    return -1;
}

/* Get Rash Cmd from Cmd_List
 * */
static int _Rash_Cmd_Dequeue(char* Cmd)
{
    if (Cmd_List.List == NULL ) {
        return -1;
    }
    strncpy(Cmd, Cmd_List.List->Cmd, sizeof(Cmd_List.List->Cmd));
    CmdNode_s *Target_Node;
    Target_Node = Cmd_List.List;
    Cmd_List.List = (CmdNode_s*) Cmd_List.List->NextNode;
    AmbaShell_MemFree(Target_Node);
    Cmd_List.Cmd_Num--;
    return 0;
}

static int test_is2_Rash(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    Rash_Stack_s *Rash_Stack = NULL;
    char Target_Cmd[255];
    File_Info_s FileInfo;
    int Cmd_Pos = 0;
    int Read_Cnt;
    AMBA_FS_FILE* Fd;
    do {
        int Stack_Depth = 10;
        if (ishelp(Argv[2]) || Argc < 3) {
            break;
        }
        if (Argc == 4) {
            Stack_Depth = atoi(Argv[3]);
            if (Stack_Depth < 1) {
                UT_IK_WARF("Stack_Depth %d is too small", Stack_Depth);
                Stack_Depth = 10;
            }
        }
        if (OK != AmbaShell_MemAlloc((void*) &Rash_Stack, sizeof(Rash_Stack_s) + sizeof(File_Info_s) * Stack_Depth)) {
            UT_IK_WARF("call AmbaShell_MemAlloc() Fail, Stack_Depth = %d", Stack_Depth);
            break;
        }
        memset(Rash_Stack, 0x0, sizeof(Rash_Stack_s) + sizeof(File_Info_s) * Stack_Depth);
        Rash_Stack->Max_Depth = Stack_Depth;
        memset(Target_Cmd, 0x0, sizeof(Target_Cmd));
        Rash_Stack->Currect_Stack = 0;
        strncpy(FileInfo.FilePath, Argv[2], sizeof(FileInfo.FilePath));
        FileInfo.Offset = 0;
        STACK_PUSH(FileInfo);
        while (!IS_STACK_EMPTY) {
            STACK_POP(FileInfo);
            Fd = AmbaFS_fopen(FileInfo.FilePath, "r");
            if (FileInfo.Offset) {
                AmbaFS_fseek(Fd, FileInfo.Offset, AMBA_FS_SEEK_START);
            }
            while (Read_Cnt = AmbaFS_fread(&Target_Cmd[Cmd_Pos], 1, 1, Fd)) {
                FileInfo.Offset += Read_Cnt;
                if (Target_Cmd[Cmd_Pos] == '\0' || Target_Cmd[Cmd_Pos] == '\n' || Target_Cmd[Cmd_Pos] == '\r') {
                    // Note: Check Is the same cmd?
                    _remove_windows_text_eof(Target_Cmd);
                    if (strncmp(Target_Cmd, "t img -rash", strlen("t img -rash")) == 0) {
                        STACK_PUSH(FileInfo);
                        strncpy(FileInfo.FilePath, &Target_Cmd[strlen("t img -rash ")], sizeof(FileInfo.FilePath));
                        FileInfo.Offset = 0;
                        STACK_PUSH(FileInfo);
                        break;
                    } else {
                        if (Target_Cmd[0] == '#') {
                            // SKip
                            //UT_IK_DEBF("Target_Cmd[0] = %d", Target_Cmd[0]);
                        } else if (strlen(Target_Cmd) > 0) {
                            //UT_IK_DEBF("Target_Cmd[0] = %d", Target_Cmd[0]);
                            _Rash_Cmd_Enqueue(Target_Cmd);
                        } else {

                        }
                    }
                    // Note: Clear
                    Cmd_Pos = 0;
                    memset(Target_Cmd, 0x0, sizeof(Target_Cmd));
                } else {
                    Cmd_Pos++;
                }
            }
            Cmd_Pos = 0;
            memset(Target_Cmd, 0x0, sizeof(Target_Cmd));
            AmbaFS_fclose(Fd);
        }

        int i;
        int Cmd_Argc;
        char *Cmd_Argv[10];
        for (i = Cmd_List.Cmd_Num - 1; i >= 0; --i) {
            _Rash_Cmd_Dequeue(Target_Cmd);
            // UT_IK_DEBF(5, "[OUT][%d] Cmd : <%s>", i, Target_Cmd);
            _string_to_arg(Target_Cmd, &Cmd_Argc, Cmd_Argv);
            AMBA_SHELL_PROC_f Proc = AmbaShell_FindProcImage(Env, Cmd_Argv[0]);
            if (Proc != NULL ) {
                Proc(Env, Cmd_Argc, Cmd_Argv);
            } else {
                UT_IK_DEBF("%s: command not found!\n", Cmd_Argv[0]);
            }
        }
        AmbaShell_MemFree(Rash_Stack);
        return 0;
    } while (0);
    AmbaPrint("\n\r"
              "Usage: t %s -rash [ash path] [max recursive depth]: Support Recursive .ash Script\n\r"
              "                  ash path: ash file path, eg: c:\\xxx.ash, not support shell cmd (eg. WHILE)\n\r"
              "                  max recursive depth: (option, default:10) max recursive ash depth",
              Argv[0]);
    return -1;
}

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
static int test_is2_warp(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_WARP_CALC_INFO_s CalcWarp = { 0 };
    int Test;
    UINT16 VCapWidth, VCapHeight, MainWidth, MainHeight;

    VCapWidth = WAIT_APP_INFO_RAW_WIDTH;
    VCapHeight = WAIT_APP_INFO_RAW_HEIGHT;
    MainWidth = WAIT_APP_INFO_MAIN_WIDTH;
    MainHeight = WAIT_APP_INFO_MAIN_HEIGHT;
    AmbaPrint("Warp size info VCapWidth= %d VCapHeight=%d MainWidth=%d MainHeight=%d", VCapWidth, VCapHeight, MainWidth, MainHeight);

    AmbaDSP_ImgGetWarpCompensation(&gTestIS2.Mode, &CalcWarp);
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current warp info: TBD");
            Rval = 0;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugWarpCompensation(UINT8 TmpWarpDebugMessageFlag);
            UINT8 WarpDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugWarpCompensation(WarpDebugFlag);
            Rval = 0;
        } else if (strcmp(Argv[2], "vert") == 0) {
            AMBA_DSP_IMG_WARP_FLIP_INFO_s WarpFlipInfo;
            UINT32 VertWarpFlipEnb = atoi(Argv[3]);
            WarpFlipInfo.VerticalEnable = VertWarpFlipEnb;
            AmbaDSP_WarpCore_SetWarpFlipEnb(&gTestIS2.Mode, &WarpFlipInfo);
            Rval = 0;
        } else if (strcmp(Argv[2], "windowinfo") == 0) {
            AMBA_DSP_IMG_WARP_WIN_INFO_s WarpWinInfo;
            AmbaDSP_WarpCore_GetWindowInfo(&gTestIS2.Mode, &WarpWinInfo);
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

            Rval = 0;
        } else if (strcmp(Argv[2], "warpcoretrasnfer") == 0 || (strcmp(Argv[2], "wct") == 0)) {
            AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
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

            AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
            TmpDzoomInfo.ZoomX = atoi(Argv[11]);
            TmpDzoomInfo.ZoomY = atoi(Argv[12]);
            TmpDzoomInfo.ShiftX = atoi(Argv[13]);
            TmpDzoomInfo.ShiftY = atoi(Argv[14]);

            AmbaPrint("DzoomInfo.ZoomX   = %d", TmpDzoomInfo.ZoomX);
            AmbaPrint("DzoomInfo.ZoomY   = %d", TmpDzoomInfo.ZoomY);
            AmbaPrint("DzoomInfo.ShiftX  = %d", TmpDzoomInfo.ShiftX);
            AmbaPrint("DzoomInfo.ShiftY  = %d", TmpDzoomInfo.ShiftY);

            AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange;
            TmpDmyRange.Bottom = atoi(Argv[15]);
            TmpDmyRange.Top = atoi(Argv[16]);
            TmpDmyRange.Left = atoi(Argv[17]);
            TmpDmyRange.Right = atoi(Argv[18]);
            AmbaPrint("DmyRange.Bottom  = %d", TmpDmyRange.Bottom);
            AmbaPrint("DmyRange.Top     = %d", TmpDmyRange.Top);
            AmbaPrint("DmyRange.Left    = %d", TmpDmyRange.Left);
            AmbaPrint("DmyRange.Right   = %d", TmpDmyRange.Right);

            AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = { 0 };
            TmpOutWinInfo.MainWinDim.Width = atoi(Argv[19]);
            TmpOutWinInfo.MainWinDim.Height = atoi(Argv[20]);
            AmbaPrint("MainWinDim.Width  = %d", TmpOutWinInfo.MainWinDim.Width);
            AmbaPrint("MainWinDim.Height = %d", TmpOutWinInfo.MainWinDim.Height);

            AMBA_DSP_IMG_EIS_INFO_s TmpEISInfo = { 0 };
            TmpEISInfo.HorSkewPhaseInc = atoi(Argv[21]);
            TmpEISInfo.VerSkewPhaseInc = atoi(Argv[22]);
            AmbaPrint("EIS.HorSkewPhaseInc = %d", TmpEISInfo.HorSkewPhaseInc);
            AmbaPrint("EIS.VerSkewPhaseInc = %d", TmpEISInfo.VerSkewPhaseInc);

            AmbaDSP_WarpCore_Init();

            AMBA_DSP_IMG_WARP_CALC_WIN_INFO_s TmpWindowInfo;
            TmpWindowInfo.VinSensorGeo = TmpVinSensorGeo;
            TmpWindowInfo.DzoomInfo = TmpDzoomInfo;
            TmpWindowInfo.OutWinInfo = TmpOutWinInfo;
            TmpWindowInfo.DmyRange = TmpDmyRange;
            TmpWindowInfo.EisInfo = TmpEISInfo;

            AmbaDSP_WarpCore_CalcWindowInfo(&gTestIS2.Mode, &TmpWindowInfo);
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

            Rval = 0;
        } else if (strcmp(Argv[2], "bypasstest") == 0) {
            AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s TestWarpCorrByPass;
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;

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

            extern int AmbaDSP_ImgSetWarpCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s *pWarpDzoomCorrByPass);

            AmbaDSP_ImgSetWarpCompensationByPass(&TmpMode, &TestWarpCorrByPass);

            Rval = 0;
        } else if (isnumber(Argv[2])) {
            Test = atoi(Argv[2]);
            if (Test == 11) {
                gNonStitching = 1;
                AmbaPrint("Force to NonStitching = %d ", gNonStitching);
                Rval = 0;
            } else if (Test == 10) {
                gNonStitching = 0;
                AmbaPrint("Force to NonStitching = %d ", gNonStitching);
                Rval = 0;
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
                        Rval = -1;
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
                /*{
                    AMBA_FS_FILE *Fid;

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
                    char *Fnca = "C:\\warp_table_3904x2604.bin";
                    char *Fmode = "wb";

                    Fid = AmbaFS_fopen(Fnca, Fmode);
                    if (Fid == NULL) {
                    AmbaPrint("file open error");
                    return Rval;
                    }
                    Rval = AmbaFS_fwrite((void const*)CalibWarpTbl, sizeof(AMBA_DSP_IMG_GRID_POINT_s),(MAX_CALIB_WARP_HOR_GRID_NUM*MAX_CALIB_WARP_VER_GRID_NUM) , Fid);
                    AmbaFS_fclose(Fid);

                }*/
                AmbaDSP_ImgCalcWarpCompensation(&gTestIS2.Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&gTestIS2.Mode);
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

                AmbaDSP_ImgCalcWarpCompensation(&gTestIS2.Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&gTestIS2.Mode);
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

                AmbaDSP_ImgCalcWarpCompensation(&gTestIS2.Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&gTestIS2.Mode);
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

                AmbaDSP_ImgCalcWarpCompensation(&gTestIS2.Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&gTestIS2.Mode);
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
                    Rval = -1;
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

                AmbaDSP_ImgCalcWarpCompensation(&gTestIS2.Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&gTestIS2.Mode);
            } else if (Test == 5) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;

                AmbaPrint("Extra effect");
                AMBA_DSP_IMG_MODE_CFG_s TmpMode;
                memset(&TmpMode, 0x0, sizeof(TmpMode));

                AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
                TmpVinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                TmpVinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                TmpVinSensorGeo.Width = CalcWarp.VinSensorGeo.Width;
                TmpVinSensorGeo.Height = CalcWarp.VinSensorGeo.Height;
                TmpVinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                TmpVinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                TmpVinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                TmpVinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;

                AMBA_DSP_IMG_WIN_DIMENSION_s TmpR2rWin = { 0 };

                AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange;
                TmpDmyRange.Bottom = (100 << 16) / TmpVinSensorGeo.Height;
                TmpDmyRange.Top = (100 << 16) / TmpVinSensorGeo.Height;
                TmpDmyRange.Left = 0;
                TmpDmyRange.Right = 0;

                AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
                TmpDzoomInfo.ZoomX = 2 << 16;
                TmpDzoomInfo.ZoomY = 2 << 16;
                TmpDzoomInfo.ShiftX = 0;
                TmpDzoomInfo.ShiftY = 0;

                AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = { 0 };
                TmpOutWinInfo.MainWinDim.Width = MainWidth;
                TmpOutWinInfo.MainWinDim.Height = MainHeight;

                AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s TmpWarpRefferenceDmyMarginPixel;
                TmpWarpRefferenceDmyMarginPixel.Enable = 1;

                AMBA_DSP_IMG_CALIB_WARP_INFO_s TmpCalcWarp;

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
                    Rval = -1;

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
                TmpCalcWarp = CalcWarp.CalibWarpInfo;

                AmbaDSP_WarpCore_Init();
                AmbaDSP_WarpCore_SetVinSensorGeo(&gTestIS2.Mode, &TmpVinSensorGeo);
                AmbaDSP_WarpCore_SetR2rOutWin(&gTestIS2.Mode, &TmpR2rWin);
                AmbaDSP_WarpCore_SetDummyWinMarginRange(&gTestIS2.Mode, &TmpDmyRange);
                AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(&gTestIS2.Mode, &TmpWarpRefferenceDmyMarginPixel);
                AmbaDSP_WarpCore_SetDzoomFactor(&gTestIS2.Mode, &TmpDzoomInfo);
                AmbaDSP_WarpCore_SetOutputWin(&gTestIS2.Mode, &TmpOutWinInfo);
                AmbaDSP_WarpCore_SetDspVideoMode(&gTestIS2.Mode, 5);
                AmbaDSP_WarpCore_SetCalibWarpInfo(&gTestIS2.Mode, &TmpCalcWarp);

                AmbaDSP_WarpCore_CalcDspWarp(&gTestIS2.Mode, 0);
                AmbaDSP_WarpCore_SetDspWarp(&gTestIS2.Mode);
            } else if (Test == 6) {
                AmbaPrint("Test only 2x dzoom with warp effect");
                INT32 W, H, TWE, THE, HGN, VGN, x, y;
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
                    Rval = -1;
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

                AmbaDSP_ImgCalcWarpCompensation(&gTestIS2.Mode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensation(&gTestIS2.Mode);
            } else if (Test == 7) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;

                AmbaPrint("Extra effect");
                AMBA_DSP_IMG_MODE_CFG_s TmpMode;
                memset(&TmpMode, 0x0, sizeof(TmpMode));

                AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
                CalcWarp.VinSensorGeo.Width = VCapWidth;
                CalcWarp.VinSensorGeo.Height = VCapHeight;
                TmpVinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                TmpVinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                TmpVinSensorGeo.Width = CalcWarp.VinSensorGeo.Width;
                TmpVinSensorGeo.Height = CalcWarp.VinSensorGeo.Height;
                TmpVinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                TmpVinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                TmpVinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                TmpVinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;

                AMBA_DSP_IMG_WIN_DIMENSION_s TmpR2rWin = { 0 };

                AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange;
                TmpDmyRange.Bottom = (100 << 16) / TmpVinSensorGeo.Height;
                TmpDmyRange.Top = (100 << 16) / TmpVinSensorGeo.Height;
                TmpDmyRange.Left = 0;
                TmpDmyRange.Right = 0;

                AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
                TmpDzoomInfo.ZoomX = 4 << 16;
                TmpDzoomInfo.ZoomY = 4 << 16;
                TmpDzoomInfo.ShiftX = 0;
                TmpDzoomInfo.ShiftY = 0;

                AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = { 0 };
                TmpOutWinInfo.MainWinDim.Width = MainWidth;
                TmpOutWinInfo.MainWinDim.Height = MainHeight;

                AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s TmpWarpRefferenceDmyMarginPixel;
                TmpWarpRefferenceDmyMarginPixel.Enable = 1;

                AMBA_DSP_IMG_CALIB_WARP_INFO_s TmpCalcWarp;

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
                    Rval = -1;

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
                TmpCalcWarp = CalcWarp.CalibWarpInfo;

                AmbaDSP_WarpCore_Init();
                AmbaDSP_WarpCore_SetVinSensorGeo(&gTestIS2.Mode, &TmpVinSensorGeo);
                AmbaDSP_WarpCore_SetR2rOutWin(&gTestIS2.Mode, &TmpR2rWin);
                AmbaDSP_WarpCore_SetDummyWinMarginRange(&gTestIS2.Mode, &TmpDmyRange);
                AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(&gTestIS2.Mode, &TmpWarpRefferenceDmyMarginPixel);
                AmbaDSP_WarpCore_SetDzoomFactor(&gTestIS2.Mode, &TmpDzoomInfo);
                AmbaDSP_WarpCore_SetOutputWin(&gTestIS2.Mode, &TmpOutWinInfo);
                AmbaDSP_WarpCore_SetDspVideoMode(&gTestIS2.Mode, 5);
                AmbaDSP_WarpCore_SetCalibWarpInfo(&gTestIS2.Mode, &TmpCalcWarp);

                AmbaDSP_WarpCore_CalcDspWarp(&gTestIS2.Mode, 0);
                AmbaDSP_WarpCore_SetDspWarp(&gTestIS2.Mode);
            } else if (Test == 8) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;

                AmbaPrint("Extra effect");
                AMBA_DSP_IMG_MODE_CFG_s TmpMode;
                memset(&TmpMode, 0x0, sizeof(TmpMode));

                AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
                TmpVinSensorGeo.StartX = CalcWarp.VinSensorGeo.StartX;
                TmpVinSensorGeo.StartY = CalcWarp.VinSensorGeo.StartY;
                TmpVinSensorGeo.Width = CalcWarp.VinSensorGeo.Width;
                TmpVinSensorGeo.Height = CalcWarp.VinSensorGeo.Height;
                TmpVinSensorGeo.HSubSample.FactorNum = CalcWarp.VinSensorGeo.HSubSample.FactorNum;
                TmpVinSensorGeo.HSubSample.FactorDen = CalcWarp.VinSensorGeo.HSubSample.FactorDen;
                TmpVinSensorGeo.VSubSample.FactorNum = CalcWarp.VinSensorGeo.VSubSample.FactorNum;
                TmpVinSensorGeo.VSubSample.FactorDen = CalcWarp.VinSensorGeo.VSubSample.FactorDen;

                AMBA_DSP_IMG_WIN_DIMENSION_s TmpR2rWin = { 0 };
                AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange = { 0 };

                AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
                TmpDzoomInfo.ZoomX = 1 << 16;
                TmpDzoomInfo.ZoomY = 1 << 16;
                TmpDzoomInfo.ShiftX = 0;
                TmpDzoomInfo.ShiftY = 0;

                AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = { 0 };
                TmpOutWinInfo.MainWinDim.Width = MainWidth;
                TmpOutWinInfo.MainWinDim.Height = MainHeight;

                AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s TmpWarpRefferenceDmyMarginPixel;
                TmpWarpRefferenceDmyMarginPixel.Enable = 0;

                AMBA_DSP_IMG_CALIB_WARP_INFO_s TmpCalcWarp;

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
                    Rval = -1;

                }

                memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));
                AmbaPrint("Test hor & ver warp table");
                for (y = 0; y < VGN; y++) {
                    for (x = 0; x < HGN; x++) {
                        //CalibWarpTbl[y*HGN+x].X = -300*(x-(HGN>>1))*((y-(VGN>>1))*(y-(VGN>>1)))/15;
                        if (y < (VGN / 2)) {
                            CalibWarpTbl[y * HGN + x].X = (-20) << 4;
                        } else {
                            CalibWarpTbl[y * HGN + x].X = 20 << 4;
                        }
                        CalibWarpTbl[y * HGN + x].Y = 0;
                        if (x == 0)
                            AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                    }
                }
                CalcWarp.CalibWarpInfo.pWarp = CalibWarpTbl;
                TmpCalcWarp = CalcWarp.CalibWarpInfo;

                AmbaDSP_WarpCore_Init();
                AmbaDSP_WarpCore_SetVinSensorGeo(&gTestIS2.Mode, &TmpVinSensorGeo);
                AmbaDSP_WarpCore_SetR2rOutWin(&gTestIS2.Mode, &TmpR2rWin);
                AmbaDSP_WarpCore_SetDummyWinMarginRange(&gTestIS2.Mode, &TmpDmyRange);
                AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(&gTestIS2.Mode, &TmpWarpRefferenceDmyMarginPixel);
                AmbaDSP_WarpCore_SetDzoomFactor(&gTestIS2.Mode, &TmpDzoomInfo);
                AmbaDSP_WarpCore_SetOutputWin(&gTestIS2.Mode, &TmpOutWinInfo);
                AmbaDSP_WarpCore_SetDspVideoMode(&gTestIS2.Mode, 5);
                AmbaDSP_WarpCore_SetCalibWarpInfo(&gTestIS2.Mode, &TmpCalcWarp);

                AmbaDSP_WarpCore_CalcDspWarp(&gTestIS2.Mode, 0);
                AmbaDSP_WarpCore_SetDspWarp(&gTestIS2.Mode);
            } else if (Test == 9) {

                // Specific Test command for surrourd view setting in Vcap
                VCapWidth = 1920;
                VCapHeight = 1080;

                AMBA_DSP_IMG_MODE_CFG_s ImgMode;
                UINT16 TmpReservedNum = atoi(Argv[3]);
                UINT8 TmpChannelIndex = atoi(Argv[4]);
                UINT8 TmpPatternNum = atoi(Argv[5]);

                memset(&ImgMode, 0, sizeof(ImgMode));
                ImgMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
                ImgMode.ContextId = TmpChannelIndex;

                AmbaPrint("TmpChannelIndex = %d", TmpChannelIndex);
                ImgMode.BatchId = AMBA_DSP_FILTER_CVT(AMBA_DSP_VIDEO_FILTER, TmpChannelIndex); //Prepare idsp to channel

                AmbaPrint("ImgMode.BatchId = %d", ImgMode.BatchId);

                INT32 W, H, TWE, THE, HGN, VGN, x, y;
                // dzoom param
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
                AmbaPrint("Test only 2x dzoom with warp effect");

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
                    Rval = -1;
                    goto done;
                }

                memset(CalibWarpTbl, 0, sizeof(CalibWarpTbl));

                if (TmpPatternNum == 0) {
                    AmbaPrint("Test hor & ver warp table");
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibWarpTbl[y * HGN + x].X = 0;
                            CalibWarpTbl[y * HGN + x].Y = 0;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                } else if (TmpPatternNum == 1) {
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibWarpTbl[y * HGN + x].X = -200 * 16;
                            CalibWarpTbl[y * HGN + x].Y = 0;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                } else if (TmpPatternNum == 2) {
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibWarpTbl[y * HGN + x].X = 200 * 16;
                            CalibWarpTbl[y * HGN + x].Y = 0;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                } else if (TmpPatternNum == 3) {
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibWarpTbl[y * HGN + x].X = -10 * (x - (HGN >> 1)) * ((y - (VGN >> 1)) * (y - (VGN >> 1))) / 15;
                            CalibWarpTbl[y * HGN + x].Y = 0;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                } else if (TmpPatternNum == 4) {
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            CalibWarpTbl[y * HGN + x].X = -30 * (x - (HGN >> 1)) * ((y - (VGN >> 1)) * (y - (VGN >> 1))) / 15;
                            CalibWarpTbl[y * HGN + x].Y = -10 * (y - (VGN >> 1)) * ((x - (HGN >> 1)) * (x - (HGN >> 1))) / 40;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                } else if (TmpPatternNum == 5) {
                    for (y = 0; y < VGN; y++) {
                        for (x = 0; x < HGN; x++) {
                            if (x > HGN / 2) {
                                CalibWarpTbl[y * HGN + x].X = -200 * 16;
                            }
                            CalibWarpTbl[y * HGN + x].Y = 0;
                            if (y == 0)
                                AmbaPrint("%d       %d", x, CalibWarpTbl[y * HGN + x].X);
                        }
                    }
                } else {
                    AmbaPrint("No this pattern !");
                }
                CalcWarp.CalibWarpInfo.pWarp = CalibWarpTbl;

                extern int AmbaDSP_ImgSetWarpCompensationReserve2(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT16 ReservedNum);

                AmbaDSP_ImgCalcWarpCompensation(&ImgMode, &CalcWarp);
                AmbaDSP_ImgSetWarpCompensationReserve2(&ImgMode, TmpReservedNum);
                AmbaDSP_ImgSetWarpCompensation(&ImgMode);

            }
            Rval = 0;
        } else {
            Rval = -1;
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
static int test_is2_ca(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    AMBA_DSP_IMG_CAWARP_CALC_INFO_s CalcCaWarp = { 0 };
    int Test;
    UINT16 VCapWidth, VCapHeight, MainWidth, MainHeight;
    VCapWidth = WAIT_APP_INFO_RAW_WIDTH;
    VCapHeight = WAIT_APP_INFO_RAW_HEIGHT;
    MainWidth = WAIT_APP_INFO_MAIN_WIDTH;
    MainHeight = WAIT_APP_INFO_MAIN_HEIGHT;
    AmbaPrint("CaWarp size info VCapWidth= %d VCapHeight=%d MainWidth=%d MainHeight=%d", VCapWidth, VCapHeight, MainWidth, MainHeight);
    AmbaDSP_ImgGetCawarpCompensation(&gTestIS2.Mode, &CalcCaWarp);

    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            AmbaPrint("current cawarp info: TBD");
            Rval = 0;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugCawarpCompensation(UINT8 TmpCawarpDebugMessageFlag);
            UINT8 CawarpDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugCawarpCompensation(CawarpDebugFlag);
            Rval = 0;
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

            extern int AmbaDSP_ImgSetCawarpCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s Mode, AMBA_DSP_IMG_BYPASS_CAWARP_INFO_s *pCAWarpCorrByPass);

            AmbaDSP_ImgSetCawarpCompensationByPass(gTestIS2.Mode, &TestCAWarpCorrByPass);
            Rval = 0;

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
                        Rval = -1;
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
                        Rval = -1;
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

                 Fid = AmbaFS_fopen(Fnca, Fmode);
                 if (Fid == NULL) {
                 AmbaPrint("file open error");
                 return Rval;
                 }
                 Rval = AmbaFS_fwrite((void const*)CalibCaWarpTbl, sizeof(AMBA_DSP_IMG_GRID_POINT_s),(MAX_CALIB_CAWARP_HOR_GRID_NUM*MAX_CALIB_CAWARP_VER_GRID_NUM) , Fid);
                 AmbaFS_fclose(Fid);

                 }*/
                AmbaDSP_ImgCalcCawarpCompensation(&gTestIS2.Mode, &CalcCaWarp);
                AmbaDSP_ImgSetCawarpCompensation(&gTestIS2.Mode);
            } else if (Test == 8) {
                INT32 W, H, TWE, THE, HGN, VGN, x, y;

                AmbaPrint("Extra effect");
                AMBA_DSP_IMG_MODE_CFG_s TmpMode;
                memset(&TmpMode, 0x0, sizeof(TmpMode));

                AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s TmpVinSensorGeo;
                TmpVinSensorGeo.StartX = CalcCaWarp.VinSensorGeo.StartX;
                TmpVinSensorGeo.StartY = CalcCaWarp.VinSensorGeo.StartY;
                TmpVinSensorGeo.Width = CalcCaWarp.VinSensorGeo.Width;
                TmpVinSensorGeo.Height = CalcCaWarp.VinSensorGeo.Height;
                TmpVinSensorGeo.HSubSample.FactorNum = CalcCaWarp.VinSensorGeo.HSubSample.FactorNum;
                TmpVinSensorGeo.HSubSample.FactorDen = CalcCaWarp.VinSensorGeo.HSubSample.FactorDen;
                TmpVinSensorGeo.VSubSample.FactorNum = CalcCaWarp.VinSensorGeo.VSubSample.FactorNum;
                TmpVinSensorGeo.VSubSample.FactorDen = CalcCaWarp.VinSensorGeo.VSubSample.FactorDen;

                AMBA_DSP_IMG_WIN_DIMENSION_s TmpR2rWin = {0};
                AMBA_DSP_IMG_DMY_RANGE_s TmpDmyRange = {0};

                AMBA_DSP_IMG_DZOOM_INFO_s TmpDzoomInfo;
                TmpDzoomInfo.ZoomX = 1 << 16;
                TmpDzoomInfo.ZoomY = 1 << 16;
                TmpDzoomInfo.ShiftX = 0;
                TmpDzoomInfo.ShiftY = 0;

                AMBA_DSP_IMG_OUT_WIN_INFO_s TmpOutWinInfo = {0};
                TmpOutWinInfo.MainWinDim.Width = MainWidth;
                TmpOutWinInfo.MainWinDim.Height= MainHeight;

                AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s TmpWarpRefferenceDmyMarginPixel;
                TmpWarpRefferenceDmyMarginPixel.Enable = 0;

                AMBA_DSP_IMG_CALIB_CAWARP_INFO_s TmpCalcCaWarp;

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
                    Rval = -1;

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
                AmbaDSP_WarpCore_SetVinSensorGeo(&gTestIS2.Mode, &TmpVinSensorGeo);
                AmbaDSP_WarpCore_SetR2rOutWin(&gTestIS2.Mode, &TmpR2rWin);
                AmbaDSP_WarpCore_SetDummyWinMarginRange(&gTestIS2.Mode, &TmpDmyRange);
                AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(&gTestIS2.Mode, &TmpWarpRefferenceDmyMarginPixel);
                AmbaDSP_WarpCore_SetDzoomFactor(&gTestIS2.Mode, &TmpDzoomInfo);
                AmbaDSP_WarpCore_SetOutputWin(&gTestIS2.Mode, &TmpOutWinInfo);
                AmbaDSP_WarpCore_SetDspVideoMode(&gTestIS2.Mode, 5);
                AmbaDSP_WarpCore_SetCalibCawarpInfo(&gTestIS2.Mode, &TmpCalcCaWarp);

                AmbaDSP_WarpCore_CalcDspCawarp(&gTestIS2.Mode, 0);
                AmbaDSP_WarpCore_SetDspCawarp(&gTestIS2.Mode);
            }
            Rval = 0;
        } else {
            Rval = -1;
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

static int test_is2_fpn(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    INT16 FPNTestWidth, FPNTestHeight;
    UINT16 VCapWidth, VCapHeight;
    VCapWidth = WAIT_APP_INFO_RAW_WIDTH;
    VCapHeight = WAIT_APP_INFO_RAW_HEIGHT;
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            //AmbaPrint();
            Rval = 0;
        } else if (strcmp(Argv[2], "highlight") == 0) {
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
            UINT8 Highligh;
            extern int AmbaDSP_ImgSetStaticBadPixelCorrectionHighlightMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT8 *pSbpHighligh);
            AmbaPrint("FPN Highlight mode");

            memset(&TmpMode, 0x0, sizeof(TmpMode));
            Highligh = atoi(Argv[3]);
            AmbaDSP_ImgSetStaticBadPixelCorrectionHighlightMode(&TmpMode, &Highligh);

            Rval = 0;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugStaticBadPixelCorrection(UINT8 TmpFPNDebugMessageFlag);
            UINT8 FPNDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugStaticBadPixelCorrection(FPNDebugFlag);
            Rval = 0;
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

                TmpSbpByPassCorr.Enable = atoi(Argv[2]);

                TmpSbpByPassCorr.PixelMapHeight = 0x3D8;
                TmpSbpByPassCorr.PixelMapWidth = 0x520;
                TmpSbpByPassCorr.PixelMapPitch = 0x1E0;
                TmpSbpByPassCorr.pMap = (UINT8*) 0x2484020;

                extern int AmbaDSP_ImgSetStaticBadPixelCorrectionByPass(AMBA_DSP_IMG_MODE_CFG_s Mode, AMBA_DSP_IMG_BYPASS_SBP_INFO_s *pSbpCorrByPass);
                AmbaDSP_ImgSetStaticBadPixelCorrectionByPass(TmpMode, &TmpSbpByPassCorr);
            }
            /*{
             AMBA_FS_FILE *Fid;
             char *Fn = "C:\\fpn_table_4608x3600.bin";
             char *Fmode = "w";

             Fid = AmbaFS_fopen(Fn, Fmode);
             if (Fid == NULL) {
             AmbaPrint("file open error");
             return Rval;
             }
             Rval = AmbaFS_fwrite((void const*)FPNMap, sizeof(UINT8),(CALIB_FPN_WIDTH*CALIB_FPN_HEIGHT/8) , Fid);
             AmbaFS_fclose(Fid);
             }*/

            Rval = 0;
        } else {
            Rval = -1;
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

#define CALIB_VNC_WIDTH  (3840)//*/1312
#define CALIB_VNC_HEIGHT (2160)//*/998
#define VIGNETTE_TABLE_WIDTH (65)
#define VIGNETTE_TABLE_HEIGTH (49)
#define VIGNETTE_VIG_STRENGTH_UNIT_SHIFT (16)
static UINT16 VignetteRedGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];
static UINT16 VignetteGreenEvenGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];
static UINT16 VignetteGreenOddGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];
static UINT16 VignetteBlueGainTbl[VIGNETTE_TABLE_WIDTH * VIGNETTE_TABLE_HEIGTH];

static int test_is2_vnc(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = -1;
    INT16 VncTestWidth, VncTestHeight;
    UINT16 VCapWidth, VCapHeight;
    VCapWidth = WAIT_APP_INFO_RAW_WIDTH;
    VCapHeight = WAIT_APP_INFO_RAW_HEIGHT;
    if (!ishelp(Argv[2])) {
        if (strcmp(Argv[2], "") == 0) {
            Rval = 0;
        } else if ((strcmp(Argv[2], "load") == 0) && (Argc == 5)) {
            UT_IK_DEBF("FIXME: Not Yet Implemented");
            Rval = 0;
        } else if (strcmp(Argv[2], "test") == 0) {
            UT_IK_DEBF("FIXME: Not Yet Implemented");
            Rval = 0;
        } else if (strcmp(Argv[2], "debug") == 0) {
            extern void AmbaDSP_ImgSetDebugVignetteCompensation(UINT8 TmpVignetteDebugMessageFlag);
            UINT8 VignetteDebugFlag = atoi(Argv[3]);
            AmbaDSP_ImgSetDebugVignetteCompensation(VignetteDebugFlag);
            Rval = 0;
        } else if (strcmp(Argv[2], "bypasstest") == 0) {
            UINT16 *pGain[4], UnitGain;
            int i, j, idx0, idx1;
            AMBA_DSP_IMG_BYPASS_VIGNETTE_INFO_s TestVigCorrByPass;
            AMBA_DSP_IMG_MODE_CFG_s TmpMode;
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

            extern int AmbaDSP_ImgSetVignetteCompensationByPass(AMBA_DSP_IMG_MODE_CFG_s Mode, AMBA_DSP_IMG_BYPASS_VIGNETTE_INFO_s *pVigCorrByPass);
            AmbaDSP_ImgSetVignetteCompensationByPass(TmpMode, &TestVigCorrByPass);

            Rval = 0;
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
             Fid = AmbaFS_fopen(FnAll, Fmode);
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

            Rval = 0;
        } else {
            Rval = -1;
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

#define MAX_IDSP_DUMP_SIZE (149824)
static UINT8 IdspDump[MAX_IDSP_DUMP_SIZE + 32];
static int _test_is2_get_idsp_debug_dump(UINT8 SecId, UINT8 *pdebugData)
{
    extern int AmbaDSP_CmdDumpIDspCfg(UINT32 BatchID, UINT32 *DramAddr, UINT32 *DramSize, UINT32 *Mode);
    int SecSize;
    UINT32 *pSecSize;
    UINT32 DramAddr;
    UINT32 DramSize;
    UINT32 SectionID;
    if (pdebugData == NULL) {
        UT_IK_ERRF("Input parameter pdebugData = NULL");
        return -1;
    }
    *((int *)pdebugData) = 0xdeadbeef; // clear chip rev for evaluating if dsp is halted
    AmbaCache_Clean((void *)pdebugData, MAX_IDSP_DUMP_SIZE);
    AmbaKAL_TaskSleep(5);
    DramAddr = ((UINT32)(pdebugData) & 0x0FFFFFFF);
    DramSize = MAX_IDSP_DUMP_SIZE;
    SectionID = (UINT32)SecId;
    UT_IK_DEBF("idsp dump address is 0x%.8X, sec_id:%d", DramAddr, SecId);
    AmbaDSP_CmdDumpIDspCfg(0, &DramAddr, &DramSize, &SectionID);
    AmbaKAL_TaskSleep(100);
    AmbaCache_Flush((void *)DramAddr, MAX_IDSP_DUMP_SIZE);
    DramAddr = ((UINT32)0xFFFFFFFF);
    if (*((int *)pdebugData) == 0xdeadbeef) {
        UT_IK_WARF("DSP is halted! Dumping post mortem section %d debug data", SecId);
    } else {
        pSecSize = (UINT32 *)(pdebugData + 8);
        SecSize = ((UINT32)*pSecSize) + 64; //64 for header,
    }
    return SecSize;
}

static int test_is2_idsp(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int Rval = 0;
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
                UT_IK_DEBF("idsp_dump addr %0.8X", pDumpAddress);
                if (Argc == 5) {
                    sprintf(FnFileName, "%c:\\%s_sec%d.bin", _TextHdlr_Get_Driver_Letter(), Argv[4], SecId);
                } else {
                    sprintf(FnFileName, "%c:\\%s_sec%d.bin", _TextHdlr_Get_Driver_Letter(), FnIdspDump, SecId);
                }

                Fid = AmbaFS_fopen(FnFileName, Fmode);
                if (Fid == NULL ) {
                    UT_IK_WARF("File open fail. Skip dumping debug data %s", FnFileName);
                    return Rval;
                }
                Rval = AmbaFS_fwrite((void const*) pDumpAddress, sizeof(UINT8), MAX_IDSP_DUMP_SIZE, Fid);
                AmbaFS_fclose(Fid);
                Rval = 0;
            } else {
                Rval = -1;
            }
        } else if (strcmp(Argv[2], "debug") == 0) {
            UINT8 Module;
            UINT8 Level;
            UINT8 Mask;
            Module = (UINT8)atoi(Argv[3]);
            Level = (UINT8)atoi(Argv[4]);
            Mask = (UINT8)atoi(Argv[5]);
            AmbaDSP_CmdSetDebugLevel(Module, Level, Mask);
            Rval = 0;
        }
    } else {
        Rval = -1;
    }

    if (Rval == -1) {
        AmbaPrint("Usage: t img -idsp dump [0, 1-7] [filename.bin]: dump idsp configuration");
        AmbaPrint("       t img -idsp dump [100] [filename.bin]: dump MCTF configuration");
        AmbaPrint("       t img -idsp debug [module(8 bits)] [level(8 bits)] [mask(8 bits): set dsp debug level");
    }
    return Rval;
}

static const AMBA_DSP_IMG_TEST_FUNC_s ImgTestList[] = { { test_is2_init, CMD_VISIBLE, "-init", "init image kernel" },
                                                        { test_is2_mode, CMD_VISIBLE, "-mode", "Test Mode setup" },
                                                        { test_is2_demosaic, CMD_VISIBLE, "-demosaic", "demosaic setup"},
                                                        { test_is2_blc, CMD_VISIBLE, "-blc", "black level correction setup" },
                                                        { test_is2_wb, CMD_VISIBLE, "-wb", "wb gain setup" },
                                                        { test_is2_r2y, CMD_VISIBLE, "-r2y", "RGB to YUV setup" },
                                                        { test_is2_cc, CMD_VISIBLE, "-cc", "color correction setup" },
                                                        { test_is2_tone, CMD_VISIBLE, "-tone", "tone curve setup" },
                                                        { test_is2_dpc, CMD_VISIBLE, "-dpc", "dynamic bad pixel correction setup" },
                                                        { test_is2_antialias, CMD_VISIBLE, "-aa", "Antialising setup" },
                                                        { test_is2_leakage, CMD_VISIBLE, "-lk", "Leakage filter setup" },
                                                        { test_is2_cnf, CMD_VISIBLE, "-cnf", "CFA noise filter setup" },
                                                        { test_is2_le, CMD_VISIBLE, "-le", "local exposure setup" },
                                                        { test_is2_cs, CMD_VISIBLE, "-cs", "chroma scale setup" },
                                                        { test_is2_cmf, CMD_VISIBLE, "-cmf", "chroma median filter setup" },
                                                        { test_is2_cc_reg, CMD_VISIBLE, "-cc_reg", "color correction register setup" },
                                                        { test_is2_defblc, CMD_HIDDEN, "-defblc", "" },
                                                        { test_is2_float, CMD_VISIBLE, "-float", "floating aaa statistics setup" },
                                                        { test_is2_stat, CMD_VISIBLE, "-stat", "aaa statistics setup" },
                                                        { test_is2_rawenc, CMD_VISIBLE, "-rawenc", "raw encode" },
                                                        { test_is2_raw, CMD_VISIBLE, "-raw", "raw capture" },
                                                        { test_is2_lowiso, CMD_VISIBLE, "-lowiso", "low iso verification function" },
                                                        { test_is2_ituner, CMD_VISIBLE, "-ituner", "ituner verification function" },
                                                        { test_is2_cf, CMD_VISIBLE, "-cf", "chroma filter setup" },
                                                        { test_is2_highiso, CMD_VISIBLE, "-highiso", "high iso verification function" },
                                                        { test_is2_Rash, CMD_VISIBLE, "-rash", "recursive amba shell" },
                                                        { test_is2_profile, CMD_HIDDEN, "-profile", "" },
                                                        { test_is2_warp, CMD_VISIBLE, "-warp", "warp setup" },
                                                        { test_is2_ca, CMD_VISIBLE, "-ca", "cawarp setup" },
                                                        { test_is2_fpn, CMD_VISIBLE, "-fpn", "pfn correction setup" },
                                                        { test_is2_vnc, CMD_VISIBLE, "-vnc", "vignette compensation setup" },
//                                                        { test_is2_test, CMD_VISIBLE, "-test", "idsp debug utilities"},
                                                        { test_is2_idsp, CMD_VISIBLE, "-idsp", "idsp debug utilities"} };




static void usage(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int i;
    AmbaPrint("\n\r"
              "Usage: t %s [OPTION] VALUE...\n\r"
              "Supported options:\n\r",
              Argv[0]);
    for (i = 0; i < GET_ARRAY_NUMBER(ImgTestList); i++) {
        if (ImgTestList[i].Visibility == CMD_VISIBLE) {
            AmbaPrint("\t%10s\t: %s", ImgTestList[i].Command, ImgTestList[i].Description);
        }
    }
    AmbaPrint("\n\t"
              "Try 't %s [OPTION] --help' for more information.",
              Argv[0]);
}

static int AmbaSSPTest_IK_Cmd(AMBA_SHELL_ENV_s *Env, int Argc, char **Argv)
{
    int i;
    for (i = 0; i < GET_ARRAY_NUMBER(ImgTestList); i++) {
        if (0 == strcmp(Argv[1], ImgTestList[i].Command)) {
            ImgTestList[i].Fp(Env, Argc, Argv);
            return 1;
        }
    }

    usage(Env, Argc, Argv);
    return -1;
}

int Img_TestAdd(void)
{
    memset(&gTestIS2, 0x0, sizeof(AMBA_DSP_IMG_TEST_IS2_s));
    AmbaTest_RegisterCommand("img", AmbaSSPTest_IK_Cmd);
    return 0;
}

