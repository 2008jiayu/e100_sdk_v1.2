/**
 * @file src/app/connected/applib/src/format/ApplibFormat.c
 *
 * Implementation of MW format utility
 *
 * History:
 *    2013/09/03 - [Martin Lai] created file
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

#include <applib.h>
#include <format/Muxer.h>
#include <format/Demuxer.h>
#include <format/Mp4Mux.h>
#include <format/ExifMux.h>
#include <stream/File.h>
#include <ctype.h>
#include <wctype.h>
#include <AmbaUtility.h>
#include <AmbaCache_Def.h>
#include "../AppLibTask_Priority.h"

//#define USE_EXT_DMX
#ifdef USE_EXT_DMX
#include <format/ExtDmx.h>
#else
#include <format/Mp4Dmx.h>
#include <format/MovDmx.h>
#include <format/ExifDmx.h>
#endif

//#define DEBUG_APPLIB_FORMAT
#if defined(DEBUG_APPLIB_FORMAT)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static int ApplibFormatInitFlag = -1;
static int ApplibFormatMuxerInitFlag = -1;
static int ApplibFormatDeMuxerInitFlag = -1;
static int ApplibFormatMuxerMp4InitFlag = -1;
static int ApplibFormatExifMuxInitFlag = -1;
static UINT32 ApplibFormatDualFileSaving = 0;
static UINT32 ApplibFormatPriStreamFileSizeAlignment = 0;
static UINT32 ApplibFormatSecStreamFileSizeAlignment = 0;

#define INDEX_BUFFER_SIZE  (1<<20) /**< 1MB */

/**
 *  @brief Initialize the format module
 *
 *  Initialize the format module
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_Init(void)
{
    int ReturnValue = -1;
    DBGMSG("[Applib - Format] <Init> start");

    if (ApplibFormatInitFlag == 0) {
        return 0;
    }
    /* Init format  */
    {
        AMP_FORMAT_INIT_CFG_s FmtInitCfg;
        void *FmtBuf = NULL, *FmtBufRaw = NULL;
        memset(&FmtInitCfg, 0x0, sizeof(AMP_FORMAT_INIT_CFG_s));
        AmpFormat_GetInitDefaultCfg(&FmtInitCfg);
        DBGMSGc2(GREEN,"[Applib - Format] <AppLibFormat_Init> FmtInitCfg.MaxMovie = %d, FmtInitCfg= 0x%x", FmtInitCfg.MaxMovie, FmtInitCfg.BufferSize);
        FmtInitCfg.BufferSize = AmpFormat_GetRequiredBufferSize(FmtInitCfg.MaxMovie, FmtInitCfg.MaxImage, FmtInitCfg.MaxSound);
        DBGMSGc2(GREEN,"[Applib - Format] <AppLibFormat_Init> FmtInitCfg.MaxMovie = %d, FmtInitCfg= 0x%x", FmtInitCfg.MaxMovie, FmtInitCfg.BufferSize);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&FmtBuf, &FmtBufRaw, FmtInitCfg.BufferSize, AMBA_CACHE_LINE_SIZE);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Init> %s:%u", __FUNCTION__, __LINE__);
            return ReturnValue;
        }
        FmtInitCfg.Buffer = FmtBuf;
        ReturnValue = AmpFormat_Init(&FmtInitCfg);
        if (ReturnValue<0) {
            AmbaPrintColor(RED,"[Applib - Format] <Init> %s fail ReturnValue = %d", __FUNCTION__,ReturnValue);
        }
    }

    /* Init Index Buffer Pool */
    {
        AMP_INDEX_INIT_CFG_s IndexBufCfg;
        void *Mp4IndexBuf = NULL, *Mp4IndexBufRaw = NULL;
        memset(&IndexBufCfg, 0x0, sizeof(AMP_INDEX_INIT_CFG_s));
        AmpIndex_GetInitDefaultCfg(&IndexBufCfg);
        IndexBufCfg.BufferSize = INDEX_BUFFER_SIZE;
        IndexBufCfg.MaxHdlr = 8;
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &Mp4IndexBuf, &Mp4IndexBufRaw, IndexBufCfg.BufferSize, AMBA_CACHE_LINE_SIZE);
        if (ReturnValue < 0 ) {
            AmbaPrintColor(RED,"[Applib - Format] <Init> Index Buf Allocate Error %s:%u", __FUNCTION__, __LINE__);
            return ReturnValue;
        } else {
            IndexBufCfg.Buffer = (UINT8 *)Mp4IndexBuf;
        }
        if (AmpIndex_Init(&IndexBufCfg) != AMP_OK)
            AmbaPrintColor(RED,"[Applib - Format] <Init> %s:%u", __FUNCTION__, __LINE__);
    }
    /* Init file stream */
    {
        AMP_FILE_STREAM_INIT_CFG_s FileInitCfg;
        void *FileBuf = NULL, *FileBufRaw = NULL;
        memset(&FileInitCfg, 0x0, sizeof(AMP_FILE_STREAM_INIT_CFG_s));

        AmpFileStream_GetInitDefaultCfg(&FileInitCfg);
        FileInitCfg.MaxHdlr = 8;
        FileInitCfg.BufferSize = AmpFileStream_GetRequiredBufferSize(FileInitCfg.MaxHdlr);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &FileBuf, &FileBufRaw, FileInitCfg.BufferSize,  AMBA_CACHE_LINE_SIZE);
        if (ReturnValue < 0 ) {
            AmbaPrintColor(RED,"[Applib - Format] <Init> Buf Allocate Error %s:%u", __FUNCTION__, __LINE__);
            return ReturnValue;
        }
        FileInitCfg.Buffer = FileBuf;
        ReturnValue = AmpFileStream_Init(&FileInitCfg);

        if (ReturnValue<0) {
            AmbaPrintColor(RED,"[Applib - Format] <Init> %s fail ReturnValue = %d", __FUNCTION__,ReturnValue);
        }
    }
    ApplibFormatInitFlag = 0;

    DBGMSG("[Applib - Format] <Init> end");
    return ReturnValue;
}


