/**
 * @file src/app/connected/applib/src/dcf/ApplibDcf_FilterDateTime.c
 *
 * Implementation of dcf filter
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

#include <wchar.h>
#include <dcf/AmpDcf.h>
#include <applib.h>
#include <dcf/ApplibDcf_FilterDateTime.h>
#include <AmbaRTC.h>

//#define DEBUG_APPLIB_DCF_FILTER_DATE_TIME
#if defined(DEBUG_APPLIB_DCF_FILTER_DATE_TIME)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GRAY,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

UINT32 g_AppLibDcfFlt_LeapYear[DCF_LEAP_YEAR_RANGE] = {16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96};
UINT32 g_AppLibDcfFlt_DaysOfMonth[11] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};


#ifdef CONFIG_APP_ARD
extern int empty_dir_count;
extern char empty_dir_group[20][MAX_FILENAME_LENGTH];
#endif

static void PerrorImpl(UINT32 nLine, char *ErrMsg)
{
    char FileName[MAX_FILENAME_LENGTH];
    strncpy(FileName, (strrchr(__FILE__, '\\') + 1), (strlen(__FILE__) - strlen((strrchr(__FILE__, '\\') + 1)) + 1));
    AmbaPrint("[Error]%s %u: %s", FileName, nLine, ErrMsg);
}

#define Perror(ErrMsg) {\
    PerrorImpl(__LINE__, ErrMsg);\
}

static BOOL AppLibFilter_GetFileType(char *Name)
{
    if ((strncmp(&Name[strlen(Name) - 3], "jpg", 3) == 0) || (strncmp(&Name[strlen(Name) - 3], "JPG", 3) == 0)) {
        return TRUE;
        DBGMSGc2(GRAY, "[%s -%d]  It is a JPG file", __FUNCTION__, __LINE__);
    } else {
        return FALSE;
    }
}

static INT8 AppLibFilter_GetCurDcfHdlr(char *Path)
{
    INT8 DcfHdlrIdx = STORAGE_DCF_HDLR_MAX - 1;
    DBGMSGc2(GRAY, "==[%s -%d] -START- DcfHdlrIdx: %d, Path:%s, %s, %s", __FUNCTION__, __LINE__, DcfHdlrIdx, Path, g_AppLibStorageRootName[0], g_AppLibStorageRootName[1]);
    while (DcfHdlrIdx >= 0) {
        if (strstr(Path, g_AppLibStorageRootName[DcfHdlrIdx]) != NULL) {
            break;
        } else {
            DcfHdlrIdx--;
        }
    }
    DBGMSGc2(GRAY, "##[%s -%d] -END- DcfHdlrIdx: %d", __FUNCTION__, __LINE__, DcfHdlrIdx);
    return DcfHdlrIdx;
}

static UINT32 AppLibFilter_GetId(UINT32 Dnum, UINT32 Fnum)
{
    const UINT32 Id = (INDEX_BASE | (Dnum << DCF_DNUM_SHIFT) | Fnum);
    DBGMSGc2(GRAY, "==[%s -%d]  -START- Dnum:%d, Fnum:%d", __FUNCTION__, __LINE__, Dnum, Fnum);
    if (Dnum < DCF_DIR_NUM_MIN) {
        DBGMSGc2(RED, "##[%s -%d] -END-, Dnum < DCF_DIR_NUM_MIN", __FUNCTION__, __LINE__);
        return 0;
    }
    if (Dnum > DCF_DIR_NUM_MAX) {
        DBGMSGc2(RED, "##[%s -%d] -END-, Dnum > DCF_DIR_NUM_MAX", __FUNCTION__, __LINE__);
        return 0;
    }
    if (Fnum < DCF_FILE_NUM_MIN) {
        DBGMSGc2(RED, "##[%s -%d] -END-, Fnum < DCF_FILE_NUM_MIN", __FUNCTION__, __LINE__);
        return 0;
    }
    if (Fnum > DCF_FILE_NUM_MAX) {
        DBGMSGc2(RED, "##[%s -%d] -END-, Fnum > DCF_FILE_NUM_MAX", __FUNCTION__, __LINE__);
        return 0;
    }
    DBGMSGc2(GRAY, "##[%s -%d] -END,- Id:%d", __FUNCTION__, __LINE__, Id);
    return Id;
}

static UINT32 AppLibFilter_IdToDnum(UINT32 Id)
{
    UINT32 num = Id & (~INDEX_BASE);  // remove INDEX_BASE
    DBGMSGc2(GRAY, "==[%s -%d]  -START- Id:%d", __FUNCTION__, __LINE__, Id);
    if ((Id >> DCF_BASE_SHIFT) != (INDEX_BASE >> DCF_BASE_SHIFT)) {
        DBGMSGc2(RED, "##[%s -%d] -END-, (Id >> DCF_BASE_SHIFT) != (INDEX_BASE >> DCF_BASE_SHIFT)", __FUNCTION__, __LINE__);
        return 0;
    }
    num = num >> DCF_DNUM_SHIFT;
    if (num < DCF_DIR_NUM_MIN) {
        DBGMSGc2(RED, "##[%s -%d] -END-, %u < DCF_DIR_NUM_MIN", __FUNCTION__, __LINE__, num);
        return 0;
    }
    if (num > DCF_DIR_NUM_MAX) {
        DBGMSGc2(RED, "##[%s -%d] -END-, %u > DCF_DIR_NUM_MAX", __FUNCTION__, __LINE__, num);
        return 0;
    }
    DBGMSGc2(GRAY, "##[%s -%d] -END-, Dnum:%d", __FUNCTION__, __LINE__, num);
    return num;
}

static UINT32 AppLibFilter_IdToFnum(UINT32 Id)
{
    UINT32 num = Id & (~INDEX_BASE);  // remove INDEX_BASE
    DBGMSGc2(GRAY, "==[%s -%d]  -START- Id:%d", __FUNCTION__, __LINE__, Id);
    if ((Id >> DCF_BASE_SHIFT) != (INDEX_BASE >> DCF_BASE_SHIFT)){
        DBGMSGc2(RED, "##[%s -%d] -END-, (Id >> DCF_BASE_SHIFT) != (INDEX_BASE >> DCF_BASE_SHIFT)", __FUNCTION__, __LINE__);
        return 0;
    }
    num = num & DCF_FNUM_MASK;
    if (num < DCF_FILE_NUM_MIN) {
        DBGMSGc2(RED, "##[%s -%d] -END-, %u < DCF_FILE_NUM_MIN", __FUNCTION__, __LINE__, num);
        return 0;
    }
    if (num > DCF_FILE_NUM_MAX) {
        DBGMSGc2(RED, "##[%s -%d] -END-, %u > DCF_FILE_NUM_MAX", __FUNCTION__, __LINE__, num);
        return 0;
    }
    DBGMSGc2(GRAY, "##[%s -%d] -END- num:%d", __FUNCTION__, __LINE__, num);
    return num;
}

static char *AppLibFilter_GetFnum(char *Name, char *pTail, UINT32 *pFnum)
{
    UINT32 num = 0;
    UINT32 fnum = 0;
    char time[DCF_FILE_PATTERN_LEN+1] = {0};
    char tmp[4] = {0};
    BOOL IsJpeg = FALSE;

    DBGMSGc2(GRAY, "==[%s -%d]  -START- Name:%s, pTail:%s, *pFnum:%d", __FUNCTION__, __LINE__, Name, pTail, *pFnum);
    // step1: check this file is JPG or MP4
    IsJpeg = AppLibFilter_GetFileType(Name);
    if (IsJpeg == TRUE) {
        DBGMSGc2(GREEN, "[%s -%d]  It is a JPG file", __FUNCTION__, __LINE__);
    }
    // step2: check file name meets digits and length meets DCF_FILE_PATTERN_LEN or not
    do {
	if (!DCF_IS_DIGIT(*pTail))
	    break;
        pTail--;
	num++;
    } while ((pTail > Name) && (!DCF_IS_DELIM(*pTail)));
//    DBGMSGc2(GRAY, "[%s] 1_num: %d", __FUNCTION__, num);
    if (num != DCF_FILE_PATTERN_LEN) {
        if (num == 0) {
            do {
//                DBGMSGc2(GRAY, "[%s] pTail: %c", __FUNCTION__, *pTail);
        	if (!DCF_IS_CHAR(*pTail))
        	    break;
                pTail--;
        	num++;
            } while ((pTail > Name) && (!DCF_IS_DELIM(*pTail)));
//            DBGMSGc2(GRAY, "[%s] 2_num: %d", __FUNCTION__, num);
            if (num == DCF_FILE_JPG_IDX_LEN) {
                do {
            	if (!DCF_IS_DIGIT(*pTail))
            	    break;
                pTail--;
            	num++;
                } while ((pTail > Name) && (!DCF_IS_DELIM(*pTail)));
//                DBGMSGc2(GRAY, "[%s] 3_num: %d", __FUNCTION__, num);
                switch (num) {
                    case DCF_FILE_PATTERN_LEN:
                        break;
                    default:
                        DBGMSGc2(RED, "##[%s] -END-, Length of file name is %d, not meets DCF_FILE_PATTERN_LEN value %d", __FUNCTION__, num, DCF_FILE_PATTERN_LEN);
                        return NULL;
                }
            } else {
        DBGMSGc2(RED, "##[%s] -END-, Length of file name is %d, not meets DCF_FILE_PATTERN_LEN value %d", __FUNCTION__, num, DCF_FILE_PATTERN_LEN);
        return NULL;
    }
        } else {
        DBGMSGc2(RED, "##[%s] -END-, Length of file name is %d, not meets DCF_FILE_PATTERN_LEN value %d", __FUNCTION__, num, DCF_FILE_PATTERN_LEN);
        return NULL;
    }
    }
    if (DCF_IS_DELIM(*pTail))
        pTail++;
    strncpy(time, pTail, DCF_FILE_PATTERN_LEN);
    //step3: prifix meets HHMMSS style or not
    // step3-1: check hour
    strncpy(tmp, time, DCF_FILE_HOUR_LEN);
    tmp[DCF_FILE_HOUR_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 0) && (num <= 23))) {
        DBGMSGc2(RED, "##[%s] -END-, Hour part of file name is %d, not in the range from 0 ~ 23", __FUNCTION__, num);
        return NULL;
    }
    fnum = (num * 3600);
    // step3-2: check minute
    strncpy(tmp, (time + DCF_FILE_HOUR_LEN), DCF_FILE_MIN_LEN);
    tmp[DCF_FILE_MIN_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 0) && (num <= 59))) {
        DBGMSGc2(RED, "##[%s] -END-, Minute part of file name is %d, not in the range from 0 ~ 59", __FUNCTION__, num);
        return NULL;
    }
    fnum += (num * 60);
    // step3-3: check second
    strncpy(tmp, (time + DCF_FILE_HOUR_LEN + DCF_FILE_MIN_LEN), DCF_FILE_SEC_LEN);
    tmp[DCF_FILE_SEC_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 0) && (num <= 59))) {
        DBGMSGc2(RED, "##[%s] -END-, Second part of file name is %d, not in the range from 0 ~ 59", __FUNCTION__, num);
        return NULL;
    }
    fnum += num;
//	DBGMSGc2(GRAY, "[%s] Fnum: %d", __FUNCTION__, fnum);
    if (IsJpeg == FALSE) {
    //step3: check VIN Channel number
        DBGMSGc2(GRAY, "[%s] VIN CHANNEL part is %c", __FUNCTION__, time[DCF_FILE_TIME_LEN]);
        if (!DCF_IS_CHAR(time[DCF_FILE_TIME_LEN])) {
            DBGMSGc2(RED, "##[%s] -END-, VIN CHANNEL part of file name is %c, not in the range from A ~ Z", __FUNCTION__, time[DCF_FILE_TIME_LEN]);
//            return NULL;
        }
        //step4: check Stream number
        DBGMSGc2(GRAY, "[%s] STREAM part is %c", __FUNCTION__, time[DCF_FILE_TIME_LEN + DCF_FILE_CH_LEN]);
        if (!DCF_IS_CHAR(time[DCF_FILE_TIME_LEN + DCF_FILE_CH_LEN])) {
            DBGMSGc2(RED, "##[%s] -END-, STREAM part of file name is %c, not in the range from A ~ Z", __FUNCTION__, time[DCF_FILE_TIME_LEN + DCF_FILE_CH_LEN]);
        return NULL;
    }
    } else {
        strncpy(tmp, (time + DCF_FILE_TIME_LEN), DCF_FILE_JPG_IDX_LEN);
        tmp[DCF_FILE_JPG_IDX_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
        if (!((num >= 0) && (num <= 99))) {
            DBGMSGc2(RED, "##[%s] -END-, JPG INDEX part of file name is %d, not in the range from 0 ~ 99", __FUNCTION__, num);
//            return NULL;
        }
    }
    *pFnum = fnum;
    pTail -= 2;
    DBGMSGc2(GRAY, "##[%s -%d] -END- pTail:%s, Fnum:%d", __FUNCTION__, __LINE__, pTail, *pFnum);
    return pTail;
}

static char *AppLibFilter_GetDnum(char *Name, char *pTail, UINT32 *pDnum)
{
    UINT32 num = 0;
    UINT32 leap = DCF_LEAP_YEAR_RANGE, dnum = 0, month = 0;
    char date[DCF_DIR_PATTERN_LEN+1] = {0};
    char tmp[4] = {0};

    DBGMSGc2(GRAY, "==[%s -%d]  -START- Name:%s, pTail:%s, *pDnum:%u", __FUNCTION__, __LINE__, Name, pTail, *pDnum);
    // step1: check folder name meets digit or not, check folder name length meets DCF_DIR_PATTERN_LEN or not
    do {
	if (!DCF_IS_DIGIT(*pTail))
	    return NULL;
        pTail--;
	num++;
    } while ((pTail > Name) && (!DCF_IS_DELIM(*pTail)));
    if (num != DCF_DIR_PATTERN_LEN) {
        DBGMSGc2(RED, "##[%s] -END-, Length of directory is %d, not meets DCF_DIR_PATTERN_LEN value %d", __FUNCTION__, num, DCF_DIR_PATTERN_LEN);
        return NULL;
    }
    //step2: prifix meets YYMMDD style or not
    if (DCF_IS_DELIM(*pTail))
        pTail++;
    strncpy(date, pTail, DCF_DIR_PATTERN_LEN);
    // step2-1: check year
    strncpy(tmp, date, DCF_DIR_YEAR_LEN);
    tmp[DCF_DIR_YEAR_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= DCF_BASE_YEAR) && (num <= 99))) {
        DBGMSGc2(RED, "##[%s] -END-, year part of directory name is %d, not in the range from 2014 ~ 2099", __FUNCTION__, num);
        return NULL;
    }
    dnum = ((num - DCF_BASE_YEAR) * 365);
    DBGMSGc2(GRAY, "[%s] year-dnum:%d", __FUNCTION__, dnum);
    // step2-2: check leap year
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
    // step2-3: check month
    strncpy(tmp, (date + DCF_DIR_YEAR_LEN), DCF_DIR_MONTH_LEN);
    tmp[DCF_DIR_MONTH_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    month = num;
    if (!((num >= 1) && (num <= 12))) {
        DBGMSGc2(RED, "##[%s] -END-, month part of directory name is %d, not in the range from 1 ~ 12", __FUNCTION__, num);
        return NULL;
    }
    num--;
    while (num) {
        dnum += g_AppLibDcfFlt_DaysOfMonth[num - 1];
        num--;
    }
    DBGMSGc2(GRAY, "[%s] month-dnum:%d", __FUNCTION__, dnum);
    if (leap > 0) {
        DBGMSGc2(GRAY, "[%s] leap year days: %d", __FUNCTION__, leap);
        if (month < 3) {
            leap--;
            DBGMSGc2(GRAY, "[%s] leap year days: %d", __FUNCTION__, leap);
        }
        dnum += leap;
    }
    DBGMSGc2(GRAY, "[%s] leap-dnum:%d", __FUNCTION__, dnum);
    // step2-4: check day
    strncpy(tmp, (date + DCF_DIR_YEAR_LEN + DCF_DIR_MONTH_LEN), DCF_DIR_DAY_LEN);
    tmp[DCF_DIR_DAY_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 1) && (num <= 31))) {
        DBGMSGc2(RED, "##[%s] -END-, day part of directory name is %d, not in the range from 1 ~ 31", __FUNCTION__, num);
        return NULL;
    }
    dnum += num;
    DBGMSGc2(GRAY, "[%s]  day-dnum:%d", __FUNCTION__, dnum);
    // step3: check it is RTC reset folder or not
    strncpy(tmp, (date + DCF_DIR_DATE_LEN), DCF_DIR_RTC_RST_LEN);
    tmp[DCF_DIR_RTC_RST_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 0) && (num <= 9))) {
        DBGMSGc2(RED, "##[%s] -END-, RTC reset part of directory name is %d, not in the range from 0 ~ 9", __FUNCTION__, num);
        return NULL;
    }
    if (dnum == 1) { // add RTC reset index if the date is 2014/01/01
        dnum += num;
    } else {
        dnum += DCF_DIR_RTC_RESET_IDX_MAX;
    }
    *pDnum = dnum;
    DBGMSGc2(GRAY, "##[%s-%d] -END- pTail:%s, *pDnum:%u", __FUNCTION__, __LINE__, pTail, *pDnum);
    return pTail;
}

static UINT32 AppLibFilter_NameAssignDnum(char* Name)
{
    char *pTail;
    char date[DCF_DIR_PATTERN_LEN+1] = {0};
    char tmp[4] = {0};
    UINT32 leap = DCF_LEAP_YEAR_RANGE, dnum = 0, month = 0;
    UINT32 num = strlen(Name);
    UINT32 DcfHdlrIdx = AppLibFilter_GetCurDcfHdlr(Name);

    DBGMSGc2(GRAY, "==[%s -%d]  -START- Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if (num < 1 + DCF_DIR_PATTERN_LEN)
        return 0;
    pTail = (Name + num - 1);
    DBGMSGc2(GRAY, "[%s] p: %s", __FUNCTION__, pTail);
    // step1: check folder name meets digit or not, check folder name length meets DCF_DIR_PATTERN_LEN or not
    num = 0;
    do {
	if (!DCF_IS_DIGIT(*pTail))
	    return 0;
        pTail--;
	num++;
    } while ((pTail > Name) && (!DCF_IS_DELIM(*pTail)));
    if (num != DCF_DIR_PATTERN_LEN) {
        DBGMSGc2(RED, "##[%s] -END-, Length of directory is %d, not meets DCF_DIR_PATTERN_LEN value %d", __FUNCTION__, num, DCF_DIR_PATTERN_LEN);
        return 0;
    }
    //step2: prifix meets YYMMDD style or not
    if (DCF_IS_DELIM(*pTail))
        pTail++;
    strncpy(date, pTail, DCF_DIR_PATTERN_LEN);
    // step2-1: check year
    strncpy(tmp, date, DCF_DIR_YEAR_LEN);
    tmp[DCF_DIR_YEAR_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= DCF_BASE_YEAR) && (num <= 99))) {
        DBGMSGc2(RED, "##[%s] -END-, year part of directory name is %d, not in the range from 2014 ~ 2099", __FUNCTION__, num);
        return 0;
    }
    dnum = ((num - DCF_BASE_YEAR) * 365);
    DBGMSGc2(GRAY, "[%s] dnum:%u, after year calculate", __FUNCTION__, dnum);
    // step2-2: check leap year
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
    // step2-3: check month
    strncpy(tmp, (date + DCF_DIR_YEAR_LEN), DCF_DIR_MONTH_LEN);
    tmp[DCF_DIR_MONTH_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    month = num;
    if (!((num >= 1) && (num <= 12))) {
        DBGMSGc2(RED, "##[%s] -END-, month part of directory name is %d, not in the range from 1 ~ 12", __FUNCTION__, num);
        return 0;
    }
    num--;
    while (num) {
        dnum += g_AppLibDcfFlt_DaysOfMonth[num - 1];
        num--;
    }
    DBGMSGc2(GRAY, "[%s] dnum:%u, after month calculate", __FUNCTION__, dnum);
    if (leap > 0) {
        DBGMSGc2(GRAY, "[%s] leap year days: %d", __FUNCTION__, leap);
        if (month < 3) {
            leap--;
            DBGMSGc2(GRAY, "[%s] leap year days: %d", __FUNCTION__, leap);
        }
        dnum += leap;
    }
    DBGMSGc2(GRAY, "[%s]  dnum:%d", __FUNCTION__, dnum);
    // step2-4: check day
    strncpy(tmp, (date + DCF_DIR_YEAR_LEN + DCF_DIR_MONTH_LEN), DCF_DIR_DAY_LEN);
    tmp[DCF_DIR_DAY_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 1) && (num <= 31))) {
        DBGMSGc2(RED, "##[%s] -END-, day part of directory name is %d, not in the range from 1 ~ 31", __FUNCTION__, num);
        return 0;
    }
    dnum += num;
    DBGMSGc2(GRAY, "[%s] dnum:%u, after day calculate", __FUNCTION__, dnum);
    // step3: check it is RTC reset folder or not
    strncpy(tmp, (date + DCF_DIR_DATE_LEN), DCF_DIR_RTC_RST_LEN);
    tmp[DCF_DIR_RTC_RST_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 0) && (num <= 9))) {
        DBGMSGc2(RED, "##[%s] -END-, RTC reset part of directory name is %d, not in the range from 0 ~ 9", __FUNCTION__, num);
        return 0;
    }
    if (dnum == 1) { // add RTC reset index if the date is 2014/01/01
        dnum += num;
        if (AppLibStorageDmf_GetRtcDirIndex(DcfHdlrIdx) < num)
            AppLibStorageDmf_SetRtcDirIndex(num, DcfHdlrIdx);
    } else {
        dnum += DCF_DIR_RTC_RESET_IDX_MAX;
    }
    DBGMSGc2(GRAY, "[%s] dnum:%u, after RTC reset check", __FUNCTION__, dnum);
    // step4: check the folder index of the date
    strncpy(tmp, (date + DCF_DIR_DATE_LEN + DCF_DIR_RTC_RST_LEN), DCF_DIR_NUM_LEN);
    tmp[DCF_DIR_NUM_LEN] = '\0';
    num = strtoul(tmp, NULL, 10);
    if (!((num >= 0) && (num <= 99))) {
        DBGMSGc2(RED, "##[%s] -END-, index part of directory name is %d, not in the range from 0 ~ 99", __FUNCTION__, num);
        return 0;
    }
    AppLibStorageDmf_SetDirIndex(dnum, num, DcfHdlrIdx);
    DBGMSGc2(GRAY, "##[%s-%d] -END- pTail:%s, Dnum:%u", __FUNCTION__, __LINE__, pTail, dnum);
    if (pTail <= Name)
        return 0;
    return dnum;
}

static UINT32 AppLibFilter_NameToDnum(char *Name)
{
    char *p;
    UINT32 dnum = 0;
    UINT32 len = strlen(Name);

    DBGMSGc2(GRAY, "==[%s -%d]  -START- Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if (len < 1 + DCF_DIR_PATTERN_LEN)
        return 0;
    p = (Name + len - 1);
    DBGMSGc2(GRAY, "[%s] p: %s", __FUNCTION__, p);
    // check directory naming form and get folder number
    p = AppLibFilter_GetDnum(Name, p, &dnum);
    if (p <= Name)
        return 0;
    DBGMSGc2(GRAY, "##[%s -%d] -END- Dnum:%d", __FUNCTION__, __LINE__, dnum);
    return dnum;
}

static UINT32 AppLibFilter_NameToId(char *Name)
{
    char *p;
    UINT32 fnum;
    UINT32 dnum;
    DBGMSGc2(GRAY, "==[%s -%d]  -START- Name:%s", __FUNCTION__, __LINE__, Name);

    K_ASSERT(Name != NULL);
    if (strlen(Name) <= (1 + DCF_DIR_PATTERN_LEN + 1 + DCF_FILE_PATTERN_LEN + 1))
        return 0;
    // reverse search  '.'
    p = (strrchr(Name, '.') - 1);
    if (p == NULL)
        return 0;
    p = AppLibFilter_GetFnum(Name, p, &fnum);
    if (p <= Name)
        return 0;
    // get folder number
    p = AppLibFilter_GetDnum(Name, p, &dnum);
    if (p == NULL)
        return 0;
    DBGMSGc2(GRAY, "##[%s -%d] -END- Dnum:%d, Fnum:%d", __FUNCTION__, __LINE__, dnum, fnum);
    return AppLibFilter_GetId(dnum, fnum);
}

static int AppLibFilter_ScanDirectory(char *Path, AMP_DCF_TABLE_HDLR_s *pTable)
{
    char pattern[MAX_FILENAME_LENGTH] = {'\0'};
    AMP_CFS_DTA dta;
    UINT32 i;
    INT8 DcfHdlrIdx = AppLibFilter_GetCurDcfHdlr(Path);

	DBGMSGc2(GRAY, "==[%s -%d]  -START-", __FUNCTION__, __LINE__);

    AppLibStorageDmf_SetCurDnum(DcfHdlrIdx, 0);
    AppLibStorageDmf_SetCurDnumIdx(DcfHdlrIdx, -1);
    strncpy(pattern, Path, MAX_FILENAME_LENGTH);
    strcat(pattern, "\\");
    for (i=0; i<DCF_DIR_PATTERN_LEN; i++)
        strcat(pattern, "?");

    if (AmpCFS_FirstDirEnt(pattern, AMP_CFS_ATTR_ALL, &dta) == AMP_OK) {
        UINT32 dnum;
        char *dn;
        char name[MAX_FILENAME_LENGTH];

        DBGMSGc2(GRAY, "\n[%s] Start to SCAN all directory in DCIM", __FUNCTION__);
        while (TRUE) {
            //get directory name
            if (dta.LongName[0] == 0)
                dn = (char *)dta.FileName;
            else
                dn = (char *)dta.LongName;
            strncpy(name, Path, MAX_FILENAME_LENGTH);
            strcat(name, "\\");
            strcat(name, dn);
            DBGMSGc2(GRAY, "[%s] Cur Dir name: %s", __FUNCTION__, name);
            if (dta.Attribute & AMP_CFS_ATTR_DIR) {
                dnum = AppLibFilter_NameAssignDnum(name);
                if (dnum > 0) {
                    if (pTable->Func->AddDirectory(pTable, dnum, name) != AMP_OK) {
                        Perror("Add directory error!");
                        return AMP_ERROR_GENERAL_ERROR;
                    }
                }
            }
            if (AmpCFS_NextDirEnt(&dta) != AMP_OK)
                break;
        }
    }
    DBGMSGc2(GRAY, "##[%s -%d] -END-", __FUNCTION__, __LINE__);
    return AMP_OK;
}

static int AppLibFilter_ScanFile(char *Path, AMP_DCF_TABLE_HDLR_s *pTable)
{
    char pattern[MAX_FILENAME_LENGTH] = {'\0'};
    AMP_CFS_DTA dta;
    UINT32 i;
    int CurDnum = 0;
    int CurDnumIdx = -1;

    DBGMSGc2(GRAY, "==[%s -%d]  -START-", __FUNCTION__, __LINE__);
    // search sub-directory
    strncpy(pattern, Path, MAX_FILENAME_LENGTH);
    strcat(pattern, "\\");
    DBGMSGc2(GRAY, "[%s] pattern: %s", __FUNCTION__, pattern);
    for (i=0; i<DCF_FILE_PATTERN_LEN; i++)
        strcat(pattern, "?");
    strcat(pattern, "*");
//	DBGMSGc2(GRAY, "[%s] pattern: %s", __FUNCTION__, pattern);

    if (AmpCFS_FirstDirEnt(pattern, (AMP_CFS_ATTR_ARCH | AMP_CFS_ATTR_RDONLY | AMP_CFS_ATTR_NONE), &dta) == AMP_OK) {
        UINT32 id, idx, rtc;
        char *fn;
        char name[MAX_FILENAME_LENGTH], tmp[4];
        BOOL IsJpeg;

		DBGMSGc2(GRAY, "\n[%s] Start to SCAN all file in current directory", __FUNCTION__);
        while (TRUE) {
            if (dta.LongName[0] == 0)
                fn = (char *)dta.FileName;
            else
                fn = (char *)dta.LongName;
            strncpy(name, Path, MAX_FILENAME_LENGTH);
            strcat(name, "\\");
            strcat(name, fn);
            IsJpeg = AppLibFilter_GetFileType(name);
            DBGMSGc2(GRAY, "[%s -%d] CUR File name: %s", __FUNCTION__, __LINE__, name);
            id = AppLibFilter_NameToId(name);
            if (id > 0) {
                INT8 DcfHdlrIdx = AppLibFilter_GetCurDcfHdlr(Path);
                const UINT32 dnum = AppLibFilter_IdToDnum(id);
                const UINT32 fnum = AppLibFilter_IdToFnum(id);
                K_ASSERT(dnum > 0);
                K_ASSERT(fnum > 0);
                strncpy(tmp, (name + 14), DCF_DIR_RTC_RST_LEN);
                tmp[DCF_DIR_RTC_RST_LEN] = '\0';
                rtc = strtoul(tmp, NULL, 10);
                strncpy(tmp, (name + 15), DCF_DIR_NUM_LEN);
                tmp[DCF_DIR_NUM_LEN] = '\0';
                idx = strtoul(tmp, NULL, 10);
                DBGMSGc2(GRAY, "[%s] Add file into DCF TABLE, dnum: %d, fnum: %d, idx:%d", __FUNCTION__, dnum, fnum, rtc, idx);

                if (pTable->Func->AddFile(pTable, dnum, fnum, name) != AMP_OK) {
//                  Perror("Add file error!");
                    DBGMSGc2(RED, "##[%s -%d] AddFile fail -END-", __FUNCTION__, __LINE__);
                    return AMP_ERROR_GENERAL_ERROR;
                }

                CurDnum = AppLibStorageDmf_GetCurDnum(DcfHdlrIdx);
                CurDnumIdx = AppLibStorageDmf_GetCurDnumIdx(DcfHdlrIdx);
                DBGMSGc2(GRAY, "[%s -%d] CurDnum: %d, CurDnumIdx: %d", __FUNCTION__, __LINE__, CurDnum, CurDnumIdx);
                // to keep the latest dnum and file amount in this directory
                if (dnum >= CurDnum) {
                    CurDnum = AppLibStorageDmf_SetCurDnum(DcfHdlrIdx, dnum);
                    DBGMSGc2(GRAY, "[%s -%d] CurDnum: %d, CurDnumIdx: %d", __FUNCTION__, __LINE__, CurDnum, CurDnumIdx);
                    if ((CurDnumIdx == -1) || (CurDnumIdx < (INT8)idx)) {
                        CurDnumIdx = AppLibStorageDmf_SetCurDnumIdx(DcfHdlrIdx, (INT8)idx);
                        AppLibStorageDmf_InitFileCountInTheDir(1, DcfHdlrIdx);
                        DBGMSGc2(GRAY, "[%s -%d] CurDnum: %d, CurDnumIdx: %d", __FUNCTION__, __LINE__, CurDnum, CurDnumIdx);
                    } else if (CurDnumIdx == (INT8)idx) {
                        if (IsJpeg == FALSE) {
                            // It is video file
                            if (name[strlen(name) - 5] == MAIN_STREAM) // only calculate main stream file for file amount  of specific folder
                                AppLibStorageDmf_AddFileCountInTheDir(DcfHdlrIdx);
                        } else {
                            // It is jpeg file
                            if (DCF_IS_DIGIT(name[strlen(name) - 5]))
                                AppLibStorageDmf_AddFileCountInTheDir(DcfHdlrIdx);
                        }
                    }
                }
            }

            if (AmpCFS_NextDirEnt(&dta) != AMP_OK)
                break;
        }
    } else {
        DBGMSGc2(RED, "[%s -%d]  No file in this directory!! Remove the %s", __FUNCTION__, __LINE__, Path);
#ifdef CONFIG_APP_ARD
		empty_dir_count++;
		strncpy(empty_dir_group[empty_dir_count-1], Path, MAX_FILENAME_LENGTH);

#else
    	AmpCFS_Rmdir(Path);
#endif
    }
    DBGMSGc2(GRAY, "##[%s -%d]  -END-", __FUNCTION__, __LINE__);
    return AMP_OK;
}

AMP_DCF_FILTER_s g_AppLibDcfFlt1 = {
    AppLibFilter_NameToDnum,
    AppLibFilter_NameToId,
    AppLibFilter_GetId,
    AppLibFilter_IdToDnum,
    AppLibFilter_IdToFnum,
    AppLibFilter_ScanDirectory,
    AppLibFilter_ScanFile
};

