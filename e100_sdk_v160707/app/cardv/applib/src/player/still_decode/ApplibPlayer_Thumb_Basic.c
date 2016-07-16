/**
 * @file src/app/connected/applib/src/player/still_decode/ApplibPlayer_Thumb_Basic.c
 *
 * Implementation of video player module in application Library
 *
 * History:
 *    2013/11/18 - [cyweng] created file
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

#include <player/still_decode/ApplibPlayer_Thumb_Basic.h>
#include <applib.h>
#include <fifo/Fifo.h>
#include <player/Decode.h>
#include <player/StillDec.h>
#include <player/decode_utility/ApplibPlayer_Internal.h>
#include <comsvc/misc/util.h>
#include <format/ApplibFormat_DemuxExif.h>
#include <format/ApplibFormat_DemuxMp4.h>
#include <AmbaCache.h>
#ifdef CONFIG_APP_ARD
#include <AmbaROM.h>
#include <system/ApplibSys_Lcd.h>
#endif

#ifdef CONFIG_APP_ARD
#define MAX_IMAGE_WIDTH (6528)
#define MAX_IMAGE_HEIGHT (4896)
#else
#define MAX_IMAGE_WIDTH (4608)
#define MAX_IMAGE_HEIGHT (3456)
#endif
#define IMAGE_CACHE_WIDTH  (320)
#define IMAGE_CACHE_HEIGHT (256)
#define IMAGE_CACHE_PITCH  (ALIGN_64(IMAGE_CACHE_WIDTH))
#define IMAGE_CACHE_SIZE   (IMAGE_CACHE_PITCH * IMAGE_CACHE_HEIGHT)
#define IMAGE_CACHE_NUM    (6)
#define THUMB_BASIC_DCHAN_VOUT_BUF_NUM (2) /** Multiple buffering. Define how many buffers are used to hold LCD vout frame data. */
#define THUMB_BASIC_FCHAN_VOUT_BUF_NUM (2) /** Multiple buffering. Define how many buffers are used to hold TV vout frame data. */

#define STLDEC_RAW_SIZE (12<<20)

static UINT8* stlRawBuf = NULL;
//static void* stlRawBufOri = NULL;
static UINT8* ImageCacheOri = NULL;
static UINT8* ImageCache = NULL;
//static UINT8* ImageDecBufOri = NULL;
static UINT8* ImageDecBuf = NULL;

static UINT8 ApplibThumbBasicDecInitFlag = 0;
static AMP_DEC_PIPE_HDLR_s *DecPipeHdlr = NULL;
static AMP_STLDEC_HDLR_s *StlDecHdlr = NULL;
static UINT32 lcdAR = (4 << 8) | 3;
static UINT32 tvAR = (16 << 8) | 9;

static UINT32 VoutBufferRequestID = 0; // Unique ID for distinguishing every allocate requests.

static int StillDecodeStartFlag = 0; ///< 0: Not started, 1: Started
#ifdef CONFIG_APP_ARD
static INT8 AudioYUVIdx = -1;
AMP_FIFO_HDLR_s *FeedAudioJpgFifoHdlr = NULL;
static int FeedAudioJpg_FifoCb(void* hdlr,
                               UINT32 event,
                               void* info)
{
    return 0;
}
#endif

static UINT8* getCacheYAddr(UINT32 Idx)
{
    return ImageCache + (Idx * 2 * IMAGE_CACHE_SIZE);
}

static UINT8* getCacheUVAddr(UINT32 Idx)
{
    return ImageCache + (((Idx * 2) + 1) * IMAGE_CACHE_SIZE);
}

// Whether the decoder is initialized and ready to decode
static UINT8 isThumbBasicDecInitialized(void)
{
    return (ApplibThumbBasicDecInitFlag == 0) ? (0) : (1);
}

/**
 * Start still decoder.
 *
 * @return 0: OK
 */
static int AppLibThmBasic_StillDecodeStart(void)
{
    // Start decoder
    if (StillDecodeStartFlag == 0) {
        if (AmpDec_Start(DecPipeHdlr, NULL, NULL) != AMP_OK) {
            AmbaPrint("%s:%u Failed to start the decoder.", __FUNCTION__, __LINE__);
            return -1;
        }
        StillDecodeStartFlag = 1;
        // Wait StillDecodeStart done
        AmbaKAL_TaskSleep(100);
    }
    return 0;
}

static int AppLibThmBasic_DSPEventJpegDecYuvDispCb(void *Hdlr,
                                                   UINT32 EventID,
                                                   void* Info)
{
    UINT32 *EventInfo = Info;
    AMP_DISP_CHANNEL_IDX_e Channel = (AMP_DISP_CHANNEL_IDX_e) EventInfo[0];
    UINT8 *LumaAddr = (UINT8 *) EventInfo[1];
    UINT8 *ChromaAddr = (UINT8 *) EventInfo[2];
    UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN
    switch (Channel) {
        case AMP_DISP_CHANNEL_DCHAN:
            VoutChannel = DISP_CH_DCHAN;
            break;
        case AMP_DISP_CHANNEL_FCHAN:
            VoutChannel = DISP_CH_FCHAN;
            break;
        default:
            return -1;
    }
    ApplibVoutBuffer_DisplayVoutBuffer(&G_VoutBufMgr, VoutChannel, LumaAddr, ChromaAddr);

    return 0;
}

