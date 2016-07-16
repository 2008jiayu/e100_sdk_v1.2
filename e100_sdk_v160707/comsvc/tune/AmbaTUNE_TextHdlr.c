/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTUNE_TextHdlr.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella Image Tuning Tools Text Handler
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaTUNE_TextHdlr.h"
#include "AmbaPrintk.h"
#include "stdio.h" // sprintf need
#include "AmbaKAL.h"
#include "AmbaTUNE_Parser.h"
#include "AmbaTUNE_Rule.h"
#include "AmbaTUNE_VideoHDRRule.h"
#include "AmbaCache.h"
#include "AmbaFS.h" //Note: AMBA_FS_FILE  need

//#include "AmbaImgCalibItuner.h" //Note: ITUNER_SYSTEM_s need
//#include "AmbaDSP_EventInfo.h"
//#include "AmbaDSP_ImgDef.h" // Note: AMBA_DSP_IMG_PIPE_VIDEO need
//#include "AmbaDSP.h" // Note: AMBA_DSP_VIDEO_FILTER need
//#include "AmbaDSP_ImgFilter.h" // Note: AMBA_DSP_IMG_NUM_EXPOSURE_CURVE need
//#include "AmbaDSP_Img3aStatistics.h" // Note: AMBA_DSP_IMG_AE_STAT_INFO_s need
//#include "AmbaCardManager.h"// Note: AmbaSCM_GetSlotStatus need
//#include "AmbaUtility.h"// Note: AmbaUtility_Slot2Drive need

#define HDLR_DEBF(fmt, arg...) AmbaPrint("[TEXT HDLR][DEBUG]"fmt, ##arg)
#define HDLR_WARF(fmt, arg...) AmbaPrint("[TEXT HDLR][WARNING]"fmt, ##arg)
#define HDLR_ERRF(fmt, arg...) AmbaPrint("[TEXT HDLR][ERROR]"fmt, ##arg)


static AMBA_FS_FILE* _posix_fopen(const char *FileName, const char *Mode)
{
    AMBA_FS_FILE *Rval = NULL;
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        K_ASSERT(0);
        /* Note: A12 not support unicode
        WCHAR w_filename[64];
        WCHAR w_mode[3];
        w_asc2uni(w_filename, FileName, sizeof(w_filename) / sizeof(w_filename[0]));
        w_asc2uni(w_mode, Mode, sizeof(w_mode) / sizeof(w_mode[0]));
        Rval = AmbaFS_fopen((char*)w_filename, (char*)w_mode);
        */
    } else {
        Rval = AmbaFS_fopen(FileName, Mode);
    }
    return Rval;
}

static int _posix_stat(const char *FileName, AMBA_FS_STAT *pStat)
{
    int Rval = 0;
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        K_ASSERT(0);
        /* A12 not support unicode
        WCHAR w_filename[128];
        w_asc2uni(w_filename, FileName, sizeof(w_filename) / sizeof(w_filename[0]));
        Rval = AmbaFS_Stat((char*)w_filename, pStat);
        */
    } else {
        Rval = AmbaFS_Stat(FileName, pStat);
    }
    return Rval;
}


#define POSIX_FILE AMBA_FS_FILE
#define POSIX_TYPE_STAT AMBA_FS_STAT
#define POSIX_FOPEN(FileName, Mode) _posix_fopen(FileName, Mode)
#define POSIX_FREAD(DestAddr, Size, Count, Fd) AmbaFS_fread(DestAddr, Size, Count, Fd)
#define POSIX_FWRITE(SrcAddr, Size, Count, Fd) AmbaFS_fwrite(SrcAddr, Size, Count, Fd)
#define POSIX_FCLOSE(Fd) AmbaFS_fclose(Fd)
#define POSIX_STAT(FileName, FileInfo) _posix_stat(FileName, FileInfo)
#define POSIX_FSYNC(Fd) AmbaFS_FSync(Fd)

#define MIN(a, b) ((a) > (b) ? (b) : (a))

typedef void (*pfunc_conv_t)(int, void*, void*);

typedef struct _ITUNER_OBJ_s_ {
    AMBA_DSP_IMG_MODE_CFG_s TuningAlgoMode;
    TUNING_MODE_EXT_e TuningModeExt;
    TUNE_Parser_Object_t* ParserObject;
    UINT8 Is_Idsp_Load_Finish;
} ITUNER_OBJ_s;

static int _TextHdlr_Check_Param_Completeness(void);
static AMBA_DSP_IMG_PIPE_e _TextHdlr_Get_Imag_Pipe(TUNING_MODE_e TuningMode);
static AMBA_DSP_IMG_FUNC_MODE_e _TextHdlr_Get_Image_Func_Mode(TUNING_MODE_EXT_e TuningModeExt);
static AMBA_DSP_IMG_ALGO_MODE_e _TextHdlr_Get_Image_Algo_Mode(TUNING_MODE_e TuningMode);
static int _TextHdlr_Update_ItunerInfo(void);
static int _TextHdlr_Save_Ext_File(char *FileName, int Size, void *Buf);
static int _TextHdlr_load_Ext_file(char *FileName, int Size, void *Buf);
static ITUNER_OBJ_s _Ituner;

static char Ituner_File_Path[64];


/**
 * @brief Get Driver Letter of SD Card
 *
 * @param None
 *
 * @return Driver Letter, or z(without SD card Insert)
 */
char _TextHdlr_Get_Driver_Letter(void)
{
    HDLR_DEBF("%s() %d, Not Yet Ready", __func__, __LINE__);
    return 'c';
}

static int _TextHdlr_Change_System_Drive(char *filepath)
{
    char drive_letter;
    drive_letter = _TextHdlr_Get_Driver_Letter();
    if (drive_letter == 'z') {
        HDLR_WARF("%s() %d, No SD Card present!", __func__, __LINE__);
        return -1;
    }
    if ((strncmp(&filepath[1], ":", 1) == 0) && (strncmp(&filepath[2], "\\", 1) == 0)) {
        filepath[0] = drive_letter;
        HDLR_DEBF("Change Drive Letter");
    } else {
        char filename[APP_MAX_FN_SIZE];
        if (strncmp(&filepath[0], "\\", 1) == 0) {
            snprintf(filename, sizeof(filename), "%c:%s", drive_letter, filepath);
        } else {
            snprintf(filename, sizeof(filename), "%c:\\%s", drive_letter, filepath);
        }

        memcpy(filepath, filename, MIN(APP_MAX_FN_SIZE, strlen(filename)));
        HDLR_DEBF("Gen File Path: File Name %s, FilePath %s", filename, filepath);
    }
    return 0;
}



/* string conversion functions */

static void asc_to_uni(const char *FName, char *Path)
{
    while (*FName != '\0') {
        *Path = *FName;
        Path++;
        FName++;
    }
    *Path = '\0';
}

//static void uni_to_asc(const char *Path, char *FName)
//{
//    while (*Path != '\0') {
//        *FName = *Path;
//        FName++;
//        Path++;
//    }
//    *FName = '\0';
//}



//
//static int _split_reg_id(char* Linebuf, char **RegId, char **Param)
//{
//    unsigned int Pos = 0;
//    char Seps[] = ".";
//
//    Pos = strcspn(Linebuf, Seps);
//    if (Pos == strlen(Linebuf)) return 0;
//    Linebuf[Pos] = 0;
//    *RegId = Linebuf;
//    *Param = Linebuf + Pos + 1;
//    return 1;
//}

#if 0
static int _read_line(POSIX_FILE *Fid, char * Line_Buf, int Buf_Size, char **Ptr1)
{
    char *Ptr;
    Ptr = Line_Buf;

    int Ret = 0;
    //if (POSIX_FEOF(Fid)) return 0;
    while(POSIX_FREAD(Ptr, 1, 1, Fid) > 0) {
        if ((*Ptr == '\r') || (*Ptr == '\n')) {
            Ptr = 0;
            Ret = 1;
            break;
        }
        (Ptr)++;
        if ((Ptr - Line_Buf) >= Buf_Size) {
            HDLR_WARF("%s() %d, Input Text String is too long", __func__, __LINE__);
            Ret = 1;
            break;
        }

    }
    return Ret;
}
#endif

static int _save_line(POSIX_FILE *Fid, char * LineBuf)
{
    return (POSIX_FWRITE(LineBuf, 1, strlen(LineBuf), Fid));
}
#define READ_BUF_SIZE 1024
static char ReadBuffer[READ_BUF_SIZE+32];
static int _TextHdlr_Parse_File(char *FileName, char* LineBuffer)
{
    int Line = 0;
    const char Fmode[] = {'r','b','\0'};
    static AMBA_FS_STAT Stat;
    POSIX_FILE *Fid;
    char* pReadBuf = &ReadBuffer[32 - (((UINT32) ReadBuffer) % 32)];
    char* pStartOfStr = pReadBuf;
    char* pDstStr = LineBuffer;
    int ReadSize;
    UINT32 len = 0;
    int i;
    POSIX_STAT(FileName, &Stat);
    Fid = POSIX_FOPEN(FileName, Fmode);
    if (Fid == 0) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }

    memset(ReadBuffer, 0x0, sizeof(ReadBuffer));

    ReadSize = Stat.Size;
    while (ReadSize > 0 && POSIX_FREAD(pReadBuf, ((ReadSize > READ_BUF_SIZE) ? READ_BUF_SIZE : ReadSize), 1, Fid) > 0) {
        for (i = 0; i < ((ReadSize > READ_BUF_SIZE) ? READ_BUF_SIZE : ReadSize); i++) {
            if (pReadBuf[i] == '\n' || pReadBuf[i] == '\r') {
                pReadBuf[i] = '\0';
            }
        }
        while (1) {
            len = strlen(pStartOfStr);
            if (&pStartOfStr[len] >= (pReadBuf + READ_BUF_SIZE)) {
                strncpy(pDstStr, pStartOfStr, READ_BUF_SIZE - (pStartOfStr - pReadBuf));
                pDstStr += (READ_BUF_SIZE - (pStartOfStr - pReadBuf));
                pStartOfStr = pReadBuf;
                // Feed ReadBuf Again
                break;
            } else {
                strncpy(pDstStr, pStartOfStr, READ_BUF_SIZE - (pStartOfStr - pReadBuf));
                pStartOfStr = &pStartOfStr[len + 1];
                //AmbaPrint("Feed String: %s", Buf);
                if (LineBuffer[0] != '\0') {
                    TUNE_Parser_Parse_Line(_Ituner.ParserObject);
                    Line++;
                }

                if (pStartOfStr - pReadBuf >= ReadSize) {
                    break;
                }
                pDstStr = LineBuffer;
            }
        }
        ReadSize -= READ_BUF_SIZE;
    }
    HDLR_DEBF("%s() %d, Read Line Num: %d", __func__, __LINE__, Line);
    POSIX_FCLOSE(Fid);
    return 0;
}
/*
static int _TextHdlr_Load_Text_HDR(char *FileName)
{
    Rule_Info_t Rule_Info;
    int i;
    char *Buf = NULL;
    int BufSize = 0;
    HDLR_DEBF("HDR file to load:%s", FileName);
    TUNE_Parser_Create(&_Ituner.ParserObject);

    TUNE_Video_HDR_Rule_Get_Info(&Rule_Info);

    for (i = 0; i < Rule_Info.RegCount; i++) {
        TUNE_Parser_Add_Reg(&Rule_Info.RegList[i], _Ituner.ParserObject);
    }
    if (0 != _TextHdlr_Change_System_Drive(FileName)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }

    if (0 != TUNE_Parser_Get_LineBuf(_Ituner.ParserObject, &Buf, &BufSize)) {
        HDLR_WARF("%s() %d, call TUNE_Parser_Get_LineBuf() Fail", __func__, __LINE__);
        return -1;
    }
    if (0 != _TextHdlr_Parse_File(FileName, Buf)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Parse_File(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }
    if (0 != _TextHdlr_Check_Param_Completeness()) {
        HDLR_WARF("%s() %d, call _TextHdlr_Check_Param_Completeness() Fail", __func__, __LINE__);
        return -1;
    }
    return 0;
}
*/
static int _TextHdlr_Load_Text(char *FileName)
{
    char *Buf = NULL;
    int BufSize = 0;
    char AscFname[64];
    memset(AscFname, 0x0, sizeof(AscFname));
    strncpy(AscFname, FileName, sizeof(AscFname));
    HDLR_DEBF("---------------------");
    HDLR_DEBF("iTuner ver. %d.%d", ITUNER_VER_MAJOR, ITUNER_VER_MINOR);
    HDLR_DEBF("---------------------");
    HDLR_DEBF("file to load:%s", AscFname);
    TUNE_Parser_Set_Opmode(ITUNER_DEC);
    if (0 != TUNE_Parser_Set_Reglist_Valid(0, _Ituner.ParserObject)) {
        HDLR_WARF("%s() %d, call TUNE_PArser_Set_Reglist_Valid() Fail", __func__, __LINE__);
        return -1;
    }
    if (0 != _TextHdlr_Change_System_Drive(FileName)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }

    if (0 != TUNE_Parser_Get_LineBuf(_Ituner.ParserObject, &Buf, &BufSize)) {
        HDLR_WARF("%s() %d, call TUNE_Parser_Get_LineBuf() Fail", __func__, __LINE__);
        return -1;
    }
    if (0 != _TextHdlr_Parse_File(AscFname, Buf)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Parse_File(%s) Fail", __func__, __LINE__, AscFname);
        return -1;
    }
    if (0 != _TextHdlr_Check_Param_Completeness()) {
        HDLR_WARF("%s() %d, call _TextHdlr_Check_Param_Completeness() Fail", __func__, __LINE__);
        return -1;
    }
    return 0;
}


