/**
 * @file src/app/connected/applib/src/format/ApplibFormat_DemuxExif.c
 *
 * Implementation of video player module in application Library
 *
 * History:
 *    2013/11/19 - [cyweng] created file
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
#include <fifo/Fifo.h>
#include <format/Demuxer.h>
#include <format/ExifDmx.h>
#include <stream/File.h>
#include <player/Decode.h>
#include <comsvc/misc/util.h>
#include "format/ApplibFormat_DemuxExif.h"

// Global variable
static void *DemuxerBufRaw = NULL;          // Original buffer address of Exif demuxer
#ifdef USE_EXT_DMX
    static void *ExtBufRaw = NULL;          // Original buffer address of external demuxer
#else
    static void *ExifInitBufRaw = NULL;     // Original Buffer address of exif demuxer
    static void *ExifHeaderBufRaw = NULL;   // Original ExifBuf address of exif demuxer
#endif
static void *FileBufRaw = NULL;             // Original buffer address of file stream
static AMP_STREAM_HDLR_s *DemuxExifStream = NULL;
static UINT8 DemuxExifStreamIsOpen = 0; // Whether the DemuxExifStream handler has opened a file successsfully and has not closed it.
static AMP_DMX_FORMAT_HDLR_s *DemuxExifFormat = NULL;
static AMP_IMAGE_INFO_s *DemuxExifImage = NULL;
static AMP_DEMUXER_PIPE_HDLR_s *DemuxExifPipe = NULL;
static AMP_FIFO_HDLR_s *DemuxExifFifoHdlr = NULL;

//static int ApplibFormatExifDeMuxInitFlag = -1;

int AppLibFormatDemuxExif_Init(void)
{
    // init demuxer
    if (0) {
        AMP_DEMUXER_INIT_CFG_s DemuxerInitCfg;
        if (DemuxerBufRaw == NULL) {
            AmpDemuxer_GetInitDefaultCfg(&DemuxerInitCfg);
            DemuxerInitCfg.BufferSize = AmpDemuxer_GetRequiredBufferSize(DemuxerInitCfg.MaxPipe, DemuxerInitCfg.MaxTask, DemuxerInitCfg.TaskInfo.StackSize);
            if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(DemuxerInitCfg.Buffer), &DemuxerBufRaw, DemuxerInitCfg.BufferSize, 1 << 5) != OK) {
                AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
                DemuxerBufRaw = NULL;
                return -1;
            }
            if (AmpDemuxer_Init(&DemuxerInitCfg) != OK) {
                AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
                AmbaKAL_BytePoolFree(DemuxerBufRaw);
                DemuxerBufRaw = NULL;
                return -1;
            }
        }
    }

#ifdef USE_EXT_DMX
    // init external demuxer
    {
        AMP_EXT_DMX_INIT_CFG_s ExtInitCfg;
        if (ExtBufRaw == NULL) {
            AmpExtDmx_GetInitDefaultCfg(&ExtInitCfg);
            ExtInitCfg.BufferSize = AmpExtDmx_GetRequiredBufferSize(ExtInitCfg.MaxHdlr);
            if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(ExtInitCfg.Buffer), &ExtBufRaw, ExtInitCfg.BufferSize, 1 << 5) != OK) {
                AmbaPrint("%s:%u", __FUNCTION__, __LINE__);
                ExtBufRaw = NULL;
                return -1;
            }
            if (AmpExtDmx_Init(&ExtInitCfg) != OK) {
                AmbaPrint("%s:%u", __FUNCTION__, __LINE__);
                AmbaKAL_BytePoolFree(ExtBufRaw);
                ExtBufRaw = NULL;
                return -1;
            }
        }
    }
#else
    // init exif demuxer
    {
        AMP_EXIF_DMX_INIT_CFG_s ExifInitCfg;
        if (ExifInitBufRaw == NULL) {
            AmpExifDmx_GetInitDefaultCfg(&ExifInitCfg);
            ExifInitCfg.BufferSize = AmpExifDmx_GetRequiredBufferSize(ExifInitCfg.MaxHdlr);
            if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(ExifInitCfg.Buffer), &ExifInitBufRaw, ExifInitCfg.BufferSize, 1 << 5) != OK) {
                AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
                ExifInitBufRaw = NULL;
                return -1;
            }
            if (AmpExifDmx_Init(&ExifInitCfg) != OK) {
                AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
                AmbaKAL_BytePoolFree(ExifInitBufRaw);
                ExifInitBufRaw = NULL;
                AmbaKAL_BytePoolFree(ExifHeaderBufRaw);
                ExifHeaderBufRaw = NULL;
                return -1;
            }
        }
    }
#endif

    // init file stream
    if (0) {
        AMP_FILE_STREAM_INIT_CFG_s FileInitCfg;
        if (FileBufRaw == NULL) {
            AmpFileStream_GetInitDefaultCfg(&FileInitCfg);
            FileInitCfg.MaxHdlr = 8;
            FileInitCfg.BufferSize = AmpFileStream_GetRequiredBufferSize(FileInitCfg.MaxHdlr);
            if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(FileInitCfg.Buffer), &FileBufRaw, FileInitCfg.BufferSize, 1 << 5) != OK) {
                AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
                FileBufRaw = NULL;
                return -1;
            }
            if (AmpFileStream_Init(&FileInitCfg) != OK) {
                AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
                AmbaKAL_BytePoolFree(FileBufRaw);
                FileBufRaw = NULL;
                return -1;
            }
        }
    }

    return 0;
}

static int DemuxExif_DmxCb(void* hdlr,
                           UINT32 event,
                           void* info)
{
    return 0;
}

static int DemuxExif_DmxFifoCb(void* hdlr,
                               UINT32 event,
                               void* info)
{
    return 0;
}

static int DemuxExif_Close(void)
{
    int Rval = 0; // Function call return
    if (DemuxExifPipe != NULL ) {
        Rval = AmpDemuxer_Remove(DemuxExifPipe);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to remove dumuxer pipe (%d).", __FUNCTION__, __LINE__, Rval);
        }
        Rval = AmpDemuxer_Delete(DemuxExifPipe);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to delete dumuxer pipe (%d).", __FUNCTION__, __LINE__, Rval);
        }
        DemuxExifPipe = NULL;
    }
    if (DemuxExifFormat != NULL ) {
#ifdef USE_EXT_DMX
        Rval = AmpExtDmx_Delete(DemuxExifFormat);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to delete exif dumuxer (%d).", __FUNCTION__, __LINE__, Rval);
        }
#else
        Rval = AmpExifDmx_Delete(DemuxExifFormat);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to delete exif dumuxer (%d).", __FUNCTION__, __LINE__, Rval);
        }
#endif
        DemuxExifFormat = NULL;
    }
    if (DemuxExifStream != NULL ) {
        if (DemuxExifStreamIsOpen) {
            Rval = DemuxExifStream->Func->Close(DemuxExifStream);
            if (Rval != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to close file stream (%d).", __FUNCTION__, __LINE__, Rval);
            }
            DemuxExifStreamIsOpen = 0;
        }
        Rval = AmpFileStream_Delete(DemuxExifStream);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to delete file stream (%d).", __FUNCTION__, __LINE__, Rval);
        }
        DemuxExifStream = NULL;
    }
    if (DemuxExifImage != NULL ) {
        /*Rval = AmpFormat_RelImageInfo(DemuxExifImage, FALSE);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to release image info (%d).", __FUNCTION__, __LINE__, Rval);
        }*/
        DemuxExifImage = NULL;
    }

    if (DemuxExifFifoHdlr != NULL ) {
        Rval = AmpFifo_Delete(DemuxExifFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to delete fifo (%d).", __FUNCTION__, __LINE__, Rval);
        }
        DemuxExifFifoHdlr = NULL;
    }
    return 0;
}

