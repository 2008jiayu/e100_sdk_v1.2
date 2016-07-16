/**
 * @file src/app/connected/applib/src/player/decode_utility/ApplibPlayer_StillTask.c
 *
 * Create a task that controls still Vout process (which involves feeding, decoding, rescaling, and displaying images)
 *
 * History:
 *    2014/01/14 - [phcheng] Create
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
#include <player/decode_utility/ApplibPlayer_StillTask.h>
#include <player/StillDec.h>
#include <comsvc/misc/util.h>
#include <format/ApplibFormat_DemuxExif.h>
#include <format/ApplibFormat_DemuxMp4.h>
#include "../../AppLibTask_Priority.h"
#include "AmbaCache.h"

#ifdef CONFIG_APP_ARD
#include <AmbaROM.h>
#endif

#define APPLIB_VOUT_TASK_MSG_NUM    (64) ///< Maximum number of Vout messages in Vout message pool.
#define STILL_VOUT_TASK_STACK_SIZE  (0x4000) ///< Size of the stack used by a Still Task (16kB).
//#define APPLIB_VOUT_DEBUG_TASK_FLOW ///< Print message

// Gloabal variable
APPLIB_STILL_TASK_RESOURCE_s APPLIB_G_STLDEC = { 0 }; // Resource for Still Task

static void *VoutMsgPoolRaw = NULL; // Original address of Vout message pool
static void *VoutStackRaw = NULL;   // Original address of Vout stack
static void *VoutStack = NULL;      // Aligned address of Vout stack
#ifdef CONFIG_APP_ARD
AMP_FIFO_HDLR_s *FeedLogoFifoHdlr = NULL;
static int FeedLogo_FifoCb(void* hdlr,
                               UINT32 event,
                               void* info)
{
    return 0;
}
#endif
/**
 * Feed encoded data from target file into raw buffer.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in,out] FeedMsg          Feed message.
 *
 * @return 0 - OK, others - Error
 */