int TextHdlr_Init(Ituner_Initial_Config_t *pInitialConfig)
{
    int i;
    AmbaItuner_Config_t Ituner_Config;
    Rule_Info_t Rule_Info;
    TUNE_Parser_Set_Opmode(ITUNER_DEC);
    memset(&_Ituner.TuningAlgoMode,0x0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    _Ituner.TuningAlgoMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    _Ituner.TuningAlgoMode.AlgoMode= AMBA_DSP_IMG_ALGO_MODE_FAST;
    _Ituner.TuningAlgoMode.BatchId= AMBA_DSP_VIDEO_FILTER;
    TUNE_Parser_Create(&_Ituner.ParserObject);
    _Ituner.Is_Idsp_Load_Finish = FALSE;
    TUNE_Rule_Get_Info(&Rule_Info);
    for (i = 0; i < Rule_Info.RegCount; i++) {
        TUNE_Parser_Add_Reg(&Rule_Info.RegList[i], _Ituner.ParserObject);
    }

    TUNE_Video_HDR_Rule_Get_Info(&Rule_Info);
    for (i = 0; i < Rule_Info.RegCount; i++) {
        TUNE_Parser_Add_Reg(&Rule_Info.RegList[i], _Ituner.ParserObject);
    }

    TUNE_Parser_Set_Reglist_Valid(0, _Ituner.ParserObject);

    /* //TBD
    for(i=0; i<MAX_REGS; i++) {
        if (img_ext_regs[i].index == -1) break;
        ituner_add_ext_reg(&img_ext_regs[i]);
    }*/
    Ituner_Config.Hook_Func.Load_Data = TextHdlr_Load_Data;
    Ituner_Config.Hook_Func.Save_Data = TextHdlr_Save_Data;
    Ituner_Config.pMemPool = (AMBA_KAL_BYTE_POOL_t *)pInitialConfig->pMemPool;
    return AmbaItuner_Init(&Ituner_Config);
}

int TextHdlr_Load_IDSP(char *Filepath)
{
    _Ituner.Is_Idsp_Load_Finish = TRUE;
    if (0 != _TextHdlr_Load_Text(Filepath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Load_Text(%s) Fail", __func__, __LINE__, Filepath);
        return -1;
    }
    _TextHdlr_Update_ItunerInfo();

    return 0;
}

static int _TextHdlr_Check_Param_Completeness(void)
{
    int Filter_Idx;
    int Param_Idx;
    int Ret = 0;
    REG_s* Reg = NULL;
    PARAM_s* Param = NULL;
    AMBA_ITUNER_VALID_FILTER_t FilterStatus;
    AmbaItuner_Get_FilterStatus(&FilterStatus);
    for (Filter_Idx = 0; Filter_Idx < _Ituner.ParserObject->RegCount; Filter_Idx++) {
        Reg = TUNE_Parser_Get_Reg(Filter_Idx, _Ituner.ParserObject);
        if (Reg == NULL) {
            HDLR_WARF("%s() %d, call TUNE_Param_Get_Reg Fail", __func__, __LINE__);
            continue;
        }
        if (FilterStatus[Reg->Index] == 1) {
            if (Reg->RegNum == 1) {
                UINT64 Mask = (0x1ULL << Reg->ParamCount) - 1;
                if ((Reg->Valid & Mask) != Mask) {
                    for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                        if (Reg->Valid & (0x1ULL << Param_Idx)) {

                        } else {
                            Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                            HDLR_WARF("%s() %d, Lost Param: %s.%s, %llx", __func__, __LINE__, Reg->Name, Param->Name, Reg->Valid);
                            Ret = -1;
                        }
                    }
                }
            } else {
                int i;
                for (i = 0; i < Reg->RegNum; i++) {
                    UINT64 Mask = ((0x1ULL << Reg->ParamCount) - 1) << (Reg->ParamCount * i);
                    if ((Reg->Valid & Mask) == 0x0ULL) {
                        // HDLR_DEBF("%s() %d, without %s[%d].*", __func__, __LINE__, Reg->Name, i);
                    } else if ((Reg->Valid & Mask) != Mask) {
                        for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                            if (Reg->Valid & (0x1ULL << (Reg->ParamCount * i + Param_Idx))) {

                            } else {
                                Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                                HDLR_WARF("%s() %d, Lost Param: %s[%d].%s, %llx", __func__, __LINE__, Reg->Name, i, Param->Name, Reg->Valid);
                                Ret = -1;
                            }
                        }
                    }
                }
            }
        }
    }
    return Ret;
}

int TextHdlr_Execute_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_ITUNER_PROC_INFO_s *ProcInfo)
{
    int Rval = 0;
    Rval = AmbaItuner_Execute(pMode, ProcInfo);
    return Rval;
}

int TextHdlr_Get_SystemInfo(ITUNER_SYSTEM_s *System) {
    AmbaItuner_Get_SystemInfo(System);
    return 0;
}

int TextHdlr_Set_SystemInfo(ITUNER_SYSTEM_s *System) {
    AmbaItuner_Set_SystemInfo(System);
    return 0;
}

int TextHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo) {
    AmbaItuner_Get_AeInfo(AeInfo);
    return 0;
}

int TextHdlr_Set_AeInfo(ITUNER_AE_INFO_s *AeInfo) {
    AmbaItuner_Set_AeInfo(AeInfo);
    return 0;
}

int TextHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo) {
    AmbaItuner_Get_WbSimInfo(WbSimInfo);
    return 0;
}

int TextHdlr_Set_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo) {
    AmbaItuner_Set_WbSimInfo(WbSimInfo);
    return 0;
}


int TextHdlr_Get_ItunerInfo(ITUNER_INFO_s *ItunerInfo) {
    ITUNER_SYSTEM_s System;
    TUNING_MODE_e TuningMode;
    AmbaItuner_Get_SystemInfo(&System);
    TuningMode = TUNE_Rule_Lookup_Tuning_Mode(System.TuningMode);
    _Ituner.TuningModeExt = TUNE_Rule_Lookup_Tuning_Mode_Ext(System.TuningModeExt);
    _Ituner.TuningAlgoMode.Pipe = _TextHdlr_Get_Imag_Pipe(TuningMode);
    _Ituner.TuningAlgoMode.AlgoMode = _TextHdlr_Get_Image_Algo_Mode(TuningMode);
    _Ituner.TuningAlgoMode.BatchId = (_Ituner.TuningAlgoMode.Pipe == AMBA_DSP_IMG_PIPE_VIDEO)? AMBA_DSP_VIDEO_FILTER : AMBA_DSP_STILL_LISO_FILTER;
    _Ituner.TuningAlgoMode.FuncMode = _TextHdlr_Get_Image_Func_Mode(_Ituner.TuningModeExt);
    memcpy(&ItunerInfo->TuningAlgoMode, &_Ituner.TuningAlgoMode, sizeof(ItunerInfo->TuningAlgoMode));
    ItunerInfo->TuningModeExt = _Ituner.TuningModeExt;
    return 0;
}
static int _TextHdlr_Update_ItunerInfo(void)
{
    ITUNER_SYSTEM_s System;
    TUNING_MODE_e TuningMode;
    AmbaItuner_Get_SystemInfo(&System);
    TuningMode = TUNE_Rule_Lookup_Tuning_Mode(System.TuningMode);
    _Ituner.TuningModeExt = TUNE_Rule_Lookup_Tuning_Mode_Ext(System.TuningModeExt);
    _Ituner.TuningAlgoMode.Pipe = _TextHdlr_Get_Imag_Pipe(TuningMode);
    _Ituner.TuningAlgoMode.AlgoMode = _TextHdlr_Get_Image_Algo_Mode(TuningMode);
    _Ituner.TuningAlgoMode.BatchId = (UINT32)((_Ituner.TuningAlgoMode.Pipe == AMBA_DSP_IMG_PIPE_VIDEO)? AMBA_DSP_VIDEO_FILTER : AMBA_DSP_STILL_LISO_FILTER);
    if (TuningMode == IMG_MODE_NIGHT_PORTRAIT_STILL) {
        System.InputPicCnt = 4;
    } else {
        System.InputPicCnt = 1;
    }
    AmbaItuner_Set_SystemInfo(&System);
    return 0;
}

static int _TextHdlr_Update_System_Info(AMBA_DSP_IMG_MODE_CFG_s *pMode)
{
    ITUNER_SYSTEM_s System;
    char const *Tuning_Mode_Str;
    AmbaItuner_Get_SystemInfo(&System);
    if (pMode->Pipe == AMBA_DSP_IMG_PIPE_VIDEO) {
        if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
            Tuning_Mode_Str = TUNE_Rule_Lookup_Tuning_Mode_Str(IMG_MODE_HIGH_ISO_VIDEO);
        } else {
            Tuning_Mode_Str = TUNE_Rule_Lookup_Tuning_Mode_Str(IMG_MODE_VIDEO);
        }
    } else { //AMBA_DSP_IMG_PIPE_STILL
        if (pMode->AlgoMode == AMBA_DSP_IMG_ALGO_MODE_HISO) {
            Tuning_Mode_Str = TUNE_Rule_Lookup_Tuning_Mode_Str(IMG_MODE_HIGH_ISO_STILL);
        } else {
            Tuning_Mode_Str = TUNE_Rule_Lookup_Tuning_Mode_Str(IMG_MODE_LOW_ISO_STILL);
        }
    }
    strncpy(System.TuningMode, Tuning_Mode_Str, sizeof(System.TuningMode));
    AmbaItuner_Set_SystemInfo(&System);
    return 0;
}


static int _TextHdlr_Get_Ext_File_Path(Ituner_Ext_File_Type_e Ext_File_Type, char *Ext_File_path)
{
    char* Ext_pos;
    if (strlen(Ituner_File_Path) < 7) {
        HDLR_ERRF("%s() %d, Ituner_File_Path = %s, it should not happened!", __func__, __LINE__,Ituner_File_Path);
        return -1;
    }
    strncpy(Ext_File_path, Ituner_File_Path, 64);
    Ext_pos = strrchr(Ext_File_path, '.');
    if (Ext_pos == NULL) {
        HDLR_WARF("%s() %d, Ituner_File_Path = %s, it should not happened!", __func__, __LINE__,Ituner_File_Path);
        return -1;
    }
    *Ext_pos = '\0';
    switch (Ext_File_Type) {
    case EXT_FILE_FPN_MAP:
        strcat(Ext_File_path, "_Fpn_Map.bin");
        break;
    case EXT_FILE_VIGNETTE_GAIN:
        strcat(Ext_File_path, "_Vignette_Gain.bin");
        break;
    case EXT_FILE_WARP_TABLE:
        strcat(Ext_File_path, "_Warp_Table.bin");
        break;
    case EXT_FILE_CA_TABLE:
        strcat(Ext_File_path, "_Ca_Table.bin");
        break;
    case EXT_FILE_CC_REG:
        strcat(Ext_File_path, "_CC_Reg.bin");
        break;
    case EXT_FILE_CC_THREED:
        strcat(Ext_File_path, "_CC_3d.bin");
        break;
    default:
        HDLR_ERRF("%s() %d, Invalid Ext_File_Type = %d", __func__, __LINE__, Ext_File_Type);
        return -1;
    }
    return 0;
}

static int _TextHdlr_Save_Raw(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char UniRawPath[256];
    char *pch;
    ITUNER_SYSTEM_s System;
    POSIX_FILE *Fd;
    AmbaItuner_Get_SystemInfo(&System);
    if ((strlen(System.OutputFilename) < 8) || Ext_File_Param->Raw_Save_Param.Address == NULL || Ext_File_Param->Raw_Save_Param.Size == 0)
    {
        HDLR_WARF("%s() %d, RawPath: %s, Addr: %x, Size: %d",
            __func__, __LINE__, System.RawPath, Ext_File_Param->Raw_Save_Param.Address, Ext_File_Param->Raw_Save_Param.Size);
        return -1;
    }
    if (0 != _TextHdlr_Change_System_Drive(System.OutputFilename)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, System.RawPath);
        return -1;
    }
    asc_to_uni(System.OutputFilename, UniRawPath);
    pch = strtok(UniRawPath,".");
    strcat(pch,".raw");

    Fd = POSIX_FOPEN(pch, "w");
    if (Fd == NULL) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, pch);
        return -1;
    }
    POSIX_FWRITE(Ext_File_Param->Raw_Save_Param.Address, Ext_File_Param->Raw_Save_Param.Size, 1, Fd);
    POSIX_FCLOSE(Fd);
    return 0;
}