static int DemuxExif_Open(void* codecHdlr,
                          char *Fn,
                          UINT8 ImageSource,
                          void* RawBuf,
                          UINT32 SizeRawBuf,
                          UINT32 *ImageWidth,
                          UINT32 *ImageHeight)
{
    int ReturnValue = 0; // Return value
    int Rval = 0; // Function call return. It's NOT the return of this function.

    // Print message
    {
        AmbaPrint("[Applib - Format] %s: Open \"%s\"", __FUNCTION__, Fn);
    }

    // Create demux fifo
    if (DemuxExifFifoHdlr == NULL) {
        AMP_FIFO_CFG_s fifoDefCfg = {0};
        AmpFifo_GetDefaultCfg(&fifoDefCfg);
        fifoDefCfg.hCodec = codecHdlr;
        fifoDefCfg.IsVirtual = 1;
        fifoDefCfg.NumEntries = 64;
        fifoDefCfg.cbEvent = DemuxExif_DmxFifoCb;
        Rval = AmpFifo_Create(&fifoDefCfg, &DemuxExifFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Create FIFO failed (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
    }

    // Erase FIFO in order to reset read/write pointer of raw buffer
    Rval = AmpFifo_EraseAll(DemuxExifFifoHdlr);
    if (Rval != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to erase fifo (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1;
        goto ReturnError;
    }

    // Get media info
    {
        APPLIB_MEDIA_INFO_s mediaInfo;
        Rval = AppLibFormat_GetMediaInfo(Fn, &mediaInfo);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Get media info failed (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        if (mediaInfo.MediaInfoType != AMP_MEDIA_INFO_IMAGE) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Media type must be image.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        DemuxExifImage = mediaInfo.MediaInfo.Image;
        // Set image source to fullview if the image source is not available.
        if (DemuxExifImage->Frame[ImageSource].Width == 0) {
            ImageSource = 0; // fullview
        }
        // Get media info details
        *ImageWidth = DemuxExifImage->Frame[ImageSource].Width;
        *ImageHeight = DemuxExifImage->Frame[ImageSource].Height;
    }

    // Initialize media info
   {
        AMP_DMX_IMAGE_INFO_CFG_s imgCfg;
        AmpDemuxer_GetDefaultImageInfoCfg(&imgCfg, DemuxExifImage);
        imgCfg.Fifo = DemuxExifFifoHdlr;
        imgCfg.BufferBase = (UINT8 *)RawBuf;
        imgCfg.BufferLimit = (UINT8 *)RawBuf + SizeRawBuf;
        Rval = AmpDemuxer_InitImageInfo(DemuxExifImage, &imgCfg);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Initialize media info failed (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
    }

    // Create file stream for Demuxer
    if (DemuxExifStream == NULL) {
        AMP_FILE_STREAM_CFG_s FileCfg;
        AmpFileStream_GetDefaultCfg(&FileCfg);
        FileCfg.Async = FALSE;
        Rval = AmpFileStream_Create(&FileCfg, &DemuxExifStream);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to create file stream (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        Rval = DemuxExifStream->Func->Open(DemuxExifStream, Fn, AMP_STREAM_MODE_RDONLY);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to open file (%d).", __FUNCTION__, __LINE__, Rval);
            DemuxExifStreamIsOpen = 0;
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        DemuxExifStreamIsOpen = 1;
    }

    // Create Demuxer for Demuxer pipe
    if (DemuxExifFormat == NULL) {
#ifdef USE_EXT_DMX
        // Open external Demuxer
        AMP_EXT_DMX_CFG_s extCfg;
        AmpExtDmx_GetDefaultCfg(AMP_MEDIA_INFO_IMAGE, &extCfg);
        extCfg.Stream = DemuxExifStream;
        Rval = AmpExtDmx_Create(&extCfg, &DemuxExifFormat);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Create demux failed (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
#else
        // Open Exif Demuxer
        AMP_EXIF_DMX_CFG_s exifCfg;
        AmpExifDmx_GetDefaultCfg(&exifCfg);
        exifCfg.Stream = DemuxExifStream;
        Rval = AmpExifDmx_Create(&exifCfg, &DemuxExifFormat);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Create demux failed (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
#endif
    }

    // Create Demuxer pipe
    if (DemuxExifPipe == NULL) {
        AMP_DEMUXER_PIPE_CFG_s dmxCfg;
        AmpDemuxer_GetDefaultCfg(&dmxCfg);
        dmxCfg.FormatCount = 1;
        dmxCfg.Format[0] = DemuxExifFormat;
        dmxCfg.Media[0] = (AMP_MEDIA_INFO_s *)DemuxExifImage;
        dmxCfg.OnEvent = DemuxExif_DmxCb;
        dmxCfg.ProcParam = ImageSource;  // feed the n frame
        AmbaPrint("[Applib - Format] %s: Feed the %u frame", __FUNCTION__, dmxCfg.ProcParam);
        dmxCfg.TaskMode = AMP_DEMUXER_TASK_MODE_CALLER; // for the codec that does not send under threshold, e.g., still decoder
        // Create Demuxer pipe
        Rval = AmpDemuxer_Create(&dmxCfg, &DemuxExifPipe);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Create Demuxer pipe failed (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        // Add the Demuxer pipe to Demuxer manager
        Rval = AmpDemuxer_Add(DemuxExifPipe);
/*
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Format] %s:%u Add Demuxer pipe failed.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
*/
    }

    return ReturnValue; // Success

ReturnError:
    // Release resource
    DemuxExif_Close();
    return ReturnValue; // Error
}

static int DemuxExif_Feed(void)
{
    int Rval = 0; // Functon call return

    if (DemuxExifPipe == NULL) {
        AmbaPrintColor(RED,"DemuxExifPipe = NULL");
    } else {
        Rval = AmpDemuxer_Start(DemuxExifPipe);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to start demuxer pipe (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    if (DemuxExifFifoHdlr == NULL) {
        AmbaPrintColor(RED,"DemuxExifFifoHdlr = NULL");
    } else {
        Rval = AmpDemuxer_OnDataRequest(DemuxExifFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to run DataRequest (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    return 0;

ReturnError:
    // Release resource
    DemuxExif_Close();
    return -1; // Error
}

int AppLibFormatDemuxExif_Feed(void* codecHdlr,
                               char* Fn,
                               UINT8 ImageSource,
                               void* RawBuf,
                               UINT32 SizeRawBuf,
                               UINT8 MPOImage,
                               UINT8 MPOIdx,
                               UINT32 *ImageWidth,
                               UINT32 *ImageHeight)
{
    int Rval = 0;
    Rval = DemuxExif_Open(codecHdlr, Fn, ImageSource, RawBuf, SizeRawBuf, ImageWidth, ImageHeight);
    if (Rval < 0) {
        return -1;
    }
    Rval = DemuxExif_Feed();
    if (Rval < 0) {
        return -1;
    }
    Rval = DemuxExif_Close();
    if (Rval < 0) {
        return -1;
    }
    AmbaPrint("DemuxExif_Feed feed done");
    return 0;
}

