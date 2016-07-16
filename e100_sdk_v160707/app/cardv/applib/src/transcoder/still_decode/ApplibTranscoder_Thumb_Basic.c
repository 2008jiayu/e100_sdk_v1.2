/**
 * @file src/app/connected/applib/src/transcoder/still_decode/ApplibTranscoder_Thumb_Basic.c
 *
 * Implementation of retrieving IDR frame
 *
 * History:
 *    2015/03/17 - [cichen] created file
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
#include <transcoder/still_decode/ApplibTranscoder_Thumb_Basic.h>
#include <transcoder/DecTranscoder.h>
#include <comsvc/ApplibComSvc_MemMgr.h>
#include <comsvc/misc/util.h>
#include <format/Demuxer.h>
#include <format/ApplibFormat_DemuxMp4.h>
#include <format/ApplibFormat_DemuxExif.h>
#include "../../AppLibTask_Priority.h"


#define TRANS_STILL_DBG_EN
#define TRANS_STILL_ERR_EN

#undef TRANS_STILL_DBG
#ifdef TRANS_STILL_DBG_EN
#define TRANS_STILL_DBG(fmt,args...) AmbaPrintColor(CYAN,fmt,##args);
#else
#define TRANS_STILL_DBG(fmt,args...)
#endif

#undef TRANS_STILL_ERR
#ifdef TRANS_STILL_ERR_EN
#define TRANS_STILL_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define TRANS_STILL_ERR(fmt,args...)
#endif

#define DEC_RAW_SIZE           (12<<20)  // Raw buffer size
#define IDRCODEC_STACK_SIZE         (4<<10)

#define FLAG_EVENT_DATA_READY (1<<0)


static AMP_AVDEC_HDLR_s *gDecTransCodecVidHdlr = NULL;
static void* gDecTransCodecBufOri = NULL; ///< Original buffer address of video codec
static void* gDecTransRawBufOri = NULL;   ///< Original buffer address of video decode raw file
static void* gDecTransRawBuf = NULL;      ///< Aligned buffer address of video decode raw file

static AMP_DEC_PIPE_HDLR_s *DecPipeHdlr = NULL;
static UINT8 gDstCodec = 0;

static UINT8 gThmBasicInited = 0;
static UINT8 gIDRCodecInit = 0;

static AMBA_KAL_EVENT_FLAG_t gDataReadyEvent = {0};

static char IDRCodecStack[IDRCODEC_STACK_SIZE];
static AMBA_KAL_TASK_t IDRCodecTask = {0};
static AMP_FIFO_HDLR_s *IDRCodecFifoHdlr = NULL;
static AMBA_KAL_SEM_t gIDRSem = {0};

static TRANS_STILL_DATA_BUF_s *gOutputBuf = NULL;

#define DEBUG_SAVE_FILE
#ifdef DEBUG_SAVE_FILE
static int SaveFile(char *Filename)
{
    AMBA_FS_FILE *File = NULL;
    WCHAR mode[3] = {'w','b','\0'};

    File = AmbaFS_fopen((char const *)Filename,(char const *) mode);
    AmbaFS_fwrite(gOutputBuf->Buf, sizeof(UINT8), gOutputBuf->RetDataSize, File);
    AmbaFS_fclose(File);

    return 0;
}
#endif

static void IDRCodec_Task(UINT32 info)
{
    AMP_BITS_DESC_s *Desc;

    while (1) {
        AmbaKAL_SemTake(&gIDRSem, AMBA_KAL_WAIT_FOREVER);
        if (AmpFifo_PeekEntry(IDRCodecFifoHdlr, &Desc, 0) == AMP_OK) {
            TRANS_STILL_DBG("[Applib - TranscoderThmBasic] <IDRCodec_Task> NewFrm incoming: @0x%x size:%d Type = %d", Desc->StartAddr, Desc->Size, Desc->Type);
            if (gOutputBuf && gOutputBuf->Buf && (gOutputBuf->BufSize >= Desc->Size)) {
                memcpy(gOutputBuf->Buf, Desc->StartAddr, Desc->Size);
                gOutputBuf->RetDataSize = Desc->Size;
            } else if ((gOutputBuf == NULL) || (gOutputBuf->Buf == NULL)) {
                TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Task> NULL pointer detected (gOutputBuf = 0x%x, gOutputBuf->Buf = 0x%x)",
                                                                                                     (UINT32)gOutputBuf, (UINT32)gOutputBuf->Buf);
            } else if (gOutputBuf->BufSize < Desc->Size) {
                TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Task> buffer is too small (gOutputBuf->BufSize = %d, Desc->Size = %d)",
                                                                                                                gOutputBuf->BufSize, Desc->Size);
            }

            AmpFifo_RemoveEntry(IDRCodecFifoHdlr, 1);
        }

        AmbaKAL_EventFlagGive(&gDataReadyEvent, FLAG_EVENT_DATA_READY);
        TRANS_STILL_DBG("[Applib - TranscoderThmBasic] <IDRCodec_Task> gDataReadyEvent event give");
    }
}

static int IDRCodec_FifoCB(void *hdlr, UINT32 event, void* info)
{
    if (event == AMP_FIFO_EVENT_DATA_READY){
        //AmbaPrint("IDRCodec_FifoCB on AMP_FIFO_EVENT_DATA_READY (0x%x)", event);
        AmbaKAL_SemGive(&gIDRSem);
    }
    return 0;
}

/*
static int IDRCodec_EraseFifo(void)
{
    int ReturnValue =0 ;

    ReturnValue = AmpFifo_EraseAll(IDRCodecFifoHdlr);
    if (ReturnValue != AMP_OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_EraseFifo> Failed to erase fifo");
        ReturnValue = -1;
    }

    return ReturnValue;
}
*/