static int _TextHdlr_Load_Raw(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    int Ret = 0;
    char UniRawPath[256];
    ITUNER_SYSTEM_s System;
    POSIX_FILE *Fd;
    int InputPitch;
    int i;
    int Offset = 0;
    AmbaItuner_Get_SystemInfo(&System);
    if (strlen(System.RawPath) < 8 || Ext_File_Param->Raw_Load_Param.Address == NULL || Ext_File_Param->Raw_Load_Param.Max_Size == 0) {
        HDLR_WARF("%s() %d, RawPath: %s, Addr: %x, Max_Size: %d",
            __func__, __LINE__, System.RawPath, Ext_File_Param->Raw_Load_Param.Address, Ext_File_Param->Raw_Load_Param.Max_Size);
        return -1;
    }
    if (0 != _TextHdlr_Change_System_Drive(System.RawPath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, System.RawPath);
        return -1;
    }
    asc_to_uni(System.RawPath, UniRawPath);
    Fd = POSIX_FOPEN(UniRawPath, "r");
    if (Fd == NULL) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, UniRawPath);
        return -1;
    }
    InputPitch = ((System.RawPitch + 31) >> 5) << 5;

    for (i = 0; i < System.RawHeight; i++) {
        if ((Offset + System.RawPitch) > Ext_File_Param->Raw_Load_Param.Max_Size) {
            HDLR_WARF("%s() %d, Load Size Buffer Full, Max_Size = %d, i = %d, InputPitch = %d",
                __func__, __LINE__, Ext_File_Param->Raw_Load_Param.Max_Size, i, InputPitch);
            Ret = -1;
        }
        POSIX_FREAD(&((UINT8*)Ext_File_Param->Raw_Load_Param.Address)[Offset], System.RawPitch, 1, Fd);
        Offset += InputPitch;
    }
    POSIX_FCLOSE(Fd);
    return Ret;
}