/**
 *  @brief Initialize the muxer.
 *
 *  Initialize the muxer.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_MuxerInit(void)
{
    int ReturnValue = 0;
    if (ApplibFormatMuxerInitFlag == 0) {
        return 0;
    }
    DBGMSG("[Applib - Format] <MuxerInit> start");
    AppLibFormat_Init();
    /* Init muxer */
    {
        AMP_MUXER_INIT_CFG_s MuxerInitCfg;
        void *MuxerBuf, *MuxerBufRaw;
        memset(&MuxerInitCfg, 0x0, sizeof(AMP_MUXER_INIT_CFG_s));
        AmpMuxer_GetInitDefaultCfg(&MuxerInitCfg);
        MuxerInitCfg.MaxPipe = 4;
        AmbaPrintColor(GREEN,"[Applib - Format] <MuxerInit> MuxerInitCfg.MaxPipe = %d, BufferSize=0x%x", MuxerInitCfg.MaxPipe,  MuxerInitCfg.BufferSize);
        MuxerInitCfg.BufferSize = AmpMuxer_GetRequiredBufferSize(MuxerInitCfg.MaxPipe, MuxerInitCfg.MaxTask, MuxerInitCfg.TaskInfo.StackSize);
        AmbaPrintColor(GREEN,"[Applib - Format] <MuxerInit> MuxerInitCfg.MaxPipe = %d, BufferSize=0x%x", MuxerInitCfg.MaxPipe,  MuxerInitCfg.BufferSize);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &MuxerBuf, &MuxerBufRaw, MuxerInitCfg.BufferSize,  AMBA_CACHE_LINE_SIZE);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <MuxerInit> %s:%u", __FUNCTION__, __LINE__);
            return ReturnValue;
        }
        MuxerInitCfg.Buffer = MuxerBuf;
        MuxerInitCfg.TaskInfo.Priority = APPLIB_MUXER_TASK_PRIORITY;
        ReturnValue = AmpMuxer_Init(&MuxerInitCfg);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <MuxerInit> %s fail ReturnValue = %d", __FUNCTION__,ReturnValue);
            return -1;
        }
    }

    /* Initialize index file */
    AppLibIndex_Init();
#ifdef CONFIG_APP_ARD
    AppLibFormatCreateMux();