static int AppLibStillDec_Feed_Internal(
    APPLIB_STILL_TASK_MSG_s *FeedMsg)
{
    APPLIB_FEED_MSG_s *MsgCtx; // Message context
    APPLIB_FILE_FORMAT_e fileFormat; // File format
    UINT32 OutputImageWidth; // Width of the original image.
    UINT32 OutputImageHeight; // Height of the original image.
    APPLIB_STILL_TASK_EVENT_GENERAL_s EventInfo; // Event information for callback function
    int Rval; // Function call return value

    // Get message context
    MsgCtx = &(FeedMsg->Message.Feed);

    // Invoke callback in the begining
    if (FeedMsg->BeginCB != NULL) {
        // Set event info
        EventInfo.ResultCode = FeedMsg->Output->ResultCode;
        Rval = FeedMsg->BeginCB(FeedMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_FEED_BEGIN, &EventInfo);
    }

#ifdef CONFIG_APP_ARD
    if(strncmp(MsgCtx->Filename,"ROMFS",strlen("ROMFS")) == 0) {
        AMP_BITS_DESC_s TmpDesc = { 0 };
        AMP_BITS_DESC_s Desc;
        UINT32 FileSize = 0;
        char *Fn = strrchr(MsgCtx->Filename, '\\');

        if(Fn == NULL){
            goto ReturnError;
        }else{
            Fn = Fn+1;
        }

        // Create demux fifo
        if (FeedLogoFifoHdlr == NULL) {
            AMP_FIFO_CFG_s fifoDefCfg = {0};
            AmpFifo_GetDefaultCfg(&fifoDefCfg);
            fifoDefCfg.hCodec = FeedMsg->StlDecHdlr;
            fifoDefCfg.IsVirtual = 1;
            fifoDefCfg.NumEntries = 64;
            fifoDefCfg.cbEvent = FeedLogo_FifoCb;
            Rval = AmpFifo_Create(&fifoDefCfg, &FeedLogoFifoHdlr);
            if (Rval != AMP_OK) {
                AmbaPrintColor(RED, "%s:%u Create FIFO failed (%d).", __FUNCTION__, __LINE__, Rval);
                goto ReturnError; // Error
            }
        }

        // Erase FIFO in order to reset read/write pointer of raw buffer
        Rval = AmpFifo_EraseAll(FeedLogoFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "%s:%u Failed to erase fifo (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }

        // Initialize descriptor
        SET_ZERO(TmpDesc);

        // prepare entry
        AmpFifo_PrepareEntry(FeedLogoFifoHdlr, &Desc);
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

        if (((TmpDesc.StartAddr + TmpDesc.Size - 1)<=((UINT8*)MsgCtx->ImageRawBuf+MsgCtx->ImageRawBufSize)) && (FileSize>0)){
            Rval = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, Fn, TmpDesc.StartAddr, FileSize, 0);
            AmbaCache_Clean(TmpDesc.StartAddr, FileSize);
            AmpFifo_WriteEntry(FeedLogoFifoHdlr, &TmpDesc);
            OutputImageWidth = 720;
            OutputImageHeight = 480;
            Rval = 0;
            AmbaPrint("Feed logo done");
            AmbaKAL_TaskSleep(100);

            if (FeedLogoFifoHdlr != NULL) {
                AmpFifo_Delete(FeedLogoFifoHdlr);
                FeedLogoFifoHdlr = NULL;
            }
    }else{
            goto ReturnError; // Error
    }
    }else {
#endif
    // Don't have to check ResultCode

    // Get file format
    fileFormat = AppLibFormat_GetFileFormat(MsgCtx->Filename); // Identifiy file format by filename extension

    // Feed file based on media type
    switch (APPLIB_GET_MEDIA_TYPE(fileFormat)) {
        case AMP_MEDIA_INFO_IMAGE:
            // Image
            Rval = AppLibFormatDemuxExif_Feed(FeedMsg->StlDecHdlr, MsgCtx->Filename, MsgCtx->FileSource, MsgCtx->ImageRawBuf,
                    MsgCtx->ImageRawBufSize, 0, 0, &OutputImageWidth, &OutputImageHeight);
            break;
        case AMP_MEDIA_INFO_MOVIE:
            // Movie
            Rval = AppLibFormatDemuxMp4_Feed(FeedMsg->StlDecHdlr, MsgCtx->Filename, MsgCtx->ImageRawBuf, MsgCtx->ImageRawBufSize,
                    &OutputImageWidth, &OutputImageHeight);
            break;
        // TODO
        //case AMP_MEDIA_INFO_SOUND:
        //    break;
        default:
            AmbaPrintColor(RED, "[Applib - StillDec] <FeedTask> File format is not supported!");
            FeedMsg->Output->ResultCode = APPLIB_STILL_TASK_UNKNOWN_FORMAT;
            goto ReturnError; // Error
    }
#ifdef CONFIG_APP_ARD
    }
#endif

    // Check feeding result
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <FeedTask> Failed to feed the file (%d)!", Rval);
        FeedMsg->Output->ResultCode = APPLIB_STILL_TASK_FEED_ERROR;
        goto ReturnError; // Error
    }

    // Set output values
    FeedMsg->Output->ResultCode = APPLIB_STILL_TASK_FEED_SUCCESS;
    FeedMsg->Output->VoutState = APPLIB_STILL_TASK_STATE_NOT_LOADED;
    FeedMsg->Output->ImageWidth = OutputImageWidth;
    FeedMsg->Output->ImageHeight = OutputImageHeight;

    // Invoke callback in the end
    if (FeedMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = FeedMsg->Output->ResultCode;
        Rval = FeedMsg->EndCB(FeedMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_FEED_END, &EventInfo);
    }

    return 0; // Success

ReturnError:
    // ResultCode has been set in previous step
    // Set state
    FeedMsg->Output->VoutState = APPLIB_STILL_TASK_STATE_NOT_LOADED;
    // Invoke callback in the end
    if (FeedMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = FeedMsg->Output->ResultCode;
        Rval = FeedMsg->EndCB(FeedMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_FEED_END, &EventInfo);
    }

    return -1;
}

/**
 * Decode data in raw buffer and output YUV data to main buffer.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in,out] DecMsg           Decode message.
 *
 * @return 0 - OK, others - Error
 */