static int IDRCodec_SetOutputBufInfo(TRANS_STILL_DATA_BUF_s *DataBuf)
{
    gOutputBuf = DataBuf;
    return 0;
}

static int IDRCodec_Init(void)
{
    AMP_FIFO_CFG_s fifoDefCfg = {0};
    int ReturnValue = 0;

    if (gIDRCodecInit != 0) {
        return 0;
    }

    // create fifo
    AmpFifo_GetDefaultCfg(&fifoDefCfg);
    fifoDefCfg.hCodec = (void*) (&gDstCodec);
    fifoDefCfg.IsVirtual = 1;
    fifoDefCfg.NumEntries = 32;
    fifoDefCfg.cbEvent = IDRCodec_FifoCB;
    ReturnValue = AmpFifo_Create(&fifoDefCfg, &IDRCodecFifoHdlr);
    if (ReturnValue != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Init> create fifo fail");
        return -1;
    }
    TRANS_STILL_DBG("[Applib - TranscoderThmBasic] <IDRCodec_Init> IDRCodecFifoHdlr = %x", IDRCodecFifoHdlr);

    /* create sem  for frame read and write to fifo*/
    ReturnValue = AmbaKAL_SemCreate(&gIDRSem, 0);
    if (ReturnValue != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Init> Sem create failed: %d", ReturnValue);
        AmpFifo_Delete(IDRCodecFifoHdlr);
        return -1;
    }


    /* Create task */
    ReturnValue = AmbaKAL_TaskCreate(&IDRCodecTask, /* pTask */
                                    "AppLib_Transcoder_IDRFifoTask", /* pTaskName */
                                    APPLIB_STILL_TRANSCODER_TASK_PRIORITY, /* Priority */
                                    IDRCodec_Task, /* void (*EntryFunction)(UINT32) */
                                    0x0, /* EntryArg */
                                    (void *) IDRCodecStack, /* pStackBase */
                                    IDRCODEC_STACK_SIZE, /* StackByteSize */
                                    AMBA_KAL_AUTO_START); /* AutoStart */

    if (ReturnValue != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Init> Task create failed: %d", ReturnValue);
        AmbaKAL_SemDelete(&gIDRSem);
        AmpFifo_Delete(IDRCodecFifoHdlr);
        return -1;
    }

    gIDRCodecInit = 1;
    return 0;
}

