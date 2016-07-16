/**
 * @file src/app/connected/applib/src/dcf/ApplibDCF_NamingRuleDateTime.c
 *
 * Implementation of DCF Applib naming rule
 *
 * History:
 *    2015/01/30 - [Evan Ji] created file
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

#include <wctype.h>
#include <cfs/AmpCfs.h>
#include <dcf/AmpDcf.h>
#include <dcf/ApplibDcfDateTime.h>
#include <dcf/ApplibDcf_FilterDateTime.h>
#include <storage/ApplibStorage_DmfDateTime.h>
#include <AmbaRTC.h>

//#define DEBUG_APPLIB_DCF_NAMING_RULE_DATE_TIME
#if defined(DEBUG_APPLIB_DCF_NAMING_RULE_DATE_TIME)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(BLUE,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static char VinChannel = VIN_MAIN_SENSOR;
static AMBA_RTC_TIME_SPEC_u PrevTime = {0};
static UINT8 JpegCount = 0;

static void PerrorImpl(UINT32 nLine, char *ErrMsg)
{
    char FileName[MAX_FILENAME_LENGTH];
    strncpy(FileName, (strrchr(__FILE__, '\\') + 1), (strlen(__FILE__) - strlen((strrchr(__FILE__, '\\') + 1)) + 1));
    AmbaPrint("[Error]%s %u: %s", FileName, nLine, ErrMsg);
}

#define Perror(ErrMsg) {\
    PerrorImpl(__LINE__, ErrMsg);\
}

/**
 * APIs for DCF Applib Naming Rule
 */
/**
 *  Get file name from file number
 *  @param [in]  fnum file number
 *  @param [out]  fname file name string
 *  @return file name string
 */
static char* Naming_FnumToFname(UINT32 fnum, char *fname)
{
    UINT32 num = fnum;
    UINT32 h, m, s;

    DBGMSGc2(BLUE, "==[%s -%d]  -START- Fnum:%d", __FUNCTION__, __LINE__, fnum);
    h = (num / 3600);
    num = num - (h * 3600);
    m = (num / 60);
    s = num - (m * 60);
    sprintf(fname, "%02d%02d%02d", h, m, s);
    DBGMSGc2(BLUE, "##[%s-%d] -END- Fname:%s", __FUNCTION__, __LINE__, fname);
    return fname;
 }

 /**
 *  Get directory name from directory number
 *  @param [in]  dnum Directory number
 *  @param [out]  dname directory name string
 *  @return directory name string
 */
static char* Naming_DnumToDname(UINT32 dnum, char *dname, UINT32 Type)
 {
    UINT32 num;
    UINT32 leap = DCF_LEAP_YEAR_RANGE, year = 0, month = 0, day = 0;

    DBGMSGc2(BLUE, "==[%s -%d]  -START- Dnum:%d", __FUNCTION__, __LINE__, dnum);
    if (dnum <= (DCF_DIR_RTC_RESET_IDX_MAX + 1))
        num = dnum - AppLibStorageDmf_GetRtcDirIndex(Type);
    else
        num = dnum - DCF_DIR_RTC_RESET_IDX_MAX;
    DBGMSGc2(BLUE, "[%s] num:%d", __FUNCTION__, num);
    // step1: calculate leap year
        while (leap) {
        day = ((g_AppLibDcfFlt_LeapYear[leap - 1] - DCF_BASE_YEAR) * 365) + 31 + 28 + (leap - 1);
        DBGMSGc2(BLUE, "[%s -%d]  days:%d", __FUNCTION__, __LINE__, day);
        if (num > day) {
            num -= leap;
            DBGMSGc2(BLUE, "[%s -%d]  leap:%d, num:%d", __FUNCTION__, __LINE__, leap, num);
            break;
        }
                leap--;
        }
    // step2: check year
    if ((num % 365) == 0) {
        year = (num / 365) + DCF_BASE_YEAR -1;
        DBGMSGc2(BLUE, "[%s] -year:%d, num:%d", __FUNCTION__, year, num);
        month = 12;
        DBGMSGc2(BLUE, "[%s] -month:%d, num:%d", __FUNCTION__, month, num);
        day = 31;
        DBGMSGc2(BLUE, "[%s] -day:%d", __FUNCTION__, day);
    } else {
        year = (num / 365) + DCF_BASE_YEAR;
        DBGMSGc2(BLUE, "[%s] -year:%d, num:%d", __FUNCTION__, year, num);
    // step3: check month
    num = (num % 365);
    while (num > g_AppLibDcfFlt_DaysOfMonth[month]) {
        num = num - g_AppLibDcfFlt_DaysOfMonth[month];
        month++;
    }
    month++;
        DBGMSGc2(BLUE, "[%s] -month:%d, num:%d", __FUNCTION__, month, num);
    // step4: check day
    day = num;
        DBGMSGc2(BLUE, "[%s] -day:%d", __FUNCTION__, day);
    }
    if (dnum <= (DCF_DIR_RTC_RESET_IDX_MAX + 1))
        sprintf(dname, "%02d%02d%02d%d%02d", year, month, day, AppLibStorageDmf_GetRtcDirIndex(Type), AppLibStorageDmf_GetDirIndex(dnum, Type));
    else
        sprintf(dname, "%02d%02d%02d0%02d", year, month, day, AppLibStorageDmf_GetDirIndex(dnum, Type));
    DBGMSGc2(BLUE, "##[%s-%d] -END- Dname:%s", __FUNCTION__, __LINE__, dname);
    return dname;
}

 /**
 *  Get directory number from directory name
 *  @param [in]  dname Directory name
 *  @return >0 success, <0 fail
 */