static int AppLibThmBasic_CodecCB(void *hdlr,
                           UINT32 event,
                           void* info)
{
    //AmbaPrint("%s on Event: 0x%08x ", __FUNCTION__, event); // Mark this line because it will be printed constantly after uCode update on 2015/03/04
    // Handle event
    switch (event) {
        case AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT:
            AppLibThmBasic_DSPEventJpegDecYuvDispCb(hdlr, event, info);
            break;
        case AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT:
            AppLibVoutBuffer_UpdateCommonBuffer(info);
            break;
        default:
            //StlDecErr("%s:%u Unknown event (0x%08x)", __FUNCTION__, __LINE__, EventID);
            break;
    }

    return 0;
}

static int AppLibThmBasic_AllocVout(void)
{
    UINT32 BufferNumber[DISP_CH_NUM] = {THUMB_BASIC_DCHAN_VOUT_BUF_NUM, THUMB_BASIC_FCHAN_VOUT_BUF_NUM};

    // Get a free space for Vout buffer
    if (AppLibVoutBuffer_Alloc(BufferNumber, APPLIB_G_MMPL, &G_VoutBufMgr) != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <Show> Failed to alloc Vout buffer!");
        return -1; // Error
    }

    return 0; // Success
}

int AppLibThmBasic_Init(void)
{
    AmbaPrint("[Applib - StillDec] <Init> start");

    if (isThumbBasicDecInitialized() != 0) {
        AmbaPrint("[Applib - StillDec] <Init> Already init");
        goto ReturnSuccess;
    }

    StillDecodeStartFlag = 0;

    // Initialize codec module
    {
        // Get the default codec module settings
        if (AppLibStillDecModule_Init() != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot initialize still codec module.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Create codec handler
    {
        AMP_STILLDEC_CFG_s codecCfg; // Codec handler config
        // Get the default codec handler settings
        if (AmpStillDec_GetDefaultCfg(&codecCfg) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot get the default codec handler settings.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
        // Allocate memory for codec raw buffer
        if (stlRawBuf == NULL) {
            /*if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &stlRawBuf, &stlRawBufOri, STLDEC_RAW_SIZE, 1 << 6) != AMP_OK) {
                AmbaPrint("[Applib - StillDec] %s:%u Cannot allocate memory.", __FUNCTION__, __LINE__);
                goto ReturnError;
            }*/
            AppLibComSvcMemMgr_AllocateDSPMemory(&stlRawBuf, STLDEC_RAW_SIZE);
        }
        // Customize the handler settings
        codecCfg.RawBuf = (UINT8*) stlRawBuf;
        AmbaPrint("[Applib - StillDec] <Init> %x -> %x", stlRawBuf, codecCfg.RawBuf);
        codecCfg.RawBufSize = STLDEC_RAW_SIZE;
        codecCfg.CbCodecEvent = AppLibThmBasic_CodecCB;
        // Get a free codec handler, and configure the initial settings
        if (AmpStillDec_Create(&codecCfg, &StlDecHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot create a codec handler.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Create decoder manager
    {
        AMP_DEC_PIPE_CFG_s pipeCfg;             // Decoder manager config
        // Get the default decoder manager settings
        if (AmpDec_GetDefaultCfg(&pipeCfg) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot get the default decoder manager settings.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
        // Customize the manager settings
        pipeCfg.Decoder[0] = StlDecHdlr;
        pipeCfg.NumDecoder = 1;
        pipeCfg.Type = AMP_DEC_STL_PIPE;
        // Create a decoder manager, and insert the codec handler into the manager
        if (AmpDec_Create(&pipeCfg, &DecPipeHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot create a decoder manager.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Activate decoder manager
    // Activate the decoder manager and all the codec handlers in the manager
    if (AmpDec_Add(DecPipeHdlr) != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Cannot activate the decoder manager.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }

    // Allocate image cache buffer
    if (ImageCacheOri == NULL) {
        if (AmpUtil_GetAlignedPool( // TODO: set width, height, num
                APPLIB_G_MMPL,
                (void**) &ImageCache,
                (void**) &ImageCacheOri,
                (IMAGE_CACHE_SIZE * 2 * IMAGE_CACHE_NUM),
                1 << 6 // Align 64
            ) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot allocate memory.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Allocate main buffer
    if (ImageDecBuf == NULL) {
        /*if (AmpUtil_GetAlignedPool( // TODO: set width, height
                APPLIB_G_MMPL,
                (void**) &ImageDecBuf,
                (void**) &ImageDecBufOri,
                (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 2),
                1 << 6 // Align 64
            ) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot allocate memory.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }*/
        AppLibComSvcMemMgr_AllocateDSPMemory(&ImageDecBuf, (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 2));
    }

    // Initialize demuxer
    if (AppLibFormatDemuxExif_Init() != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Cannot initialize Exif demuxer.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }
    if (AppLibFormatDemuxMp4_Init() != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Cannot initialize Mp4 demuxer.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }

    // Initialize Vout buffer manager
    if (AppLibVoutBuffer_Init(&G_VoutBufMgr, NULL, NULL) != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to initialize Vout buffer manager.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }

ReturnSuccess:
    // Set flag
    ApplibThumbBasicDecInitFlag = 1;
    AmbaPrint("[Applib - StillDec] <Init> end");
    return 0; // Success

ReturnError:
    // Undo previous actions
    if (AppLibThmBasic_Deinit() != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to undo actions.", __FUNCTION__, __LINE__);
    }
    // Reset flag
    ApplibThumbBasicDecInitFlag = 0;
    AmbaPrint("[Applib - StillDec] <Init> End");
    return -1; // Error
}

static int AppLibThmBasic_CleanCache(AMP_YUV_BUFFER_s *Buffer)
{
    // If the cacheable Vout buffer is modified by ARM, it needs to be cleaned so that DSP can get currect data.
    switch (Buffer->ColorFmt) {
#ifdef CONFIG_APP_ARD
    case AMP_YUV_422:
#else
    case AMP_YUV_420:
#endif
        AmbaCache_Clean((void *)Buffer->LumaAddr, Buffer->Height * Buffer->Pitch);
        AmbaCache_Clean((void *)Buffer->ChromaAddr, Buffer->Height * Buffer->Pitch);
        break;
#ifdef CONFIG_APP_ARD
    case AMP_YUV_420:
#else
    case AMP_YUV_422:
#endif
        AmbaCache_Clean((void *)Buffer->LumaAddr, Buffer->Height * Buffer->Pitch);
        AmbaCache_Clean((void *)Buffer->ChromaAddr, Buffer->Height * Buffer->Pitch >> 1); // Replace "/ 2" by ">> 1"
        break;
    default:
        return -1; // Error
    }
    return 0; // Success
}

int AppLibThmBasic_ClearScreen()
{
    // Check preliminary steps
    if (isThumbBasicDecInitialized() == 0) {
        AmbaPrint("[Applib - StillDec] <Show> Must init first");
        goto ReturnError; // Error
    }

    // Start decoder
    // If seamless is enabled and the decoder is not started yet, LCD buffer will be updated by DSP command.
    if (AppLibThmBasic_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibThmBasic_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Allocate Vout buffer
    if (AppLibThmBasic_AllocVout() != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to alloc Vout buffer.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Multiple buffering: Take a free and clean Vout buffer.
    if (ApplibVoutBuffer_TakeVoutBuffer_AllChannel(&G_VoutBufMgr, &VoutBufferRequestID) != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <Show> Failed to take Vout buffer!");
        goto ReturnError; // Error
    }

    // Don't have to clean Vout buffer. "SwitchVoutBuffer" has done the trick.
    //ApplibVoutBuffer_CleanNextVoutBuffer_AllChannel(&G_VoutBufMgr);

    {
        UINT32 ChannelIdx; // Index of a channel in channel array
        UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN
        AMP_YUV_BUFFER_s Buffer;
        int Rval = 0;
        //
        for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
            if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
                AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u", __FUNCTION__, __LINE__);
                goto ReturnError; // Error
            }

            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) != 0) {
                Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, VoutBufferRequestID, &Buffer);
                if (Rval != 0) {
                    AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to get Vout buffer (%d)!", __FUNCTION__, __LINE__, Rval);
                    goto ReturnError; // Error
                }

                // Clean cache after writing the data in cacheable Vout buffer
                AppLibThmBasic_CleanCache(&Buffer);
            }
        }
    }

    // Display Vout
    {
        AMP_STILLDEC_DISPLAY_s display;
        AMP_YUV_BUFFER_s buf;
        // Display LCD
        if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, DISP_CH_DCHAN) != 0) {
            // Initialization
            SET_ZERO(display);
            SET_ZERO(buf);
            if (ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, DISP_CH_DCHAN, VoutBufferRequestID, &buf) != 0) {
                AmbaPrint("[Applib - StillDec] <Show> Failed to get Vout buffer!");
                return -1;
            }
            buf.AOI.X = 0;
            buf.AOI.Y = 0;
            buf.AOI.Width = buf.Width;
            buf.AOI.Height = buf.Height;

            display.Vout = AMP_DISP_CHANNEL_DCHAN; // LCD
            display.Buf = &buf;
            AmpStillDec_Display(StlDecHdlr, &display);
#ifdef CONFIG_APP_ARD
            ApplibVoutBuffer_DisplayVoutBuffer(&G_VoutBufMgr, DISP_CH_DCHAN, buf.LumaAddr,buf.ChromaAddr);
#endif
        }
        // Display TV
        if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, DISP_CH_FCHAN) != 0) {
            // Initialization
            SET_ZERO(display);
            SET_ZERO(buf);
            if (ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, DISP_CH_FCHAN, VoutBufferRequestID, &buf) != 0) {
                AmbaPrint("[Applib - StillDec] <Show> Failed to get Vout buffer!");
                return -1;
            }
            buf.AOI.X = 0;
            buf.AOI.Y = 0;
            buf.AOI.Width = buf.Width;
            buf.AOI.Height = buf.Height;

            display.Vout = AMP_DISP_CHANNEL_FCHAN; // TV
            display.Buf = &buf;
            AmpStillDec_Display(StlDecHdlr, &display);
#ifdef CONFIG_APP_ARD
            ApplibVoutBuffer_DisplayVoutBuffer(&G_VoutBufMgr, DISP_CH_FCHAN, buf.LumaAddr,buf.ChromaAddr);
#endif
        }
    }

    return 0; // Success

ReturnError:
    return -1; // Error. Don't give semaphore here!
}

#ifdef CONFIG_APP_ARD
static INT8 InvalidYUVIdx = -1;
AMP_FIFO_HDLR_s *FeedInvalidJpgFifoHdlr = NULL;
static int FeedInvalidJpg_FifoCb(void* hdlr,
                               UINT32 event,
                               void* info)
{
    return 0;
}

void ThumbShowInvalidJPG(void) {
    AMP_BITS_DESC_s TmpDesc = { 0 };
    AMP_BITS_DESC_s Desc;
    UINT32 FileSize = 0;
    char *Fn = "invalid.jpg";
    int t, er;
    UINT32 ImageWidth, ImageHeight;

    // Create demux fifo
    if (FeedInvalidJpgFifoHdlr == NULL) {
        AMP_FIFO_CFG_s fifoDefCfg = {0};
        AmpFifo_GetDefaultCfg(&fifoDefCfg);
        fifoDefCfg.hCodec = StlDecHdlr;
        fifoDefCfg.IsVirtual = 1;
        fifoDefCfg.NumEntries = 64;
        fifoDefCfg.cbEvent = FeedInvalidJpg_FifoCb;
        er= AmpFifo_Create(&fifoDefCfg, &FeedInvalidJpgFifoHdlr);

        if (er != AMP_OK) {
            AmbaPrintColor(RED, "%s:%u Create FIFO failed (%d).", __FUNCTION__, __LINE__, er);
        }else{
            AmbaPrintColor(GREEN, "%s:%u Create VFIFO = 0x%x", __FUNCTION__, __LINE__, FeedInvalidJpgFifoHdlr);
        }
    }

    // Erase FIFO in order to reset read/write pointer of raw buffer
    er = AmpFifo_EraseAll(FeedInvalidJpgFifoHdlr);
    if (er != AMP_OK) {
        AmbaPrintColor(RED, "%s:%u Failed to erase fifo (%d).", __FUNCTION__, __LINE__, er);
    }

    // Initialize descriptor
    SET_ZERO(TmpDesc);

    // prepare entry
    AmpFifo_PrepareEntry(FeedInvalidJpgFifoHdlr, &Desc);
    TmpDesc.StartAddr = Desc.StartAddr;
    TmpDesc.Type = AMP_FIFO_TYPE_JPEG_FRAME;

    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, Fn) == 1) {
        FileSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, Fn, 0x0);
        if (FileSize == 0) {
            AmbaPrintColor(RED,"Invalid AmbaROM_GetSize fail");
        }else{
            TmpDesc.Size = FileSize;
        }
    }else{
        AmbaPrintColor(RED,"Invalid AmbaROM_FileExists fail");
    }

    if (((TmpDesc.StartAddr + TmpDesc.Size - 1)<=((UINT8*)stlRawBuf+STLDEC_RAW_SIZE)) && (FileSize>0)){
        er = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, Fn, TmpDesc.StartAddr, FileSize, 0);
        AmbaKAL_TaskSleep(50);
        AmpFifo_WriteEntry(FeedInvalidJpgFifoHdlr, &TmpDesc);
        ImageWidth = 640;
        ImageHeight = 480;
        er = 0;
        AmbaPrintColor(GREEN,"Feed Invalid Jpg done");
        AmbaKAL_TaskSleep(500);
        if (FeedInvalidJpgFifoHdlr != NULL) {
            AmpFifo_Delete(FeedInvalidJpgFifoHdlr);
            FeedInvalidJpgFifoHdlr = NULL;
        }
    }else{
        AmbaPrintColor(RED,"Feed Audio Jpg failed.");
        er = 1;
    }
}
#endif