static int AppLibStillDec_Decode_Internal(
    APPLIB_STILL_TASK_MSG_s *DecMsg)
{
    APPLIB_DECODE_MSG_s *MsgCtx;        // Message context
    AMP_STILLDEC_DECODE_s DecodeConfig; // Decode config
    AMP_AREA_s RescaleSrc = {0};        // [Decode Input] Area in main buffer to be rescaled to cache. Valid when IsRescaleToCache = 1.
    AMP_YUV_BUFFER_s RescaleDest;       // [Decode Input] Area in cache buffer to be rescaled to. Valid when IsRescaleToCache = 1.
    AMP_ROTATION_e Rotate;              // [Decode Input] Rotate option while rescaling to cache. Valid when IsRescaleToCache = 1.
    UINT32 ImagePitch;                  // [Decode Output] Pitch of original image.
    UINT32 DecodeState;                 // [Decode Output] Decode result.
    APPLIB_STILL_TASK_EVENT_GENERAL_s EventInfo; // Event information for callback function
    int Rval;                           // Function call return value

    // Invoke callback in the begining
    if (DecMsg->BeginCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DecMsg->Output->ResultCode;
        Rval = DecMsg->BeginCB(DecMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_DECODE_BEGIN, &EventInfo);
    }

    // Preliminary check
    // Invoke all callback functions inspite of results of previous process to avoid system hanging.
    if (DecMsg->Output->ResultCode < 0) {
        goto ReturnError; // Error occurred in one of the one of the previous steps.
    }
    if (DecMsg->Output->ResultCode != APPLIB_STILL_TASK_FEED_SUCCESS) {
        AmbaPrintColor(RED, "[Applib - StillDec] <DecodeTask> Feed first! Current = %d", DecMsg->Output->ResultCode);
        goto ReturnError; // Incorrect order of messages.
    }

    // Get message context
    MsgCtx = &(DecMsg->Message.Decode);

    // Configure input of decoder
    DecodeConfig.NumFile = 1;
    DecodeConfig.DecodeBuf = MsgCtx->ImageDecBuf;
    DecodeConfig.SizeDecodeBuf = MsgCtx->ImageDecBufSize;
    DecodeConfig.CropFromDecodedBuf = &RescaleSrc;
    DecodeConfig.RescaleDest = &RescaleDest;
    DecodeConfig.Rotate = &Rotate;
    if (MsgCtx->IsRescaleToCache) {
        DecodeConfig.CropFromDecodedBuf->X = 0;
        DecodeConfig.CropFromDecodedBuf->Y = 0;
        DecodeConfig.CropFromDecodedBuf->Width = DecMsg->Output->ImageWidth;
        DecodeConfig.CropFromDecodedBuf->Height = DecMsg->Output->ImageHeight;

        *(DecodeConfig.RescaleDest) = MsgCtx->ImageCacheBuffer;

        *(DecodeConfig.Rotate) = MsgCtx->ImageCacheRotate;
    } else {
        DecodeConfig.CropFromDecodedBuf = NULL; // Do not rescale
    }

    // Configure output of decoder
    DecodeConfig.DecodedImgPitch = &ImagePitch;
    DecodeConfig.DecodeState = &DecodeState;

    // Start decoding
    Rval = AmpStillDec_Decode(DecMsg->StlDecHdlr, &DecodeConfig);
    if (Rval != OK) {
        AmbaPrintColor(RED, "[Applib - StillDec] <DecodeTask> Failed to decode the file (%d)!", Rval);
        DecMsg->Output->ResultCode = APPLIB_STILL_TASK_DECODE_ERROR;
        // Set state
        DecMsg->Output->VoutState = APPLIB_STILL_TASK_STATE_NOT_LOADED;
        goto ReturnError; // Error
    }

    // Don't have to clean or flush cache and main buffer since they're only accessed by DSP

    // Set output value
    DecMsg->Output->ResultCode = APPLIB_STILL_TASK_DECODE_SUCCESS;
    DecMsg->Output->VoutState = APPLIB_STILL_TASK_STATE_LOADED;
    DecMsg->Output->ImagePitch = DecodeConfig.DecodedImgPitch[0];
    DecMsg->Output->ImageColorFmt = DecodeConfig.OutColorFmt;
    DecMsg->Output->ImageDecChromaAddr = DecodeConfig.OutAddrUV;

    // Invoke callback in the end
    if (DecMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DecMsg->Output->ResultCode;
        Rval = DecMsg->EndCB(DecMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_DECODE_END, &EventInfo);
    }

    return 0; // Success

ReturnError:
    // Invoke callback in the end
    if (DecMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DecMsg->Output->ResultCode;
        Rval = DecMsg->EndCB(DecMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_DECODE_END, &EventInfo);
    }

    return -1;
}