static UINT32 Naming_DnameToDnum(char *dname, UINT32 Type)
 {
    UINT32 num = 0, month = 0;
    UINT32 leap = DCF_LEAP_YEAR_RANGE, dnum = 0;
    char tmp[4] = {0};

    DBGMSGc2(BLUE, "==[%s -%d]  -START- Name:%s", __FUNCTION__, __LINE__, dname);
    // step1: check year
    strncpy(tmp, dname, DCF_DIR_YEAR_LEN);
    tmp[DCF_DIR_YEAR_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= DCF_BASE_YEAR) && (num <= 99))) {
        DBGMSGc2(RED, "##[%s] -END-, year part of directory name is %d, not in the range from 2014 ~ 2099", __FUNCTION__, num);
        return -1;
    }
    dnum = ((num - DCF_BASE_YEAR) * 365);
    DBGMSGc2(BLUE, "[%s]  dnum:%d - year checked", __FUNCTION__, dnum);
    // step2: check leap year
    if (num > DCF_BASE_LEAP_YEAR) {
        while (leap) {
            if (g_AppLibDcfFlt_LeapYear[leap - 1] > num)
                leap--;
            else
                break;
        }
    } else {
        leap = 0;
    }
    // step3: check month
    strncpy(tmp, (dname + DCF_DIR_YEAR_LEN), DCF_DIR_MONTH_LEN);
    tmp[DCF_DIR_MONTH_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    month = num;
    if (!((num >= 1) && (num <= 12))) {
        DBGMSGc2(RED, "##[%s] -END-, month part of directory name is %d, not in the range from 1 ~ 12", __FUNCTION__, num);
        return -1;
    }
    num--;
    while (num) {
        dnum += g_AppLibDcfFlt_DaysOfMonth[num - 1];
        num--;
    }
    DBGMSGc2(BLUE, "[%s]  dnum:%d - month checked", __FUNCTION__, dnum);
    if (leap > 0) {
        DBGMSGc2(BLUE, "[%s] leap year days: %d", __FUNCTION__, leap);
        if (month < 3) {
            leap--;
            DBGMSGc2(BLUE, "[%s] leap year days: %d", __FUNCTION__, leap);
        }
        dnum += leap;
    }
    DBGMSGc2(BLUE, "[%s]  dnum:%d - leap year checked", __FUNCTION__, dnum);
    // step4: check day
    strncpy(tmp, (dname + DCF_DIR_YEAR_LEN + DCF_DIR_MONTH_LEN), DCF_DIR_DAY_LEN);
    tmp[DCF_DIR_DAY_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 1) && (num <= 31))) {
        DBGMSGc2(RED, "##[%s] -END-, day part of directory name is %d, not in the range from 1 ~ 31", __FUNCTION__, num);
        return -1;
    }
    dnum += num;
    DBGMSGc2(BLUE, "[%s]  dnum:%d - day checked", __FUNCTION__, dnum);
    if (dnum == 1) { // add RTC reset index if the date is 2014/01/01)
        dnum += AppLibStorageDmf_GetRtcDirIndex(Type);
    } else {
        dnum += DCF_DIR_RTC_RESET_IDX_MAX;
    }
    DBGMSGc2(BLUE, "##[%s -%d] -END- Dnum:%u", __FUNCTION__, __LINE__, dnum);
    return dnum;
}

/**
 *  Check if an Object ID is Valid
 *  @param [in]  pFilter address of filter
 *  @param [in]  ObjId Object Id
 *  @return TRUEValid, FALSE Invalid
 */