#endif
    ApplibFormatMuxerInitFlag = 0;
    DBGMSG("AppLibFormat_MuxerInit end");
    return ReturnValue;
}

/**
 *  @brief Initialize demuxer
 *
 *  Initialize demuxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_DemuxerInit(void)
{
    int ReturnValue = 0;
    DBGMSG("[Applib - Format] <DemuxerInit> start");
    if (ApplibFormatDeMuxerInitFlag == 0) {
        return 0;
    }
    AppLibFormat_Init();
    /* Initial Demuxer */
    {
        AMP_DEMUXER_INIT_CFG_s DemuxerInitCfg;
        void *DemuxerBuf = NULL, *DemuxerBufRaw = NULL;

        AmpDemuxer_GetInitDefaultCfg(&DemuxerInitCfg);
        AmbaPrintColor(GREEN,"[Applib - Format] <DemuxerInit> DemuxerInitCfg.MaxPipe = %d, DemuxerInitCfg= 0x%x", DemuxerInitCfg.MaxPipe, DemuxerInitCfg.BufferSize);
        DemuxerInitCfg.BufferSize = AmpDemuxer_GetRequiredBufferSize(DemuxerInitCfg.MaxPipe, DemuxerInitCfg.MaxTask, DemuxerInitCfg.TaskInfo.StackSize);
        AmbaPrintColor(GREEN,"[Applib - Format] <DemuxerInit> DemuxerInitCfg.MaxPipe = %d, DemuxerInitCfg= 0x%x", DemuxerInitCfg.MaxPipe, DemuxerInitCfg.BufferSize);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &DemuxerBuf, &DemuxerBufRaw, DemuxerInitCfg.BufferSize,  AMBA_CACHE_LINE_SIZE);
        if (ReturnValue < 0 ) {
            AmbaPrintColor(RED,"[Applib - Format] <DemuxerInit> %s:%u", __FUNCTION__, __LINE__);
            return -1;
        }
        DemuxerInitCfg.Buffer = DemuxerBuf;
        DemuxerInitCfg.TaskInfo.Priority = APPLIB_DEMUXER_TASK_PRIORITY;
        AmpDemuxer_Init(&DemuxerInitCfg);
    }

    ApplibFormatDeMuxerInitFlag = 0;
    DBGMSG("[Applib - Format] <DemuxerInit> End");
    return ReturnValue;
}


/**
 *  @brief The initialization of mp4 muxer.
 *
 *  The initialization of mp4 muxer.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_Mp4MuxerInit(void)
{
    /* MP4 Muxer Init */
    int ReturnValue = 0;
    AMP_MP4_MUX_INIT_CFG_s Mp4MuxInitCfg;
    void *Mp4Buf = NULL, *Mp4BufRaw = NULL;
    if (ApplibFormatMuxerMp4InitFlag == 0) {
        return 0;
    }
    memset(&Mp4MuxInitCfg, 0x0, sizeof(AMP_MP4_MUX_INIT_CFG_s));
    AmpMp4Mux_GetInitDefaultCfg(&Mp4MuxInitCfg);
    Mp4MuxInitCfg.MaxHdlr = 4;
    Mp4MuxInitCfg.BufferSize = AmpMp4Mux_GetRequiredBufferSize(Mp4MuxInitCfg.MaxHdlr);
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &Mp4Buf, &Mp4BufRaw, Mp4MuxInitCfg.BufferSize, 32);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Format] <MuxerInit> %s:%u", __FUNCTION__, __LINE__);
        return ReturnValue;
    }
    Mp4MuxInitCfg.Buffer = Mp4Buf;
    if (AmpMp4Mux_Init(&Mp4MuxInitCfg) != AMP_OK) {
        AmbaPrintColor(RED,"[Applib - Format] <MuxerInit> %s:%u", __FUNCTION__, __LINE__);
        AmbaKAL_BytePoolFree(Mp4BufRaw);
        return -1;
    }
    ApplibFormatMuxerMp4InitFlag = 0;
    return 0;
}