static int _TextHldr_Load_Vignette(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_VIGNETTE_s VignetteCompensation;
    AmbaItuner_Get_VignetteCompensation(&VignetteCompensation);
    if (VignetteCompensation.Enable) {
        HDLR_DEBF("%s() %d, Load Vignette Gain", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (strlen(VignetteCompensation.CalibTablePath) < 4 || Ext_File_Param->Vignette_Gain_Load_Param.Address == NULL || Ext_File_Param->Vignette_Gain_Load_Param.Size == 0) {
            HDLR_DEBF("%s() %d, Vignette_Gain_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, VignetteCompensation.CalibTablePath, Ext_File_Param->Vignette_Gain_Load_Param.Address, Ext_File_Param->Vignette_Gain_Load_Param.Size);
            return 0;
        }
        if (_TextHdlr_load_Ext_file(VignetteCompensation.CalibTablePath, Ext_File_Param->Vignette_Gain_Load_Param.Size, Ext_File_Param->Vignette_Gain_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,VignetteCompensation.CalibTablePath, Ext_File_Param->Vignette_Gain_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Vignette Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHldr_Load_Bypass_Vignette(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_VIGNETTE_BYPASS_s VignetteCompensationByPass;
    AmbaItuner_Get_VignetteCompensationByPass(&VignetteCompensationByPass);
    if (VignetteCompensationByPass.Enable) {
        HDLR_DEBF("%s() %d, Load Bypass Vignette Gain", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }

        if (strlen(VignetteCompensationByPass.GainPath) < 4 || Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Address == NULL || Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, Bypass Vignette_Gain_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, VignetteCompensationByPass.GainPath, Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Address, Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Size);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(VignetteCompensationByPass.GainPath, Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Size, Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,VignetteCompensationByPass.GainPath, Ext_File_Param->Bypass_Vignette_Gain_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Bypass Vignette Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}
static int _TextHdlr_Load_Warp_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_WARP_s WarpCompensation;
    AmbaItuner_Get_WarpCompensation(&WarpCompensation);
    if (WarpCompensation.Enable) {
        HDLR_DEBF("%s() %d, Load Warp Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (strlen(WarpCompensation.WarpGridTablePath) < 4 || Ext_File_Param->Warp_Table_Load_Param.Address == NULL || Ext_File_Param->Warp_Table_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, Warp_Table_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, WarpCompensation.WarpGridTablePath, Ext_File_Param->Warp_Table_Load_Param.Address, Ext_File_Param->Warp_Table_Load_Param.Size);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(WarpCompensation.WarpGridTablePath, Ext_File_Param->Warp_Table_Load_Param.Size, Ext_File_Param->Warp_Table_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,WarpCompensation.WarpGridTablePath, Ext_File_Param->Warp_Table_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Warp Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Load_Bypass_Horizontal_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_WARP_DZOOM_BYPASS_s WarpCompensationDzoomByPass;
    AmbaItuner_Get_WarpCompensationDzoomByPass(&WarpCompensationDzoomByPass);
    if (WarpCompensationDzoomByPass.WarpControl) {
        POSIX_TYPE_STAT FileInfo;
        HDLR_DEBF("%s() %d, Load Warp Horizontal Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }

        if (strlen(WarpCompensationDzoomByPass.WarpHorizontalTablePath) < 4 || Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Address == NULL || Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Max_Size == 0) {
            HDLR_ERRF("%s() %d, Warp_Horizontal_Table_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, WarpCompensationDzoomByPass.WarpHorizontalTablePath, Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Address, Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Max_Size);
            return -1;
        }

        if (POSIX_STAT(WarpCompensationDzoomByPass.WarpHorizontalTablePath, &FileInfo) != OK) {
            HDLR_WARF("%s() %d, call POSIX_STAT(%s) Fail", __func__, __LINE__, WarpCompensationDzoomByPass.WarpHorizontalTablePath);
            return -1;
        }
        if (FileInfo.Size > Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Max_Size) {
            HDLR_WARF("%s() %d, call Invalid File Size", __func__, __LINE__);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(WarpCompensationDzoomByPass.WarpHorizontalTablePath, FileInfo.Size, Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,WarpCompensationDzoomByPass.WarpHorizontalTablePath, Ext_File_Param->Bypass_Warp_Horizontal_Table_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Bypass Horizontal Warp Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Load_Bypass_Vertical_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_WARP_DZOOM_BYPASS_s WarpCompensationDzoomByPass;
    AmbaItuner_Get_WarpCompensationDzoomByPass(&WarpCompensationDzoomByPass);
    if (WarpCompensationDzoomByPass.VertWarpEnable) {
        POSIX_TYPE_STAT FileInfo;
        HDLR_DEBF("%s() %d, Load Warp Vertical Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }

        if (strlen(WarpCompensationDzoomByPass.WarpVerticalTablePath) < 4 || Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Address == NULL || Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Max_Size == 0) {
            HDLR_ERRF("%s() %d, Warp_Vertical_Table_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, WarpCompensationDzoomByPass.WarpVerticalTablePath, Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Address, Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Max_Size);
            return -1;
        }

        if (POSIX_STAT(WarpCompensationDzoomByPass.WarpVerticalTablePath, &FileInfo) != OK) {
            HDLR_WARF("%s() %d, call POSIX_STAT(%s) Fail", __func__, __LINE__, WarpCompensationDzoomByPass.WarpVerticalTablePath);
            return -1;
        }
        if (FileInfo.Size > Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Max_Size) {
            HDLR_WARF("%s() %d, call Invalid File Size", __func__, __LINE__);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(WarpCompensationDzoomByPass.WarpVerticalTablePath, FileInfo.Size, Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,WarpCompensationDzoomByPass.WarpVerticalTablePath, Ext_File_Param->Bypass_Warp_Vertical_Table_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Bypass Vertical Warp Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Load_Ca_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_CHROMA_ABERRATION_s ChromaAberrationInfo;
    AmbaItuner_Get_ChromaAberrationInfo(&ChromaAberrationInfo);
    if (ChromaAberrationInfo.Enable) {
        HDLR_DEBF("%s() %d, Load Ca Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (strlen(ChromaAberrationInfo.CaGridTablePath) < 4 || Ext_File_Param->Ca_Table_Load_Param.Address == NULL || Ext_File_Param->Ca_Table_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, Ca_Table_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, ChromaAberrationInfo.CaGridTablePath, Ext_File_Param->Ca_Table_Load_Param.Address, Ext_File_Param->Ca_Table_Load_Param.Size);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(ChromaAberrationInfo.CaGridTablePath, Ext_File_Param->Ca_Table_Load_Param.Size, Ext_File_Param->Ca_Table_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,ChromaAberrationInfo.CaGridTablePath, Ext_File_Param->Ca_Table_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Ca Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHldr_Load_Bypass_Ca_Horizontal_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_CHROMA_ABERRATION_BYPASS_s ChromaAberrationInfoByPass;
    AmbaItuner_Get_ChromaAberrationInfoByPass(&ChromaAberrationInfoByPass);

    if (ChromaAberrationInfoByPass.HorzWarpEnable) {
        POSIX_TYPE_STAT FileInfo;
        HDLR_DEBF("%s() %d, Load Bypass Horizontal Ca Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }

        if (strlen(ChromaAberrationInfoByPass.WarpHorzTablePath) < 4 || Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Address == NULL || Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Max_Size == 0) {
            HDLR_ERRF("%s() %d, Bypass Horizontal Ca_Table_Path = %s, Address = %p, Max_Size = %d",
                    __func__, __LINE__, ChromaAberrationInfoByPass.WarpHorzTablePath, Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Address, Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Max_Size);
            return -1;
        }

        if (POSIX_STAT(ChromaAberrationInfoByPass.WarpHorzTablePath, &FileInfo) != OK) {
            HDLR_WARF("%s() %d, call POSIX_STAT(%s) Fail", __func__, __LINE__, ChromaAberrationInfoByPass.WarpHorzTablePath);
            return -1;
        }
        if (FileInfo.Size > Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Max_Size) {
            HDLR_WARF("%s() %d, call Invalid File Size", __func__, __LINE__);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(ChromaAberrationInfoByPass.WarpHorzTablePath, FileInfo.Size, Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,ChromaAberrationInfoByPass.WarpHorzTablePath, Ext_File_Param->Bypass_Ca_Horizontal_Table_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Bypass Horizontal Ca Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}
static int _TextHdlr_Load_Bypass_Ca_Vertical_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_CHROMA_ABERRATION_BYPASS_s ChromaAberrationInfoByPass;
    AmbaItuner_Get_ChromaAberrationInfoByPass(&ChromaAberrationInfoByPass);

    if (ChromaAberrationInfoByPass.VertWarpEnable) {
        POSIX_TYPE_STAT FileInfo;
        HDLR_DEBF("%s() %d, Load Bypass Vertical Ca Table", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }

        if (strlen(ChromaAberrationInfoByPass.WarpVertTablePath) < 4 || Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Address == NULL || Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Max_Size == 0) {
            HDLR_ERRF("%s() %d, Bypass Vertical Ca_Table_Path = %s, Address = %p, Max_Size = %d",
                    __func__, __LINE__, ChromaAberrationInfoByPass.WarpVertTablePath, Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Address, Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Max_Size);
            return -1;
        }

        if (POSIX_STAT(ChromaAberrationInfoByPass.WarpVertTablePath, &FileInfo) != OK) {
            HDLR_WARF("%s() %d, call POSIX_STAT(%s) Fail", __func__, __LINE__, ChromaAberrationInfoByPass.WarpVertTablePath);
            return -1;
        }
        if (FileInfo.Size > Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Max_Size) {
            HDLR_WARF("%s() %d, call Invalid File Size", __func__, __LINE__);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(ChromaAberrationInfoByPass.WarpVertTablePath, FileInfo.Size, Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,ChromaAberrationInfoByPass.WarpVertTablePath, Ext_File_Param->Bypass_Ca_Vertical_Table_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Bypass Vertical Ca Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Load_FPN_Map(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_FPN_s StaticBadPixelCorrection;
    AmbaItuner_Get_StaticBadPixelCorrection(&StaticBadPixelCorrection);
    if (StaticBadPixelCorrection.Enable) {
        HDLR_DEBF("%s() %d, Load FPN Map", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (strlen(StaticBadPixelCorrection.MapPath) < 4 || Ext_File_Param->FPN_MAP_Ext_Load_Param.Address == NULL || Ext_File_Param->FPN_MAP_Ext_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, FPN_Map_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, StaticBadPixelCorrection.MapPath, Ext_File_Param->FPN_MAP_Ext_Load_Param.Address, Ext_File_Param->FPN_MAP_Ext_Load_Param.Size);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(StaticBadPixelCorrection.MapPath, Ext_File_Param->FPN_MAP_Ext_Load_Param.Size, Ext_File_Param->FPN_MAP_Ext_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,StaticBadPixelCorrection.MapPath, Ext_File_Param->FPN_MAP_Ext_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, FPN Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Load_Bypass_FPN_Map(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_FPN_BYPASS_s StaticBadPixelCorrectionByPass;
    AmbaItuner_Get_StaticBadPixelCorrectionByPass(&StaticBadPixelCorrectionByPass);
    if (StaticBadPixelCorrectionByPass.Enable) {
        HDLR_DEBF("%s() %d, Load Bypass FPN Map", __func__, __LINE__);
        if (Ext_File_Param == NULL) {
            HDLR_ERRF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
            return -1;
        }
        if (strlen(StaticBadPixelCorrectionByPass.MapPath) < 4 || Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Address == NULL || Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Size == 0) {
            HDLR_ERRF("%s() %d, FPN_Map_Path = %s, Address = %p, Size = %d",
                    __func__, __LINE__, StaticBadPixelCorrectionByPass.MapPath, Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Address, Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Size);
            return -1;
        }
        if (_TextHdlr_load_Ext_file(StaticBadPixelCorrectionByPass.MapPath, Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Size, Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Address) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                    __func__, __LINE__,StaticBadPixelCorrectionByPass.MapPath, Ext_File_Param->Bypass_FPN_MAP_Ext_Load_Param.Address);
            return -1;
        }
    } else {
        HDLR_DEBF("%s() %d, Bypass FPN Disable, Skip Load Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Load_CC_Reg(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    AmbaItuner_Get_ColorCorrection(&ColorCorrection);
    if (strlen(ColorCorrection.RegPath) < 4 || Ext_File_Param->CC_Reg_Load_Param.Address == NULL || Ext_File_Param->CC_Reg_Load_Param.Size == 0) {
        HDLR_ERRF("%s() %d, CC Reg Path = %s, Address = %p, Size = %d",
                __func__, __LINE__, ColorCorrection.RegPath, Ext_File_Param->CC_Reg_Load_Param.Address, Ext_File_Param->CC_Reg_Load_Param.Size);
        return -1;
    }
    if (_TextHdlr_load_Ext_file(ColorCorrection.RegPath, Ext_File_Param->CC_Reg_Load_Param.Size, Ext_File_Param->CC_Reg_Load_Param.Address) < 0) {
        HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                __func__, __LINE__,ColorCorrection.RegPath, Ext_File_Param->CC_Reg_Load_Param.Address);
        return -1;
    }
    return 0;
}

static int _TextHdlr_Load_CC_ThreeD(Ituner_Ext_File_Param_s *Ext_File_Param)
{
   ITUNER_COLOR_CORRECTION_s ColorCorrection;
   AmbaItuner_Get_ColorCorrection(&ColorCorrection);
   if (strlen(ColorCorrection.ThreeDPath) < 4 || Ext_File_Param->CC_ThreeD_Load_Param.Address == NULL || Ext_File_Param->CC_ThreeD_Load_Param.Size == 0) {
       HDLR_ERRF("%s() %d, CC 3D Path = %s, Address = %p, Size = %d",
               __func__, __LINE__, ColorCorrection.ThreeDPath, Ext_File_Param->CC_ThreeD_Load_Param.Address, Ext_File_Param->CC_ThreeD_Load_Param.Size);
       return -1;
   }
   if (_TextHdlr_load_Ext_file(ColorCorrection.ThreeDPath, Ext_File_Param->CC_ThreeD_Load_Param.Size, Ext_File_Param->CC_ThreeD_Load_Param.Address) < 0) {
       HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
               __func__, __LINE__,ColorCorrection.ThreeDPath, Ext_File_Param->CC_ThreeD_Load_Param.Address);
       return -1;
   }
   return 0;
}

static int _TextHdlr_Load_HdrAlphaTable(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_VIDEO_HDR_ALPHA_CALC_ALPHA_s AlphaTable;
   AmbaItuner_Get_VideoHdrAlphaCalcAlpha(Ext_File_Param->HDR_AlphaTable_Load_Param.Index, &AlphaTable);
   if (strlen(AlphaTable.AlphaTablePath) < 4 || Ext_File_Param->HDR_AlphaTable_Load_Param.Address == NULL || Ext_File_Param->HDR_AlphaTable_Load_Param.Size == 0) {
       HDLR_ERRF("%s() %d, Hdr Alpha Path = %s, Address = %p, Size = %d",
               __func__, __LINE__, AlphaTable.AlphaTablePath, Ext_File_Param->HDR_AlphaTable_Load_Param.Address, Ext_File_Param->HDR_AlphaTable_Load_Param.Size);
       return -1;
   }
   if (_TextHdlr_load_Ext_file(AlphaTable.AlphaTablePath, Ext_File_Param->HDR_AlphaTable_Load_Param.Size, Ext_File_Param->HDR_AlphaTable_Load_Param.Address) < 0) {
       HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
               __func__, __LINE__,AlphaTable.AlphaTablePath, Ext_File_Param->HDR_AlphaTable_Load_Param.Address);
       return -1;
   }
   return 0;
}

static int _TextHdlr_Load_HdrLinearTable(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_VIDEO_HDR_AMP_LINEARIZATION_s LinearTable;
    AmbaItuner_Get_VideoHdrAmplifierLinearization(Ext_File_Param->HDR_Linear_Load_Param.Index, &LinearTable);
    if (strlen(LinearTable.Linear0LookupTablePath) < 4 || Ext_File_Param->HDR_Linear_Load_Param.Address[0] == NULL || Ext_File_Param->HDR_Linear_Load_Param.Size[0] == 0) {
        HDLR_ERRF("%s() %d, Hdr Linear Path = %s, Address = %p, Size = %d",
                  __func__,
                  __LINE__,
                  LinearTable.Linear0LookupTablePath,
                  Ext_File_Param->HDR_Linear_Load_Param.Address[0],
                  Ext_File_Param->HDR_Linear_Load_Param.Size[0]);
    } else if (_TextHdlr_load_Ext_file(LinearTable.Linear0LookupTablePath, Ext_File_Param->HDR_Linear_Load_Param.Size[0], Ext_File_Param->HDR_Linear_Load_Param.Address[0]) < 0) {
        HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail", __func__, __LINE__, LinearTable.Linear0LookupTablePath, Ext_File_Param->HDR_Linear_Load_Param.Address[0]);
    }
    if (strlen(LinearTable.Linear1LookupTablePath) < 4 || Ext_File_Param->HDR_Linear_Load_Param.Address[1] == NULL || Ext_File_Param->HDR_Linear_Load_Param.Size[1] == 0) {
        HDLR_ERRF("%s() %d, Hdr Linear Table Path = %s, Address = %p, Size = %d",
                  __func__,
                  __LINE__,
                  LinearTable.Linear1LookupTablePath,
                  Ext_File_Param->HDR_Linear_Load_Param.Address[1],
                  Ext_File_Param->HDR_Linear_Load_Param.Size[1]);
    } else if (_TextHdlr_load_Ext_file(LinearTable.Linear1LookupTablePath, Ext_File_Param->HDR_Linear_Load_Param.Size[1], Ext_File_Param->HDR_Linear_Load_Param.Address[1]) < 0) {
        HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail", __func__, __LINE__, LinearTable.Linear1LookupTablePath, Ext_File_Param->HDR_Linear_Load_Param.Address[1]);
    }
    return 0;
}


static int _TextHdlr_Load_FIRST_SHARPEN_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
   ITUNER_SHARPEN_BOTH_s FirstSharpenBoth;
   AMBA_DSP_IMG_SHARPEN_NOISE_s FirstSharpenNoise;
   AMBA_DSP_IMG_LEVEL_s    MinCoringResult;
   AMBA_DSP_IMG_LEVEL_s    CoringIndexScale;
   AMBA_DSP_IMG_LEVEL_s    ScaleCoring;
       
   AmbaItuner_Get_SharpenBoth(&FirstSharpenBoth);
   AmbaItuner_Get_SharpenNoise(&FirstSharpenNoise);
   AmbaItuner_Get_MinCoringResult(&MinCoringResult);
   AmbaItuner_Get_CoringIndexScale(&CoringIndexScale);
   AmbaItuner_Get_ScaleCoring(&ScaleCoring);
   
   if((FirstSharpenNoise.LevelStrAdjust.Method == 1)||\
         (MinCoringResult.Method == 1)||\
         (CoringIndexScale.Method == 1)||\
         (ScaleCoring.Method == 1)){
   if (strlen(FirstSharpenBoth.ThreeDTablePath) < 4 || Ext_File_Param->FIRST_SHARPEN_Load_Param.Address == NULL || Ext_File_Param->FIRST_SHARPEN_Load_Param.Size == 0) {
       HDLR_ERRF("%s() %d, FirstSharpenBoth.ThreeDTablePath Path = %s, Address = %p, Size = %d",
               __func__, __LINE__, FirstSharpenBoth.ThreeDTablePath, Ext_File_Param->FIRST_SHARPEN_Load_Param.Address, Ext_File_Param->FIRST_SHARPEN_Load_Param.Size);
       return -1;
   }
   if (_TextHdlr_load_Ext_file(FirstSharpenBoth.ThreeDTablePath, Ext_File_Param->FIRST_SHARPEN_Load_Param.Size, Ext_File_Param->FIRST_SHARPEN_Load_Param.Address) < 0) {
       HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
               __func__, __LINE__,FirstSharpenBoth.ThreeDTablePath, Ext_File_Param->FIRST_SHARPEN_Load_Param.Address);
       return -1;
   }
   }
#endif
   return 0;
}

static int _TextHdlr_Load_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
   ITUNER_ASF_INFO_s ASF;
   AmbaItuner_Get_AsfInfo(&ASF);
   if((ASF.AsfInfo.LevelStrAdjust.Method == 1)|| (ASF.AsfInfo.T0T1Div.Method == 1)){
   if (strlen(ASF.ThreeDTablePath) < 4 || Ext_File_Param->ASF_Load_Param.Address == NULL || Ext_File_Param->ASF_Load_Param.Size == 0) {
       HDLR_ERRF("%s() %d, ASF.ThreeDTablePath Path = %s, Address = %p, Size = %d",
               __func__, __LINE__, ASF.ThreeDTablePath, Ext_File_Param->ASF_Load_Param.Address, Ext_File_Param->ASF_Load_Param.Size);
       return -1;
   }
   if (_TextHdlr_load_Ext_file(ASF.ThreeDTablePath, Ext_File_Param->ASF_Load_Param.Size, Ext_File_Param->ASF_Load_Param.Address) < 0) {
       HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
               __func__, __LINE__,ASF.ThreeDTablePath, Ext_File_Param->ASF_Load_Param.Address);
       return -1;
   }
   }
#endif
   return 0;
}

static int _TextHdlr_Load_HISO_LOW_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
   ITUNER_ASF_INFO_s HisoLowASF;
   AmbaItuner_Get_HisoLowAsf(&HisoLowASF);
   if((HisoLowASF.AsfInfo.LevelStrAdjust.Method == 1)||(HisoLowASF.AsfInfo.T0T1Div.Method == 1)){
   if (strlen(HisoLowASF.ThreeDTablePath) < 4 || Ext_File_Param->HISO_LOW_ASF_Load_Param.Address == NULL || Ext_File_Param->HISO_LOW_ASF_Load_Param.Size == 0) {
       HDLR_ERRF("%s() %d, HisoLowASF.ThreeDTablePath Path = %s, Address = %p, Size = %d",
               __func__, __LINE__, HisoLowASF.ThreeDTablePath, Ext_File_Param->HISO_LOW_ASF_Load_Param.Address, Ext_File_Param->HISO_LOW_ASF_Load_Param.Size);
       return -1;
   }
   if (_TextHdlr_load_Ext_file(HisoLowASF.ThreeDTablePath, Ext_File_Param->HISO_LOW_ASF_Load_Param.Size, Ext_File_Param->HISO_LOW_ASF_Load_Param.Address) < 0) {
       HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
               __func__, __LINE__,HisoLowASF.ThreeDTablePath, Ext_File_Param->HISO_LOW_ASF_Load_Param.Address);
       return -1;
   }
   }
#endif
   return 0;
}

static int _TextHdlr_Load_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
    ITUNER_HISO_CHROMA_FILTER_COMBINE_s HisoChromaFilterVeryLowCombine;
    AmbaItuner_Get_HisoChromaFilterVeryLowCombine(&HisoChromaFilterVeryLowCombine);
    if((HisoChromaFilterVeryLowCombine.CombineInfo.EitherMaxChangeOrT0T1AddLevelCb.Method ==1)||
        (HisoChromaFilterVeryLowCombine.CombineInfo.EitherMaxChangeOrT0T1AddLevelCr.Method == 1)){
    if (strlen(HisoChromaFilterVeryLowCombine.ThreeDTablePath) < 4 || Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Address == NULL || Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Size == 0) {
        HDLR_ERRF("%s() %d, HisoChromaFilterVeryLowCombine.ThreeDTablePath Path = %s, Address = %p, Size = %d",
                __func__, __LINE__, HisoChromaFilterVeryLowCombine.ThreeDTablePath, Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Address, Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Size);
        return -1;
    }
    if (_TextHdlr_load_Ext_file(HisoChromaFilterVeryLowCombine.ThreeDTablePath, Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Size, Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Address) < 0) {
        HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                __func__, __LINE__,HisoChromaFilterVeryLowCombine.ThreeDTablePath, Ext_File_Param->HISO_CHROMA_VERYLOW_COMBINE_Load_Param.Address);
        return -1;
    }
    }
#endif
    return 0;
}

static int _TextHdlr_Load_LOW_ASF_COMBINE_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
    ITUNER_HISO_LUMA_FILTER_COMBINE_s HisoLowASFCombine;
    AmbaItuner_Get_HisoLowASFCombine(&HisoLowASFCombine);
    if(HisoLowASFCombine.CombineInfo.EitherMaxChangeOrT0T1AddLevel.Method == 1){
    if (strlen(HisoLowASFCombine.ThreeDTablePath) < 4 || Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Address == NULL || Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Size == 0) {
        HDLR_ERRF("%s() %d, HisoLowASFCombine.ThreeDTablePath Path = %s, Address = %p, Size = %d",
                __func__, __LINE__, HisoLowASFCombine.ThreeDTablePath, Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Address, Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Size);
        return -1;
    }
    if (_TextHdlr_load_Ext_file(HisoLowASFCombine.ThreeDTablePath, Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Size, Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Address) < 0) {
        HDLR_WARF("%s() %d, call _TextHdlr_load_ext_file(%s, Addr : %x) Fail",
                __func__, __LINE__,HisoLowASFCombine.ThreeDTablePath, Ext_File_Param->HISO_LOW_ASF_COMBINE_Load_Param.Address);
        return -1;
    }
    }
#endif
    return 0;
}

static int _TextHdlr_Save_FPN_Map(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char FilePath[MAX_NAME_LENS];
    ITUNER_FPN_s StaticBadPixelCorrection;
    AmbaItuner_Get_StaticBadPixelCorrection(&StaticBadPixelCorrection);
    if (StaticBadPixelCorrection.Enable) {
        AMBA_DSP_IMG_SBP_CORRECTION_s SbpCorr;
        AmbaItuner_Get_SbpCorr(&SbpCorr);
        if (SbpCorr.CalibSbpInfo.SbpBuffer != NULL) {
            UINT32 FPN_Map_Size = ITUNER_CAL_FPN_MAP_SIZE(StaticBadPixelCorrection.CailbVinWidth, StaticBadPixelCorrection.CailbVinHeight);
            HDLR_DEBF("%s() %d, Dump FPN Map", __func__, __LINE__);
            _TextHdlr_Get_Ext_File_Path(EXT_FILE_FPN_MAP, FilePath);
            strncpy(StaticBadPixelCorrection.MapPath, FilePath, sizeof(StaticBadPixelCorrection.MapPath));
            AmbaItuner_Set_StaticBadPixelCorrection(&StaticBadPixelCorrection);
            HDLR_DEBF("%s() %d, Dump FPN Map Address %s, size %d, to %s", __func__, __LINE__, SbpCorr.CalibSbpInfo.SbpBuffer,  FPN_Map_Size, StaticBadPixelCorrection.MapPath);
            if (_TextHdlr_Save_Ext_File(StaticBadPixelCorrection.MapPath, FPN_Map_Size, SbpCorr.CalibSbpInfo.SbpBuffer) < 0) {
                HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, StaticBadPixelCorrection.MapPath, SbpCorr.CalibSbpInfo.SbpBuffer);
                return -1;
            }
        } else {
            HDLR_WARF("%s() %d, EXT_FILE_FPN_MAP, FPN_Map_Addr = NULL", __func__, __LINE__);
        }
    } else {
        HDLR_DEBF("%s() %d, FPN Disable, Skip Dump Process", __func__, __LINE__);
    }
    return 0;
}
static int _TextHdlr_Save_Vignette(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char FilePath[MAX_NAME_LENS];
    ITUNER_VIGNETTE_s VignetteCompensation;
    AmbaItuner_Get_VignetteCompensation(&VignetteCompensation);
    if (VignetteCompensation.Enable) {
        AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s VignetteCalcInfo;
        AmbaItuner_Get_VignetteCalcInfo(&VignetteCalcInfo);
        if (VignetteCalcInfo.CalibVignetteInfo.pVignetteRedGain != NULL && VignetteCalcInfo.CalibVignetteInfo.pVignetteGreenEvenGain != NULL &&
            VignetteCalcInfo.CalibVignetteInfo.pVignetteGreenOddGain != NULL && VignetteCalcInfo.CalibVignetteInfo.pVignetteBlueGain != NULL) {
            _TextHdlr_Get_Ext_File_Path(EXT_FILE_VIGNETTE_GAIN, FilePath);
            strncpy(VignetteCompensation.CalibTablePath, FilePath, sizeof(VignetteCompensation.CalibTablePath));
            AmbaItuner_Set_VignetteCompensation(&VignetteCompensation);
            HDLR_DEBF("%s() %d, Dump Vignette Gain Address %s, size %d * 4, to %s",
                    __func__, __LINE__, VignetteCalcInfo.CalibVignetteInfo.pVignetteRedGain,  ITUNER_VIGNETTE_GAIN_MAP_SIZE, VignetteCompensation.CalibTablePath);
            {
                POSIX_FILE *Fid;
                char UniFname[MAX_NAME_LENS];
                int i;
                UINT16* SrcAddr[] = {
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteRedGain,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteGreenEvenGain,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteGreenOddGain,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteBlueGain
                };
                if (0 != _TextHdlr_Change_System_Drive(VignetteCompensation.CalibTablePath)) {
                    HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, VignetteCompensation.CalibTablePath);
                    return -1;
                }
                asc_to_uni(VignetteCompensation.CalibTablePath, UniFname);
                Fid = POSIX_FOPEN(UniFname, "w");
                if (Fid == 0) {
                    AmbaPrint("[Error] Ext file open fails!");
                    return -1;
                }
                for (i = 0; i < 4; i++) {
                    if (POSIX_FWRITE(SrcAddr[i], ITUNER_VIGNETTE_GAIN_MAP_SIZE, 1, Fid) <= 0) {
                        HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail",
                            __func__, __LINE__, SrcAddr[i]);
                        POSIX_FCLOSE(Fid);
                        return -1;
                    }
                }
                POSIX_FCLOSE(Fid);
            }
            /*
            if (_TextHdlr_Save_Ext_File(VignetteCompensation.CalibTablePath, ITUNER_VIGNETTE_GAIN_MAP_SIZE, VignetteCalcInfo.CalibVignetteInfo.pVignetteRedGain) < 0) {
                HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail",
                        __func__, __LINE__, VignetteCompensation.CalibTablePath, VignetteCalcInfo.CalibVignetteInfo.pVignetteRedGain);
                return -1;
            }
            */
        } else {
            HDLR_WARF("%s() %d, EXT_FILE_VIGNETTE_GAIN, VignetteGain Address: %x %x %x %x",
                    __func__, __LINE__,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteRedGain,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteGreenEvenGain,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteGreenOddGain,
                    VignetteCalcInfo.CalibVignetteInfo.pVignetteBlueGain);
        }
    } else {
        HDLR_DEBF("%s() %d, Vignette Disable, Skip Dump Process", __func__, __LINE__);
    }
    return 0;
}
static int _TextHdlr_Save_Warp_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char FilePath[MAX_NAME_LENS];
    ITUNER_WARP_s WarpCompensation;
    AmbaItuner_Get_WarpCompensation(&WarpCompensation);
    if (WarpCompensation.Enable) {
        AMBA_DSP_IMG_WARP_CALC_INFO_s WarpCalcInfo;
        AmbaItuner_Get_WarpCalcInfo(&WarpCalcInfo);
        if (WarpCalcInfo.CalibWarpInfo.pWarp != NULL) {
            UINT32 Warp_Table_Size = ITUNER_CAL_WARP_TABLE_SIZE(WarpCompensation.HorizontalGridNumber, WarpCompensation.VerticalGridNumber);
            _TextHdlr_Get_Ext_File_Path(EXT_FILE_WARP_TABLE, FilePath);
            strncpy(WarpCompensation.WarpGridTablePath, FilePath, sizeof(WarpCompensation.WarpGridTablePath));
            AmbaItuner_Set_WarpCompensation(&WarpCompensation);
            HDLR_DEBF("%s() %d, Dump Warp Table Address %x, size %d, to %s",
                    __func__, __LINE__, WarpCalcInfo.CalibWarpInfo.pWarp,  Warp_Table_Size, WarpCompensation.WarpGridTablePath);
            if (_TextHdlr_Save_Ext_File(WarpCompensation.WarpGridTablePath, Warp_Table_Size, WarpCalcInfo.CalibWarpInfo.pWarp) < 0) {
                HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail",
                        __func__, __LINE__, WarpCompensation.WarpGridTablePath, WarpCalcInfo.CalibWarpInfo.pWarp);
                return -1;
            }
        } else {
            HDLR_WARF("%s() %d, EXT_FILE_WARP_TABLE, WarpTable Address: NULL", __func__, __LINE__);
        }
    } else {
        HDLR_DEBF("%s() %d, Warp Disable, Skip Dump Process", __func__, __LINE__);
    }
    return 0;
}
static int _TextHdlr_Save_Ca_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char FilePath[MAX_NAME_LENS];
    ITUNER_CHROMA_ABERRATION_s ChromaAberrationInfo;
    AmbaItuner_Get_ChromaAberrationInfo(&ChromaAberrationInfo);
    if (ChromaAberrationInfo.Enable) {
        AMBA_DSP_IMG_CAWARP_CALC_INFO_s CACalcInfo;
        AmbaItuner_Get_CACalcInfo(&CACalcInfo);
        if (CACalcInfo.CalibCaWarpInfo.pCaWarp != NULL) {
            UINT32 CA_Table_Size = ITUNER_CAL_CA_TABLE_SIZE(ChromaAberrationInfo.HorizontalGridNumber, ChromaAberrationInfo.VerticalGridNumber);
            _TextHdlr_Get_Ext_File_Path(EXT_FILE_CA_TABLE, FilePath);
            strncpy(ChromaAberrationInfo.CaGridTablePath, FilePath, sizeof(ChromaAberrationInfo.CaGridTablePath));
            AmbaItuner_Set_ChromaAberrationInfo(&ChromaAberrationInfo);
            HDLR_DEBF("%s() %d, Dump Ca Table Address %x, size %d, to %s",
                    __func__, __LINE__, CACalcInfo.CalibCaWarpInfo.pCaWarp,  CA_Table_Size, ChromaAberrationInfo.CaGridTablePath);
            if (_TextHdlr_Save_Ext_File(ChromaAberrationInfo.CaGridTablePath, CA_Table_Size, CACalcInfo.CalibCaWarpInfo.pCaWarp) < 0) {
                HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail",
                        __func__, __LINE__, ChromaAberrationInfo.CaGridTablePath, CACalcInfo.CalibCaWarpInfo.pCaWarp);
                return -1;
            }
        } else {
            HDLR_WARF("%s() %d, EXT_FILE_CA_TABLE, Ca_Table Address: NULL", __func__, __LINE__);
        }
    } else {
        HDLR_DEBF("%s() %d, Ca Disable, Skip Dump Process", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Save_CC_Reg(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char FilePath[MAX_NAME_LENS];
    AMBA_DSP_IMG_COLOR_CORRECTION_REG_s CcReg;
    AmbaItuner_Get_CcReg(&CcReg);
    if ((void*)CcReg.RegSettingAddr != NULL) {
        ITUNER_COLOR_CORRECTION_s ColorCorrection;
        HDLR_DEBF("%s() %d, Dump CC Reg", __func__, __LINE__);
        // Note: Update ColorCorrection Path
        _TextHdlr_Get_Ext_File_Path(EXT_FILE_CC_REG, FilePath);
        AmbaItuner_Get_ColorCorrection(&ColorCorrection);
        strncpy(ColorCorrection.RegPath, FilePath, sizeof(ColorCorrection.RegPath));
        AmbaItuner_Set_ColorCorrection(&ColorCorrection);
        HDLR_DEBF("%s() %d, Dump CC Reg Address %x to %s", __func__, __LINE__,  CcReg.RegSettingAddr, ColorCorrection.RegPath);
        if (_TextHdlr_Save_Ext_File(ColorCorrection.RegPath, AMBA_DSP_IMG_CC_REG_SIZE, (void*)CcReg.RegSettingAddr) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, ColorCorrection.RegPath, CcReg.RegSettingAddr);
            return -1;
        }

    } else {
        HDLR_WARF("%s() %d, EXT_FILE_CC_REG, RegSettingAddr = NULL", __func__, __LINE__);
    }
    return 0;
}
static int _TextHdlr_Save_CC_ThreeD(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    char FilePath[MAX_NAME_LENS];
    AMBA_DSP_IMG_COLOR_CORRECTION_s CcThreed;
    AmbaItuner_Get_CcThreeD(&CcThreed);
    if ((void*)CcThreed.MatrixThreeDTableAddr != NULL) {
        ITUNER_COLOR_CORRECTION_s ColorCorrection;
        _TextHdlr_Get_Ext_File_Path(EXT_FILE_CC_THREED, FilePath);
        AmbaItuner_Get_ColorCorrection(&ColorCorrection);
        strncpy(ColorCorrection.ThreeDPath, FilePath, sizeof(ColorCorrection.ThreeDPath));
        AmbaItuner_Set_ColorCorrection(&ColorCorrection);
        HDLR_DEBF("%s() %d, Dump CC ThreeD Address %x to %s", __func__, __LINE__,  CcThreed.MatrixThreeDTableAddr, ColorCorrection.ThreeDPath);
        if (_TextHdlr_Save_Ext_File(ColorCorrection.ThreeDPath, AMBA_DSP_IMG_CC_3D_SIZE, (void*)CcThreed.MatrixThreeDTableAddr) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, ColorCorrection.ThreeDPath, CcThreed.MatrixThreeDTableAddr);
            return -1;
        }
    } else {
        HDLR_WARF("%s() %d, EXT_FILE_CC_THREED, RegSettingAddr = NULL", __func__, __LINE__);
    }
    return 0;
}

static int _TextHdlr_Save_FIRST_SHARPEN_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
    char FilePath[MAX_NAME_LENS];
    ITUNER_SHARPEN_BOTH_s FirstSharpenBoth;
    UINT8 ShpAOrSpatialFilterSelect;
    AmbaItuner_Get_SharpenBoth(&FirstSharpenBoth);
    AmbaItuner_Get_ShpAOrSpatialFilterSelect(&ShpAOrSpatialFilterSelect);

    if(ShpAOrSpatialFilterSelect != 0){
    if (FirstSharpenBoth.BothInfo.ThreeD.pTable != NULL) {
        _TextHdlr_Get_Ext_File_Path(EXT_FILE_FIRST_SHARPEN, FilePath);
        strncpy(FirstSharpenBoth.ThreeDTablePath, FilePath, sizeof(FirstSharpenBoth.ThreeDTablePath));
        AmbaItuner_Set_SharpenBoth(&FirstSharpenBoth);
        HDLR_DEBF("%s() %d, Dump FirstSharpenBoth ThreeD Table Address %x to %s", __func__, __LINE__,  FirstSharpenBoth.BothInfo.ThreeD.pTable, FirstSharpenBoth.ThreeDTablePath);
        if (_TextHdlr_Save_Ext_File(FirstSharpenBoth.ThreeDTablePath, 4096, (void*)FirstSharpenBoth.BothInfo.ThreeD.pTable) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, FirstSharpenBoth.ThreeDTablePath, FirstSharpenBoth.BothInfo.ThreeD.pTable);
            return -1;
        }
        } else {
            HDLR_WARF("%s() %d, EXT_FILE_FIRST_SHARPEN, RegSettingAddr = NULL", __func__, __LINE__);
        }
    }else{
        HDLR_DEBF("%s() %d, First Sharpen not be selected, Skip Dump Process", __func__, __LINE__);
    }
#endif
    return 0;
}

static int _TextHdlr_Save_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
    char FilePath[MAX_NAME_LENS];
    ITUNER_ASF_INFO_s ASF;
    UINT8 ShpAOrSpatialFilterSelect;
    
    AmbaItuner_Get_AsfInfo(&ASF);
    AmbaItuner_Get_ShpAOrSpatialFilterSelect(&ShpAOrSpatialFilterSelect);

    if (ASF.AsfInfo.Adapt.ThreeD.pTable != NULL) {
        _TextHdlr_Get_Ext_File_Path(EXT_FILE_ASF, FilePath);
        strncpy(ASF.ThreeDTablePath, FilePath, sizeof(ASF.ThreeDTablePath));
        AmbaItuner_Set_AsfInfo(&ASF);
        HDLR_DEBF("%s() %d, Dump ASF ThreeD Table Address %x to %s", __func__, __LINE__, ASF.AsfInfo.Adapt.ThreeD.pTable, ASF.ThreeDTablePath);
        if (_TextHdlr_Save_Ext_File(ASF.ThreeDTablePath, 4096, (void*)ASF.AsfInfo.Adapt.ThreeD.pTable) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, ASF.ThreeDTablePath, ASF.AsfInfo.Adapt.ThreeD.pTable);
            return -1;
        }
        } else {
            HDLR_WARF("%s() %d, EXT_FILE_ASF, RegSettingAddr = NULL", __func__, __LINE__);
        }
#endif
    return 0;
}

static int _TextHdlr_Save_HISO_LOW_ASF_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
    char FilePath[MAX_NAME_LENS];
    ITUNER_ASF_INFO_s HisoLowASF;
    AmbaItuner_Get_HisoLowAsf(&HisoLowASF);
    if (HisoLowASF.AsfInfo.Adapt.ThreeD.pTable != NULL) {
        _TextHdlr_Get_Ext_File_Path(EXT_FILE_HISO_LOW_ASF, FilePath);
        strncpy(HisoLowASF.ThreeDTablePath, FilePath, sizeof(HisoLowASF.ThreeDTablePath));
        AmbaItuner_Set_HisoLowAsf(&HisoLowASF);
        HDLR_DEBF("%s() %d, Dump HisoLowASF ThreeD Table Address %x to %s", __func__, __LINE__, HisoLowASF.AsfInfo.Adapt.ThreeD.pTable, HisoLowASF.ThreeDTablePath);
        if (_TextHdlr_Save_Ext_File(HisoLowASF.ThreeDTablePath, 4096, (void*)HisoLowASF.AsfInfo.Adapt.ThreeD.pTable) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, HisoLowASF.ThreeDTablePath, HisoLowASF.AsfInfo.Adapt.ThreeD.pTable);
            return -1;
        }
    } else {
        HDLR_WARF("%s() %d, EXT_FILE_HISO_LOW_ASF, RegSettingAddr = NULL", __func__, __LINE__);
    }
#endif
    return 0;
}

static int _TextHdlr_Save_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table(Ituner_Ext_File_Param_s *Ext_File_Param)
{
#if 0
    char FilePath[MAX_NAME_LENS];
    ITUNER_HISO_CHROMA_FILTER_COMBINE_s HisoChromaFilterVeryLowCombine;
    AmbaItuner_Get_HisoChromaFilterVeryLowCombine(&HisoChromaFilterVeryLowCombine);
    if (HisoChromaFilterVeryLowCombine.CombineInfo.ThreeD.pTable != NULL) {
        _TextHdlr_Get_Ext_File_Path(EXT_FILE_HISO_CHROMA_VERYLOW_COMBINE, FilePath);
        strncpy(HisoChromaFilterVeryLowCombine.ThreeDTablePath, FilePath, sizeof(HisoChromaFilterVeryLowCombine.ThreeDTablePath));
        AmbaItuner_Set_HisoChromaFilterVeryLowCombine(&HisoChromaFilterVeryLowCombine);
        HDLR_DEBF("%s() %d, Dump HisoChromaFilterVeryLowCombine ThreeD Table Address %x to %s", __func__, __LINE__,  HisoChromaFilterVeryLowCombine.CombineInfo.ThreeD.pTable, HisoChromaFilterVeryLowCombine.ThreeDTablePath);
        if (_TextHdlr_Save_Ext_File(HisoChromaFilterVeryLowCombine.ThreeDTablePath, 4096, (void*)HisoChromaFilterVeryLowCombine.CombineInfo.ThreeD.pTable) < 0) {
            HDLR_WARF("%s() %d, call _TextHdlr_Save_Ext_File(%s, Addr : %x) Fail", __func__, __LINE__, HisoChromaFilterVeryLowCombine.ThreeDTablePath, HisoChromaFilterVeryLowCombine.CombineInfo.ThreeD.pTable);
            return -1;
        }
    } else {
        HDLR_WARF("%s() %d, EXT_FILE_HISO_CHROMA_VERYLOW_COMBINE, RegSettingAddr = NULL", __func__, __LINE__);
    }
#endif
    return 0;
}

static int _TextHdlr_Save_Jpg(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_SYSTEM_s System;
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    POSIX_FILE *Fd;
    UINT8 Output_Filename_Len;
    HDLR_DEBF("%s() %d, Input FPath: %s", __func__, __LINE__, Ext_File_Param->JPG_Save_Param.Target_File_Path);
    if (Ext_File_Param->JPG_Save_Param.Address == NULL || Ext_File_Param->JPG_Save_Param.Size == 0) {
        HDLR_WARF("%s() %d, Y Addr = %p, Size = %d",
            __func__, __LINE__, Ext_File_Param->JPG_Save_Param.Address, Ext_File_Param->JPG_Save_Param.Size);
        return -1;
    }
    AmbaItuner_Get_SystemInfo(&System);
    memset(FilePath, 0x0, sizeof(FilePath));
    strncpy(FilePath, Ext_File_Param->JPG_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FilePath);
        return -1;
    }
    FileName_Pos = strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        HDLR_WARF("%s() %d, FilePath : %s Invalid FileName_Pos: %s", __func__, __LINE__, FilePath, FileName_Pos);
        return -1;
    }
    Output_Filename_Len = strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + strlen(System.OutputFilename);
    } else {
        Ext_Name_Pos = strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            HDLR_WARF("%s() %d, Invalid FilePath : %s", __func__, __LINE__, FilePath);
            return -1;
        }
    }
    sprintf(Ext_Name_Pos, ".jpg");
    Fd = POSIX_FOPEN(FilePath, "w");
    if (Fd == NULL) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__);
        return -1;
    }
    POSIX_FWRITE(Ext_File_Param->JPG_Save_Param.Address, Ext_File_Param->JPG_Save_Param.Size, 1, Fd);
    POSIX_FCLOSE(Fd);
    return 0;
}

static int _TextHdlr_Save_Y(Save_Data_to_Path_Info_s *Y_Info)
{
    ITUNER_SYSTEM_s System;
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    int i;
    POSIX_FILE *Fd;
    UINT8 Output_Filename_Len;

    if ((Y_Info->Address == NULL) || Y_Info->Width == 0 || Y_Info->Height == 0 || Y_Info->Pitch == 0) {
        HDLR_WARF("%s() %d, Y Addr = %p, Width = %d, Height = %d, Pitch = %d",
                  __func__, __LINE__, Y_Info->Address, Y_Info->Width, Y_Info->Height, Y_Info->Pitch);
        return -1;
    }
    AmbaItuner_Get_SystemInfo(&System);
    memset(FilePath, 0x0, sizeof(FilePath));
    strncpy(FilePath, Y_Info->Target_File_Path, sizeof(FilePath));
    if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FilePath);
        return -1;
    }
    FileName_Pos = strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        HDLR_WARF("%s() %d, FilePath : %s Invalid FileName_Pos: %s", __func__, __LINE__, FilePath, FileName_Pos);
        return -1;
    }
    Output_Filename_Len = strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + strlen(System.OutputFilename);
    } else {
        Ext_Name_Pos = strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            HDLR_WARF("%s() %d, Invalid FilePath : %s", __func__, __LINE__, FilePath);
            return -1;
        }

    }
    sprintf(Ext_Name_Pos, ".y");
    AmbaCache_Invalidate(Y_Info->Address, Y_Info->Pitch * Y_Info->Height);
    HDLR_DEBF("%s() %d, Save File: %s Start", __func__, __LINE__, FilePath);
    Fd = POSIX_FOPEN(FilePath, "w");
    if (Fd == NULL) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__);
        return -1;
    }
    for (i = 0; i < Y_Info->Height; i++) {
        POSIX_FWRITE(&((UINT8*)Y_Info->Address)[Y_Info->StartX + (i + Y_Info->StartY) * Y_Info->Pitch], Y_Info->Width, 1, Fd);
    }
    POSIX_FCLOSE(Fd);
    HDLR_DEBF("%s() %d, Save File: %s Done", __func__, __LINE__, FilePath);
    return 0;
}