/**
 * Rescale data to Vout buffer.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in,out] RescMsg          Rescale message.
 *
 * @return 0 - OK, others - Error
 */
static int AppLibStillDec_Rescale_Internal(APPLIB_STILL_TASK_MSG_s *RescMsg)
{
    APPLIB_RESCALE_MSG_s *MsgCtx;                       // Message context
    AMP_STILLDEC_RESCALE_s RescaleConfig;               // Rescale configuration
    AMP_YUV_BUFFER_s SrcBuf;                            // Rescale from the specified AOI in the buffer
    AMP_YUV_BUFFER_s DestBuf;                           // Rescale to   the specified AOI in the buffer
    AMP_ROTATION_e Rotate;                              // Rotation option while rescaling
    UINT8 LumaGain;                                     // Luma gain while rescaling
    int Rval;                                           // Function call return value
    int ReturnValue = 0;                                // Return value
    APPLIB_STILL_TASK_EVENT_CHANNEL_s EventInfo;        // Event information for callback function

    // Get message context
    MsgCtx = &(RescMsg->Message.Rescale);

    // Invoke callback in the begining
    if (RescMsg->BeginCB != NULL) {
        // Set event info
        EventInfo.ResultCode = RescMsg->Output->ResultCode;
        EventInfo.Channel = MsgCtx->Channel;
        Rval = RescMsg->BeginCB(RescMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_RESCALE_BEGIN, &EventInfo);
    }

    // Preliminary check
    // Invoke all callback functions inspite of results of previous process to avoid system hanging.
    if (RescMsg->Output->ResultCode < 0) {
        // Set output value
        ReturnValue = -1; // Error occurred in one of the one of the previous steps.
        goto ReturnResult;
    }
    // Check Vout channel
    switch (MsgCtx->Channel) {
        case DISP_CH_DCHAN:
        case DISP_CH_FCHAN:
            // Legal channel
            break;
        default:
            AmbaPrintColor(RED, "[Applib - StillDec] <RescaleTask> Unknown channel (%d)!", __FUNCTION__, __LINE__, MsgCtx->Channel);
            // Set output value
            RescMsg->Output->ResultCode = APPLIB_STILL_TASK_RESCALE_ERROR;
            ReturnValue = -1; // Error
            goto ReturnResult;
    }

    // Configure rescale source buffer
    SrcBuf = MsgCtx->ImageSrcBuffer;
    // Configure rescale destination buffer
    DestBuf = MsgCtx->ImageDestBuffer;
    // Configure rotate option
    Rotate = MsgCtx->ImageRotate;
    // Configure luma gain
    LumaGain = MsgCtx->LumaGain;
    // Configure rescale settings
    RescaleConfig.NumBuf = 1; // Rescale one file at a time
    RescaleConfig.Src = &SrcBuf;
    RescaleConfig.Dest = &DestBuf;
    RescaleConfig.Rotate = &Rotate;
    RescaleConfig.LumaGain = &LumaGain;
    // Rescale data to Vout buffer.
    Rval = AmpStillDec_Rescale(RescMsg->StlDecHdlr, &RescaleConfig);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <RescaleTask> Rescale failed (%d)!", Rval);
        // Set output value
        RescMsg->Output->ResultCode = APPLIB_STILL_TASK_RESCALE_ERROR;
        ReturnValue = -1; // Error
        goto ReturnResult;
    }

    // Don't have to clean or flush cache and main buffer since they're only accessed by DSP

    // Set output value
    RescMsg->Output->ResultCode = APPLIB_STILL_TASK_RESCALE_SUCCESS;
    ReturnValue = 0; // Success