/**
 *  @brief The initialization of exif muxer.
 *
 *  The initialization of exif muxer.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_ExifMuxerInit(void)
{
    int ReturnValue = 0;
    if (ApplibFormatExifMuxInitFlag == -1) {
        AMP_EXIF_MUX_INIT_CFG_s ExifInitCfg;
        void *ExifBuf = NULL, *ExifBufRaw = NULL;

        AmpExifMux_GetInitDefaultCfg(&ExifInitCfg);
        ExifInitCfg.BufferSize = AmpExifMux_GetRequiredBufferSize(ExifInitCfg.MaxHdlr, ExifInitCfg.ExifHeadSize);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &ExifBuf, &ExifBufRaw, ExifInitCfg.BufferSize, 32);
        if (ReturnValue >= 0) {
            ExifInitCfg.Buffer = (UINT8 *)ExifBuf;
            ReturnValue = AmpExifMux_Init(&ExifInitCfg);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED, "[Applib - Format] <MuxExifInit> Init fail in line %u", __LINE__);
                AmbaKAL_BytePoolFree(ExifBufRaw);
            } else {
                ApplibFormatExifMuxInitFlag = 0;
            }
        } else {
            AmbaPrintColor(RED, "[Applib - Format] <MuxExifInit> Allocate byte fail in line %u", __LINE__);
            ReturnValue = -1;
        }
    }

    return ReturnValue;
}


/**
 *  @brief Get file format
 *
 *  Get file format based on filen ame extension.
 *
 *  @return enumeration of file format
 */