int AppLibThmBasic_Show(APPLIB_THUMB_BASIC_TABLE_s *LocactionInfo,
                        UINT8 NumFiles,
                        APPLIB_THUMB_BASIC_FILE_s *Files,
                        UINT8 Decoded)
{
    int t, er;
    static UINT32 ImageW[IMAGE_CACHE_NUM], ImageH[IMAGE_CACHE_NUM], ImageAR[IMAGE_CACHE_NUM];

#define TO_REAL_PIXEL(x, realSize) ((x*realSize)/10000)

    if (NumFiles > IMAGE_CACHE_NUM) {
        return -1;
    }

    // check if need to decode
    // first version, decode every file

    // Check preliminary steps
    if (isThumbBasicDecInitialized() == 0) {
        AmbaPrint("[Applib - StillDec] <Show> Must init first");
        return -1;
    }

    // Start decoder
    // If seamless is enabled and the decoder is not started yet, LCD buffer will be updated by DSP command.
    if (AppLibThmBasic_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibThmBasic_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Feed and decode
    if (Decoded == 0) {
        int decNum = 0;
        UINT32 ImageWidth, ImageHeight;
        AMP_STILLDEC_DECODE_s decode;
        UINT32 pitch[IMAGE_CACHE_NUM];
        UINT32 state[IMAGE_CACHE_NUM];
        AMP_AREA_s crop[IMAGE_CACHE_NUM];
        AMP_YUV_BUFFER_s tar[IMAGE_CACHE_NUM];
        AMP_ROTATION_e rotate[IMAGE_CACHE_NUM] = { AMP_ROTATE_0 };
        decode.NumFile = decNum;
        decode.DecodeBuf = ImageDecBuf;
        decode.SizeDecodeBuf = MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 2;
        decode.DecodedImgPitch = pitch;
        decode.DecodeState = state;
        decode.CropFromDecodedBuf = crop;
        decode.RescaleDest = tar;
        decode.Rotate = rotate;
#ifdef CONFIG_APP_ARD
        AudioYUVIdx = -1;
#endif
        for (t = 0; t < NumFiles; t++) {
            // feed
            APPLIB_FILE_FORMAT_e fileFormat; // File format
            fileFormat = AppLibFormat_GetFileFormat(Files[t].Filename); // Identifiy file format by filename extension
            switch (fileFormat) {
                case APPLIB_FILE_FORMAT_JPG:
                case APPLIB_FILE_FORMAT_THM:
                    // image
                    er = AppLibFormatDemuxExif_Feed(StlDecHdlr, Files[t].Filename, Files[t].FileSource, stlRawBuf, STLDEC_RAW_SIZE,
                            0, 0, &ImageWidth, &ImageHeight);
#ifdef CONFIG_APP_ARD
                    if (er != 0) {
                        ImageWidth = 640;
                        ImageHeight = 480;
                        ThumbShowInvalidJPG();
                        er = 0;
                    }
#endif
                    break;
                case APPLIB_FILE_FORMAT_MP4:
                case APPLIB_FILE_FORMAT_MOV:
                    // movie
                    er = AppLibFormatDemuxMp4_Feed(StlDecHdlr, Files[t].Filename, stlRawBuf, STLDEC_RAW_SIZE, &ImageWidth, &ImageHeight);

#ifdef CONFIG_APP_ARD
                    if (er != 0) {
                        ImageWidth = 640;
                        ImageHeight = 480;
                        ThumbShowInvalidJPG();
                        er = 0;
                    }
#endif
                    break;
#ifdef CONFIG_APP_ARD
                case APPLIB_FILE_FORMAT_AAC:
                    {
                        AMP_BITS_DESC_s TmpDesc = { 0 };
                        AMP_BITS_DESC_s Desc;
                        UINT32 FileSize = 0;
                        char *Fn = "audio.jpg";

                        if(AudioYUVIdx != -1){
                            continue;
                        }

                        // Create demux fifo
                        if (FeedAudioJpgFifoHdlr == NULL) {
                            AMP_FIFO_CFG_s fifoDefCfg = {0};
                            AmpFifo_GetDefaultCfg(&fifoDefCfg);
                            fifoDefCfg.hCodec = StlDecHdlr;
                            fifoDefCfg.IsVirtual = 1;
                            fifoDefCfg.NumEntries = 64;
                            fifoDefCfg.cbEvent = FeedAudioJpg_FifoCb;
                            er= AmpFifo_Create(&fifoDefCfg, &FeedAudioJpgFifoHdlr);
			if (er != AMP_OK) {
				AmbaPrintColor(RED, "%s:%u Create FIFO failed (%d).", __FUNCTION__, __LINE__, er);
			}else{
				AmbaPrintColor(GREEN, "%s:%u Create VFIFO = 0x%x", __FUNCTION__, __LINE__, FeedAudioJpgFifoHdlr);
			}
                        }

                        // Erase FIFO in order to reset read/write pointer of raw buffer
                        er = AmpFifo_EraseAll(FeedAudioJpgFifoHdlr);
                        if (er != AMP_OK) {
                            AmbaPrintColor(RED, "%s:%u Failed to erase fifo (%d).", __FUNCTION__, __LINE__, er);
                        }

                        // Initialize descriptor
                        SET_ZERO(TmpDesc);

                        // prepare entry
                        AmpFifo_PrepareEntry(FeedAudioJpgFifoHdlr, &Desc);
                        TmpDesc.StartAddr = Desc.StartAddr;
                        TmpDesc.Type = AMP_FIFO_TYPE_JPEG_FRAME;

                        if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, Fn) == 1) {
                            FileSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, Fn, 0x0);
                            if (FileSize == 0) {
                                AmbaPrintColor(RED,"Logo AmbaROM_GetSize fail");
                            }else{
                                TmpDesc.Size = FileSize;
                            }
                        }else{
                            AmbaPrintColor(RED,"Logo AmbaROM_FileExists fail");
                        }

                        if (((TmpDesc.StartAddr + TmpDesc.Size - 1)<=((UINT8*)stlRawBuf+STLDEC_RAW_SIZE)) && (FileSize>0)){
                            er = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, Fn, TmpDesc.StartAddr, FileSize, 0);
                            AmbaKAL_TaskSleep(50);
                            AmpFifo_WriteEntry(FeedAudioJpgFifoHdlr, &TmpDesc);
                            ImageWidth = 640;
                            ImageHeight = 480;
                            er = 0;
                            AmbaPrintColor(GREEN,"Feed Audio Jpg done");
                            AmbaKAL_TaskSleep(500);
                            AudioYUVIdx = t;
                            if (FeedAudioJpgFifoHdlr != NULL) {
                                AmpFifo_Delete(FeedAudioJpgFifoHdlr);
                                FeedAudioJpgFifoHdlr = NULL;
                            }
                        }else{
                            AmbaPrintColor(RED,"Feed Audio Jpg failed.");
                            er = 1;
                        }
                    }
                    break;
#endif
                default:
                    AmbaPrint("[Applib - StillDec] <Decode> File format is not supported");
                    return -1;
            }
            // Check feeding result
            if (er != 0) {
                AmbaPrint("[Applib - StillDec] <Decode> Failed to feed the file");
                return -1;
            }
            //TBD
            ImageW[t] = ImageWidth;
            ImageH[t] = ImageHeight;
            ImageAR[t] = ASPECT_RATIO(ImageH[t], ImageW[t]);

            /* Do multiple files at a time
            decode.NumFile = NumFiles;
            // decode and rescale to cache
            decode.CropFromDecodedBuf[t].X = 0;
            decode.CropFromDecodedBuf[t].Y = 0;
            decode.CropFromDecodedBuf[t].Width = ImageW[t];    //get image width;
            decode.CropFromDecodedBuf[t].Height = ImageH[t];    //get image height;

            decode.RescaleDest[t].LumaAddr = getCacheYAddr(t);
            decode.RescaleDest[t].ChromaAddr = getCacheUVAddr(t);
            decode.RescaleDest[t].ColorFmt = AMP_YUV_422;
            decode.RescaleDest[t].Width = IMAGE_CACHE_WIDTH;
            decode.RescaleDest[t].Height = IMAGE_CACHE_HEIGHT;
            decode.RescaleDest[t].Pitch = IMAGE_CACHE_PITCH;
            decode.RescaleDest[t].AOI.X = 0;
            decode.RescaleDest[t].AOI.Y = 0;
            decode.RescaleDest[t].AOI.Width = IMAGE_CACHE_WIDTH;
            decode.RescaleDest[t].AOI.Height = IMAGE_CACHE_HEIGHT;
            */

            // Do one file at a time
            decode.NumFile = 1;
            // decode and rescale to cache
            decode.CropFromDecodedBuf[0].X = 0;
            decode.CropFromDecodedBuf[0].Y = 0;
            decode.CropFromDecodedBuf[0].Width = ImageW[t];    //get image width;
            decode.CropFromDecodedBuf[0].Height = ImageH[t];    //get image height;

            decode.RescaleDest[0].LumaAddr = getCacheYAddr(t);
            decode.RescaleDest[0].ChromaAddr = getCacheUVAddr(t);
            decode.RescaleDest[0].ColorFmt = AMP_YUV_422;
            decode.RescaleDest[0].Width = IMAGE_CACHE_WIDTH;
            decode.RescaleDest[0].Height = IMAGE_CACHE_HEIGHT;
            decode.RescaleDest[0].Pitch = IMAGE_CACHE_PITCH;
            decode.RescaleDest[0].AOI.X = 0;
            decode.RescaleDest[0].AOI.Y = 0;
            decode.RescaleDest[0].AOI.Width = IMAGE_CACHE_WIDTH;
            decode.RescaleDest[0].AOI.Height = IMAGE_CACHE_HEIGHT;

            // Start decoding
            AmpStillDec_Decode(StlDecHdlr, &decode);

            // Don't have to clean or flush cache and main buffer since they're only accessed by DSP
        }
    }

    // Allocate Vout buffer
    if (AppLibThmBasic_AllocVout() != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to alloc Vout buffer.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // rescale from cache to vout buffer
    {
        AMP_STILLDEC_RESCALE_s rescale;
        AMP_YUV_BUFFER_s src;
        AMP_YUV_BUFFER_s dest;
        AMP_ROTATION_e rotate = AMP_ROTATE_0;
        UINT8 lumaGain = 128; // 128: original luma
        APPLIB_DISP_SIZE_CAL_s cal;

        // Multiple buffering: Take a free and clean Vout buffer.
        if (ApplibVoutBuffer_TakeVoutBuffer_AllChannel(&G_VoutBufMgr, &VoutBufferRequestID) != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> Failed to take Vout buffer!");
        }

        // Don't have to clean Vout buffer. "SwitchVoutBuffer" has done the trick.
        //ApplibVoutBuffer_CleanNextVoutBuffer_AllChannel(&G_VoutBufMgr);

        {
            UINT32 ChannelIdx; // Index of a channel in channel array
            UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN
            AMP_YUV_BUFFER_s Buffer;
            int Rval = 0;
            //
            for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
                if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
                    AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u", __FUNCTION__, __LINE__);
                    return -1; // Error
                }

                if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) != 0) {
                    Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, VoutBufferRequestID, &Buffer);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to get Vout buffer (%d)!", __FUNCTION__, __LINE__, Rval);
                        return -1; // Error
                    }

                    // Clean cache after writing the data in cacheable Vout buffer
                    AppLibThmBasic_CleanCache(&Buffer);
                }
            }
        }

        for (t = 0; t < NumFiles; t++) {
            rescale.NumBuf = 1;
            rescale.Src = &src;
            rescale.Dest = &dest;
            rescale.Rotate = &rotate;
            rescale.LumaGain = &lumaGain;

            src.ColorFmt = AMP_YUV_422;
#ifdef CONFIG_APP_ARD
            if(AppLibFormat_GetFileFormat(Files[t].Filename) == APPLIB_FILE_FORMAT_AAC){
                if(AudioYUVIdx != -1){
                    src.LumaAddr = getCacheYAddr(AudioYUVIdx);
                    src.ChromaAddr = getCacheUVAddr(AudioYUVIdx);
                }else{
                    AmbaPrint("Err:No Audio YUV");
                }
            }else{
#endif
            src.LumaAddr = getCacheYAddr(t);
            src.ChromaAddr = getCacheUVAddr(t);
#ifdef CONFIG_APP_ARD
            }
#endif
            src.Width = IMAGE_CACHE_WIDTH;
            src.Height = IMAGE_CACHE_HEIGHT;
            src.Pitch = IMAGE_CACHE_PITCH;
            src.AOI.X = 0;
            src.AOI.Y = 0;
            src.AOI.Width = IMAGE_CACHE_WIDTH;
            src.AOI.Height = IMAGE_CACHE_HEIGHT;

            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, DISP_CH_DCHAN) != 0) {
                cal.ImageAr = ImageAR[t];
                cal.ImageWidth = 0;    //ImageW[t]; not 1:1 pixel in cache
                cal.ImageHeight = 0;    //ImageH[t];
                cal.ImageRotate = AMP_ROTATE_0;
                cal.DeviceAr = lcdAR;
#ifdef CONFIG_APP_ARD
                if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90){
                    cal.ImageRotate = AMP_ROTATE_90;
                    cal.DeviceAr = AppLibSysLcd_GetDispAR(LCD_CH_DCHAN);
                }