ReturnResult:
    // Don't have to change the state
    // Invoke callback in the end
    if (RescMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = RescMsg->Output->ResultCode;
        EventInfo.Channel = MsgCtx->Channel;
        Rval = RescMsg->EndCB(RescMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_RESCALE_END, &EventInfo);
    }

    return ReturnValue;
}

/**
 * Display Vout buffer.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in,out] DispMsg          Display message.
 *
 * @return 0 - OK, others - Error
 */
static int AppLibStillDec_Display_Internal(APPLIB_STILL_TASK_MSG_s *DispMsg)
{
    APPLIB_DISPLAY_MSG_s *MsgCtx;                       // Message context
    AMP_STILLDEC_DISPLAY_s DisplayConfig;               // Display configuration
    AMP_YUV_BUFFER_s VoutBuf;                           // Display the specified AOI in the buffer
    APPLIB_STILL_TASK_EVENT_CHANNEL_s EventInfo;        // Event information for callback function
    int Rval;                                           // Function call return value
    int ReturnValue = 0;                                // Return value

    // Get message context
    MsgCtx = &(DispMsg->Message.Display);

    // Invoke callback in the begining
    if (DispMsg->BeginCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DispMsg->Output->ResultCode;
        EventInfo.Channel = MsgCtx->Channel;
        Rval = DispMsg->BeginCB(DispMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_DISPLAY_BEGIN, &EventInfo);
    }

    // Preliminary check
    // Allow a task to display a buffer without feeding, decoding, and rescaling
    // Invoke all callback functions inspite of results of previous process to avoid system hanging.
    if (DispMsg->Output->ResultCode < 0) {
        // Set output value
        ReturnValue = -1; // Error occurred in one of the one of the previous steps.
        goto ReturnResult;
    }

    // Configure Vout buffer
    VoutBuf = MsgCtx->VoutBuffer;
    // Configure display settings
    switch (MsgCtx->Channel) {
        case DISP_CH_DCHAN:
            DisplayConfig.Vout = AMP_DISP_CHANNEL_DCHAN; // LCD
            break;
        case DISP_CH_FCHAN:
            DisplayConfig.Vout = AMP_DISP_CHANNEL_FCHAN; // TV
            break;
        default:
            AmbaPrintColor(RED, "[Applib - StillDec] <DisplayTask> Unknown channel (%d)!", __FUNCTION__, __LINE__, MsgCtx->Channel);
            // Set output value
            DispMsg->Output->ResultCode = APPLIB_STILL_TASK_DISPLAY_ERROR;
            ReturnValue = -1; // Error
            goto ReturnResult;
    }
    DisplayConfig.Buf = &VoutBuf;

    // Display Vout buffer
    // NOTE: This is a non-blocking function. Don't wait DSP callback for better performance.
    Rval = AmpStillDec_Display(DispMsg->StlDecHdlr, &DisplayConfig);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <DisplayTask> Display failed (%d)!", Rval);
        // Set output value
        DispMsg->Output->ResultCode = APPLIB_STILL_TASK_DISPLAY_ERROR;
        ReturnValue = -1; // Error
        goto ReturnResult;
    }

    // Don't have to clean or flush cache and main buffer since they're only accessed by DSP

    // Set output value
    DispMsg->Output->ResultCode = APPLIB_STILL_TASK_DISPLAY_SUCCESS;
    ReturnValue = 0; // Success

ReturnResult:
    // Don't have to change the state
    // Invoke callback in the end
    if (DispMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DispMsg->Output->ResultCode;
        EventInfo.Channel = MsgCtx->Channel;
        Rval = DispMsg->EndCB(DispMsg->StlDecHdlr, APPLIB_STILL_TASK_EVENT_DISPLAY_END, &EventInfo);
    }

    return ReturnValue;
}

/**
 * Dummy message is sent before or after a set of messages to handle        \n
 * some action at the beginning or ending of them.                          \n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in,out] DummyMsg         Dummy message.
 *
 * @return 0 - OK, others - Error
 */