APPLIB_FILE_FORMAT_e AppLibFormat_GetFileFormat(const char* fn)
{
#define C99_MODE /** wchar.h and wctype.h are available */
#ifdef  C99_MODE
    APPLIB_FILE_FORMAT_e ReturnValue = APPLIB_FILE_FORMAT_UNKNOWN;
    char fileNameTemp[MAX_FILENAME_LENGTH] = {0}; // A temporary space storing filename
    char *fileExt; // Address of extension within the array "fileNameTemp".
    UINT32 t = 0; // Index to parse the extension
    UINT32 fileNameLength = 0; // Length of filename for preliminary check

#define CHAR_STRING_EOS    ('\0') /** End of a wide-character string */
#define RVAL_STRING_EQUAL   (0)     /** Return value (from strcmp) indicating that both strings are equal */
#define RVAL_CHAR_NOT_FOUND (NULL)  /** Return value (from strrchr) indicating that the character is not found */

    DBGMSG("[Applib - Format] <GetFileFormat> start");

    // Step 0: Preliminary Check
    // Get wide string length (excluding end of string)
    fileNameLength = strlen(fn);
    // Check maximum length
    if (fileNameLength >= MAX_FILENAME_LENGTH) {    // Recall that the length doesn't include EOS, hence the case
                                                    // "fileNameLength = MAX_FILENAME_LENGTH" leads to overflow.
        AmbaPrintColor(RED,"[Applib - Format] <GetFileFormat> %s:%u Filename overflow", __FUNCTION__, __LINE__);
        DBGMSG("[Applib - Format] <GetFileFormat> End");
        return APPLIB_FILE_FORMAT_UNKNOWN;
    }

    // Step 1: Locate filename extension
    // Copy filename to a temporary space
    strcpy(fileNameTemp, fn);
    // "strrchr" searches backwards through the filename to find the last '.' character in name
    fileExt = strrchr(fileNameTemp, '.');
    if (fileExt != RVAL_CHAR_NOT_FOUND) {
        // Skip the '.' character
        ++fileExt;
        // Convert the extension to lower case
        for (t = 0; t < MAX_FILENAME_LENGTH; ++t) {
            if (fileExt[t] == CHAR_STRING_EOS) {
                break;
            }
            // Convert an upper-case wide character to lower case
            fileExt[t] = towlower(fileExt[t]);
        }

        // Step 2: Identify lower-case file format
        // Media type: Movie
        if (strcmp(fileExt, "mp4") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_MP4;
        } else if (strcmp(fileExt, "mov") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_MOV;
        // Media type: Image
        } else if (strcmp(fileExt, "jpg") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_JPG;
        } else if (strcmp(fileExt, "jpeg") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_JPG;
        } else if (strcmp(fileExt, "thm") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_THM;
        // Media type: Sound
        } else if (strcmp(fileExt, "mp3") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_MP3;
        } else if (strcmp(fileExt, "aac") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_AAC;
        } else if (strcmp(fileExt, "wav") == RVAL_STRING_EQUAL) {
            ReturnValue = APPLIB_FILE_FORMAT_WAV;
        // Media type: Unknown
        } else {
            ReturnValue = APPLIB_FILE_FORMAT_UNKNOWN;
        }
    }
    else { // Filename doesn't have an extension
        ReturnValue = APPLIB_FILE_FORMAT_UNKNOWN;
    }

    // Step 3: Return result
    AmbaPrint("[Applib - Format] <GetFileFormat> End");
    return ReturnValue;
#else
    // Warning: This function may not work when the filename contains non-ASCII character.
    APPLIB_FILE_FORMAT_e ReturnValue = APPLIB_FILE_FORMAT_UNKNOWN;
    char fileNameAscii[MAX_FILENAME_LENGTH] = {0}; // A space storing ASCII filename
    char *fileExtAscii; // Address of extension within the array "fileNameAscii".
    UINT32 t = 0; // Index to parse the extension
    UINT32 fileNameLength = 0; // Length of filename for preliminary check

#define CHAR_STRING_EOS     ('\0')  /** End of a string */
#define RVAL_STRING_EQUAL   (0)     /** Return value (from w_strcmp) indicating that both strings are equal */
#define RVAL_CHAR_NOT_FOUND (NULL)  /** Return value (from strrchr) indicating that the character is not found */

    DBGMSG("[Applib - Format] <GetFileFormat> start");

    // Step 0: Preliminary Check
    // Convert Unicode file name to ASCII
    //AmbaUtility_Unicode2Ascii(fn, fileNameAscii);
    // Get string length (excluding end of string)
    fileNameLength = strlen(fn);
    // Check maximum length
    if (fileNameLength >= MAX_FILENAME_LENGTH) {    // Recall that the length doesn't include EOS, hence the case
                                                    // "fileNameLength = MAX_FILENAME_LENGTH" leads to overflow.
        AmbaPrintColor(RED,"[Applib - Format] <GetFileFormat> %s:%u Filename overflow", __FUNCTION__, __LINE__);
        DBGMSG("[Applib - Format] <GetFileFormat> End");
        return APPLIB_FILE_FORMAT_UNKNOWN;
    }

    // Step 1: Locate filename extension
    // "strrchr" searches backwards through the filename to find the last '.' character in name
    fileExtAscii = strrchr(fn, '.');
    if (fileExtAscii != RVAL_CHAR_NOT_FOUND) {
        // Skip the '.' character
        ++fileExtAscii;
        // Convert the extension to lower case
        for (t = 0; t < MAX_FILENAME_LENGTH; ++t) {
            if (fileExtAscii[t] == CHAR_STRING_EOS) {
                break;
            }
            // Convert an upper-case wide character to lower case
            fileExtAscii[t] = tolower(fileExtAscii[t]);
        }

        // Step 2: Identify lower-case file format
        // Media type: Movie
        if (strcmp(fileExtAscii, "mp4") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_MP4;
        } else if (strcmp(fileExtAscii, "mov") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_MOV;
        // Media type: Image
        } else if (strcmp(fileExtAscii, "jpg") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_JPG;
        } else if (strcmp(fileExtAscii, "jpeg") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_JPG;
        } else if (strcmp(fileExtAscii, "thm") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_THM;
        // Media type: Sound
        } else if (strcmp(fileExtAscii, "mp3") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_MP3;
        } else if (strcmp(fileExtAscii, "aac") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_AAC;
        } else if (strcmp(fileExtAscii, "wav") == 0) {
            ReturnValue = APPLIB_FILE_FORMAT_WAV;
        // Media type: Unknown
        } else {
            ReturnValue = APPLIB_FILE_FORMAT_UNKNOWN;
        }
    }
    else { // Filename doesn't have extension
        ReturnValue = APPLIB_FILE_FORMAT_UNKNOWN;
    }

    // Step 3: Return result
    DBGMSG("[Applib - Format] <GetFileFormat> End");
    return ReturnValue;
#endif
}

/**
 *  @brief Get media info
 *
 *  Determine the type of media info (video, image, or sound) by filename extension.
 *
 *  @return execution result
 */