static int IDRCodec_Uninit(void)
{
    int ReturnValue = 0;

    if (gIDRCodecInit == 0) {
        return 0;
    }

    ReturnValue = AmbaKAL_TaskTerminate(&IDRCodecTask);
    if (ReturnValue != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Uninit> terminate task fail");
    }

    ReturnValue = AmbaKAL_TaskDelete(&IDRCodecTask);
    if (ReturnValue != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Uninit> delete task fail");
    }

    ReturnValue = AmbaKAL_SemDelete(&gIDRSem);
    if (ReturnValue != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Uninit> delete sem fail");
    }

    if (IDRCodecFifoHdlr) {
        ReturnValue = AmpFifo_Delete(IDRCodecFifoHdlr);
        if (ReturnValue != OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <IDRCodec_Uninit> delete fifo fail");
        }

        IDRCodecFifoHdlr = NULL;
    }

    gIDRCodecInit = 0;

    return 0;
}


static int TranscodeCallBack(void *hdlr, UINT32 event, void* info)
{
    return 0;
}

static int CheckImageSource(char *Filename, UINT8 ImageSource)
{
    APPLIB_MEDIA_INFO_s MediaInfo = {0};
    int ReturnValue = 0;

    if ((!Filename) || (ImageSource >= 3)) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <CheckImageSource> invalid param");
        return -1;
    }

    ReturnValue = AppLibFormat_GetMediaInfo(Filename, &MediaInfo);
    if (ReturnValue != AMP_OK) {
        return -1;
    }

    if (MediaInfo.MediaInfoType != AMP_MEDIA_INFO_IMAGE) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <CheckImageSource> Media isn't an image");
        return -1;
    }

    if (MediaInfo.MediaInfo.Image->Frame[ImageSource].Width == 0) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <CheckImageSource> image source %d does not exist", ImageSource);
        return -1;
    }

    return 0;
}

static int CreateTranscoderCodec(AMP_AVDEC_HDLR_s **pDecTransCodecHdlr)
{
    AMP_DEC_TRANSCODER_CFG_s TransCfg = {0};
    AMP_DEC_TRANSCODER_INIT_CFG_s TransInitCfg = {0};
    static UINT8 TransInited = 0;
    int ReturnValue = 0;

    if (TransInited == 0) {
        AmpDecTranscoder_GetInitDefaultCfg(&TransInitCfg);
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(TransInitCfg.WorkingBuf), &gDecTransCodecBufOri,
                TransInitCfg.WorkingBufSize, 1 << 5) != AMP_OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <CreateTranscoderCodec> Cannot allocate memory.");
            return -1;
        }
        AmpDecTranscoder_Init(&TransInitCfg);
        TransInited = 1;
    }

    AmpDecTranscoder_GetDefaultCfg(&TransCfg);

    if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &gDecTransRawBuf, &gDecTransRawBufOri,
                                DEC_RAW_SIZE, 1 << 5) != AMP_OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <CreateTranscoderCodec> Cannot allocate memory.");
        return -1;
    }

    TransCfg.RawBuffer = gDecTransRawBuf;
    TransCfg.RawBufferSize = DEC_RAW_SIZE;
    TransCfg.CbTranscode = TranscodeCallBack;
    TransCfg.DstCodec = (AMP_AVDEC_HDLR_s*) (&gDstCodec);
    TransCfg.DstCodecType = 1;
    ReturnValue = AmpDecTranscoder_Create(&TransCfg, pDecTransCodecHdlr);
    if (ReturnValue != AMP_OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <CreateTranscoderCodec> AmpDecTranscoder_Create() fail (%d)", ReturnValue);
        return -1;
    }

    return 0;
}