static int _TextHdlr_Save_UV(Save_Data_to_Path_Info_s *UV_Info)
{
    ITUNER_SYSTEM_s System;
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    int i;
    POSIX_FILE *Fd;
    UINT8 Output_Filename_Len;
    if (UV_Info->Address == NULL || UV_Info->Width == 0 || UV_Info->Height == 0 || UV_Info->Pitch == 0) {
        HDLR_WARF("%s() %d, UV Addr = %p, Width = %d, Height = %d, Pitch = %d",
                  __func__, __LINE__, UV_Info->Address, UV_Info->Width, UV_Info->Height, UV_Info->Pitch);
        return -1;
    }
    AmbaItuner_Get_SystemInfo(&System);
    memset(FilePath, 0x0, sizeof(FilePath));
    strncpy(FilePath, UV_Info->Target_File_Path, sizeof(FilePath));
    if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s)", __func__, __LINE__, FilePath);
        return -1;
    }

    FileName_Pos = strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        HDLR_WARF("%s() %d, Invalid FileName_Pos: %s", __func__, __LINE__, FileName_Pos);
        return -1;
    }
    Output_Filename_Len = strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + strlen(System.OutputFilename);
    } else {
        Ext_Name_Pos = strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            HDLR_WARF("%s() %d, Invalid FilePath : %s", __func__, __LINE__, FilePath);
            return -1;
        }

    }
    sprintf(Ext_Name_Pos, ".uv");
    AmbaCache_Invalidate(UV_Info->Address, UV_Info->Pitch * UV_Info->Height);
    HDLR_DEBF("%s() %d, Save File: %s Start", __func__, __LINE__, FilePath);
    Fd = POSIX_FOPEN(FilePath, "w");
    if (Fd == NULL) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__);
        return -1;
    }
    for (i = 0; i < UV_Info->Height; i++) {
        POSIX_FWRITE(&((UINT8*)UV_Info->Address)[UV_Info->StartX + (i + UV_Info->StartY) * UV_Info->Pitch], UV_Info->Width, 1, Fd);
    }
    POSIX_FCLOSE(Fd);
    HDLR_DEBF("%s() %d, Save File: %s Done", __func__, __LINE__, FilePath);
    return 0;
}
static int _TextHdlr_Save_CFA_STAT(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_SYSTEM_s System;
    AMBA_DSP_EVENT_CFA_3A_DATA_s *Cfa_Stat;
    AMBA_DSP_3A_HEADER_s *headInfo;
    AMBA_DSP_CFA_AE_s *silceAeStat = NULL;
    AMBA_DSP_CFA_AWB_s *silceAwbStat = NULL;
    UINT8 w = 0, h = 0;
    UINT8 Output_Filename_Len;
    int i, j, idx;
    char lineBuf[640];
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    POSIX_FILE *Fd;
    if (Ext_File_Param == NULL) {
        HDLR_WARF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
        return -1;
    }
    if ( Ext_File_Param->Stat_Save_Param.Address == NULL || Ext_File_Param->Stat_Save_Param.Size < sizeof(AMBA_DSP_EVENT_CFA_3A_DATA_s)) {
        HDLR_WARF("%s() %d, Ext_File_Param->Stat_Save_Param.Address = %x, Size = %d(Need size :%d)",
                  __func__, __LINE__, Ext_File_Param->Stat_Save_Param.Address, Ext_File_Param->Stat_Save_Param.Size);
        return -1;
    }

    AmbaItuner_Get_SystemInfo(&System);
    memset(FilePath, 0x0, sizeof(FilePath));
    strncpy(FilePath, Ext_File_Param->Stat_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s)", __func__, __LINE__, FilePath);
        return -1;
    }
    FileName_Pos = strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        HDLR_WARF("%s() %d, Invalid FileName_Pos: %s", __func__, __LINE__, FileName_Pos);
        return -1;
    }
    Output_Filename_Len = strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + strlen(System.OutputFilename);
        sprintf(Ext_Name_Pos, "_CFA_STAT.txt");
    } else {
        Ext_Name_Pos = strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            HDLR_WARF("%s() %d, Invalid FilePath : %s", __func__, __LINE__, FilePath);
            return -1;
        }

    }


    Cfa_Stat = (AMBA_DSP_EVENT_CFA_3A_DATA_s*)Ext_File_Param->Stat_Save_Param.Address;
    HDLR_DEBF("%s() %d, FilePath = %s", __func__, __LINE__, FilePath);



    headInfo = &Cfa_Stat->Header;
    w = headInfo->AeTileNumCol;
    h = headInfo->AeTileNumRow;
    Fd = POSIX_FOPEN(FilePath, "w");
    if (Fd < 0) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, FilePath);
        return -1;
    }

    sprintf(lineBuf, "CFA AE statistics %dx%d\n", w, h);
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    silceAeStat = &Cfa_Stat->Ae[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * AMBA_DSP_3A_AE_TILE_COLUMN_COUNT + i;
            sprintf(num, "%5d ", silceAeStat[idx].LinY);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, 1, strlen(lineBuf), Fd);
    }

    w = headInfo->AwbTileNumCol;
    h = headInfo->AwbTileNumRow;
    silceAwbStat = &Cfa_Stat->Awb[0];
    sprintf(lineBuf, "CFA AWB statistics\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    sprintf(lineBuf, "---  Red statistics   ---\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            UINT16 temp;
            idx = j * AMBA_DSP_3A_AWB_TILE_COLUMN_COUNT + i;
            temp = silceAwbStat[idx].SumR;
            sprintf(num, "%5d ", temp);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    }
    sprintf(lineBuf, "---  Green statistics   ---\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            UINT16 temp;
            idx = j * AMBA_DSP_3A_AWB_TILE_COLUMN_COUNT + i;
            temp = silceAwbStat[idx].SumG;
            sprintf(num, "%5d ", temp);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    }
    sprintf(lineBuf, "---  Blue statistics   ---\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    for (j=0; j<h; j++) {
        lineBuf[0]='\0';
        for (i=0; i<w; i++) {
            char num[10]="\0";
            UINT16 temp;
            idx = j*AMBA_DSP_3A_AWB_TILE_COLUMN_COUNT + i;
            temp = silceAwbStat[idx].SumB;
            sprintf(num,"%5d ", temp);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    }

    sprintf(lineBuf, "CFA AWB tile act_width: %d\n" , headInfo->AwbTileActiveWidth);
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    sprintf(lineBuf, "CFA AWB tile act_height: %d\n", headInfo->AwbTileActiveHeight);
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    sprintf(lineBuf, "CFA AWB RGB shift: %d\n" , headInfo->AwbRgbShift);
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    POSIX_FSYNC(Fd);
    POSIX_FCLOSE(Fd);
    HDLR_DEBF("%s() %d, Dump Cfa Stat to %s Done", __func__, __LINE__, FilePath);
    return 0;
}
static int _TextHdlr_Save_RGB_STAT(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_SYSTEM_s System;
    AMBA_DSP_EVENT_RGB_3A_DATA_s *rgbStat;
    AMBA_DSP_3A_HEADER_s *headInfo;
    AMBA_DSP_RGB_AE_s *silceAeStat = NULL;
    AMBA_DSP_RGB_AF_s *silceAfStat = NULL;
    UINT8 w = 0, h = 0;
    UINT8 Output_Filename_Len;
    int i, j, idx;
    char lineBuf[640];
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    POSIX_FILE *Fd;
    if (Ext_File_Param == NULL) {
        HDLR_WARF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
        return -1;
    }
    if (Ext_File_Param->Stat_Save_Param.Address == NULL || Ext_File_Param->Stat_Save_Param.Size < sizeof(AMBA_DSP_EVENT_RGB_3A_DATA_s)) {
        HDLR_WARF("%s() %d, Ext_File_Param->Stat_Save_Param.Address = %x, Size = %d(Need size :%d)", __func__, __LINE__, Ext_File_Param->Stat_Save_Param.Address, Ext_File_Param->Stat_Save_Param.Size);
        return -1;
    }

    AmbaItuner_Get_SystemInfo(&System);
    memset(FilePath, 0x0, sizeof(FilePath));
    strncpy(FilePath, Ext_File_Param->Stat_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s)", __func__, __LINE__, FilePath);
        return -1;
    }
    FileName_Pos = strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        HDLR_WARF("%s() %d, Invalid FileName_Pos: %s", __func__, __LINE__, FileName_Pos);
        return -1;
    }
    Output_Filename_Len = strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + strlen(System.OutputFilename);
        sprintf(Ext_Name_Pos, "_RGB_STAT.txt");
    } else {
        Ext_Name_Pos = strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            HDLR_WARF("%s() %d, Invalid FilePath : %s", __func__, __LINE__, FilePath);
            return -1;
        }

    }


    rgbStat = (AMBA_DSP_EVENT_RGB_3A_DATA_s*) Ext_File_Param->Stat_Save_Param.Address;
    HDLR_DEBF("%s() %d, FilePath = %s", __func__, __LINE__, FilePath);

    headInfo = &rgbStat->Header;
    w = headInfo->AeTileNumCol;
    h = headInfo->AeTileNumRow;
    Fd = POSIX_FOPEN(FilePath, "w");
    if (Fd < 0) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, FilePath);
        return -1;
    }

    sprintf(lineBuf, "RGB AE statistics %dx%d\n", w, h);
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    silceAeStat = &rgbStat->Ae[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * AMBA_DSP_3A_AE_TILE_COLUMN_COUNT + i;
            sprintf(num, "%5d ", silceAeStat[idx].SumY);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, 1, strlen(lineBuf), Fd);
    }

    w = headInfo->AfTileNumCol;
    h = headInfo->AfTileNumRow;
    sprintf(lineBuf, "RGB AF statistics %dx%d\n", w, h);
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    sprintf(lineBuf, "---  SumFY  ---\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    silceAfStat = &rgbStat->Af[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * AMBA_DSP_3A_AF_TILE_COLUMN_COUNT + i;
            sprintf(num, "%5d ", silceAfStat[idx].SumFY);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, 1, strlen(lineBuf), Fd);
    }

    sprintf(lineBuf, "---  SumFV1  ---\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    silceAfStat = &rgbStat->Af[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * AMBA_DSP_3A_AF_TILE_COLUMN_COUNT + i;
            sprintf(num, "%5d ", silceAfStat[idx].SumFV1);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, 1, strlen(lineBuf), Fd);
    }

    sprintf(lineBuf, "---  SumFV2  ---\n");
    POSIX_FWRITE(lineBuf, strlen(lineBuf), 1, Fd);
    silceAfStat = &rgbStat->Af[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * AMBA_DSP_3A_AF_TILE_COLUMN_COUNT + i;
            sprintf(num, "%5d ", silceAfStat[idx].SumFV2);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, 1, strlen(lineBuf), Fd);
    }

    POSIX_FSYNC(Fd);
    POSIX_FCLOSE(Fd);
    HDLR_DEBF("%s() %d, Dump Cfa Stat to %s Done", __func__, __LINE__, FilePath);

    return 0;
}
static int _TextHdlr_Save_HDR_CFA_Histogram(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    ITUNER_SYSTEM_s System;
    UINT8 Output_Filename_Len;
    int j;
    char lineBuf[640];
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    POSIX_FILE *Fd;
    AMBA_DSP_HDR_HIST_STAT_s *pHdrAaaHist;
    if (Ext_File_Param == NULL) {
        HDLR_WARF("%s() %d, Ext_File_Param = NULL", __func__, __LINE__);
        return -1;
    }
    if ( Ext_File_Param->Stat_Save_Param.Address == NULL || Ext_File_Param->Stat_Save_Param.Size < sizeof(AMBA_DSP_HDR_HIST_STAT_s)) {
        HDLR_WARF("%s() %d, Ext_File_Param->Stat_Save_Param.Address = %x, Size = %d(Need size :%d)",
                  __func__, __LINE__, Ext_File_Param->Stat_Save_Param.Address, Ext_File_Param->Stat_Save_Param.Size);
        return -1;
    }

    AmbaItuner_Get_SystemInfo(&System);
    memset(FilePath, 0x0, sizeof(FilePath));
    strncpy(FilePath, Ext_File_Param->Stat_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s)", __func__, __LINE__, FilePath);
        return -1;
    }
    FileName_Pos = strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        HDLR_WARF("%s() %d, Invalid FileName_Pos: %s", __func__, __LINE__, FileName_Pos);
        return -1;
    }
    Output_Filename_Len = strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + strlen(System.OutputFilename);
        sprintf(Ext_Name_Pos, "_HDR_CFA_HIST.txt");
    } else {
        Ext_Name_Pos = strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            HDLR_WARF("%s() %d, Invalid FilePath : %s", __func__, __LINE__, FilePath);
            return -1;
        }

    }

    HDLR_DEBF("%s() %d, FilePath = %s", __func__, __LINE__, FilePath);

    Fd = POSIX_FOPEN(FilePath, "w");
    if (Fd < 0) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, FilePath);
        return -1;
    }
    pHdrAaaHist = (AMBA_DSP_EVENT_RGB_3A_DATA_s*) Ext_File_Param->Stat_Save_Param.Address;
    for (j = 0; j < 128; j++) {
        lineBuf[0] = '\0';
        {
            char num[10] = "\0";
            sprintf(num, "%7d ", pHdrAaaHist->HistoBinR[j]);
            strcat(lineBuf, num);
            sprintf(num, "%7d ", pHdrAaaHist->HistoBinG[j]);
            strcat(lineBuf, num);
            sprintf(num, "%7d ", pHdrAaaHist->HistoBinB[j]);
            strcat(lineBuf, num);
        }
        strcat(lineBuf, "\n");
        POSIX_FWRITE(lineBuf, 1, strlen(lineBuf), Fd);
    }

    POSIX_FSYNC(Fd);
    POSIX_FCLOSE(Fd);
    HDLR_DEBF("%s() %d, Dump Cfa Stat to %s Done", __func__, __LINE__, FilePath);
    return 0;
}
static int _TextHdlr_Save_YUV(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    int Ret = 0;
    ITUNER_INFO_s ItunerInfo;
    if (_Ituner.Is_Idsp_Load_Finish == FALSE) {
        // Note: Without load idsp, so it's a raw capture case
        char FilePath[MAX_NAME_LENS];
        char *FileExtName_Pos;
        memset(FilePath, 0x0, sizeof(FilePath));
        strncpy(FilePath, Ext_File_Param->YUV_Save_Param.Y_Info.Target_File_Path, sizeof(FilePath));
        if (0 != _TextHdlr_Change_System_Drive(FilePath)) {
            HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FilePath);
            return -1;
        }
        FileExtName_Pos = strrchr(FilePath, '.');
        if (FileExtName_Pos == NULL) {
            HDLR_WARF("%s() %d, FilePath : %s Invalid FileExtName_Pos: %s", __func__, __LINE__, FilePath, FileExtName_Pos);
            return -1;
        }
        sprintf(FileExtName_Pos + 1, "%s", "txt");

        TextHdlr_Get_ItunerInfo(&ItunerInfo);
        Ret |= TextHdlr_Save_IDSP(&ItunerInfo.TuningAlgoMode, FilePath);
    }
    Ret |= _TextHdlr_Save_Y(&Ext_File_Param->YUV_Save_Param.Y_Info);
    Ret |= _TextHdlr_Save_UV(&Ext_File_Param->YUV_Save_Param.UV_Info);
    return Ret;
}