int AppLibFormat_GetMediaInfo(char *fn, APPLIB_MEDIA_INFO_s *mediaInfo)
{
    int ReturnValue = AMP_OK;
    APPLIB_FILE_FORMAT_e FileFormat;
    AMP_STREAM_HDLR_s *IOStream = NULL;
    UINT8 IOStreamIsOpen = 0; // Whether the IOStream handler has opened a file successsfully and has not closed it.

    DBGMSG("[Applib - Format] <GetMediaInfo> start");
    {
        DBGMSGc2(GREEN, "[Applib - Format] <GetMediaInfo> File %s ", fn);
    }

    // Get file format
    FileFormat = AppLibFormat_GetFileFormat(fn);
    if (FileFormat == APPLIB_FILE_FORMAT_UNKNOWN) {
        AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to get file format", __FUNCTION__, __LINE__);
        ReturnValue = AMP_ERROR_GENERAL_ERROR;
        goto ReturnResult;
    }

    // Open file stream
    {
        AMP_FILE_STREAM_CFG_s FileCfg;
        // Get default file stream configuration
        AmpFileStream_GetDefaultCfg(&FileCfg);
        FileCfg.Async = FALSE; // MUST set Async to FALSE
        // Create file stream
        ReturnValue = AmpFileStream_Create(&FileCfg, &IOStream);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to create file stream (%d)!", __FUNCTION__, __LINE__, ReturnValue);
            ReturnValue = AMP_ERROR_GENERAL_ERROR;
            goto ReturnResult;
        }
        ReturnValue = IOStream->Func->Open(IOStream, fn, AMP_STREAM_MODE_RDONLY);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to open file stream (%d)!", __FUNCTION__, __LINE__, ReturnValue);
            IOStreamIsOpen = 0;
            ReturnValue = AMP_ERROR_GENERAL_ERROR;
            goto ReturnResult;
        }
        IOStreamIsOpen = 1;
    }

    // Set media type
    mediaInfo->MediaInfoType = APPLIB_GET_MEDIA_TYPE(FileFormat);

    // Get media info
    {
        AMP_DMX_FORMAT_PARSE_FP ParseFunction = NULL;
        switch (FileFormat) {
            // MediaType: Movie
            case APPLIB_FILE_FORMAT_MP4:
#ifdef USE_EXT_DMX
                // Open Media info (extdmx)
                ParseFunction = AmpExtDmx_Parse;
#else
                // Open Media info (Mp4dmx)
                ParseFunction = AmpMp4Dmx_Parse;
#endif
                break;
            case APPLIB_FILE_FORMAT_MOV:
#ifdef USE_EXT_DMX
                // Open Media info (extdmx)
                ParseFunction = AmpExtDmx_Parse;
#else
                // Open Media info (Movdmx)
                ParseFunction = AmpMovDmx_Parse;
#endif
                break;
            // MediaType: Image
            case APPLIB_FILE_FORMAT_JPG:
            case APPLIB_FILE_FORMAT_THM:
#ifdef USE_EXT_DMX
                // Open media info (extdmx)
                ParseFunction = AmpExtDmx_Parse;
#else
                // Open media info (exifdmx)
                ParseFunction = AmpExifDmx_Parse;
#endif
                break;
            // MediaType: Sound
            // TODO: Need functions to parse sound
//        case APPLIB_FILE_FORMAT_MP3:
//        case APPLIB_FILE_FORMAT_AAC:
//        case APPLIB_FILE_FORMAT_WAV:
//#ifdef USE_EXT_DMX
//                // Open media info (extdmx)
//                ParseFunction = AmpExtDmx_Parse;
//#else
//                // Open media info
//                ParseFunction = XXX_Parse;
//#endif
//            break;
            default:
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u File type is not supported (%d)", __FUNCTION__, __LINE__, FileFormat);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult;
                break;
        }

        // Get media info
        switch (mediaInfo->MediaInfoType) {
            case AMP_MEDIA_INFO_MOVIE:
                ReturnValue = AmpFormat_GetMovieInfo(fn, ParseFunction, IOStream, &(mediaInfo->MediaInfo.Movie));
                break;
            case AMP_MEDIA_INFO_IMAGE:
                ReturnValue = AmpFormat_GetImageInfo(fn, ParseFunction, IOStream, &(mediaInfo->MediaInfo.Image));
                break;
            case AMP_MEDIA_INFO_SOUND:
                ReturnValue = AmpFormat_GetSoundInfo(fn, ParseFunction, IOStream, &(mediaInfo->MediaInfo.Sound));
                break;
            default:
                // Do nothing
                break;
        }

        // Check result
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <GetMovieInfo> %s:%u Failed to get media info (Format = %d, Result = %d)", __FUNCTION__, __LINE__, FileFormat, ReturnValue);
            goto ReturnResult; // Don't release media info. It's not valid.
        }
    }

    // Check validity of "AmpFormat_GetMovieInfo", "AmpFormat_GetImageInfo", or "AmpFormat_GetSoundInfo"
    // Return value of AmpFormat_GetMovieInfo will be AMP_OK even if the media info is invalid.
    switch (mediaInfo->MediaInfoType) {
        case AMP_MEDIA_INFO_MOVIE:
            if (mediaInfo->MediaInfo.Movie == NULL) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Media info is NULL", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult; // Don't release media info. The address is invalid.
            }
            if (mediaInfo->MediaInfo.Movie->TrackCount == 0) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Track is 0", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult_RelMediaInfo;
            }
            if (mediaInfo->MediaInfo.Movie->Valid == 0) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Media info is invalid", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult_RelMediaInfo;
            }
            break;
        case AMP_MEDIA_INFO_IMAGE:
            if (mediaInfo->MediaInfo.Image == NULL) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Media info is NULL", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult; // Don't release media info. The address is invalid.
            }
            if (mediaInfo->MediaInfo.Image->Valid == 0) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Media info is invalid", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult_RelMediaInfo;
            }
            break;
        case AMP_MEDIA_INFO_SOUND:
            if (mediaInfo->MediaInfo.Sound == NULL) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Media info is NULL", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult; // Don't release media info. The address is invalid.
            }
            if (mediaInfo->MediaInfo.Sound->Valid == 0) { // Invalid. Return Error.
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Media info is invalid", __FUNCTION__, __LINE__);
                ReturnValue = AMP_ERROR_GENERAL_ERROR;
                goto ReturnResult_RelMediaInfo;
            }
            break;
        default:
            // Do nothing
            break;
    }