int AppLibTranscoderThmBasic_Init(void)
{
    int ReturnValue = -1;

    if (gThmBasicInited == 1) {
        TRANS_STILL_DBG("[AppLib - TranscoderThmBasic] <Init> already inited");
        return 0;
    }

    AppLibFormat_DemuxerInit();
    AppLibFormatDemuxMp4_Init();

    if (AppLibFormatDemuxExif_Init() != AMP_OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Init> init Exif demuxer fail");
        goto ReturnError;
    }

    if (gDecTransCodecVidHdlr == NULL) {
        ReturnValue = CreateTranscoderCodec(&gDecTransCodecVidHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <Init> Failed to create video transcoder codec (%d)", ReturnValue);
            goto ReturnError;
        }
    }

    // Create decoder manager
    if (DecPipeHdlr == NULL) {
        AMP_DEC_PIPE_CFG_s PipeCfg;
        // Get default config
        ReturnValue = AmpDec_GetDefaultCfg(&PipeCfg);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <Init> Failed to get default decoder manager config (%d).", ReturnValue);
            goto ReturnError;
        }

        PipeCfg.Decoder[0] = gDecTransCodecVidHdlr;
        PipeCfg.NumDecoder = 1;
        PipeCfg.Type = AMP_DEC_VID_PIPE;

        ReturnValue = AmpDec_Create(&PipeCfg, &DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <Init> Failed to create decoder manager (%d).", ReturnValue);
            goto ReturnError;
        }
    }

    // Active pipe
    ReturnValue = AmpDec_Add(DecPipeHdlr);
    if (ReturnValue != AMP_OK) {
        TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <Init> Failed to activate decoder manager (%d).", ReturnValue);
        goto ReturnError;
    }

    if (AmbaKAL_EventFlagCreate(&gDataReadyEvent) != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Init> Create event flag fail");
        goto ReturnError;
    }

    ReturnValue = IDRCodec_Init();
    if (ReturnValue != AMP_OK) {
        TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <Init> Failed to init IDR codec");
        goto ReturnError;
    }

    gThmBasicInited = 1;

    return AMP_OK;

ReturnError:
    AppLibTranscoderThmBasic_Uninit();

    TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <Init> fail");
    return -1;
}

int AppLibTranscoderThmBasic_Uninit(void)
{
    int ReturnValue = 0;

    // deinit
    if (DecPipeHdlr) {
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Exit> Failed to stop codec manager (%d).", ReturnValue);
        }
        ReturnValue = AmpDec_Remove(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Exit> Failed to remove codec manager (%d).", ReturnValue);
        }
        ReturnValue = AmpDec_Delete(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Exit> Failed to delete codec manager (%d).", ReturnValue);
        }

        DecPipeHdlr = NULL;
    }

    if (gDecTransCodecVidHdlr) {
        ReturnValue = AmpDecTranscoder_Delete(gDecTransCodecVidHdlr);
         if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Exit> Failed to delete transcoder (%d).", ReturnValue);
        }
        gDecTransCodecVidHdlr = NULL;
    }

    if (gDecTransRawBufOri) {
        ReturnValue = AmbaKAL_BytePoolFree(gDecTransRawBufOri);
        if (ReturnValue != AMP_OK) {
            TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Exit> Failed to free memory at 0x%08x (%d).", gDecTransRawBufOri, ReturnValue);
        }
        gDecTransRawBufOri = NULL;
    }

    if (AmbaKAL_EventFlagDelete(&gDataReadyEvent) != OK) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <Exit> Failed to delete event flag");
    }

    IDRCodec_Uninit();

    gThmBasicInited = 0;
    return ReturnValue;
}

int AppLibTranscoderThmBasic_GetIdrFrame(char *Filename, TRANS_STILL_DATA_BUF_s *DataBuf)
{
    APPLIB_FILE_FORMAT_e FileFormat = APPLIB_FILE_FORMAT_UNKNOWN;
    UINT32 ImageWidth = 0;
    UINT32 ImageHeight = 0;
    UINT32 ActualFlag = 0;
    int ReturnValue = 0;

    if ((!Filename) || (!DataBuf) || (DataBuf->Buf == NULL) || (DataBuf->BufSize == 0)) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <GetIdrFrame> invalid param");
        return -1;
    }

    if (gThmBasicInited == 0) {
        TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <GetIdrFrame> not inited");
        return -1;
    }

    memset(DataBuf->Buf, 0, DataBuf->BufSize);
    IDRCodec_SetOutputBufInfo(DataBuf);

    FileFormat = AppLibFormat_GetFileFormat(Filename);
    switch (FileFormat) {
    case APPLIB_FILE_FORMAT_MP4:
    case APPLIB_FILE_FORMAT_MOV:
        ReturnValue = AppLibFormatDemuxMp4_Feed(gDecTransCodecVidHdlr, Filename, gDecTransRawBuf, DEC_RAW_SIZE, &ImageWidth, &ImageHeight);
        if (ReturnValue != 0) {
            return -1;
        }
        break;
    default:
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <GetIdrFrame> File format is not supported (FileFormat = 0x%x)", FileFormat);
        return -1;
    }

    AmbaKAL_EventFlagTake(&gDataReadyEvent, FLAG_EVENT_DATA_READY, TX_AND_CLEAR, &ActualFlag, AMBA_KAL_WAIT_FOREVER);

    #ifdef DEBUG_SAVE_FILE
    {
        char Tempbuf[64];
        snprintf(Tempbuf, sizeof(Tempbuf),"C:\\idr");
        SaveFile(Tempbuf);
    }
    #endif

    TRANS_STILL_DBG("[Applib - TranscoderThmBasic] <GetIdrFrame> ImageWidth = %d, ImageHeight = %d", ImageWidth, ImageHeight);

    return 0;
}

