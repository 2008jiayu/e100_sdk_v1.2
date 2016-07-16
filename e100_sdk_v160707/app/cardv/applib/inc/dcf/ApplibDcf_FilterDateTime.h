/**
 * @file src/app/connected/applib/src/dcf/ApplibDcf_FilterDateTime.h
 *
 * Header of DCF filter
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

#ifndef APPLIB_DCF_FILTER_DATE_TIME_H_
#define APPLIB_DCF_FILTER_DATE_TIME_H_
/**
 * DCF Filter
 */
/**
 * The u32 difinition for index
 * base: bit 31~30 (magic number, to avoid app passes an incorrect id)
 * dnum: bit 29~17 (dnum <= 20*365 + 5 + 9, so 1FFF is enough)
 * fnum: bit 16~0 (fnum range is 24*60*60, so 3FFFF is enough)
 *  ---------------------------
 *  | base  |  dnum  |  fnum  |
 *  ---------------------------
 *  | 2bits  | 13bits  | 17bits |
 *  ---------------------------
 */
#define INDEX_BASE          0x40000000    /* File index basement */
#define DCF_BASE_MASK       0x00000003
#define DCF_DNUM_MASK       0x00001FFF
#define DCF_FNUM_MASK       0x0001FFFF
#define DCF_BASE_SHIFT      30
#define DCF_DNUM_SHIFT      17

/**
 * Base date
 * The base date is 2014/01/01
**/
#define DCF_BASE_YEAR   14
#define DCF_BASE_MONTH   1
#define DCF_BASE_DAY   1
#define DCF_BASE_LEAP_YEAR   16 // leap year start from 2016
#define DCF_LEAP_YEAR_RANGE 21

/**
 * Directory (Folder) naming rule is:
 * [YYMMDD][Count for RTC reset][Count for a day]
 * [YYMMDD]: 2 digits for the year, 2 digits for the month, and 2 digits for the day.
 * [Count for RTC reset]: RTC is started at 00:00:00 on 2014/01/01. It may have the same YYMMDD when it has no battery to keep RTC tick,
 *                                     so it needs to distinguish it. Range of this element is 1 ~ 9. The element should be 0 when RTC is working with battery.
 * [Count for a day]: The file amount of the Directory (Folder) is 1000. It may generate more than 1000 files a day and needs to distinguish it.
 *                              Range of this element is 0 ~ 99.
 * Ex: If RTC time is 10:25:30 - 2015/01/20 and no file has been created in this day. Directory name should be: [150120][0][00].
 *       If RTC time is 00:00:30 - 2014/01/01 and it has a directory for the same date. Directory name should be: [140101][2][00].
**/
#define DCF_DIR_PFX_LEN     0
#define DCF_DIR_YEAR_LEN     2
#define DCF_DIR_MONTH_LEN     2
#define DCF_DIR_DAY_LEN     2
#define DCF_DIR_RTC_RST_LEN     1
#define DCF_DIR_NUM_LEN     2
#define DCF_DIR_DATE_LEN     (DCF_DIR_YEAR_LEN + DCF_DIR_MONTH_LEN + DCF_DIR_DAY_LEN) // 6
#define DCF_DIR_PATTERN_LEN     (DCF_DIR_DATE_LEN + DCF_DIR_RTC_RST_LEN + DCF_DIR_NUM_LEN) // 9
/**
 * Directory (Folder) index range for daily
**/
#define DCF_DIR_NUM_MIN     1
#define DCF_DIR_NUM_MAX     (20*365+5+9)    // 25 years * 365 days + 6 leap year days + 9 RTC reset count
/**
 * Range of Directory (Folder) amount for a day
**/
#define DCF_DIR_AMOUNT_MIN     0
#define DCF_DIR_AMOUNT_MAX     99
/**
 * File naming rule is:
 * [HHMMSS][VIN channel index][Stream index]
 * [YYMMDD]: 2 digits for the hour, 2 digits for the minute, and 2 digits for the second.
 * [VIN channel index]: 1: Near part sensor, 2: #1 rear part sensor, 3: #2 rear part sensor, etc.
 * [Stream index]: 0: Main stream, 1: 2nd stream, 2: 3rd stream, etc.
 * Ex: If RTC time is 10:25:30 and VIN comes form near part sensor and main stream, the file name should be 10253010
**/
#define DCF_FILE_HOUR_LEN     2
#define DCF_FILE_MIN_LEN     2
#define DCF_FILE_SEC_LEN     2
#define DCF_FILE_CH_LEN     1
#define DCF_FILE_STREAM_LEN     1
#define DCF_FILE_JPG_IDX_LEN    2
#define DCF_FILE_TIME_LEN     (DCF_FILE_HOUR_LEN + DCF_FILE_MIN_LEN + DCF_FILE_SEC_LEN)
#define DCF_FILE_PATTERN_LEN     (DCF_FILE_TIME_LEN + DCF_FILE_CH_LEN + DCF_FILE_STREAM_LEN)
/**
 * File index range for daily (It has 86400 seconds a day)
**/
#define DCF_FILE_NUM_MIN    0
#define DCF_FILE_NUM_MAX    86399     // hard limit: 86399, it has 84000 seconds a day
/**
 * Range of file amount in a directory (folder)
**/
#define DCF_FILE_AMOUNT_MIN     1
#define DCF_FILE_AMOUNT_MAX     1000
/**
 * Range of Directory (Folder) for RTC reset
**/
#define DCF_DIR_RTC_RESET_IDX_MIN    1
#define DCF_DIR_RTC_RESET_IDX_MAX    9