ReturnResult_RelMediaInfo:
    {
        UINT8 IsRemove = (ReturnValue == AMP_OK) ? (0) : (1);
        switch (mediaInfo->MediaInfoType) {
            case AMP_MEDIA_INFO_MOVIE:
                if (AmpFormat_RelMovieInfo(mediaInfo->MediaInfo.Movie, IsRemove) != 0) {
                    AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to release media info", __FUNCTION__, __LINE__);
                }
                break;
            case AMP_MEDIA_INFO_IMAGE:
                if (AmpFormat_RelImageInfo(mediaInfo->MediaInfo.Image, IsRemove) != 0) {
                    AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to release media info", __FUNCTION__, __LINE__);
                }
                break;
            case AMP_MEDIA_INFO_SOUND:
                if (AmpFormat_RelSoundInfo(mediaInfo->MediaInfo.Sound, IsRemove) != 0) {
                    AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to release media info", __FUNCTION__, __LINE__);
                }
                break;
            default:
                // Do nothing
                break;
        }
    }

ReturnResult:
    // Close file stream
    if (IOStream != NULL) {
        int Rval = 0;
        if (IOStreamIsOpen) {
            Rval = IOStream->Func->Close(IOStream);
            if (Rval != 0) {
                AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to close file stream (%d).", __FUNCTION__, __LINE__, Rval);
            }
            IOStreamIsOpen = 0;
        }
        Rval = AmpFileStream_Delete(IOStream);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - Format] <GetMediaInfo> %s:%u Failed to delete file stream (%d).", __FUNCTION__, __LINE__, Rval);
        }
        IOStream = NULL;
    }

    DBGMSG("[Applib - Format] <GetMediaInfo> End");

    return ReturnValue;
}