#endif
                ApplibVoutBuffer_GetVoutWidth(&G_VoutBufMgr, DISP_CH_DCHAN, &(cal.DeviceWidth));
                ApplibVoutBuffer_GetVoutHeight(&G_VoutBufMgr, DISP_CH_DCHAN, &(cal.DeviceHeight));
                if (Files[t].Focused == 1) {
                    cal.WindowWidth = TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Width, cal.DeviceWidth);
                    cal.WindowHeight = TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Height, cal.DeviceHeight);
                } else {
                    cal.WindowWidth = TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Width, cal.DeviceWidth);
                    cal.WindowHeight = TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Height, cal.DeviceHeight);
                }
                cal.MagFactor = MAGNIFICATION_FACTOR_BASE;
                cal.ImageShiftX = 0;
                cal.ImageShiftY = 0;
                cal.AutoAdjust = 0;

                Applib_DisplaySizeCal(&cal);

                ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, DISP_CH_DCHAN, VoutBufferRequestID, &dest);
#ifdef CONFIG_APP_ARD
                if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90){
                if (Files[t].Focused == 1) {
                    dest.AOI.X = cal.OutputOffsetX + TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Y, dest.Width);
                    dest.AOI.Y = cal.OutputOffsetY + TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.X, dest.Height);
                    lumaGain = LocactionInfo->AreaFocused[t].LumaGain;
                } else {
                    dest.AOI.X = cal.OutputOffsetX + TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Y, dest.Width);
                    dest.AOI.Y = cal.OutputOffsetY + TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.X, dest.Height);
                    lumaGain = LocactionInfo->AreaNormal[t].LumaGain;
                }
                dest.AOI.X = dest.Width - dest.AOI.X -cal.OutputWidth;
                rotate = AMP_ROTATE_90;
                }else{
#endif
                if (Files[t].Focused == 1) {
                    dest.AOI.X = cal.OutputOffsetX + TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.X, dest.Width);
                    dest.AOI.Y = cal.OutputOffsetY + TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Y, dest.Height);
                    lumaGain = LocactionInfo->AreaFocused[t].LumaGain;
                } else {
                    dest.AOI.X = cal.OutputOffsetX + TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.X, dest.Width);
                    dest.AOI.Y = cal.OutputOffsetY + TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Y, dest.Height);
                    lumaGain = LocactionInfo->AreaNormal[t].LumaGain;
                }