int AppLibTranscoderThmBasic_GetImage(char *Filename, TRANS_STILL_IMAGE_SOURCE_TYPE_e Type, TRANS_STILL_DATA_BUF_s *DataBuf)
{
    APPLIB_FILE_FORMAT_e FileFormat = APPLIB_FILE_FORMAT_UNKNOWN;
    UINT8 ImageSource = 0;
    UINT32 ImageWidth = 0;
    UINT32 ImageHeight = 0;
    UINT32 ActualFlag = 0;
    int ReturnValue = 0;

    if ((!Filename) || (!DataBuf) || (Type >= TRANS_STILL_IMAGE_SOURCE_NUM) ||
        (DataBuf->Buf == NULL) || (DataBuf->BufSize == 0)) {
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <GetImage> invalid param");
        return -1;
    }

    if (gThmBasicInited == 0) {
        TRANS_STILL_ERR("[AppLib - TranscoderThmBasic] <GetImage> not inited");
        return -1;
    }

    /* map image source */
    switch (Type) {
    case TRANS_STILL_IMAGE_SOURCE_FULL:
        ImageSource = 0;
        break;
    case TRANS_STILL_IMAGE_SOURCE_THUMBNAIL:
        ImageSource = 1;
        break;
    case TRANS_STILL_IMAGE_SOURCE_SCREENNAIL:
        ImageSource = 2;
        break;
    default:
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <GetImage> invalid image source type");
        break;
    }

    /* confirm image have specified source type */
    ReturnValue = CheckImageSource(Filename, ImageSource);
    if (ReturnValue != 0) {
        return -1;
    }

    memset(DataBuf->Buf, 0, DataBuf->BufSize);
    IDRCodec_SetOutputBufInfo(DataBuf);

    FileFormat = AppLibFormat_GetFileFormat(Filename);
    switch (FileFormat) {
    case APPLIB_FILE_FORMAT_JPG:
        ReturnValue = AppLibFormatDemuxExif_Feed(gDecTransCodecVidHdlr, Filename, ImageSource, gDecTransRawBuf, DEC_RAW_SIZE,
                                                    0, 0, &ImageWidth, &ImageHeight);
        if (ReturnValue != 0) {
            return -1;
        }
        break;
    default:
        TRANS_STILL_ERR("[Applib - TranscoderThmBasic] <GetImage> File format is not supported (FileFormat = 0x%x)", FileFormat);
        return -1;
    }

    AmbaKAL_EventFlagTake(&gDataReadyEvent, FLAG_EVENT_DATA_READY, TX_AND_CLEAR, &ActualFlag, AMBA_KAL_WAIT_FOREVER);

    #ifdef DEBUG_SAVE_FILE
    {
        char Tempbuf[64];
        snprintf(Tempbuf, sizeof(Tempbuf),"C:\\img.%d",ImageSource);
        SaveFile(Tempbuf);
    }
    #endif

    TRANS_STILL_DBG("[Applib - TranscoderThmBasic] <GetImage> ImageSource = %d, ImageWidth = %d, ImageHeight = %d",ImageSource, ImageWidth, ImageHeight);

    return 0;
}