/**
 *  @brief Set dual file saving setting.
 *
 *  Set dual file saving setting.
 *
 *  @param [in] dualStreams Dual stream
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_SetDualFileSaving(int dualFileSaving)
{
    ApplibFormatDualFileSaving = dualFileSaving;
    return 0;
}

/**
 *  @brief Set the file alignment size of primary stream.
 *
 *  Set the file alignment size of primary stream.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_SetPriStreamFileSizeAlignment(UINT32 size)
{
    ApplibFormatPriStreamFileSizeAlignment = size;
    return 0;
}


/**
 *  @brief Set the file alignment size of secondary stream.
 *
 *  Set the file alignment size of secondary stream.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormat_SetSecStreamFileSizeAlignment(UINT32 size)
{
    ApplibFormatSecStreamFileSizeAlignment = size;
    return 0;
}

/**
 *  @brief Get dual file saving setting.
 *
 *  Get dual file saving setting.
 *
 *  @return The dual file saving setting.
 */
int AppLibFormat_GetDualFileSaving(void)
{
    return ApplibFormatDualFileSaving;
}

/**
 *  @brief Get the file alignment size of primary stream
 *
 *  Get the file alignment size of primary stream
 *
 *  @return Alignment size
 */
UINT32 AppLibFormat_GetPriStreamFileSizeAlignment(void)
{
    return ApplibFormatPriStreamFileSizeAlignment;
}

/**
 *  @brief Get the file alignment size of secondary stream.
 *
 *  Get the file alignment size of secondary stream.
 *
 *  @return Alignment size
 */
UINT32 AppLibFormat_GetSecStreamFileSizeAlignment(void)
{
    return ApplibFormatSecStreamFileSizeAlignment;
}


/**
 *  @brief Get media modification time
 *
 *  Retrun the modification time of specified file.
 *
 *  @return execution result
 */
int AppLibFormat_GetMediaTimeInfo(char *fn, char *timeBuf, int timeBufSize)
{
    AMP_CFS_STAT Status = {0};
    int ReturnValue = 0;

    if ((!fn) || (!timeBuf) || (timeBufSize == 0)) {
        DBGMSG("[Applib - Format] <GetMediaTimeInfo> invalid parameter");
        return -1;
    }

    DBGMSG("[Applib - Format] <GetMediaTimeInfo> fn = %s",fn);
    ReturnValue = AmpCFS_Stat(fn, &Status);
    if (ReturnValue != AMP_OK) {
        return -1;
    }

    snprintf(timeBuf, timeBufSize, "%04d-%02d-%02d %02d:%02d:%02d", Status.LastMdyDate.Year+1980, Status.LastMdyDate.Month , Status.LastMdyDate.Day
                                                                  , Status.LastMdyTime.Hour, Status.LastMdyTime.Minute, Status.LastMdyTime.Second);
    DBGMSG("[Applib - Format] <GetMediaTimeInfo> timeBuf: %s", timeBuf);

    return 0;
}

#ifdef CONFIG_APP_ARD
/**< Mutex for avoid deadlock.(happens between g_AmpCfs.Mutex and g_AppLibDcf.Mutex)*/
static AMBA_KAL_MUTEX_t AppLibFormatMutex = {0};
static UINT8 AppLibFormatMutexInit = 0;
int AppLibFormatCreateMux(void)
{
    if(AppLibFormatMutexInit == 0){
        if (AmbaKAL_MutexCreate(&AppLibFormatMutex) != OK) {
            AmbaPrintColor(RED,"Create AppLibFormatMutex Fail!");
            return -1;
        }else{
            AmbaPrintColor(GREEN,"Create AppLibFormatMutex ok!");
            AppLibFormatMutexInit = 1;
            return 0;
        }
    }else{
        return 0;
    }
}

int AppLibFormatTakeMux(void)
{
    if (AmbaKAL_MutexTake(&AppLibFormatMutex, AMBA_KAL_WAIT_FOREVER) != OK) {
        AmbaPrintColor(RED,"Take AppLibFormatMutex Fail!");
        return -1;
    }else{
        return 0;
    }
}

int AppLibFormatGiveMux(void)
{
    if (AmbaKAL_MutexGive(&AppLibFormatMutex) != OK) {
        AmbaPrintColor(RED,"Give AppLibFormatMutex Fail!");
        return -1;
    }else{
        return 0;
    }
}
#endif