#ifdef CONFIG_APP_ARD
                }
#endif
                dest.AOI.Width = cal.OutputWidth;
                dest.AOI.Height = cal.OutputHeight;

                AmpStillDec_Rescale(StlDecHdlr, &rescale);
#ifdef CONFIG_APP_ARD
                rotate = AMP_ROTATE_0;
#endif

                // Don't have to clean or flush cache and main buffer since they're only accessed by DSP
            }
            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, DISP_CH_FCHAN) != 0) {
                cal.ImageAr = ImageAR[t];
                cal.ImageWidth = 0;    //ImageW[t]; not 1:1 pixel in cache
                cal.ImageHeight = 0;    //ImageH[t];
                cal.ImageRotate = AMP_ROTATE_0;
                cal.DeviceAr = tvAR;
                ApplibVoutBuffer_GetVoutWidth(&G_VoutBufMgr, DISP_CH_FCHAN, &(cal.DeviceWidth));
                ApplibVoutBuffer_GetVoutHeight(&G_VoutBufMgr, DISP_CH_FCHAN, &(cal.DeviceHeight));
                if (Files[t].Focused == 1) {
                    cal.WindowWidth = TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Width, cal.DeviceWidth);
                    cal.WindowHeight = TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Height, cal.DeviceHeight);
                } else {
                    cal.WindowWidth = TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Width, cal.DeviceWidth);
                    cal.WindowHeight = TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Height, cal.DeviceHeight);
                }
                cal.MagFactor = MAGNIFICATION_FACTOR_BASE;
                cal.ImageShiftX = 0;
                cal.ImageShiftY = 0;
                cal.AutoAdjust = 0;

                Applib_DisplaySizeCal(&cal);

                ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, DISP_CH_FCHAN, VoutBufferRequestID, &dest);
                if (Files[t].Focused == 1) {
                    dest.AOI.X = cal.OutputOffsetX + TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.X, dest.Width);
                    dest.AOI.Y = cal.OutputOffsetY + TO_REAL_PIXEL(LocactionInfo->AreaFocused[t].Area.Y, dest.Height);
                    lumaGain = LocactionInfo->AreaFocused[t].LumaGain;
                } else {
                    dest.AOI.X = cal.OutputOffsetX + TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.X, dest.Width);
                    dest.AOI.Y = cal.OutputOffsetY + TO_REAL_PIXEL(LocactionInfo->AreaNormal[t].Area.Y, dest.Height);
                    lumaGain = LocactionInfo->AreaNormal[t].LumaGain;
                }
                dest.AOI.Width = cal.OutputWidth;
                dest.AOI.Height = cal.OutputHeight;

                AmpStillDec_Rescale(StlDecHdlr, &rescale);

                // Don't have to clean or flush cache and main buffer since they're only accessed by DSP
            }
        }
    }

    // Display Vout
    // Display LCD
    if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, DISP_CH_DCHAN) != 0) {
        AMP_STILLDEC_DISPLAY_s display;
        AMP_YUV_BUFFER_s buf;
        if (ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, DISP_CH_DCHAN, VoutBufferRequestID, &buf) != 0) {
            AmbaPrint("[Applib - StillDec] <Show> Get Vout buffer failed!");
            return -1;
        }
        buf.AOI.X = 0;
        buf.AOI.Y = 0;
        buf.AOI.Width = buf.Width;
        buf.AOI.Height = buf.Height;

        display.Vout = AMP_DISP_CHANNEL_DCHAN; // LCD
        display.Buf = &buf;
        AmpStillDec_Display(StlDecHdlr, &display);
    }
    // Display TV
    if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, DISP_CH_FCHAN) != 0) {
        AMP_STILLDEC_DISPLAY_s display;
        AMP_YUV_BUFFER_s buf;
        if (ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, DISP_CH_FCHAN, VoutBufferRequestID, &buf) != 0) {
            AmbaPrint("[Applib - StillDec] <Show> Get Vout buffer failed!");
            return -1;
        }
        buf.AOI.X = 0;
        buf.AOI.Y = 0;
        buf.AOI.Width = buf.Width;
        buf.AOI.Height = buf.Height;

        display.Vout = AMP_DISP_CHANNEL_FCHAN; // TV
        display.Buf = &buf;
        AmpStillDec_Display(StlDecHdlr, &display);
    }

    return 0;
}