static int _TextHdlr_Save_and_Load_Dummy(Ituner_Ext_File_Param_s *Ext_File_Param)
{
    HDLR_WARF("%s() %d, Not Yet Implement", __func__, __LINE__);
    return 0;
}

int TextHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s* Ext_File_Param)
{
    int (*Load_Data_Api_List[EXT_FILE_MAX])(Ituner_Ext_File_Param_s *Ext_File_Param) =
    {
        _TextHdlr_Load_FPN_Map, // EXT_FILE_FPN_MAP
        _TextHdlr_Load_Bypass_FPN_Map,
        _TextHldr_Load_Vignette,
        _TextHldr_Load_Bypass_Vignette,
        _TextHdlr_Load_Warp_Table,
        _TextHdlr_Load_Bypass_Horizontal_Table,
        _TextHdlr_Load_Bypass_Vertical_Table,
        _TextHdlr_Load_Ca_Table,
        _TextHldr_Load_Bypass_Ca_Horizontal_Table,
        _TextHdlr_Load_Bypass_Ca_Vertical_Table,
        _TextHdlr_Load_CC_Reg,
        _TextHdlr_Load_CC_ThreeD,
        _TextHdlr_Load_FIRST_SHARPEN_ThreeD_Table,
        _TextHdlr_Load_ASF_ThreeD_Table,
        _TextHdlr_Load_HISO_LOW_ASF_ThreeD_Table,
        _TextHdlr_Load_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table,
        _TextHdlr_Load_LOW_ASF_COMBINE_ThreeD_Table,

        _TextHdlr_Load_Raw,
        _TextHdlr_Save_and_Load_Dummy, //EXT_FILE_JPG
        _TextHdlr_Save_and_Load_Dummy, //EXT_FILE_YUV
        _TextHdlr_Save_and_Load_Dummy, //EXT_FILE_CFA_STAT
        _TextHdlr_Save_and_Load_Dummy,
        _TextHdlr_Load_HdrAlphaTable,
        _TextHdlr_Load_HdrLinearTable,
    };
    if (Ext_File_Type >= EXT_FILE_MAX) {
        HDLR_ERRF("%s() %d, Invalid Ext_File_Type %d", __func__, __LINE__, Ext_File_Type);
        return -1;
    }
    return Load_Data_Api_List[Ext_File_Type](Ext_File_Param);
}

int TextHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, Ituner_Ext_File_Param_s* Ext_File_Param)
{
    int (*Save_Data_Api_List[EXT_FILE_MAX])(Ituner_Ext_File_Param_s *Ext_File_Param) =
    {
        _TextHdlr_Save_FPN_Map, // EXT_FILE_FPN_MAP
        _TextHdlr_Save_and_Load_Dummy,//_TextHdlr_Save_Bypass_FPN_Map,
        _TextHdlr_Save_Vignette,
        _TextHdlr_Save_and_Load_Dummy,//_TextHldr_Save_Bypass_Vignette,
        _TextHdlr_Save_Warp_Table,
        _TextHdlr_Save_and_Load_Dummy,//_TextHdlr_Save_Bypass_Horizontal_Table,
        _TextHdlr_Save_and_Load_Dummy,//_TextHdlr_Save_Bypass_Vertical_Table,
        _TextHdlr_Save_Ca_Table,
        _TextHdlr_Save_and_Load_Dummy, //_TextHldr_Save_Bypass_Ca_Horizontal_Table,
        _TextHdlr_Save_and_Load_Dummy, //_TextHdlr_Save_Bypass_Ca_Vertical_Table,
        _TextHdlr_Save_CC_Reg,
        _TextHdlr_Save_CC_ThreeD,
        _TextHdlr_Save_FIRST_SHARPEN_ThreeD_Table,
        _TextHdlr_Save_ASF_ThreeD_Table,
        _TextHdlr_Save_HISO_LOW_ASF_ThreeD_Table,
        _TextHdlr_Save_and_Load_Dummy, //_TextHdlr_Save_HISO_CHROMA_MED_COMBINE
        _TextHdlr_Save_HISO_CHROMA_VERYLOW_COMBINE_ThreeD_Table,
        
        _TextHdlr_Save_Raw,
        _TextHdlr_Save_Jpg,
        _TextHdlr_Save_YUV,
        _TextHdlr_Save_CFA_STAT,
        _TextHdlr_Save_RGB_STAT,
        _TextHdlr_Save_HDR_CFA_Histogram,
        _TextHdlr_Save_and_Load_Dummy,//HdrAlphaTable SAVE TBD
        _TextHdlr_Save_and_Load_Dummy,//HdrLinearTable SAVE TBD
    };
    if (Ext_File_Type >= EXT_FILE_MAX) {
        HDLR_ERRF("%s() %d, Invalid Ext_File_Type %d", __func__, __LINE__, Ext_File_Type);
        return -1;
    }
    return Save_Data_Api_List[Ext_File_Type](Ext_File_Param);
}