static BOOL Naming_IsIdValid(AMP_DCF_FILTER_s *pFilter, UINT32 ObjId)
{
    UINT32 Dnum, Fnum;
DBGMSGc2(BLUE, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    Dnum = pFilter->IdToDnum(ObjId);
    if ((Dnum >= DCF_DIR_NUM_MIN) && (Dnum <= DCF_DIR_NUM_MAX)) {
        Fnum = pFilter->IdToFnum(ObjId);
        if ((Fnum >= DCF_FILE_NUM_MIN) && (Fnum <= DCF_FILE_NUM_MAX)) {
DBGMSGc2(BLUE, "==[%s-%d] -END-, true", __FUNCTION__, __LINE__);
            return TRUE;
        }
    }
DBGMSGc2(BLUE, "==[%s-%d] -END-, false", __FUNCTION__, __LINE__);
    return FALSE;
}

static UINT32 Naming_GetCurDcfHdlr(const char *RootName)
{
    UINT32 DcfHdlrIdx = STORAGE_DCF_HDLR_MAX - 1;
    while (DcfHdlrIdx >= 0) {
        if (strstr(RootName, g_AppLibStorageRootName[DcfHdlrIdx]) != NULL) {
            break;
        } else {
            DcfHdlrIdx--;
        }
    }
    DBGMSGc2(BLUE, "[%s -%d] DcfHdlrIdx: %d", __FUNCTION__, __LINE__, DcfHdlrIdx);
    return DcfHdlrIdx;
}

static BOOL Naming_CheckTimeSame(AMBA_RTC_TIME_SPEC_u *curTime)
{
    if (PrevTime.Calendar.Second != curTime->Calendar.Second) {
        return FALSE;
    } else {
        if (PrevTime.Calendar.Minute != curTime->Calendar.Minute) {
            return FALSE;
        } else {
            if (PrevTime.Calendar.Hour != curTime->Calendar.Hour) {
                return FALSE;
            } else {
                if (PrevTime.Calendar.Day != curTime->Calendar.Day) {
                    return FALSE;
                } else {
                    if (PrevTime.Calendar.Month != curTime->Calendar.Month) {
                        return FALSE;
                    } else {
                        if (PrevTime.Calendar.Year != curTime->Calendar.Year) {
                            return FALSE;
                        } else {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
}

static int Naming_GetDirForFirstDay(AMP_DCF_FILTER_s *Filter, const char *RootName, char *FileName,
                                                const char *ExtName, char *DirPath, int DirPathMaxLen)
{
    static UINT32 ScannedMaxFnum = 0;
    static UINT32 ScannedFileAmount = 0;
    static char ScannedDirPath[MAX_FILENAME_LENGTH] = {0};
    UINT8 DcfHdlrIdx = Naming_GetCurDcfHdlr(RootName);
    int DirIndexTmp = -1;
    int DirIndex = -1;
    UINT8 TargetRtc = 0;
    UINT32 TargetDirIndex = 0;
    UINT32 TargetDnum = 0;  //last Dnum has been used.
    UINT32 TargetFnum = 0;
    char TmpDirPath[MAX_FILENAME_LENGTH] = {0};
    char TmpName[MAX_FILENAME_LENGTH] = {0};
    int i = 0;
    UINT32 ObjectId = 0;
    UINT32 MaxFnum = 0;
    UINT32 FileAmount = 0;
    UINT8 FlagCreateFolder = 0;
    AMP_CFS_STAT Stat = {0};
    BOOL FlagJpgCont = FALSE; //TRUE: This object is for cont. JPEG (ex. The 2nd capture forth in Burst/PES mode)
    int ReturnValue = 0;
    BOOL Refreshed = FALSE;

    K_ASSERT(Filter != NULL);
    K_ASSERT(RootName != NULL);
    K_ASSERT(FileName != NULL);
    K_ASSERT(ExtName != NULL);
    K_ASSERT(DirPath != NULL);
    K_ASSERT(DirPathMaxLen != 0);

    Refreshed = AppLibStorageDmf_GetRefreshStatus(TRUE);

    if (((strncmp(ExtName,"jpg",3) == 0) || (strncmp(ExtName,"JPG",3) == 0)) && (JpegCount >0)) {
        FlagJpgCont = TRUE;
    }

    for(i=1;i<=(DCF_DIR_RTC_RESET_IDX_MAX+1);i++) {
        DirIndexTmp = AppLibStorageDmf_GetDirIndex(i,DcfHdlrIdx);
        if (DirIndexTmp != -1) {
            TargetDnum = i;
            DirIndex = DirIndexTmp;
        }
    }

    AmbaPrintColor(CYAN,"==[%s -%d] TargetDnum: %d",__FUNCTION__,__LINE__, TargetDnum);
    if (TargetDnum == 0) {
        /* folder for 2014/01/01 does not exist. create a new folder. RTC index range is 1~9. */
        TargetRtc = 1;
        TargetDirIndex = 0;
        FlagCreateFolder = 1;
    } else {
        TargetRtc = TargetDnum - 1;
        TargetDirIndex = DirIndex;

        snprintf(TmpDirPath,MAX_FILENAME_LENGTH,"%s\\140101%d%02d",RootName,TargetRtc,TargetDirIndex);
        snprintf(TmpName,MAX_FILENAME_LENGTH,"%s\\%s.%s",TmpDirPath,FileName,ExtName);
        AmbaPrintColor(CYAN,"==[%s -%d] TmpName:%s",__FUNCTION__,__LINE__,TmpName);
        ObjectId = Filter->NameToId(TmpName);
        if (ObjectId == 0) {
            AmbaPrintColor(RED,"==[%s -%d] NameToId() fail",__FUNCTION__,__LINE__);
            return STORAGE_DMF_UNREACHABLE;
        }

        TargetFnum = Filter->IdToFnum(ObjectId);

        AmbaPrintColor(CYAN,"==[%s -%d] Refreshed = %d, TmpDirPath = %s, ScannedDirPath = %s", __FUNCTION__,__LINE__, Refreshed, TmpDirPath, ScannedDirPath);
        if (Refreshed || (strcmp(TmpDirPath, ScannedDirPath) != 0)) {
            ReturnValue = AppLibStorageDmf_ScanDirForMaxFnum(DcfHdlrIdx, TargetDnum, TmpDirPath, &MaxFnum, &FileAmount);
            if (ReturnValue != 0) {
                AmbaPrintColor(RED,"==[%s -%d] AppLibStorageDmf_ScanDir() fail",__FUNCTION__,__LINE__);
                return STORAGE_DMF_UNREACHABLE;
            }

            strcpy(ScannedDirPath, TmpDirPath);
            ScannedMaxFnum = MaxFnum;
            ScannedFileAmount = FileAmount;
        }

        AmbaPrintColor(CYAN,"==[%s -%d] TargetFnum = %d, ScannedMaxFnum = %d", __FUNCTION__,__LINE__, TargetFnum, ScannedMaxFnum);
        AmbaPrintColor(CYAN,"==[%s -%d] ScannedFileAmount = %d", __FUNCTION__,__LINE__, ScannedFileAmount);
        if (TargetFnum > ScannedMaxFnum) {
            /* There is no conflict with Fnum. */
            if (ScannedFileAmount >= DCF_FILE_AMOUNT_MAX) {
                /* The number of file exceeded maximun number of a directory. Create a new folder. */
                TargetDirIndex++;
                FlagCreateFolder = 1;
            } else {
                ScannedMaxFnum = TargetFnum;
                ScannedFileAmount++;
            }
        } else if ((TargetFnum == ScannedMaxFnum) && FlagJpgCont) {
            /* Keep JPGs with the same objectID in the same directory.
                      So we don't check (ScannedFileAmount >= DCF_FILE_AMOUNT_MAX) in this case. */

            /* Do nothing in this case */
        } else {
            if (TargetRtc >= 9) {
                AmbaPrintColor(RED, "==[%s -%d] Run out of directory index!! (TargetRtc = %d)",__FUNCTION__,__LINE__,TargetRtc);
                return STORAGE_DMF_UNREACHABLE;
            }

            TargetRtc++;
            TargetDirIndex = 0;
            FlagCreateFolder = 1;
        }
    }


    if (TargetDirIndex > DCF_DIR_AMOUNT_MAX) {
        AmbaPrintColor(RED, "==[%s -%d] Run out of directory index!! (TargetDirIndex = %d)",__FUNCTION__,__LINE__,TargetDirIndex);
        return STORAGE_DMF_DIR_IDX_REACH_LIMIT;
    }

    memset(DirPath, 0, DirPathMaxLen);
    snprintf(DirPath, DirPathMaxLen,"%s\\140101%d%02d", RootName, TargetRtc, TargetDirIndex);
    if (FlagCreateFolder) {
        AmbaPrintColor(CYAN,"==[%s -%d] DirPath = %s",__FUNCTION__,__LINE__, DirPath);
        if (AmpCFS_Stat(DirPath, &Stat) != AMP_OK) {
            if (AmpCFS_Mkdir(DirPath) != AMP_OK) {
                AmbaPrintColor(RED, "==[%s -%d] Make directory Error (%s)!",__FUNCTION__,__LINE__,DirPath);
                return STORAGE_DMF_MKDIR_FAIL;
            }
        } else {
            DBGMSGc2(BLUE, "[%s] %s is exist already!!!", __FUNCTION__, DirPath);
        }
    }

    AppLibStorageDmf_SetRtcDirIndex(TargetRtc, DcfHdlrIdx);
    AppLibStorageDmf_SetCurDnumIdx(DcfHdlrIdx, TargetDirIndex);
    AppLibStorageDmf_SetDirIndex((TargetRtc+1), TargetDirIndex, DcfHdlrIdx);
    AppLibStorageDmf_SetCurDnum(DcfHdlrIdx, (TargetRtc+1));

    return 0;
}
static int Naming_GetDir(AMP_DCF_FILTER_s *Filter, const char *RootName, char *FileName,
                               const char *ExtName, AMBA_RTC_TIME_SPEC_u *CurTime, char *DirPath, int DirPathMaxLen)
{
    static UINT32 ScannedMaxFnum = 0;
    static UINT32 ScannedFileAmount = 0;
    static char ScannedDirPath[MAX_FILENAME_LENGTH] = {0};
    UINT8 TargetRtc = 0;
    UINT32 TargetDirIndex = 0;
    UINT32 TargetDnum = 0;
    UINT32 TargetFnum = 0;
    UINT8 DcfHdlrIdx = Naming_GetCurDcfHdlr(RootName);
    char TimeBuf[16] = {0};
    char *pTimeBuf = NULL;
    UINT32 MaxFnum = 0;
    UINT32 FileAmount = 0;
    UINT8 FlagCreateFolder = 0; //1: have to create new folder, 0: no need to create new folder
    char TmpDirPath[MAX_FILENAME_LENGTH] = {0};
    char TmpName[MAX_FILENAME_LENGTH] = {0};
    UINT32 ObjectId = 0;
    AMP_CFS_STAT Stat = {0};
    int ReturnValue = 0;
    BOOL FlagJpgCont = FALSE; //TRUE: This object is for cont. JPEG (ex. The 2nd capture forth in Burst/PES mode)
    BOOL Refreshed = FALSE;

    K_ASSERT(Filter != NULL);
    K_ASSERT(RootName != NULL);
    K_ASSERT(FileName != NULL);
    K_ASSERT(ExtName != NULL);
    K_ASSERT(CurTime != NULL);
    K_ASSERT(DirPath != NULL);
    K_ASSERT(DirPathMaxLen != 0);

    Refreshed = AppLibStorageDmf_GetRefreshStatus(TRUE);

    if (((strncmp(ExtName,"jpg",3) == 0) || (strncmp(ExtName,"JPG",3) == 0)) && (JpegCount >0)) {
        FlagJpgCont = TRUE;
    }

    snprintf(TimeBuf, sizeof(TimeBuf), "%04d%02d%02d",CurTime->Calendar.Year,CurTime->Calendar.Month,CurTime->Calendar.Day);
    pTimeBuf = (char *)((UINT32)TimeBuf + 2);

    TargetDnum = Naming_DnameToDnum(pTimeBuf, DcfHdlrIdx);
    if (TargetDnum <= 0) {
        AmbaPrintColor(RED, "==[%s -%d] Dname translate to Dnum fail",__FUNCTION__,__LINE__);
        return STORAGE_DMF_UNREACHABLE;
    }

    TargetDirIndex = AppLibStorageDmf_GetDirIndex(TargetDnum, DcfHdlrIdx);
    AmbaPrintColor(CYAN,"==[%s -%d] TargetDirIndex = %d",__FUNCTION__,__LINE__,TargetDirIndex);

#ifdef CONFIG_APP_ARD
    if (TargetDirIndex != -1) {
        AMBA_FS_STAT Status;
        snprintf(TmpDirPath,MAX_FILENAME_LENGTH,"%s\\%s%d%02d",RootName,pTimeBuf,TargetRtc,TargetDirIndex);
        if (AmbaFS_Stat(TmpDirPath, &Status) != OK) {
            AmbaPrint("%s, %d", __func__, __LINE__);
            TargetDirIndex = -1;
        }
        memset(TmpDirPath, 0, MAX_FILENAME_LENGTH);
    }
#endif

    if (TargetDirIndex == -1) {
        /* folder that matchs 'CurTime' doesn't exist. Create a new folder. */
        TargetDirIndex = 0;
        FlagCreateFolder = 1;
    } else {
        snprintf(TmpDirPath,MAX_FILENAME_LENGTH,"%s\\%s%d%02d",RootName,pTimeBuf,TargetRtc,TargetDirIndex);
        snprintf(TmpName,MAX_FILENAME_LENGTH,"%s\\%s.%s",TmpDirPath,FileName,ExtName);
        AmbaPrintColor(CYAN,"<%s> TmpName:%s",__FUNCTION__,TmpName);
        ObjectId = Filter->NameToId(TmpName);
        if (ObjectId == 0) {
            AmbaPrintColor(RED,"==[%s -%d] NameToId() fail",__FUNCTION__,__LINE__);
            return STORAGE_DMF_UNREACHABLE;
        }

        TargetFnum = Filter->IdToFnum(ObjectId);

        AmbaPrintColor(CYAN,"==[%s -%d] Refreshed = %d, TmpDirPath = %s, ScannedDirPath = %s", __FUNCTION__,__LINE__, Refreshed, TmpDirPath, ScannedDirPath);
        if (Refreshed || (strcmp(TmpDirPath, ScannedDirPath) != 0)) {
            ReturnValue = AppLibStorageDmf_ScanDirForMaxFnum(DcfHdlrIdx, TargetDnum, TmpDirPath, &MaxFnum, &FileAmount);
            if (ReturnValue != 0) {
                AmbaPrintColor(RED,"==[%s -%d] AppLibStorageDmf_ScanDir() fail",__FUNCTION__,__LINE__);
                return STORAGE_DMF_UNREACHABLE;
            }

            strcpy(ScannedDirPath, TmpDirPath);
            ScannedMaxFnum = MaxFnum;
            ScannedFileAmount = FileAmount;
        }

        AmbaPrintColor(CYAN,"==[%s -%d] TargetFnum = %d, ScannedMaxFnum = %d", __FUNCTION__,__LINE__, TargetFnum, ScannedMaxFnum);
        AmbaPrintColor(CYAN,"==[%s -%d] ScannedFileAmount = %d, FlagJpgCont = %s", __FUNCTION__,__LINE__, ScannedFileAmount,FlagJpgCont ? "TRUE" : "FALSE");
        if (TargetFnum > ScannedMaxFnum) {
            /* There is no conflict with Fnum. */
            if (ScannedFileAmount >= DCF_FILE_AMOUNT_MAX) {
                /* The number of file exceeded maximun number of a directory. Create a new folder. */
                TargetDirIndex++;
                FlagCreateFolder = 1;
            } else {
                ScannedMaxFnum = TargetFnum;
                ScannedFileAmount++;
            }
        } else if ((TargetFnum == ScannedMaxFnum) && FlagJpgCont) {
            /* Keep JPGs with the same objectID in the same directory.
                      So we don't check (ScannedFileAmount >= DCF_FILE_AMOUNT_MAX) in this case. */

            /* Do nothing in this case */
        } else {
            TargetDirIndex++;
            FlagCreateFolder = 1;
        }
    }

    if (TargetDirIndex > DCF_DIR_AMOUNT_MAX) {
        AmbaPrintColor(RED, "==[%s -%d] Run out of directory index!! (TargetDirIndex = %d)",__FUNCTION__,__LINE__,TargetDirIndex);
        return STORAGE_DMF_DIR_IDX_REACH_LIMIT;
    }

    memset(DirPath, 0, DirPathMaxLen);
    snprintf(DirPath, DirPathMaxLen,"%s\\%s%d%02d", RootName, pTimeBuf, TargetRtc, TargetDirIndex);
    if (FlagCreateFolder) {
        AmbaPrintColor(CYAN,"==[%s -%d] DirPath = %s",__FUNCTION__,__LINE__, DirPath);
        if (AmpCFS_Stat(DirPath, &Stat) != AMP_OK) {
            if (AmpCFS_Mkdir(DirPath) != AMP_OK) {
                AmbaPrintColor(RED, "==[%s -%d] Make directory Error (%s)!",__FUNCTION__,__LINE__,DirPath);
                return STORAGE_DMF_MKDIR_FAIL;
            }
        } else {
            DBGMSGc2(GREEN, "[%s] %s is exist already!!!", __FUNCTION__, DirPath);
        }
    }

    AppLibStorageDmf_SetCurDnumIdx(DcfHdlrIdx, TargetDirIndex);
    AppLibStorageDmf_SetDirIndex(TargetDnum, TargetDirIndex, DcfHdlrIdx);
    AppLibStorageDmf_SetCurDnum(DcfHdlrIdx, TargetDnum);

    return 0;
}
/**
 *  Get object name
 *  @param [in]  pFilter address of filter
 *  @param [in]  LastObjId last Object's id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >0 success, <0 failure
 */

/**
 *  Get object name
 *  @param [in]  pFilter address of filter
 *  @param [in]  LastObjId last Object's id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtName extension name
 *  @param [in]  VIN channel number, if value is 0: MAIN sensor, 1: Rear sensor 1, 2: Rear sensor 2, etc
 *  @param [out] ObjName address of object name
 *  @return >0 success, <0 failure
 */
 #if 0
static int Naming_GetObjectName(AMP_DCF_FILTER_s *pFilter, UINT32 LastObjId, const char *RootName, const char *ExtName, char *ObjName)
{
    AMBA_RTC_TIME_SPEC_u CurTime = {0};
    char FileName[MAX_FILENAME_LENGTH];
    char DirPath[MAX_FILENAME_LENGTH];
    BOOL IsJpeg = FALSE;
    UINT32 ReturnValue = 0;

    ObjName[0] = '\0';
    AmbaPrintColor(BLUE, "==[%s -%d] -START-,  LastObjId:%d, RootName:%s, ExtName:%s, VinChannel:%d, ObjName:%s",
                                        __FUNCTION__, __LINE__, LastObjId, RootName, ExtName, VinChannel, ObjName);
    //Dnum = pFilter->IdToDnum(ObjId);
    ReturnValue = AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &CurTime);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"==[%s -%d] AmbaRTC_GetSystemTime() fail",__FUNCTION__,__LINE__);
        return STORAGE_DMF_UNREACHABLE;
    }

    /**< Check file type */
    if ((strncmp(ExtName,"jpg",3) == 0) || (strncmp(ExtName,"JPG",3) == 0)) {
        IsJpeg = TRUE;

        /**< Check RTC time is the same or not */
        if (Naming_CheckTimeSame(&CurTime) == FALSE) {
            memcpy(&PrevTime, &CurTime, sizeof(AMBA_RTC_TIME_SPEC_u));
            JpegCount = 0;
        } else {
            JpegCount++;
        }
        DBGMSGc2(BLUE, "[%s -%d] It is jpeg file", __FUNCTION__, __LINE__);
    }
    
#ifdef CONFIG_APP_ARD
	if((CurTime.Calendar.Hour == 0)&&(CurTime.Calendar.Minute == 0)&&(CurTime.Calendar.Second == 0))
		CurTime.Calendar.Second = 1;		
#endif	
    /**< Get file name */
    if (IsJpeg == TRUE) {
        sprintf(FileName, "%02d%02d%02d%02d", CurTime.Calendar.Hour, CurTime.Calendar.Minute, CurTime.Calendar.Second, JpegCount);
    } else {
        sprintf(FileName, "%02d%02d%02d%c%c", CurTime.Calendar.Hour, CurTime.Calendar.Minute, CurTime.Calendar.Second, (VinChannel), (MAIN_STREAM));
    }

    /* The naming rule is different between base day (2014/01/01) and other days.
          So it is dealt with by different functions. */
    if ((CurTime.Calendar.Year == 2014) && (CurTime.Calendar.Month == 1) && (CurTime.Calendar.Day== 1)) {
        ReturnValue = Naming_GetDirForFirstDay(pFilter, RootName, FileName, ExtName, DirPath, MAX_FILENAME_LENGTH);
    } else {
        ReturnValue = Naming_GetDir(pFilter, RootName, FileName, ExtName, &CurTime, DirPath, MAX_FILENAME_LENGTH);
    }

    if (ReturnValue != 0) {
        return ReturnValue;
    }

    strncpy(ObjName, DirPath, MAX_FILENAME_LENGTH);
    strcat(ObjName, "\\");
    strcat(ObjName, FileName);
    strcat(ObjName, ".");
    strcat(ObjName, ExtName);
    AmbaPrintColor(BLUE, "==[%s -%d]  -END-, ObjName:%s", __FUNCTION__, __LINE__, ObjName);
    return 0;
}
#else
static int Naming_GetObjectName(AMP_DCF_FILTER_s *pFilter, UINT32 LastObjId, const char *RootName, const char *ExtName, char *ObjName)
{
    AMBA_RTC_TIME_SPEC_u CurTime = {0};
    char FileName[MAX_FILENAME_LENGTH];
    char DirPath[MAX_FILENAME_LENGTH];
    BOOL IsJpeg = FALSE;
    UINT32 ReturnValue = 0;

    ObjName[0] = '\0';
    AmbaPrintColor(BLUE, "==[%s -%d] -START-,  LastObjId:%d, RootName:%s, ExtName:%s, VinChannel:%d, ObjName:%s",
                                        __FUNCTION__, __LINE__, LastObjId, RootName, ExtName, VinChannel, ObjName);
    //Dnum = pFilter->IdToDnum(ObjId);
    ReturnValue = AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &CurTime);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"==[%s -%d] AmbaRTC_GetSystemTime() fail",__FUNCTION__,__LINE__);
        return STORAGE_DMF_UNREACHABLE;
    }

    /**< Check file type */
    if ((strncmp(ExtName,"jpg",3) == 0) || (strncmp(ExtName,"JPG",3) == 0)) {
        IsJpeg = TRUE;

        /**< Check RTC time is the same or not */
        if (Naming_CheckTimeSame(&CurTime) == FALSE) {
            memcpy(&PrevTime, &CurTime, sizeof(AMBA_RTC_TIME_SPEC_u));
            JpegCount = 0;
        } else {
            JpegCount++;
        }
        DBGMSGc2(BLUE, "[%s -%d] It is jpeg file", __FUNCTION__, __LINE__);
    }
    
#ifdef CONFIG_APP_ARD
    if((CurTime.Calendar.Hour == 0)&&(CurTime.Calendar.Minute == 0)&&(CurTime.Calendar.Second == 0))
        CurTime.Calendar.Second = 1;        
#endif  
    /**< Get file name */
    if (IsJpeg == TRUE) {
        sprintf(FileName, "%02d%02d%02d%02d", CurTime.Calendar.Hour, CurTime.Calendar.Minute, CurTime.Calendar.Second, JpegCount);
    } else {
        sprintf(FileName, "%02d%02d%02d%c%c", CurTime.Calendar.Hour, CurTime.Calendar.Minute, CurTime.Calendar.Second, (VinChannel), (MAIN_STREAM));
    }

    /* The naming rule is different between base day (2014/01/01) and other days.
          So it is dealt with by different functions. */
    if ((CurTime.Calendar.Year == 2014) && (CurTime.Calendar.Month == 1) && (CurTime.Calendar.Day== 1)) {
        ReturnValue = Naming_GetDirForFirstDay(pFilter, RootName, FileName, ExtName, DirPath, MAX_FILENAME_LENGTH);
    } else {
        ReturnValue = Naming_GetDir(pFilter, RootName, FileName, ExtName, &CurTime, DirPath, MAX_FILENAME_LENGTH);
    }

    if (ReturnValue != 0) {
        return ReturnValue;
    }

    strncpy(ObjName, DirPath, MAX_FILENAME_LENGTH);
    strcat(ObjName, "\\");
    strcat(ObjName, FileName);
    strcat(ObjName, ".");
    strcat(ObjName, ExtName);
    AmbaPrintColor(BLUE, "==[%s -%d]  -END-, ObjName:%s", __FUNCTION__, __LINE__, ObjName);
    return 0;
}
#endif
/**
 *  Get fullname of an extended object
 *  @param [in]  pFilter address of filter
 *  @param [in]  ObjId Object Id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtType type of extended object
 *  @param [in]  SeqNum sequence number
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >0 success, <0 failure
 */
static int Naming_GetExtObjectName(AMP_DCF_FILTER_s *pFilter, UINT32 ObjId, const char *RootName, UINT8 ExtType, UINT8 SeqNum, const char *ExtName, char *ObjName)
{
    char TmpName[MAX_FILENAME_LENGTH];
    char DirPath[MAX_FILENAME_LENGTH] = {'\0'};
    AMP_CFS_STAT Stat;
    UINT32 Dnum, Fnum;
    UINT8 DcfHdlrIdx = Naming_GetCurDcfHdlr(RootName);
    DBGMSGc2(BLUE, "==[%s-%d] -START-,  ObjId:%d, RootName:%s, ExtType:%d, SeqNum:%d, ExtName:%s, ObjName:%s", __FUNCTION__, __LINE__, ObjId, RootName, ExtType, SeqNum, ExtName, ObjName);
    Dnum = pFilter->IdToDnum(ObjId);
    K_ASSERT((Dnum >= DCF_DIR_NUM_MIN) && (Dnum <= DCF_DIR_NUM_MAX));
    Fnum = pFilter->IdToFnum(ObjId);
    K_ASSERT((Fnum >= DCF_FILE_NUM_MIN) && (Fnum <= DCF_FILE_NUM_MAX));
    /**< Get dir name */
    Naming_DnumToDname(Dnum, TmpName, DcfHdlrIdx);
    DBGMSGc2(BLUE, "[%s-%d] Dname: %s", __FUNCTION__, __LINE__, TmpName);
    strncpy(DirPath, RootName, MAX_FILENAME_LENGTH);
    strcat(DirPath, "\\");
    strcat(DirPath, TmpName);

    /**< Check and Create directory */
    if (AmpCFS_Stat(DirPath, &Stat) != AMP_OK) { /**< check if directory exist */
        if (AmpCFS_Mkdir(DirPath) != AMP_OK) {
            DBGMSGc2(RED, "Create directory Error!");
            return STORAGE_DMF_MKDIR_FAIL;
        }
    } else {
        DBGMSGc2(GREEN, "[%s]  %s is exist already!!!", __FUNCTION__, DirPath);
    }
    /**< Get file name */
    switch (ExtType) {
    case APPLIB_DCF_EXT_OBJECT_IMAGE_THM:
        Naming_FnumToFname(Fnum, TmpName);
        sprintf(&TmpName[DCF_FILE_TIME_LEN], "%c%c",  VinChannel, MAIN_STREAM);
        DBGMSGc2(BLUE, "[%s-%d]  case APPLIB_DCF_EXT_OBJECT_IMAGE_THM: Fname: %s", TmpName);
        break;
    case APPLIB_DCF_EXT_OBJECT_VIDEO_THM:
        Naming_FnumToFname(Fnum, TmpName);
        sprintf(&TmpName[DCF_FILE_TIME_LEN], "%c%c", VinChannel, MAIN_STREAM);
        DBGMSGc2(BLUE, "[%s-%d]  case APPLIB_DCF_EXT_OBJECT_VIDEO_THM: Fname: %s", TmpName);
        break;
    case APPLIB_DCF_EXT_OBJECT_SPLIT_FILE:
//        Naming_GetSplitFileName(SeqNum, Fnum, TmpName, "");
        DBGMSGc2(RED, "[%s-%d]  case APPLIB_DCF_EXT_OBJECT_SPLIT_FILE not support yet . . .");
        return STORAGE_DMF_EXT_OBJECT_SPLIT_FILE_SET_FAIL;
//        break;
    case APPLIB_DCF_EXT_OBJECT_SPLIT_THM:
//        Naming_GetSplitFileName(SeqNum, Fnum, TmpName, DCF_FILE_THM_STR);
        DBGMSGc2(RED, "[%s-%d]  case APPLIB_DCF_EXT_OBJECT_SPLIT_THM not support yet . . .");
        return STORAGE_DMF_EXT_OBJECT_SPLIT_THM_SET_FAIL;
//        break;
    default:
        DBGMSGc2(RED, "Incorrect ExtType!");
        return STORAGE_DMF_INCORRECT_EXT_FILE_TYPE;
    }
    strncpy(ObjName, DirPath, MAX_FILENAME_LENGTH);
    strcat(ObjName, "\\");
    strcat(ObjName, TmpName);
    strcat(ObjName, ".");
    strcat(ObjName, ExtName);
    DBGMSGc2(BLUE, "==[%s-%d] -END-, ObjName:%s", __FUNCTION__, __LINE__, ObjName);
    return 0;
}

/**
 * A global variable to point to functions of the Naming Rule
 */
APPLIB_DCF_NAMING_s g_AppLibDcfNaming1 = {
    Naming_IsIdValid,
    Naming_GetObjectName,
    Naming_GetExtObjectName
};