int AppLibThmBasic_Deinit(void)
{
    AmbaPrint("[Applib - StillDec] <Stop> Start");

    // In case this module is not completely initialized, continue the following steps even if isThumbBasicDecInitialized() == 0

    StillDecodeStartFlag = 0;

    // Deinit still decoder manager
    if (DecPipeHdlr != NULL) {
        // Cannot "Stop" still decoder manager. Video only.
        //AmpDec_Stop(DecPipeHdlr);

        if (AmpDec_Remove(DecPipeHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot deactivate the still decoder manager.", __FUNCTION__, __LINE__);
        }

        if (AmpDec_Delete(DecPipeHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot deinit the still decoder manager.", __FUNCTION__, __LINE__);
        }
        DecPipeHdlr = NULL;
    }

    // Deinit still decoder
    if (StlDecHdlr != NULL) {
        if (AmpStillDec_Delete(StlDecHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot deinit the still decoder.", __FUNCTION__, __LINE__);
        }
        StlDecHdlr = NULL;
    }

    // Release raw buffer
    if (stlRawBuf != NULL) {
        /*if (AmbaKAL_BytePoolFree(stlRawBufOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot release the raw buffer.", __FUNCTION__, __LINE__);
        }*/
        AppLibComSvcMemMgr_FreeDSPMemory();
        stlRawBuf = NULL;
    }

    // Don't release codec module here (AppLibStillDecModule_Deinit())

    // Don't release Vout buffer here

    // Release cache buffer
    if (ImageCacheOri != NULL) {
        if (AmbaKAL_BytePoolFree(ImageCacheOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot release the cache buffer.", __FUNCTION__, __LINE__);
        }
        ImageCacheOri = NULL;
    }
    ImageCache = NULL;

    // Release main buffer
    if (ImageDecBuf != NULL) {
        /*if (AmbaKAL_BytePoolFree(ImageDecBufOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot release the main buffer.", __FUNCTION__, __LINE__);
        }*/
        AppLibComSvcMemMgr_FreeDSPMemory();
        ImageDecBuf = NULL;
    }

    // Set decoder flag
    ApplibThumbBasicDecInitFlag = 0;

    AmbaPrint("[Applib - StillDec] <Stop> End");
    return 0;
}