static int _TextHdlr_load_Ext_file(char *FileName, int Size, void *Buf)
{
    POSIX_FILE *Fid;
    int Ret = 0;
    if (0 != _TextHdlr_Change_System_Drive(FileName)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }
    //asc_to_uni(FileName, UniFname);
//    HDLR_DEBF("%s() %d, FileName: %s, Addr: %x, Size: %d", __func__, __LINE__, FileName, Buf, Size);

    Fid = POSIX_FOPEN(FileName, "r");
    if (Fid == 0) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }
    if (POSIX_FREAD(Buf, Size, 1, Fid)) {

    } else {
        HDLR_WARF("%s() %d, call POSIX_FREAD() Fail, Buf: %p, Size: %d", __func__, __LINE__, Buf, Size);
        Ret = -2;
    }
    POSIX_FCLOSE(Fid);
    return Ret;
}

static int _TextHdlr_Save_Ext_File(char *FileName, int Size, void *Buf)
{
    POSIX_FILE *Fid;
    char UniFname[MAX_NAME_LENS];
    int Ret = 0;
    if (0 != _TextHdlr_Change_System_Drive(FileName)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }
    asc_to_uni(FileName, UniFname);
    Fid = POSIX_FOPEN(UniFname, "w");
    if (Fid == 0) {
        HDLR_WARF("%s() %d, call POSIX_FOPEN(%s) Fail", __func__, __LINE__, FileName);
        return -1;
    }
    if (POSIX_FWRITE(Buf, Size, 1, Fid)) {

    } else {
        HDLR_WARF("%s() %d, call POSIX_FWRITE() Fail, Buf: %p, Size: %d", __func__, __LINE__, Buf, Size);
        Ret = -2;
    }
    POSIX_FCLOSE(Fid);
    return Ret;
}
static int _TextHdlr_Fill_Ext_File_Path(void)
{
    AMBA_ITUNER_VALID_FILTER_t Ituner_Valid;
    AmbaItuner_Get_FilterStatus(&Ituner_Valid);
    // Note: Without Return Fail, Because it should be happen.
    if (Ituner_Valid[ITUNER_COLOR_CORRECTION] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_CC_REG, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_CC_REG) Fail", __func__, __LINE__);
        }
        if (0 != TextHdlr_Save_Data(EXT_FILE_CC_THREED, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_CC_THREED) Fail", __func__, __LINE__);
        }
    }
    if (Ituner_Valid[ITUNER_STATIC_BAD_PIXEL_CORRECTION] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_FPN_MAP, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_FPN_MAP) Fail", __func__, __LINE__);
        }
    }
    if (Ituner_Valid[ITUNER_VIGNETTE_COMPENSATION] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_VIGNETTE_GAIN, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_VIGNETTE_GAIN) Fail", __func__, __LINE__);
        }
    }
    if (Ituner_Valid[ITUNER_WARP_COMPENSATION] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_WARP_TABLE, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_WARP_TABLE) Fail", __func__, __LINE__);
        }
    }
    if (Ituner_Valid[ITUNER_CHROMA_ABERRATION_INFO] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_CA_TABLE, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_CA_TABLE) Fail", __func__, __LINE__);
        }
    }

    if (Ituner_Valid[ITUNER_SHARPEN_BOTH] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_FIRST_SHARPEN, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_FIRST_SHARPEN) Fail", __func__, __LINE__);
        }
    }
    if (Ituner_Valid[ITUNER_ASF_INFO] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_ASF, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_ASF) Fail", __func__, __LINE__);
        }
    }

    if (Ituner_Valid[ITUNER_HISO_LOW_ASF] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_HISO_LOW_ASF, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_HISO_LOW_ASF) Fail", __func__, __LINE__);
        }
    }

    if (Ituner_Valid[ITUNER_HISO_CHROMA_FILTER_VERY_LOW_COMBINE] == 1) {
        if (0 != TextHdlr_Save_Data(EXT_FILE_HISO_CHROMA_VERYLOW_COMBINE, NULL)) {
            HDLR_WARF("%s() %d, call TextHdlr_Save_Data(EXT_FILE_HISO_CHROMA_VERYLOW_COMBINE) Fail", __func__, __LINE__);
        }
    }

    return 0;
}

int TextHdlr_Save_IDSP(AMBA_DSP_IMG_MODE_CFG_s *pMode, char *Filepath)
{
    //char AscFilepath[MAX_NAME_LENS];
    AMBA_ITUNER_VALID_FILTER_t Filter_Status;
    char *Buf = NULL;
    int BufSize = 0;
    POSIX_FILE *Fd;
    int RegIndex;
    int ParamIndex;
    REG_s *Reg;
    _TextHdlr_Change_System_Drive(Filepath);
    strncpy(Ituner_File_Path, Filepath, sizeof(Ituner_File_Path));
    // Note: Refresh GData, System...
    if (0 != _TextHdlr_Update_System_Info(pMode)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Update_System_Info() Fail", __func__, __LINE__);
        return -1;
    }
    if (0 != AmbaItuner_Refresh(pMode)) {
        HDLR_WARF("%s() %d, call AmbaItuner_Refresh() Fail", __func__, __LINE__);
        return -1;
    }
    if (0 != _TextHdlr_Fill_Ext_File_Path()) {
        HDLR_WARF("%s() %d, call _TextHdlr_Fill_Ext_File_Path() Fail", __func__, __LINE__);
        return -1;
    }
    TUNE_Parser_Set_Reglist_Valid(SKIP_VALID_CHECK, _Ituner.ParserObject);
    // Note: Don't dump internal param
    TUNER_Parser_Set_Reg_Valid("internal", 0, _Ituner.ParserObject);
    TUNER_Parser_Set_Reg_Valid("hdr_alpha_calc_cfg_internal", 0, _Ituner.ParserObject);
    //uni_to_asc(Filepath, AscFilepath);
    if (0 != _TextHdlr_Change_System_Drive(Filepath)) {
        HDLR_WARF("%s() %d, call _TextHdlr_Change_System_Drive(%s) Fail", __func__, __LINE__, Filepath);
        return -1;
    }

    if (0 != TUNE_Parser_Get_LineBuf(_Ituner.ParserObject, &Buf, &BufSize)) {
        HDLR_WARF("%s() %d, call TUNE_Parser_Get_LineBuf() Fail", __func__, __LINE__);
        return -1;
    }
    Fd = POSIX_FOPEN(Filepath, "w");
    if (Fd == 0) {
        HDLR_WARF("%s() %d, File %s open fail!", __func__, __LINE__, Filepath);
        return -1;
    }
    TUNE_Parser_Set_Opmode(ITUNER_ENC);
    AmbaItuner_Get_FilterStatus(&Filter_Status);
    // TODO : Remove _Ituner.ParserObject->RegCount, use api to get count
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
            _save_line(Fd, Buf);
        }
    }
    POSIX_FCLOSE(Fd);
    return 0;
}

//int ituner_add_ext_reg(REG_s *pReg)
//{
//    if (_Ituner.ExtRegCount < MAX_REGS) {
//        _Ituner.ExtRegs[_Ituner.ExtRegCount] = pReg;
//        pReg->Index = _Ituner.ExtRegCount;
//        _Ituner.ExtRegCount++;
//    } else {
//        AmbaPrint("[iTuner]Extended register exceed maximum REG number");
//        return -1;
//    }
//    return 0;
//}


//REG_s* ituner_lookup_ext_reg(const char* Name)
//{
//    return _lookup_reg(_Ituner.ExtRegCount, Name, _Ituner.ExtRegs);
//}

static AMBA_DSP_IMG_PIPE_e _TextHdlr_Get_Imag_Pipe(TUNING_MODE_e TuningMode)
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

static AMBA_DSP_IMG_FUNC_MODE_e _TextHdlr_Get_Image_Func_Mode(TUNING_MODE_EXT_e TuningModeExt)
{
    AMBA_DSP_IMG_FUNC_MODE_e ImageFuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
    if (TuningModeExt == SINGLE_SHOT_MULTI_EXPOSURE_HDR) {
        ImageFuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
    } else if (TuningModeExt == SINGLE_SHOT) {
        ImageFuncMode = AMBA_DSP_IMG_FUNC_MODE_FV;
    } else if (TuningModeExt == DYNAMIC_OB) {
        ImageFuncMode = AMBA_DSP_IMG_FUNC_MODE_RAW2RAW;
    } else {
        HDLR_WARF("%s() %d, TuningModeExt = %d", __func__, __LINE__, TuningModeExt);
    }
    return ImageFuncMode;
}

static AMBA_DSP_IMG_ALGO_MODE_e _TextHdlr_Get_Image_Algo_Mode(TUNING_MODE_e TuningMode)
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