static int AppLibStillDec_Dummy_Internal(APPLIB_STILL_TASK_MSG_s *DummyMsg)
{
    APPLIB_DUMMY_MSG_s *MsgCtx;                         // Message context
    APPLIB_STILL_TASK_EVENT_CHANNEL_s EventInfo;        // Event information for callback function

    // Get message context
    MsgCtx = &(DummyMsg->Message.Dummy);

    // Invoke callback in the begining
    if (DummyMsg->BeginCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DummyMsg->Output->ResultCode;
        EventInfo.Channel = MsgCtx->Channel;
        DummyMsg->BeginCB(DummyMsg->StlDecHdlr, MsgCtx->EventID, &EventInfo);
    }

    // Do nothing

    // Invoke callback in the end
    if (DummyMsg->EndCB != NULL) {
        // Set event info
        EventInfo.ResultCode = DummyMsg->Output->ResultCode;
        EventInfo.Channel = MsgCtx->Channel;
        DummyMsg->EndCB(DummyMsg->StlDecHdlr, MsgCtx->EventID, &EventInfo);
    }

    return 0; // Success
}

/**
 * Handle unknown message type.\n
 * Do not change any content of the message or reference the location to which the message point.
 *
 * @param [in,out] Msg              Display message.
 *
 * @return 0 - OK, others - Error
 */
static int AppLibStillDec_Unknown_Internal(const APPLIB_STILL_TASK_MSG_s *Msg)
{
    // Do not access any content of the message or invoke any callback.
    // Since we don't know if any of its content is valid.
    AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Cannot recognize MessageType: %d", Msg->MessageType);
    return 0;
}

/**
 * Behavior based on message type:\n
 *     1. Feed a file to raw buffer.\n
 *     2. Decode raw buffer to main buffer.\n
 *        [Optional] Rescale the data from main buffer to cache buffer.\n
 *     3. Rescale data from one buffer to another.\n
 *     4. Display a buffer on-screen.
 *
 * @param [in] EntryArg             Input to a task when it's created.
 */
static void AppLibStillDec_StillVoutTask(UINT32 EntryArg)
{
    APPLIB_STILL_TASK_MSG_s VoutMsg;       // Message
    int Rval;                                   // Function call return value

    while (1) {
        // Clean message
        SET_ZERO(VoutMsg);

        // Receive decode message.
        Rval = AmpMsgQueue_Receive(&APPLIB_G_STLDEC.VoutMsgQueue, &VoutMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != AMP_MQ_SUCCESS) {
            switch (Rval) {
                case AMP_MQ_INVALID_ERROR: // Invalid pointer of the message queue handler.
                    AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Task stopped! Invalid pointer of the message queue handler (%d)!", Rval);
                    return; // Stop the task
                case AMP_MQ_NOT_CREATED_ERROR: // The message queue handler is not created.
                    AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Task stopped! The message queue handler is not created (%d)!", Rval);
                    return; // Stop the task
                case AMP_MQ_MSG_DEST_ERROR: // Invalid destination pointer for message.
                    AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Task stopped! Invalid destination pointer for message (%d)!", Rval);
                    return; // Stop the task
                case AMP_MQ_DELETED: // The queue has been deleted while the thread was suspended.
                    AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Task stopped! The queue has been deleted while the thread was suspended (%d)!", Rval);
                    return; // Stop the task
                default:
                    AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Failed to receive message (%d)!", Rval);
                    // Sleep for a while to avoid busy waiting.
                    AmbaKAL_TaskSleep(10);
                    continue; // Try again
            }
        }

        // Check output
        if (VoutMsg.Output == NULL) {
            AmbaPrintColor(RED, "[Applib - StillDec] <StillVoutTask> Message output must be given!");
            continue; // Try again
        }

        switch (VoutMsg.MessageType) {
            case APPLIB_STILL_TASK_MSG_FEED:
                // Feed encoded data from target file into raw buffer.
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(BLUE, "[Applib - StillDec] <StillVoutTask> Feed: Start");
#endif
                Rval = AppLibStillDec_Feed_Internal(&VoutMsg);
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(BLUE, "[Applib - StillDec] <StillVoutTask> Feed: End");
#endif
                break; // Get the next message.

            case APPLIB_STILL_TASK_MSG_DECODE:
                // Decode data in raw buffer and output YUV data to main buffer.
                // (Optional) Rescale data in main buffer and output YUV data to cache buffer (usually smaller than original size)
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(CYAN, "[Applib - StillDec] <StillVoutTask> Decode: Start");
#endif
                Rval = AppLibStillDec_Decode_Internal(&VoutMsg);
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(CYAN, "[Applib - StillDec] <StillVoutTask> Decode: End");
#endif
                break; // Get the next message.

            case APPLIB_STILL_TASK_MSG_RESCALE:
                // Rescale data from a buffer to Vout buffer.
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(GREEN, "[Applib - StillDec] <StillVoutTask> Rescale: Start");
#endif
                Rval = AppLibStillDec_Rescale_Internal(&VoutMsg);
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(GREEN, "[Applib - StillDec] <StillVoutTask> Rescale: End");
#endif
                break; // Get the next message.

            case APPLIB_STILL_TASK_MSG_DISPLAY:
                // Display Vout buffer on-screen.
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(YELLOW, "[Applib - StillDec] <StillVoutTask> Display: Start");
#endif
                Rval = AppLibStillDec_Display_Internal(&VoutMsg);
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(YELLOW, "[Applib - StillDec] <StillVoutTask> Display: End");
#endif
                break; // Get the next message.

            case APPLIB_STILL_TASK_MSG_DUMMY:
                // Dummy message.
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(YELLOW, "[Applib - StillDec] <StillVoutTask> Dummy: Start");
#endif
                Rval = AppLibStillDec_Dummy_Internal(&VoutMsg);
#ifdef APPLIB_VOUT_DEBUG_TASK_FLOW
                AmbaPrintColor(YELLOW, "[Applib - StillDec] <StillVoutTask> Dummy: End");
#endif
                break; // Get the next message.

            default:
                Rval = AppLibStillDec_Unknown_Internal(&VoutMsg);
                break; // Get the next message.
        }
    }
}