#define DCF_SEQ_NUM_LEN     2
#define DCF_SEQ_NUM_MAX     99

#define DCF_PATH_DELIMITER  '\\'

#define DCF_IS_DELIM(x) ((x == '\\') || (x == '/'))
#define DCF_IS_DIGIT(x) ((x >= '0')&&(x <= '9'))
#define DCF_IS_CHAR(x)  (((x >= 'a')&&(x <= 'z')) || ((x >= 'A')&&(x <= 'Z')))
/**
 * VIN sensor index
**/
typedef enum APPLIB_DCF_VIN_CH_e_ {
    VIN_MAIN_SENSOR  = 'A',
    VIN_REAR_SENSOR_1,
    VIN_REAR_SENSOR_2,
    VIN_REAR_SENSOR_3,
} APPLIB_DCF_VIN_CH_e;
/**
 * Stream index
**/
typedef enum APPLIB_DCF_STREAM_e_ {
    MAIN_STREAM  = 'A',
    SEC_STREAM,
    THIRD_STREAM,
} APPLIB_DCF_STREAM_e;

typedef struct APPLIB_DCF_FILTER_DIR_s_ {
    char Dname[12];
    UINT32 Dnum;
	UINT32 Famount; // File amount in specific directory. MAX value is DCF_FILE_AMOUNT_MAX
} APPLIB_DCF_FILTER_DIR_s;

typedef struct APPLIB_DCF_FILTER_DIR_TABLE_s_ {
	UINT32 Count;
    APPLIB_DCF_FILTER_DIR_s DirTbl[100];
}APPLIB_DCF_FILTER_DIR_TABLE_s;

typedef struct APPLIB_DCF_FILTER_DIR_IDX_s_ {
    char Dname[12]; // Specific directory name. It include "YYMMDDX", which YY:Year, MM:Month, DD:Day, X: RTC reset times. Ex: 140101 1
    UINT32 DIdx; // Index of specific directory prefix. Ex: 140101 1 23, which "23" means the 24th directory of 1401011 prefix directory.
}APPLIB_DCF_FILTER_DIR_IDX_s;

typedef struct APPLIB_DCF_FILTER_DIR_IDX_TABLE_s_ {
	UINT32 Count;
    APPLIB_DCF_FILTER_DIR_IDX_s DirTbl[100];
}APPLIB_DCF_FILTER_DIR_IDX_TABLE_s;

extern AMP_DCF_FILTER_s g_AppLibDcfFlt1;
extern UINT32 g_AppLibDcfFlt_LeapYear[DCF_LEAP_YEAR_RANGE];
extern UINT32 g_AppLibDcfFlt_DaysOfMonth[11];
/**
 * @}
 */
#endif /* APPLIB_DCF_FILTER_DATE_TIME_H_ */