int AppLibStillDec_IsTaskInitialized(void)
{
    return APPLIB_G_STLDEC.IsInit;
}

int AppLibStillDec_InitTask(void)
{
    int Rval = 0; // Function call return value

    AmbaPrint("[Applib - StillDec] <InitTask> Start");

    if (APPLIB_G_STLDEC.IsInit) {
        AmbaPrint("[Applib - StillDec] <InitTask> Already init");
        goto ReturnSuccess;
    }

    /** Step 1: Create message pool for message queue */
    if (VoutMsgPoolRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                (void**) &(APPLIB_G_STLDEC.VoutMsgPool),
                &VoutMsgPoolRaw,
                (sizeof(APPLIB_STILL_TASK_MSG_s) * APPLIB_VOUT_TASK_MSG_NUM),
                1 << 5 // Align 32
            );
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - StillDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 2: Create message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_STLDEC.VoutMsgQueue) == 0) {
        Rval = AmpMsgQueue_Create(
                &(APPLIB_G_STLDEC.VoutMsgQueue),
                APPLIB_G_STLDEC.VoutMsgPool,
                sizeof(APPLIB_STILL_TASK_MSG_s),
                APPLIB_VOUT_TASK_MSG_NUM
            );
        if (Rval != AMP_MQ_SUCCESS) {
            AmbaPrintColor(RED, "[Applib - StillDec] %s:%u Failed to create message queue (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 3: Create stack for task */
    if (VoutStackRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                &VoutStack,
                &VoutStackRaw,
                STILL_VOUT_TASK_STACK_SIZE,
                1 << 5 // Align 32
            );
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - StillDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 4: Create task */
    Rval = AmbaKAL_TaskCreate(
            &(APPLIB_G_STLDEC.VoutTask), // pTask
            "AppLib_StillPlayerTask", // pTaskName
            APPLIB_STILL_PLAYER_TASK_PRIORITY, // Priority
            AppLibStillDec_StillVoutTask, // void (*EntryFunction)(UINT32)
            0x0, // EntryArg
            VoutStack, // pStackBase
            STILL_VOUT_TASK_STACK_SIZE, // StackByteSize
            AMBA_KAL_AUTO_START // AutoStart
        );
    if (Rval != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - StillDec] %s:%u Failed to create task (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    /** Step 5: Initialize demuxer */
    if (AppLibFormatDemuxExif_Init() != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Cannot initialize Exif demuxer.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }
    if (AppLibFormatDemuxMp4_Init() != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Cannot initialize Mp4 demuxer.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }

    /** Step 6: Set flag */
    APPLIB_G_STLDEC.IsInit = 1;

ReturnSuccess:
    AmbaPrint("[Applib - StillDec] <InitTask> End");
    return 0; // Success

ReturnError:
    AppLibStillDec_DeinitTask();
    AmbaPrint("[Applib - StillDec] <InitTask> End");
    return -1; // Error
}

int AppLibStillDec_DeinitTask(void)
{
    int ReturnValue = 0; // Success
    int Rval = 0; // Function call return

    AmbaPrint("[Applib - StillDec] <DeinitTask> Start");

    /** Step 1: Reset flag */
    APPLIB_G_STLDEC.IsInit = 0;

    /** Step 2: Delete task */
    Rval = AmbaKAL_TaskTerminate(&(APPLIB_G_STLDEC.VoutTask));
    // TX_THREAD_ERROR: The task is not created.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR
        AmbaPrint("[Applib - StillDec] %s:%u Failed to terminate task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }
    Rval = AmbaKAL_TaskDelete(&(APPLIB_G_STLDEC.VoutTask));
    // TX_THREAD_ERROR: The task is not created.
    // TX_DELETE_ERROR: The task is not terminated.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR) && (Rval != TX_DELETE_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR and TX_DELETE_ERROR
        AmbaPrint("[Applib - StillDec] %s:%u Failed to delete task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 3: Release stack */
    if (VoutStackRaw != NULL) {
        Rval = AmbaKAL_BytePoolFree(VoutStackRaw);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to release stack (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
        VoutStackRaw = NULL;
        VoutStack = NULL;
    }

    /** Step 4: Delete message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_STLDEC.VoutMsgQueue)) {
        Rval = AmpMsgQueue_Delete(&APPLIB_G_STLDEC.VoutMsgQueue);
        if (Rval != AMP_MQ_SUCCESS) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to delete message queue (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
    }

    /** Step 5: Release message pool */
    if (VoutMsgPoolRaw != NULL) {
        if (AmbaKAL_BytePoolFree(VoutMsgPoolRaw) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to release message pool.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
        }
        VoutMsgPoolRaw = NULL;
        APPLIB_G_STLDEC.VoutMsgPool = NULL;
    }

    AmbaPrint("[Applib - StillDec] <DeinitTask> End");
    return ReturnValue;
}

int AppLibStillDec_SendVoutMsg(const APPLIB_STILL_TASK_MSG_s *VoutMsg, const UINT32 Timeout)
{
    if (AppLibStillDec_IsTaskInitialized() == 0) {
        return -1; // Failure. The task is not initialized.
    }
    return AmpMsgQueue_Send(&APPLIB_G_STLDEC.VoutMsgQueue, VoutMsg, Timeout);
}

int AppLibStillDec_InitVoutMsgOutput_BeforeLoad(APPLIB_STILL_TASK_OUTPUT_s *MsgOut)
{
    // Preliminary check
    if (MsgOut == NULL) {
        return -1; // Error. Invalid address.
    }

    // Initialize message output
    MsgOut->ResultCode = APPLIB_STILL_TASK_SUCCESS;
    MsgOut->VoutState = APPLIB_STILL_TASK_STATE_NOT_LOADED;
    MsgOut->ImageWidth = 0;
    MsgOut->ImageHeight = 0;
    MsgOut->ImagePitch = 0;
    MsgOut->ImageColorFmt = AMP_YUV_420;
    MsgOut->ImageDecChromaAddr = NULL;

    return 0; // Success
}

int AppLibStillDec_InitVoutMsgOutput_BeforeShow(APPLIB_STILL_TASK_OUTPUT_s *MsgOut)
{
    // Preliminary check
    if (MsgOut == NULL) {
        return -1; // Error. Invalid address.
    }

    // Initialize message output
    // Assume that the file has been loaded successfully.
    MsgOut->ResultCode = APPLIB_STILL_TASK_DECODE_SUCCESS;
    //MsgOut->VoutState = APPLIB_STILL_TASK_STATE_LOADED; // Do NOT change to loaded state

    return 0; // Success
}
