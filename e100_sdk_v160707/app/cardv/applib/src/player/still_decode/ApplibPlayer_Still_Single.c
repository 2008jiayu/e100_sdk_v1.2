/**
 * @file src/app/connected/applib/src/player/still_decode/ApplibPlayer_Still_Single.c
 *
 * Implementation of video player module in application Library
 *
 * History:
 *    2013/11/28 - [phcheng] created file
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

#include <player/still_decode/AppLibPlayer_Still_Single.h>
#include <applib.h>
#include <fifo/Fifo.h>
#include <player/Decode.h>
#include <player/StillDec.h>
#include <player/decode_utility/ApplibPlayer_StillTask.h>
#include <player/decode_utility/ApplibPlayer_Internal.h>
#include <comsvc/misc/util.h>
#include <AmbaCache.h>
#ifdef CONFIG_APP_ARD
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
#define IMAGE_CACHE_NUM (2)
#define IMAGE_CACHE_COLOR_FORMAT (AMP_YUV_422) /** Color format of image rescaled to cache buffer. */
#define SINGLE_STILL_DCHAN_VOUT_BUF_NUM (2) /** Multiple buffering. Define how many buffers are used to hold LCD vout frame data. */
#define SINGLE_STILL_FCHAN_VOUT_BUF_NUM (2) /** Multiple buffering. Define how many buffers are used to hold TV vout frame data. */
#define STLDEC_RAW_SIZE (12<<20)
#define SINGLE_STILL_TIMEOUT (10000) /** Waiting time (in milliseconds) of getting semaphore in non-blocking functions */
#define APPLIB_INVALID_WAIT_EVENT_ID (0)

//#define DEBUG_VOUT_TASK_CB_FLOW /** Printk when running callback in Still Task */
//#define DEBUG_VOUT_TASK_SEM_FLOW /** Printk when semaphore is taken or given */
//#define DEBUG_VOUT_TASK_EVENT_FLAG_FLOW /** Printk when event flag is taken or given */

#define SINGLE_STILL_VOUT_SEM_NUM (1) /** Maximum value of SingleStillVoutSem */
static AMBA_KAL_SEM_t SingleStillVoutSem;    // Semaphore for Still Task with initial value (1). Only one file at a time.
/**
 * Resource for loading and showing single still.
 */
typedef struct _APPLIB_STILL_DECODE_RESOURCE_s_{
    /**
     * Image aspect ratio (derived from ImageW and ImageH) in main buffer.
     */
    UINT32 ImageAR;
    /**
     * Output from Still Task.
     */
    APPLIB_STILL_TASK_OUTPUT_s SingleStillVoutMsgOutput;
    /**
     * PIP frame calculation parameter for each channel.
     */
    APPLIB_PIP_FRAME_CAL_s PipCal[DISP_CH_NUM];
    /**
     * Information of loading an image.
     */
    APPLIB_STILL_FILE_s LoadStillInfo;
    /**
     * Information of showing an image on-screen.
     */
    APPLIB_STILL_SINGLE_s ShowStillInfo;
    /**
     * Unique ID for distinguishing every allocate requests.
     */
    UINT32 VoutBufferRequestID;
    /**
     * Event flag which will be set at the end of a load operation.
     */
    UINT32 LoadEndEventFlag;
    /**
     * Event flag which will be set at the end of a show operation.
     */
    UINT32 ShowEndEventFlag;
} APPLIB_STILL_DECODE_RESOURCE_s;
static APPLIB_STILL_DECODE_RESOURCE_s StlDecRes; // Resource for loading and showing single still

static AMBA_KAL_EVENT_FLAG_t LoadEventFlag; // Event flag for load function
static AMBA_KAL_EVENT_FLAG_t LoadEventFlagValid; // Whether the event flag for load function is valid or not
static AMBA_KAL_EVENT_FLAG_t ShowEventFlag; // Event flag for show function
static AMBA_KAL_EVENT_FLAG_t ShowEventFlagValid; // Whether the event flag for show function is valid or not
static UINT32 CurrentLoadEventFlag = 1;
static UINT32 CurrentShowEventFlag = 1;

//static void *ImageRawBufOri = NULL;         // Original buffer address of raw buffer (storing raw data from file)
static UINT8 *ImageRawBuf = NULL;            // Aligned buffer address of raw buffer
//static void *ImageMainBufOri = NULL;        // Original buffer address of main buffer (storing decoded data from raw buffer)
static UINT8 *ImageMainLumaAddr = NULL;     // Aligned buffer address of main buffer
static void *ImageCacheBufOri = NULL;       // Original buffer address of cache buffer (storing rescaled data from main buffer)
static UINT8 *ImageCacheBuf = NULL;         // Aligned buffer address of cache buffer

static UINT8 ApplibStillDecInitFlag = 0; // Whether the still decoder is initialized and ready to decode.
static AMP_DEC_PIPE_HDLR_s *DecPipeHdlr = NULL;
static AMP_STLDEC_HDLR_s *StlDecHdlr = NULL;
static UINT32 DeviceAR[DISP_CH_NUM] = {VAR_4x3, VAR_16x9};

static int StillDecodeStartFlag = 0; ///< 0: Not started, 1: Started

static UINT8* getCacheYAddr(UINT32 Idx)
{
    return ImageCacheBuf + ((Idx << 1) * IMAGE_CACHE_SIZE);
}

static UINT8* getCacheUVAddr(UINT32 Idx)
{
    return ImageCacheBuf + (((Idx << 1) + 1) * IMAGE_CACHE_SIZE);
}

// Get Y Address in main buffer
static UINT8* getMainYAddr(void)
{
    return ImageMainLumaAddr;
}

// Get UV Address in main buffer
static UINT8* getMainUVAddr(void)
{
    return StlDecRes.SingleStillVoutMsgOutput.ImageDecChromaAddr;
}

// Whether the decoder is initialized and ready to decode
static UINT8 isStillDecInitialized(void)
{
    return (ApplibStillDecInitFlag == 0) ? (0) : (1);
}

static int AppLibStillSingle_SemTake(UINT32 Timeout)
{
    int Rval = 0;                   // Function call return value

    // Take semaphore
#ifdef DEBUG_VOUT_TASK_SEM_FLOW
    AmbaPrintColor(RED, "[Applib - StillDec] Take semaphore begin");
#endif
    Rval = AmbaKAL_SemTake(&SingleStillVoutSem, Timeout);
#ifdef DEBUG_VOUT_TASK_SEM_FLOW
    if (Rval == 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] Take semaphore end (Success)");
    } else {
        AmbaPrintColor(RED, "[Applib - StillDec] Take semaphore end (Failure: %d)", Rval);
    }
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <SemTake> %s:%u Failed to take semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
    }

    return Rval;
}

/**
 * Start still decoder.
 *
 * @return 0: OK
 */
static int AppLibStillSingle_StillDecodeStart(void)
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

static int AppLibStillSingle_SemGive(void)
{
    int Rval = 0;                   // Function call return value
    UINT32 SemVal = 0;              // Current value of the semaphore

    // Get current semaphore value
#ifdef DEBUG_VOUT_TASK_SEM_FLOW
    AmbaPrintColor(RED, "[Applib - StillDec] Give semaphore begin");
#endif
    Rval = AmbaKAL_SemQuery(&SingleStillVoutSem, &SemVal);
#ifdef DEBUG_VOUT_TASK_SEM_FLOW
    if (Rval == 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] Give semaphore end (Success)");
    } else {
        AmbaPrintColor(RED, "[Applib - StillDec] Give semaphore end (Failure: %d)", Rval);
    }
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <SemGive> %s:%u Failed to query semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        return Rval; // Error
    }

    // Check semaphore value
    // Cannot exceed the maximum value of the semaphore
    if (SemVal > SINGLE_STILL_VOUT_SEM_NUM) {
        // TODO: ASSERT
        AmbaPrint("[Applib - StillDec] <SemGive> %s:%u Invalid semaphore count (%d)!", __FUNCTION__, __LINE__, SemVal);
        return -1;
    }
    if (SemVal == SINGLE_STILL_VOUT_SEM_NUM) {
        AmbaPrint("[Applib - StillDec] <SemGive> %s:%u Semaphore count has reached its limit (%d)!", __FUNCTION__, __LINE__, SINGLE_STILL_VOUT_SEM_NUM);
        return -1;
    }

    // Give semaphore
    Rval = AmbaKAL_SemGive(&SingleStillVoutSem);
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <SemGive> %s:%u Failed to give semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        return Rval; // Error
    }

    return 0; // Success
}

// Whether the decoder has decoded a file and ready to display
static UINT8 AppLibStillSingle_IsStillDecDecoded(const APPLIB_STILL_DECODE_RESOURCE_s *StlDecRes)
{
    return (StlDecRes->SingleStillVoutMsgOutput.VoutState == APPLIB_STILL_TASK_STATE_NOT_LOADED) ? (0) : (1);
}

static int AppLibStillSingle_InitStillDecResource_BeforeLoad(APPLIB_STILL_DECODE_RESOURCE_s *StlDecRes)
{
    if (StlDecRes == NULL) {
        return -1; // Error
    }
    StlDecRes->ImageAR = 0;
    // Initialize message output
    if (AppLibStillDec_InitVoutMsgOutput_BeforeLoad(&StlDecRes->SingleStillVoutMsgOutput) != 0) {
        AmbaPrint("[Applib - StillDec] <Load> Failed to initialize message output!");
        return -1;
    }
    SET_ZERO(StlDecRes->LoadStillInfo);
    StlDecRes->LoadEndEventFlag = 0;
    return 0;
}

static int AppLibStillSingle_InitStillDecResource_BeforeShow(APPLIB_STILL_DECODE_RESOURCE_s *StlDecRes)
{
    if (StlDecRes == NULL) {
        return -1; // Error
    }
    // Initialize message output
    if (AppLibStillDec_InitVoutMsgOutput_BeforeShow(&StlDecRes->SingleStillVoutMsgOutput) != 0) {
        AmbaPrint("[Applib - StillDec] <Show> Failed to initialize message output!");
        return -1;
    }
    SET_ZERO(StlDecRes->PipCal);
    SET_ZERO(StlDecRes->ShowStillInfo);
    StlDecRes->ShowEndEventFlag = 0;
    return 0;
}

static int AppLibStillSingle_IsShowPip(const APPLIB_STILL_SINGLE_s *StillInfo, const UINT32 VoutChannel)
{
    switch (VoutChannel) {
        case DISP_CH_DCHAN:
            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) == 0) {
                return 0;
            }
            return ((StillInfo->AreaDchanPIP.Width != 0) &&
                    (StillInfo->AreaDchanPIP.Height != 0)) ? (1) : (0);
        case DISP_CH_FCHAN:
            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) == 0) {
                return 0;
            }
            return ((StillInfo->AreaFchanPIP.Width != 0) &&
                    (StillInfo->AreaFchanPIP.Height != 0)) ? (1) : (0);
        default:
            // Unknown channel
            return 0;
    }
}

static int AppLibStillSingle_FlushCache(AMP_YUV_BUFFER_s *Buffer)
{
    // If the cacheable Vout buffer is modified by DSP, it needs to be flushed so that ARM can get currect data.
    switch (Buffer->ColorFmt) {
#ifdef CONFIG_APP_ARD
    case AMP_YUV_422:
#else
    case AMP_YUV_420:
#endif
        AmbaCache_Invalidate((void *)Buffer->LumaAddr, Buffer->Height * Buffer->Pitch);
        AmbaCache_Invalidate((void *)Buffer->ChromaAddr, Buffer->Height * Buffer->Pitch);
        break;
#ifdef CONFIG_APP_ARD
    case AMP_YUV_420:
#else
    case AMP_YUV_422:
#endif
        AmbaCache_Invalidate((void *)Buffer->LumaAddr, Buffer->Height * Buffer->Pitch);
        AmbaCache_Invalidate((void *)Buffer->ChromaAddr, Buffer->Height * Buffer->Pitch >> 1); // Replace "/ 2" by ">> 1"
        break;
    default:
        return -1; // Error
    }
    return 0; // Success
}

static int AppLibStillSingle_CleanCache(AMP_YUV_BUFFER_s *Buffer)
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

static int AppLibStillSingle_DrawPipFrame(void)
{
    APPLIB_DRAW_FRAME_CONFIG_s FrameConfig[DISP_CH_NUM];
    UINT32 ChannelIdx; // Index of a channel in channel array
    UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN
    int Rval = 0;                                   // Function call return value
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
            return -1;
        }
        // Draw frame
        if (AppLibStillSingle_IsShowPip(&StlDecRes.ShowStillInfo, VoutChannel)) {
            Applib_PipFrameSizeCal(&(StlDecRes.PipCal[ChannelIdx]));
            Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, StlDecRes.VoutBufferRequestID, &(FrameConfig[ChannelIdx].TargetBuffer));
            if (Rval != 0) {
                AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to get Vout buffer (%d)!", __FUNCTION__, __LINE__, Rval);
                return -1;
            }
            switch (VoutChannel) {
                case DISP_CH_DCHAN:
                    // Width of a shape in LCD must be a multiple of 6. Since pixels on an LCD are either red, green, or blue.
                    // Hence a multiple of 3 guarantees full RGB information. In addition, because data are stored in
                    // 4:2:2 or 4:2:0 format, a multiple of 2 guarantees full UV information. As a result, a multiple of 6
                    // is required.
                    FrameConfig[ChannelIdx].Thickness = 6; // Must be a multiple of 6
                    break;
                case DISP_CH_FCHAN:
                    // Width of a shape in TV must be a multiple of 2. Differemt from LCD, pixels on a TV
                    // are composed of red, green, and blue elements. Hence we only need a multiple of 2
                    // to guarantee full UV information.
                    FrameConfig[ChannelIdx].Thickness = 4; // Must be a multiple of 2
                    break;
                default:
                    AmbaPrint("[Applib - StillDec] <Show> %s:%u Unknown channel (%d)!", __FUNCTION__, __LINE__, VoutChannel);
                    return -1; // Error
            }
            FrameConfig[ChannelIdx].TargetBuffer.AOI = StlDecRes.PipCal[ChannelIdx].OutputPipFrameAOI;
            // Yellow
            FrameConfig[ChannelIdx].FrameColor.PenColorY = 203;
            FrameConfig[ChannelIdx].FrameColor.PenColorU = 31;
            FrameConfig[ChannelIdx].FrameColor.PenColorV = 164;
            FrameConfig[ChannelIdx].FrameColor.PenColorAlpha= 180;
            // Flush cache before reading the data in cacheable Vout buffer
            AppLibStillSingle_FlushCache(&FrameConfig[ChannelIdx].TargetBuffer);
            // Draw frame
            Rval = Applib_Draw_Frame(&(FrameConfig[ChannelIdx]));
            if (Rval != 0) {
                AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to draw frame (%d)!", __FUNCTION__, __LINE__, Rval);
                return -1;
            }
            // Clean cache after writing the data in cacheable Vout buffer
            AppLibStillSingle_CleanCache(&FrameConfig[ChannelIdx].TargetBuffer);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_LoadEndCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Load> Load end CB (Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.LoadStillInfo.LoadEndCB != NULL) {
        Rval = StlDecRes.LoadStillInfo.LoadEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Load> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    // Give event flag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Load> Give event flag (0x%08x) begin", StlDecRes.LoadEndEventFlag);
#endif
    if (StlDecRes.LoadEndEventFlag != 0) {
        Rval = AmbaKAL_EventFlagGive(&LoadEventFlag, StlDecRes.LoadEndEventFlag); // Give LoadEndEventFlag
    }
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Load> Give event flag (0x%08x) end (%d)", StlDecRes.LoadEndEventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to give event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        // Don't return here!
    }

    // Give semaphore which is taken in the begining of "AppLibStillSingle_Load"
    Rval = AppLibStillSingle_SemGive();
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to give semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }

    return 0; // Success
}

static int AppLibStillSingle_FeedBeginCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Load> Feed begin CB (Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.LoadStillInfo.FeedBeginCB != NULL) {
        Rval = StlDecRes.LoadStillInfo.FeedBeginCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Load> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_FeedEndCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Load> Feed end CB (Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.LoadStillInfo.FeedEndCB != NULL) {
        Rval = StlDecRes.LoadStillInfo.FeedEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Load> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DecodeBeginCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Load> Decode begin CB (Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.LoadStillInfo.DecodeBeginCB != NULL) {
        Rval = StlDecRes.LoadStillInfo.DecodeBeginCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Load> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DecodeEndCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Load> Decode end CB (Result: %d)", EventInfo->ResultCode);
#endif

    if (StlDecRes.SingleStillVoutMsgOutput.ResultCode >= 0) {
        // Successful decode
        // Get image information
        StlDecRes.ImageAR = ASPECT_RATIO(StlDecRes.SingleStillVoutMsgOutput.ImageHeight, StlDecRes.SingleStillVoutMsgOutput.ImageWidth);
    } else {
        // Unsuccessful decode
        // Reset image inforamtion
        StlDecRes.ImageAR = 0;
    }

    // Invoke APP CB
    if (StlDecRes.LoadStillInfo.DecodeEndCB != NULL) {
        Rval = StlDecRes.LoadStillInfo.DecodeEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Load> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return AppLibStillSingle_LoadEndCB(Hdlr, EventID, Info);
}

static int AppLibStillSingle_ShowEndCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Show end CB (Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.ShowEndCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.ShowEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    // Give event flag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> Give ShowEndEventFlag (0x%08x) begin", StlDecRes.ShowEndEventFlag);
#endif
    if (StlDecRes.ShowEndEventFlag != 0) {
        Rval = AmbaKAL_EventFlagGive(&ShowEventFlag, StlDecRes.ShowEndEventFlag); // Give ShowEndEventFlag
    }
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> Give ShowEndEventFlag (0x%08x) end (%d)", StlDecRes.ShowEndEventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to give event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        // Don't return here!
    }

    // Give semaphore which is taken in the begining of "AppLibStillSingle_Show"
    Rval = AppLibStillSingle_SemGive();
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to give semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }

    return 0; // Success
}

static int AppLibStillSingle_DisplayAllChanBeginCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Display begin CB (All channels. Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.DisplayAllChanBeginCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.DisplayAllChanBeginCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DisplayAllChanWaitCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Display wait CB (All channels. Result: %d)", EventInfo->ResultCode);
#endif

    if (StlDecRes.SingleStillVoutMsgOutput.ResultCode >= 0) {
        // Successful display
        // Do nothing and return success
    } else {
        // Unsuccessful display
        // Do nothing and return success
    }

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.DisplayAllChanWaitCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.DisplayAllChanWaitCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DisplayAllChanEndCB(void *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    UINT32 *EventInfo = Info;
    UINT32 RequestID = *EventInfo;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Display end CB (All channels. RequestID: %d)", RequestID);
#endif

    // TODO: Find StlDecRes corresponding to RequestID
    if (StlDecRes.SingleStillVoutMsgOutput.ResultCode >= 0) {
        // Successful display
        // Do nothing and return success
    } else {
        // Unsuccessful display
        // Do nothing and return success
    }

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.DisplayAllChanEndCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.DisplayAllChanEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    // Invoke ShowEnd CB
    return AppLibStillSingle_ShowEndCB(Hdlr, EventID, Info);
}

static int AppLibStillSingle_DisplayBeginCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_CHANNEL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Display begin CB (Channel: %d. Result: %d)", EventInfo->Channel, EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.DisplayBeginCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.DisplayBeginCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DisplayWaitCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_CHANNEL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Display wait CB (Channel: %d. Result: %d)", EventInfo->Channel, EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.DisplayWaitCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.DisplayWaitCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DisplayEndCB(void *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value
#ifdef DEBUG_VOUT_TASK_CB_FLOW
    UINT32 *EventInfo = Info;
    UINT32 ChannelIdx = EventInfo[0];
    UINT32 RequestID = EventInfo[1];
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Display end CB (Channel: %d. RequestID: %d)", ChannelIdx, RequestID);
#endif

    // TODO: Find StlDecRes corresponding to RequestID
    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.DisplayEndCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.DisplayEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_RescaleAllChanBeginCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Rescale begin CB (All channels. Result: %d)", EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.RescaleAllChanBeginCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.RescaleAllChanBeginCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_RescaleAllChanEndCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_GENERAL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Rescale end CB (All channels. Result: %d)", EventInfo->ResultCode);
#endif

    if (StlDecRes.SingleStillVoutMsgOutput.ResultCode >= 0) {
        // Successful rescale
        // Draw PIP frame
        if (AppLibStillSingle_DrawPipFrame() != 0) {
            // Don't return here! Keep running in order to invoke callback.
        }
    } else {
        // Unsuccessful rescale
        // Do nothing and return success
    }

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.RescaleAllChanEndCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.RescaleAllChanEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_RescaleBeginCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_CHANNEL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Rescale begin CB (Channel: %d. Result: %d)", EventInfo->Channel, EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.RescaleBeginCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.RescaleBeginCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_RescaleEndCB(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info)
{
    int Rval = 0;                                   // Function call return value

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    APPLIB_STILL_TASK_EVENT_CHANNEL_s *EventInfo = Info;
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Rescale end CB (Channel: %d. Result: %d)", EventInfo->Channel, EventInfo->ResultCode);
#endif

    // Invoke APP CB
    if (StlDecRes.ShowStillInfo.RescaleEndCB != NULL) {
        Rval = StlDecRes.ShowStillInfo.RescaleEndCB(Hdlr, EventID, Info);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - StillDec] <Show> %s:%u Failed to invoke callback (%d)!", __FUNCTION__, __LINE__, Rval);
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_GetEventFlag(const UINT32 EventID, UINT32 *OutputEventFlag)
{
    // Preliminary check
    if (OutputEventFlag == NULL) {
        return -1; // Error
    }
    // Check EventID validity
    if (EventID == APPLIB_INVALID_WAIT_EVENT_ID) {
        return -1; // Error
    }

    // Map Event ID to Event Flag
    *OutputEventFlag = EventID;

    return 0;
}

static int AppLibStillSingle_AllocVout(void)
{
    UINT32 BufferNumber[DISP_CH_NUM] = {SINGLE_STILL_DCHAN_VOUT_BUF_NUM, SINGLE_STILL_FCHAN_VOUT_BUF_NUM};

//#define TEST_MULTI_BUFFER
#ifdef TEST_MULTI_BUFFER
AmbaPrintColor(RED, "[Applib - StillDec] <Show> LCD Before alloc %d/%d",G_VoutBufMgr.VoutBuffer[0].NextBufIdx,G_VoutBufMgr.VoutBuffer[0].BufferNumber);
AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> TV  Before alloc %d/%d",G_VoutBufMgr.VoutBuffer[1].NextBufIdx,G_VoutBufMgr.VoutBuffer[1].BufferNumber);
#endif
    // Get a free space for Vout buffer
    if (AppLibVoutBuffer_Alloc(BufferNumber, APPLIB_G_MMPL, &G_VoutBufMgr) != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <Show> Failed to alloc Vout buffer!");
        return -1; // Error
    }
#ifdef TEST_MULTI_BUFFER
AmbaPrintColor(RED, "[Applib - StillDec] <Show> LCD After  alloc %d/%d",G_VoutBufMgr.VoutBuffer[0].NextBufIdx,G_VoutBufMgr.VoutBuffer[0].BufferNumber);
AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> TV  After  alloc %d/%d",G_VoutBufMgr.VoutBuffer[1].NextBufIdx,G_VoutBufMgr.VoutBuffer[1].BufferNumber);
#endif

    // Check available Vout channel
    // Currently we cannot handle the situation that no Vout channel is available,
    // since the callback "DisplayEnd" will never be invoked and the system will hang
    // if we wait for the "DisplayEnd" event.
    {
        UINT32 ChannelIdx; // Index of a channel in channel array
        UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN
        UINT8 Count = 0; // Number of available Vout channel
        // Count available Vout buffer
        for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
            if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
                AmbaPrint("[Applib - StillDec] <Show> %s:%u", __FUNCTION__, __LINE__);
                return -1; // Error
            }

            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) != 0) {
                Count++;
            }
        }
        if (Count == 0) {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u No available Vout buffer.", __FUNCTION__, __LINE__);
            return -1; // Error
        }
    }

    return 0; // Success
}

static int AppLibStillSingle_DSPEventJpegDecYuvDispCb(void *Hdlr,
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

static int AppLibStillSingle_CodecCB(void *Hdlr,
                           UINT32 EventID,
                           void* Info)
{
    //AmbaPrint("%s on Event: 0x%08x ", __FUNCTION__, EventID); // Mark this line because it will be printed constantly after uCode update on 2015/03/04
    // Handle event
    switch (EventID) {
        case AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT:
            AppLibStillSingle_DSPEventJpegDecYuvDispCb(Hdlr, EventID, Info);
            break;
        case AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT:
            AppLibVoutBuffer_UpdateCommonBuffer(Info);
            break;
        default:
            //StlDecErr("%s:%u Unknown event (0x%08x)", __FUNCTION__, __LINE__, EventID);
            break;
    }

    return 0; // Success
}

int AppLibStillSingle_Init(void)
{
    int Rval = 0;                               // Function call return value

    AmbaPrint("[Applib - StillDec] <Init> Start");

    StillDecodeStartFlag = 0;

    // Preliminary check
    if (isStillDecInitialized() != 0) {
        AmbaPrint("[Applib - StillDec] <Init> Already init");
        goto ReturnSuccess;
    }

    // Reset decode resource
    SET_ZERO(StlDecRes);

    // Initialize decode task
    if (AppLibStillDec_IsTaskInitialized() == 0) {
        AppLibStillDec_InitTask();
    }

    // Initialize codec module
    {
        // Get the default codec module settings
        if (AppLibStillDecModule_Init() != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to initialize still codec module.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Create codec handler
    {
        AMP_STILLDEC_CFG_s codecCfg; // Codec handler config
        // Get the default codec handler settings
        if (AmpStillDec_GetDefaultCfg(&codecCfg) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to get the default codec handler settings.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
        // Allocate memory for codec raw buffer
        if (ImageRawBuf == NULL) {
            /*if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &ImageRawBuf, &ImageRawBufOri, STLDEC_RAW_SIZE, 1 << 6) != AMP_OK) {
                AmbaPrint("[Applib - StillDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
                goto ReturnError;
            }*/
            AppLibComSvcMemMgr_AllocateDSPMemory(&ImageRawBuf, STLDEC_RAW_SIZE);
        }
        // Customize the handler settings
        codecCfg.RawBuf = ImageRawBuf;
        AmbaPrint("[Applib - StillDec] <Init> %x -> %x", ImageRawBuf, codecCfg.RawBuf);
        codecCfg.RawBufSize = STLDEC_RAW_SIZE;
        codecCfg.CbCodecEvent = AppLibStillSingle_CodecCB;
        // Get a free codec handler, and configure the initial settings
        if (AmpStillDec_Create(&codecCfg, &StlDecHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to create a codec handler.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Create decoder manager
    {
        AMP_DEC_PIPE_CFG_s pipeCfg;             // Decoder manager config
        // Get the default decoder manager settings
        if (AmpDec_GetDefaultCfg(&pipeCfg) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to get the default decoder manager settings.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
        // Customize the manager settings
        pipeCfg.Decoder[0] = StlDecHdlr;
        pipeCfg.NumDecoder = 1;
        pipeCfg.Type = AMP_DEC_STL_PIPE;
        // Create a decoder manager, and insert the codec handler into the manager
        if (AmpDec_Create(&pipeCfg, &DecPipeHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to create a decoder manager.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Activate decoder manager
    // Activate the decoder manager and all the codec handlers in the manager
    if (AmpDec_Add(DecPipeHdlr) != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to activate the decoder manager.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }

    // Allocate image cache buffer
    if (ImageCacheBufOri == NULL) {
        if (AmpUtil_GetAlignedPool( // TODO: set width, height, num
                APPLIB_G_MMPL,
                (void**) &ImageCacheBuf,
                &ImageCacheBufOri,
                (IMAGE_CACHE_SIZE * 2 * IMAGE_CACHE_NUM),
                1 << 6 // Align 64
            ) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }

    // Allocate main buffer
    if (ImageMainLumaAddr == NULL) {
        /*if (AmpUtil_GetAlignedPool( // TODO: set width, height
                APPLIB_G_MMPL,
                (void**) &ImageMainLumaAddr,
                &ImageMainBufOri,
                (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 2),
                1 << 6 // Align 64
            ) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }*/
        AppLibComSvcMemMgr_AllocateDSPMemory(&ImageMainLumaAddr, (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 2));
    }

    // Create semaphore
    Rval = AmbaKAL_SemCreate(&SingleStillVoutSem, SINGLE_STILL_VOUT_SEM_NUM);
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to create semaphore (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    // Create event flag
    Rval = AmbaKAL_EventFlagCreate(&LoadEventFlag);
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to create event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagClear(&LoadEventFlag, 0xFFFFFFFF); // Clear all
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to clear event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagCreate(&LoadEventFlagValid);
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to create event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagClear(&LoadEventFlagValid, 0xFFFFFFFF); // Clear all
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to clear event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagCreate(&ShowEventFlag);
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to create event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagClear(&ShowEventFlag, 0xFFFFFFFF); // Clear all
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to clear event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagCreate(&ShowEventFlagValid);
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to create event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }
    Rval = AmbaKAL_EventFlagClear(&ShowEventFlagValid, 0xFFFFFFFF); // Clear all
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to clear event flag (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    // Initialize Vout buffer manager
    if (AppLibVoutBuffer_Init(&G_VoutBufMgr, AppLibStillSingle_DisplayEndCB, AppLibStillSingle_DisplayAllChanEndCB) != 0) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to initialize Vout buffer manager.", __FUNCTION__, __LINE__);
        goto ReturnError;
    }

ReturnSuccess:
    // Set flag
    ApplibStillDecInitFlag = 1;
    AmbaPrint("[Applib - StillDec] <Init> End");
    return 0; // Success

ReturnError:
    // Undo previous actions
    if (AppLibStillSingle_Deinit() != AMP_OK) {
        AmbaPrint("[Applib - StillDec] %s:%u Failed to undo actions.", __FUNCTION__, __LINE__);
    }
    // Reset flag
    ApplibStillDecInitFlag = 0;
    AmbaPrint("[Applib - StillDec] <Init> End");
    return -1; // Error
}

static int AppLibStillSingle_Rescale(APPLIB_STILL_SINGLE_s *StillInfo)
{
    int Rval = 0;                                                               // Function call return value

#define TO_REAL_PIXEL(x, realSize) (((x)*(realSize))/10000)

    // Start decoder
    if (AppLibStillSingle_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibStillSingle_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Send a dummy message before sending rescale messages of all channels
    {
        APPLIB_STILL_TASK_MSG_s PreRescaleMsg;
        // Clear message
        SET_ZERO(PreRescaleMsg);
        // Configure dummy message
        PreRescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PreRescaleMsg.StlDecHdlr = StlDecHdlr;
        PreRescaleMsg.BeginCB = NULL; // Do nothing
        PreRescaleMsg.EndCB = AppLibStillSingle_RescaleAllChanBeginCB;
        PreRescaleMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
        PreRescaleMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PreRescaleMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_RESCALE_ALL_CHAN_BEGIN;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PreRescaleMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    {
        APPLIB_DISP_SIZE_CAL_s Cal[DISP_CH_NUM];
        APPLIB_DISP_SIZE_CAL_ARRAY_s CalPointerArr;
        AMP_ROTATION_e Rotate = StillInfo->ImageRotate;
        UINT8 LumaGain = 128; // 128: original luma
        UINT8 AutoAdjust = 1; // Adjust the position of image when it's been shifted too much
        UINT32 ChannelIdx; // Index of a channel in channel array
        UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN

        // Clear calculation
        SET_ZERO(Cal);
        // Clear calculation parameter array
        SET_ZERO(CalPointerArr);
        // Set calculation parameter
        for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
            if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
                AmbaPrint("[Applib - StillDec] <Show> %s:%u", __FUNCTION__, __LINE__);
                goto ReturnError;
            }
            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) != 0) {
                CalPointerArr.Cal[ChannelIdx] = &(Cal[ChannelIdx]);
                Cal[ChannelIdx].ImageAr = StlDecRes.ImageAR;
                Cal[ChannelIdx].ImageWidth = StlDecRes.SingleStillVoutMsgOutput.ImageWidth;
                Cal[ChannelIdx].ImageHeight = StlDecRes.SingleStillVoutMsgOutput.ImageHeight;
                Cal[ChannelIdx].ImageRotate = Rotate;
                Cal[ChannelIdx].DeviceAr = DeviceAR[ChannelIdx];
#ifdef CONFIG_APP_ARD
                if((VoutChannel == DISP_CH_DCHAN)&&(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90)){
                    Cal[ChannelIdx].ImageRotate = AMP_ROTATE_90;
                    Cal[ChannelIdx].DeviceAr = AppLibSysLcd_GetDispAR(LCD_CH_DCHAN);
                }
#endif
                if (ApplibVoutBuffer_GetVoutWidth(&G_VoutBufMgr, VoutChannel, &(Cal[ChannelIdx].DeviceWidth)) != 0) {
                    goto ReturnError;
                }
                if (ApplibVoutBuffer_GetVoutHeight(&G_VoutBufMgr, VoutChannel, &(Cal[ChannelIdx].DeviceHeight)) != 0) {
                    goto ReturnError;
                }
                if (VoutChannel == DISP_CH_DCHAN) { // TODO: Modify StillInfo
                    Cal[ChannelIdx].WindowWidth = TO_REAL_PIXEL(StillInfo->AreaDchanDisplayMain.Width, Cal[ChannelIdx].DeviceWidth);
                    Cal[ChannelIdx].WindowHeight = TO_REAL_PIXEL(StillInfo->AreaDchanDisplayMain.Height, Cal[ChannelIdx].DeviceHeight);
                } else {
                    Cal[ChannelIdx].WindowWidth = TO_REAL_PIXEL(StillInfo->AreaFchanDisplayMain.Width, Cal[ChannelIdx].DeviceWidth);
                    Cal[ChannelIdx].WindowHeight = TO_REAL_PIXEL(StillInfo->AreaFchanDisplayMain.Height, Cal[ChannelIdx].DeviceHeight);
                }
                Cal[ChannelIdx].MagFactor = StillInfo->MagFactor;
                // Don't have to set ImageShiftX, ImageShiftY
                Cal[ChannelIdx].AutoAdjust = AutoAdjust;
            }
        }
        // Calculate multiple channel
        if (Applib_DisplaySizeCal_MultiChannel(&CalPointerArr, StillInfo->ImageShiftX, StillInfo->ImageShiftY,
            &(StillInfo->OutputRealImageShiftX), &(StillInfo->OutputRealImageShiftY)) != 0) {
            goto ReturnError;
        }

        // Rescale the data to Vout buffer.
        for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
            if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
                goto ReturnError;
            }

            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) != 0) {
                // Send a dummy message before rescaling
                {
                    APPLIB_STILL_TASK_MSG_s PreRescaleMsg;
                    // Clear message
                    SET_ZERO(PreRescaleMsg);
                    // Configure dummy message
                    PreRescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
                    PreRescaleMsg.StlDecHdlr = StlDecHdlr;
                    PreRescaleMsg.BeginCB = NULL; // Do nothing
                    PreRescaleMsg.EndCB = AppLibStillSingle_RescaleBeginCB;
                    PreRescaleMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                    PreRescaleMsg.Message.Dummy.Channel = VoutChannel;
                    PreRescaleMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_RESCALE_CHAN_BEGIN;
                    // Send message to Still Task
                    Rval = AppLibStillDec_SendVoutMsg(&PreRescaleMsg, AMBA_KAL_WAIT_FOREVER);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError; // Error
                    }
                }

                // Main picture: Rescale from main buffer to vout buffer
                {
                    APPLIB_STILL_TASK_MSG_s RescaleMsg; // Message to Still Task
                    AMP_YUV_BUFFER_s SrcBuf;
                    AMP_YUV_BUFFER_s DestBuf;
                    // Clear message
                    SET_ZERO(RescaleMsg);
                    // Clear buf
                    SET_ZERO(SrcBuf);
                    SET_ZERO(DestBuf);
                    // Configure rescale source buffer
                    SrcBuf.ColorFmt = StlDecRes.SingleStillVoutMsgOutput.ImageColorFmt;
                    SrcBuf.LumaAddr = getMainYAddr();
                    SrcBuf.ChromaAddr = getMainUVAddr();
                    SrcBuf.Width = StlDecRes.SingleStillVoutMsgOutput.ImageWidth;
                    SrcBuf.Height = StlDecRes.SingleStillVoutMsgOutput.ImageHeight;
                    SrcBuf.Pitch = StlDecRes.SingleStillVoutMsgOutput.ImagePitch;
                    SrcBuf.AOI.X = (UINT32) Cal[ChannelIdx].OutputSrcImgOffsetX;
                    SrcBuf.AOI.Y = (UINT32) Cal[ChannelIdx].OutputSrcImgOffsetY;
                    SrcBuf.AOI.Width = (UINT32) Cal[ChannelIdx].OutputSrcImgWidth;
                    SrcBuf.AOI.Height = (UINT32) Cal[ChannelIdx].OutputSrcImgHeight;
                    // TODO: Check the size of OutputSrcImgWidth and OutputSrcImgHeight.
                    //       If one of them is too small (eg. less than 100), rescale it to a larger buffer
                    //       and replace lcdSrc by the new buffer.
#if 0
#define MULTI_STAGE_SIZE_THRESHOLD (100) /** Minimum size of a buffer that does NOT need multistage buffering. */
                    if ((Cal[ChannelIdx].OutputSrcImgWidth <= MULTI_STAGE_SIZE_THRESHOLD) ||
                        (Cal[ChannelIdx].OutputSrcImgHeight <= MULTI_STAGE_SIZE_THRESHOLD)) {
                        AMP_YUV_BUFFER_s SourceBuf, MiddleBuf;
#define APPLIB_FLOOR(x)     ((UINT32)(x))
#define APPLIB_CEILING(x)   ((x)-(UINT32)(x) > 0 ? (UINT32)((x) + 1) : (UINT32)(x))
#define TO_DOUBLE(x)        ((double)(x))
                        SourceBuf = SrcBuf[ChannelIdx];
                        SourceBuf.AOI.X = APPLIB_FLOOR(Cal[ChannelIdx].OutputSrcImgOffsetX);
                        SourceBuf.AOI.Y = APPLIB_FLOOR(Cal[ChannelIdx].OutputSrcImgOffsetY);
                        SourceBuf.AOI.Width = MIN(APPLIB_CEILING(Cal[ChannelIdx].OutputSrcImgOffsetX + Cal[ChannelIdx].OutputSrcImgWidth), SrcBuf[ChannelIdx].Width) - SourceBuf.AOI.X;
                        SourceBuf.AOI.Height = MIN(APPLIB_CEILING(Cal[ChannelIdx].OutputSrcImgOffsetY + Cal[ChannelIdx].OutputSrcImgHeight), SrcBuf[ChannelIdx].Height) - SourceBuf.AOI.Y;

AmbaPrintColor(RED,"(%f %f %f %f) -> (%d %d %d %d)",
Cal[ChannelIdx].OutputSrcImgOffsetX,Cal[ChannelIdx].OutputSrcImgOffsetY,Cal[ChannelIdx].OutputSrcImgWidth,Cal[ChannelIdx].OutputSrcImgHeight,
SourceBuf.AOI.X,SourceBuf.AOI.Y,SourceBuf.AOI.Width,SourceBuf.AOI.Height);
                        MiddleBuf.ColorFmt = IMAGE_CACHE_COLOR_FORMAT; // TODO: Given by caller
                        MiddleBuf.LumaAddr = getCacheYAddr(1); // TODO: Given by caller
                        MiddleBuf.ChromaAddr = getCacheUVAddr(1); // TODO: Given by caller
                        // No need to retain original aspect ratio here
                        MiddleBuf.Width = IMAGE_CACHE_WIDTH; // TODO: Given by caller
                        MiddleBuf.Height = IMAGE_CACHE_HEIGHT; // TODO: Given by caller
                        MiddleBuf.Pitch = IMAGE_CACHE_PITCH; // TODO: Given by caller
                        MiddleBuf.AOI.X = 0;
                        MiddleBuf.AOI.Y = 0;
                        MiddleBuf.AOI.Width = MiddleBuf.Width;
                        MiddleBuf.AOI.Height = MiddleBuf.Height;

                        // Set Step ID
                        StepIdx = 0;
                        // Configure rescale message
                        RescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_RESCALE;
                        RescaleMsg.StlDecHdlr = StlDecHdlr;
                        RescaleMsg.BeginCB = NULL; // Do nothing
                        RescaleMsg.EndCB = NULL; // Do nothing
                        RescaleMsg.Output = &SingleStillVoutMsgOutput;
                        RescaleMsg.Message.Rescale.ImageSrcBuffer = SrcBuf;
                        RescaleMsg.Message.Rescale.ImageDestBuffer = MiddleBuf;
                        RescaleMsg.Message.Rescale.ImageRotate = AMP_ROTATE_0;
                        RescaleMsg.Message.Rescale.LumaGain = 128;
                        RescaleMsg.Message.Rescale.Channel = VoutChannel;
                        // Set valid
                        RescaleMsgIsValid[ChannelIdx][StepIdx] = 1;
                        // Send message later

                        // Change source buffer of the second stage to the middle buffer
                        lcdSrc = MiddleBuf;
                        MiddleBuf.AOI.X = (UINT32)((Cal[ChannelIdx].OutputSrcImgOffsetX - TO_DOUBLE(SourceBuf.AOI.X)) * TO_DOUBLE(MiddleBuf.Width) / SourceBuf.AOI.Width);
                        MiddleBuf.AOI.Y = (UINT32)((Cal[ChannelIdx].OutputSrcImgOffsetY - TO_DOUBLE(SourceBuf.AOI.Y)) * TO_DOUBLE(MiddleBuf.Height) / SourceBuf.AOI.Height);
                        MiddleBuf.AOI.Width = (UINT32)(Cal[ChannelIdx].OutputSrcImgWidth * TO_DOUBLE(MiddleBuf.Width) / SourceBuf.AOI.Width);
                        MiddleBuf.AOI.Height = (UINT32)(Cal[ChannelIdx].OutputSrcImgHeight * TO_DOUBLE(MiddleBuf.Height) / SourceBuf.AOI.Height);
                    }
#endif
                    Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, StlDecRes.VoutBufferRequestID, &DestBuf);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> Get Vout buffer failed!");
                        goto ReturnError;
                    }
                    if (VoutChannel == DISP_CH_DCHAN) { // TODO: Modify StillInfo
                        DestBuf.AOI.X = Cal[ChannelIdx].OutputOffsetX + TO_REAL_PIXEL(StillInfo->AreaDchanDisplayMain.X, DestBuf.Width);
                        DestBuf.AOI.Y = Cal[ChannelIdx].OutputOffsetY + TO_REAL_PIXEL(StillInfo->AreaDchanDisplayMain.Y, DestBuf.Height);
                    } else {
                        DestBuf.AOI.X = Cal[ChannelIdx].OutputOffsetX + TO_REAL_PIXEL(StillInfo->AreaFchanDisplayMain.X, DestBuf.Width);
                        DestBuf.AOI.Y = Cal[ChannelIdx].OutputOffsetY + TO_REAL_PIXEL(StillInfo->AreaFchanDisplayMain.Y, DestBuf.Height);
                    }
                    DestBuf.AOI.Width = Cal[ChannelIdx].OutputWidth;
                    DestBuf.AOI.Height = Cal[ChannelIdx].OutputHeight;

                    // Get information for calculating PIP frame
                    StlDecRes.PipCal[ChannelIdx].ImageSrcWidth = SrcBuf.Width;
                    StlDecRes.PipCal[ChannelIdx].ImageSrcHeight = SrcBuf.Height;
                    StlDecRes.PipCal[ChannelIdx].ImageSrcAOI= SrcBuf.AOI;

                    // Configure rescale message
                    RescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_RESCALE;
                    RescaleMsg.StlDecHdlr = StlDecHdlr;
                    RescaleMsg.BeginCB = NULL; // Do nothing
                    RescaleMsg.EndCB = NULL; // Do nothing
                    RescaleMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                    RescaleMsg.Message.Rescale.ImageSrcBuffer = SrcBuf;
                    RescaleMsg.Message.Rescale.ImageDestBuffer = DestBuf;
                    RescaleMsg.Message.Rescale.ImageRotate = Rotate;
                    RescaleMsg.Message.Rescale.LumaGain = LumaGain;
                    RescaleMsg.Message.Rescale.Channel = VoutChannel;

#ifdef CONFIG_APP_ARD
                    if((VoutChannel == DISP_CH_DCHAN)&&(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90)){
                        RescaleMsg.Message.Rescale.ImageRotate = AMP_ROTATE_90;
                        #if 1
                        if(strcmp(StlDecRes.LoadStillInfo.Filename,"ROMFS:\\logo.jpg")==0){
                            //Logo use full screen
                            DestBuf.AOI.X = DestBuf.AOI.Y = 0;
                            DestBuf.AOI.Width = DestBuf.Width;
                            DestBuf.AOI.Height = DestBuf.Height;
                            AmbaPrintColor(GREEN,"logo full");
                        }
                        #endif
                        if(DestBuf.AOI.Y<4) {
                            DestBuf.AOI.Y = 4;
                            DestBuf.AOI.Height -= DestBuf.AOI.Y;
                         }
                        DestBuf.AOI.Height &= 0xFFFC;
                        RescaleMsg.Message.Rescale.ImageDestBuffer = DestBuf;
                    }
#endif
                    // Send message to Still Task
                    Rval = AppLibStillDec_SendVoutMsg(&RescaleMsg, AMBA_KAL_WAIT_FOREVER);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError; // Error
                    }
                }

                // PIP area: Rescale from cache to Vout buffer
                // Since PIP is usually a small area showing the whole image, rescaling from
                // cache buffer (with rather smaller size than main buffer) can reduce
                // the computational cost at the expense of negligible loss of the sharpness.
                {
                    APPLIB_STILL_TASK_MSG_s PipRescaleMsg; // Message to Still Task
                    AMP_YUV_BUFFER_s PipSrcBuf;
                    AMP_YUV_BUFFER_s PipDestBuf;
                    AMP_ROTATION_e PipRotate = StillInfo->ImageRotate;
                    UINT8 PipLumaGain = 128; // 128: original luma
                    APPLIB_DISP_SIZE_CAL_s PipCal;
                    UINT8 PipAutoAdjust = 1; // PIP don't shift or zoom, so setting to 0 or 1 are equivalent.

                    if (AppLibStillSingle_IsShowPip(StillInfo, VoutChannel)) {
                        // Clear message
                        SET_ZERO(PipRescaleMsg);
                        // Clear buf
                        SET_ZERO(PipSrcBuf);
                        SET_ZERO(PipDestBuf);
                        // Clear calculation
                        SET_ZERO(PipCal);
                        // Set calculation parameter.
                        // Calculate the area in buffer to be displayed.
                        PipCal.ImageAr = StlDecRes.ImageAR;
                        PipCal.ImageWidth = IMAGE_CACHE_WIDTH;
                        PipCal.ImageHeight = IMAGE_CACHE_HEIGHT;
                        PipCal.ImageRotate = PipRotate;
                        PipCal.DeviceAr = DeviceAR[ChannelIdx];
                        if (ApplibVoutBuffer_GetVoutWidth(&G_VoutBufMgr, VoutChannel, &(PipCal.DeviceWidth)) != 0) {
                            goto ReturnError;
                        }
                        if (ApplibVoutBuffer_GetVoutHeight(&G_VoutBufMgr, VoutChannel, &(PipCal.DeviceHeight)) != 0) {
                            goto ReturnError;
                        }
                        if (VoutChannel == DISP_CH_DCHAN) { // TODO: Modify StillInfo
                            PipCal.WindowWidth = TO_REAL_PIXEL(StillInfo->AreaDchanPIP.Width, PipCal.DeviceWidth);
                            PipCal.WindowHeight = TO_REAL_PIXEL(StillInfo->AreaDchanPIP.Height, PipCal.DeviceHeight);
                        } else {
                            PipCal.WindowWidth = TO_REAL_PIXEL(StillInfo->AreaFchanPIP.Width, PipCal.DeviceWidth);
                            PipCal.WindowHeight = TO_REAL_PIXEL(StillInfo->AreaFchanPIP.Height, PipCal.DeviceHeight);
                        }
                        PipCal.MagFactor = MAGNIFICATION_FACTOR_BASE; // No magnification
                        PipCal.ImageShiftX = 0;
                        PipCal.ImageShiftY = 0;
                        PipCal.AutoAdjust = PipAutoAdjust;
                        Applib_DisplaySizeCal(&PipCal);

                        // Rescale the data to Vout buffer.
                        // Configure rescale source buffer.
                        PipSrcBuf.ColorFmt = IMAGE_CACHE_COLOR_FORMAT;
                        PipSrcBuf.LumaAddr = getCacheYAddr(0);
                        PipSrcBuf.ChromaAddr = getCacheUVAddr(0);
                        PipSrcBuf.Width = IMAGE_CACHE_WIDTH;
                        PipSrcBuf.Height = IMAGE_CACHE_HEIGHT;
                        PipSrcBuf.Pitch = IMAGE_CACHE_PITCH;
                        PipSrcBuf.AOI.X = (UINT32) PipCal.OutputSrcImgOffsetX;
                        PipSrcBuf.AOI.Y = (UINT32) PipCal.OutputSrcImgOffsetY;
                        PipSrcBuf.AOI.Width = (UINT32) PipCal.OutputSrcImgWidth;
                        PipSrcBuf.AOI.Height = (UINT32) PipCal.OutputSrcImgHeight;
                        // Configure rescale destination buffer
                        Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, StlDecRes.VoutBufferRequestID, &PipDestBuf);
                        if (Rval != 0) {
                            AmbaPrint("[Applib - StillDec] <Show> Get Vout buffer failed!");
                            goto ReturnError;
                        }
                        if (VoutChannel == DISP_CH_DCHAN) { // TODO: Modify StillInfo
                            PipDestBuf.AOI.X = PipCal.OutputOffsetX + TO_REAL_PIXEL(StillInfo->AreaDchanPIP.X, PipDestBuf.Width);
                            PipDestBuf.AOI.Y = PipCal.OutputOffsetY + TO_REAL_PIXEL(StillInfo->AreaDchanPIP.Y, PipDestBuf.Height);
                        } else {
                            PipDestBuf.AOI.X = PipCal.OutputOffsetX + TO_REAL_PIXEL(StillInfo->AreaFchanPIP.X, PipDestBuf.Width);
                            PipDestBuf.AOI.Y = PipCal.OutputOffsetY + TO_REAL_PIXEL(StillInfo->AreaFchanPIP.Y, PipDestBuf.Height);
                        }
                        PipDestBuf.AOI.Width = PipCal.OutputWidth;
                        PipDestBuf.AOI.Height = PipCal.OutputHeight;

                        // Get information for calculating PIP frame
                        StlDecRes.PipCal[ChannelIdx].ImagePipAOI = PipDestBuf.AOI;
                        StlDecRes.PipCal[ChannelIdx].ImageRotate = StillInfo->ImageRotate;

                        // Configure rescale message
                        PipRescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_RESCALE;
                        PipRescaleMsg.StlDecHdlr = StlDecHdlr;
                        PipRescaleMsg.BeginCB = NULL; // Do nothing
                        PipRescaleMsg.EndCB = NULL; // Do nothing
                        PipRescaleMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                        PipRescaleMsg.Message.Rescale.ImageSrcBuffer = PipSrcBuf;
                        PipRescaleMsg.Message.Rescale.ImageDestBuffer = PipDestBuf;
                        PipRescaleMsg.Message.Rescale.ImageRotate = PipRotate;
                        PipRescaleMsg.Message.Rescale.LumaGain = PipLumaGain;
                        PipRescaleMsg.Message.Rescale.Channel = VoutChannel;
                        // Send message to Still Task
                        Rval = AppLibStillDec_SendVoutMsg(&PipRescaleMsg, AMBA_KAL_WAIT_FOREVER);
                        if (Rval != 0) {
                            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                            goto ReturnError; // Error
                        }
                    }
                }

                // Send a dummy message after rescaling
                {
                    APPLIB_STILL_TASK_MSG_s PostRescaleMsg;
                    // Clear message
                    SET_ZERO(PostRescaleMsg);
                    // Configure dummy message
                    PostRescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
                    PostRescaleMsg.StlDecHdlr = StlDecHdlr;
                    PostRescaleMsg.BeginCB = NULL; // Do nothing
                    PostRescaleMsg.EndCB = AppLibStillSingle_RescaleEndCB;
                    PostRescaleMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                    PostRescaleMsg.Message.Dummy.Channel = VoutChannel;
                    PostRescaleMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_RESCALE_CHAN_END;
                    // Send message to Still Task
                    Rval = AppLibStillDec_SendVoutMsg(&PostRescaleMsg, AMBA_KAL_WAIT_FOREVER);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError; // Error
                    }
                }
            }
        }
    }

    // Send a dummy message after sending rescale messages of all channels
    {
        APPLIB_STILL_TASK_MSG_s PostRescaleMsg;
        // Clear message
        SET_ZERO(PostRescaleMsg);
        // Configure dummy message
        PostRescaleMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PostRescaleMsg.StlDecHdlr = StlDecHdlr;
        PostRescaleMsg.BeginCB = NULL; // Do nothing
        PostRescaleMsg.EndCB = AppLibStillSingle_RescaleAllChanEndCB;
        PostRescaleMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
        PostRescaleMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PostRescaleMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_RESCALE_ALL_CHAN_END;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PostRescaleMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    return 0; // Success

ReturnError:
    // Abort all messages sent in this function. However all callback functions will still be invoked to avoid system hanging.
    StlDecRes.SingleStillVoutMsgOutput.ResultCode = APPLIB_STILL_TASK_MSG_QUEUE_ABORT;
    return -1; // Error
}

static int AppLibStillSingle_DisplayVout(void)
{
    int Rval = 0;                               // Function call return value

    // Start decoder
    // If seamless is enabled and the decoder is not started yet, LCD buffer will be updated by DSP command.
    if (AppLibStillSingle_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibStillSingle_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Send a dummy message before sending display messages of all channels
    {
        APPLIB_STILL_TASK_MSG_s PreDisplayMsg;
        // Clear message
        SET_ZERO(PreDisplayMsg);
        // Configure dummy message
        PreDisplayMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PreDisplayMsg.StlDecHdlr = StlDecHdlr;
        PreDisplayMsg.BeginCB = NULL; // Do nothing
        PreDisplayMsg.EndCB = AppLibStillSingle_DisplayAllChanBeginCB;
        PreDisplayMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
        PreDisplayMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PreDisplayMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_DISPLAY_ALL_CHAN_BEGIN;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PreDisplayMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    {
        UINT32 ChannelIdx; // Index of a channel in channel array
        UINT32 VoutChannel; // LCD = DISP_CH_DCHAN; TV = DISP_CH_FCHAN
        // Display Vout buffer
        for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
            if (Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel) != 0) {
                goto ReturnError;
            }

            if (ApplibVoutBuffer_IsVoutReady(&G_VoutBufMgr, VoutChannel) != 0) {
                // Send a dummy message before displaying
                {
                    APPLIB_STILL_TASK_MSG_s PreDisplayMsg;
                    // Clear message
                    SET_ZERO(PreDisplayMsg);
                    // Configure dummy message
                    PreDisplayMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
                    PreDisplayMsg.StlDecHdlr = StlDecHdlr;
                    PreDisplayMsg.BeginCB = NULL; // Do nothing
                    PreDisplayMsg.EndCB = AppLibStillSingle_DisplayBeginCB;
                    PreDisplayMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                    PreDisplayMsg.Message.Dummy.Channel = VoutChannel;
                    PreDisplayMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_DISPLAY_CHAN_BEGIN;
                    // Send message to Still Task
                    Rval = AppLibStillDec_SendVoutMsg(&PreDisplayMsg, AMBA_KAL_WAIT_FOREVER);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError; // Error
                    }
                }

                {
                    APPLIB_STILL_TASK_MSG_s DisplayMsg; // Display messages.
                    AMP_YUV_BUFFER_s VoutBuf;
                    // Clear message
                    SET_ZERO(DisplayMsg);
                    // Clear buffer
                    SET_ZERO(VoutBuf);

                    if (ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, StlDecRes.VoutBufferRequestID, &VoutBuf) != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Get Vout buffer failed (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError;
                    }

                    VoutBuf.AOI.X = 0;
                    VoutBuf.AOI.Y = 0;
                    VoutBuf.AOI.Width = VoutBuf.Width;
                    VoutBuf.AOI.Height = VoutBuf.Height;

                    // Configure display message
                    DisplayMsg.MessageType = APPLIB_STILL_TASK_MSG_DISPLAY;
                    DisplayMsg.StlDecHdlr = StlDecHdlr;
                    DisplayMsg.BeginCB = NULL; // Do nothing
                    DisplayMsg.EndCB = NULL; // Do nothing
                    DisplayMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                    DisplayMsg.Message.Display.VoutBuffer = VoutBuf;
                    DisplayMsg.Message.Display.Channel = VoutChannel;
                    // Send message to Still Task
                    Rval = AppLibStillDec_SendVoutMsg(&DisplayMsg, AMBA_KAL_WAIT_FOREVER);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError; // Error
                    }
                }

                // Send a dummy message after displaying
                {
                    APPLIB_STILL_TASK_MSG_s PostDisplayMsg;
                    // Clear message
                    SET_ZERO(PostDisplayMsg);
                    // Configure dummy message
                    PostDisplayMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
                    PostDisplayMsg.StlDecHdlr = StlDecHdlr;
                    PostDisplayMsg.BeginCB = NULL; // Do nothing
                    // Since the display function of DSP (AmpStillDec_Display) is a non-blocking function, invoke "WaitCB" instead of "EndCB"
                    PostDisplayMsg.EndCB = AppLibStillSingle_DisplayWaitCB;
                    PostDisplayMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
                    PostDisplayMsg.Message.Dummy.Channel = VoutChannel;
                    PostDisplayMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_DISPLAY_CHAN_END;
                    // Send message to Still Task
                    Rval = AppLibStillDec_SendVoutMsg(&PostDisplayMsg, AMBA_KAL_WAIT_FOREVER);
                    if (Rval != 0) {
                        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
                        goto ReturnError; // Error
                    }
                }
            }
        }
    }

    // Send a dummy message after sending display messages of all channels
    {
        APPLIB_STILL_TASK_MSG_s PostDisplayMsg;
        // Clear message
        SET_ZERO(PostDisplayMsg);
        // Configure dummy message
        PostDisplayMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PostDisplayMsg.StlDecHdlr = StlDecHdlr;
        PostDisplayMsg.BeginCB = NULL; // Do nothing
        // Since the display function of DSP (AmpStillDec_Display) is a non-blocking function, invoke "WaitCB" instead of "EndCB"
        PostDisplayMsg.EndCB = AppLibStillSingle_DisplayAllChanWaitCB;
        PostDisplayMsg.Output = &StlDecRes.SingleStillVoutMsgOutput;
        PostDisplayMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PostDisplayMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_DISPLAY_ALL_CHAN_END;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PostDisplayMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    return 0; // Success

ReturnError:
    // Abort all messages sent in this function. However all callback functions will still be invoked to avoid system hanging.
    StlDecRes.SingleStillVoutMsgOutput.ResultCode = APPLIB_STILL_TASK_MSG_QUEUE_ABORT;
    return -1; // Error
}

static int AppLibStillSingle_Load_Internal(APPLIB_STILL_DECODE_RESOURCE_s *StlDecResource)
{
    int Rval = 0;                                   // Function call return value

    // TODO: check if need to decode. First version, decode every file.

    // Assume the decoder is initialized

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Load> Load begin");
#endif

    // Send a dummy message before sending feed message(s)
    {
        APPLIB_STILL_TASK_MSG_s PreFeedMsg;
        // Clear message
        SET_ZERO(PreFeedMsg);
        // Configure dummy message
        PreFeedMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PreFeedMsg.StlDecHdlr = StlDecHdlr;
        PreFeedMsg.BeginCB = NULL; // Do nothing
        PreFeedMsg.EndCB = AppLibStillSingle_FeedBeginCB;
        PreFeedMsg.Output = &(StlDecResource->SingleStillVoutMsgOutput);
        PreFeedMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PreFeedMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_FEED_ALL_BEGIN;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PreFeedMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    // Feed file
    {
        APPLIB_STILL_TASK_MSG_s FeedMsg;           // Message to Still Task
        // Clear message
        SET_ZERO(FeedMsg);
        // Configure feed message
        FeedMsg.MessageType = APPLIB_STILL_TASK_MSG_FEED;
        FeedMsg.StlDecHdlr = StlDecHdlr;
        FeedMsg.BeginCB = NULL; // Do nothing
        FeedMsg.EndCB = NULL; // Do nothing
        FeedMsg.Output = &(StlDecResource->SingleStillVoutMsgOutput);
        memcpy(FeedMsg.Message.Feed.Filename, StlDecResource->LoadStillInfo.Filename, sizeof(FeedMsg.Message.Feed.Filename)); // Copy filename
        FeedMsg.Message.Feed.FileSource = StlDecResource->LoadStillInfo.FileSource;
        // No need to set VideoTimeShift
        FeedMsg.Message.Feed.ImageRawBuf = ImageRawBuf;
        FeedMsg.Message.Feed.ImageRawBufSize = STLDEC_RAW_SIZE;

        // Send a feed message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&FeedMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    // Send a dummy message after sending feed message(s)
    {
        APPLIB_STILL_TASK_MSG_s PostFeedMsg;
        // Clear message
        SET_ZERO(PostFeedMsg);
        // Configure dummy message
        PostFeedMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PostFeedMsg.StlDecHdlr = StlDecHdlr;
        PostFeedMsg.BeginCB = NULL; // Do nothing
        PostFeedMsg.EndCB = AppLibStillSingle_FeedEndCB;
        PostFeedMsg.Output = &(StlDecResource->SingleStillVoutMsgOutput);
        PostFeedMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PostFeedMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_FEED_ALL_END;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PostFeedMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    // Send a dummy message before sending decode message(s)
    {
        APPLIB_STILL_TASK_MSG_s PreDecodeMsg;
        // Clear message
        SET_ZERO(PreDecodeMsg);
        // Configure dummy message
        PreDecodeMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PreDecodeMsg.StlDecHdlr = StlDecHdlr;
        PreDecodeMsg.BeginCB = NULL; // Do nothing
        PreDecodeMsg.EndCB = AppLibStillSingle_DecodeBeginCB;
        PreDecodeMsg.Output = &(StlDecResource->SingleStillVoutMsgOutput);
        PreDecodeMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PreDecodeMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_DECODE_ALL_BEGIN;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PreDecodeMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    // Decode file
    {
        APPLIB_STILL_TASK_MSG_s DecodeMsg;         // Message to Still Task
        // Clear message
        SET_ZERO(DecodeMsg);
        // Configure decode message
        DecodeMsg.MessageType = APPLIB_STILL_TASK_MSG_DECODE;
        DecodeMsg.StlDecHdlr = StlDecHdlr;
        DecodeMsg.BeginCB = NULL; // Do nothing
        DecodeMsg.EndCB = NULL; // Do nothing
        DecodeMsg.Output = &StlDecResource->SingleStillVoutMsgOutput;
        DecodeMsg.Message.Decode.ImageDecBuf = ImageMainLumaAddr;
        DecodeMsg.Message.Decode.ImageDecBufSize = MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 2;
        DecodeMsg.Message.Decode.IsRescaleToCache = 1;
        DecodeMsg.Message.Decode.ImageCacheBuffer.ColorFmt = IMAGE_CACHE_COLOR_FORMAT;
        DecodeMsg.Message.Decode.ImageCacheBuffer.Width = IMAGE_CACHE_WIDTH;
        DecodeMsg.Message.Decode.ImageCacheBuffer.Height = IMAGE_CACHE_HEIGHT;
        DecodeMsg.Message.Decode.ImageCacheBuffer.Pitch = IMAGE_CACHE_PITCH;
        DecodeMsg.Message.Decode.ImageCacheBuffer.LumaAddr = getCacheYAddr(0);
        DecodeMsg.Message.Decode.ImageCacheBuffer.ChromaAddr = getCacheUVAddr(0);
        DecodeMsg.Message.Decode.ImageCacheBuffer.AOI.X = 0;
        DecodeMsg.Message.Decode.ImageCacheBuffer.AOI.Y = 0;
        DecodeMsg.Message.Decode.ImageCacheBuffer.AOI.Width = IMAGE_CACHE_WIDTH;
        DecodeMsg.Message.Decode.ImageCacheBuffer.AOI.Height = IMAGE_CACHE_HEIGHT;
        DecodeMsg.Message.Decode.ImageCacheRotate = AMP_ROTATE_0;

        // Clear cache buffer before rescaling to cache
        if (DecodeMsg.Message.Decode.IsRescaleToCache != 0) {
            UINT32 ImageCacheSize;
            // Set cache to black color
            ImageCacheSize = DecodeMsg.Message.Decode.ImageCacheBuffer.Pitch * DecodeMsg.Message.Decode.ImageCacheBuffer.Height;
            switch (DecodeMsg.Message.Decode.ImageCacheBuffer.ColorFmt) {
                case AMP_YUV_420:
                    AppLib_SetYuvBuf_Black(
                            DecodeMsg.Message.Decode.ImageCacheBuffer.LumaAddr,
                            DecodeMsg.Message.Decode.ImageCacheBuffer.ChromaAddr,
                            ImageCacheSize,
                            (ImageCacheSize >> 1));
                    break;
                default:
                    AppLib_SetYuvBuf_Black(
                            DecodeMsg.Message.Decode.ImageCacheBuffer.LumaAddr,
                            DecodeMsg.Message.Decode.ImageCacheBuffer.ChromaAddr,
                            ImageCacheSize,
                            ImageCacheSize);
                    break;
            }
        }

        // Send a decode message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&DecodeMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    // Send a dummy message after sending decode message(s)
    {
        APPLIB_STILL_TASK_MSG_s PostDecodeMsg;
        // Clear message
        SET_ZERO(PostDecodeMsg);
        // Configure dummy message
        PostDecodeMsg.MessageType = APPLIB_STILL_TASK_MSG_DUMMY;
        PostDecodeMsg.StlDecHdlr = StlDecHdlr;
        PostDecodeMsg.BeginCB = NULL; // Do nothing
        PostDecodeMsg.EndCB = AppLibStillSingle_DecodeEndCB;
        PostDecodeMsg.Output = &(StlDecResource->SingleStillVoutMsgOutput);
        PostDecodeMsg.Message.Dummy.Channel = DISP_CH_DUAL; // All channel
        PostDecodeMsg.Message.Dummy.EventID = APPLIB_STILL_TASK_EVENT_DECODE_ALL_END;
        // Send message to Still Task
        Rval = AppLibStillDec_SendVoutMsg(&PostDecodeMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            goto ReturnError; // Error
        }
    }

    return 0; // Success

ReturnError:
    return -1; // Error
}

int AppLibStillSingle_Load(APPLIB_STILL_FILE_s *StillFile)
{
    int Rval = 0;                                   // Function call return value
    UINT32 EventFlag = 0;

#define APPLIB_INVALID_WAIT_EVENT_ID (0)

    // Initialize output value
    if (StillFile->OutputWaitEventID != NULL) {
        *(StillFile->OutputWaitEventID) = APPLIB_INVALID_WAIT_EVENT_ID;
    }

    // Check initialization
    if (isStillDecInitialized() == 0) {
        AmbaPrint("[Applib - StillDec] <Load> Init first!");
        goto ReturnError; // Error
    }

    // Start decoder (before decode message)
    if (AppLibStillSingle_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibStillSingle_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Take semaphore
    Rval = AppLibStillSingle_SemTake(SINGLE_STILL_TIMEOUT);
    if (Rval != 0) {
        if (Rval == TX_NO_INSTANCE) { // TODO: Wait DSP definition
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Take semaphore timeout (%dms)!", __FUNCTION__, __LINE__, SINGLE_STILL_TIMEOUT);
        } else {
            AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to take semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        }
        goto ReturnError; // Error. Don't go to ReturnError_GiveSem. There's no semaphore to give.
    }

    // TODO: Take available event ID, and get event flag from event ID
    // Get next event flag
    CurrentLoadEventFlag = CurrentLoadEventFlag << 1;
    if (CurrentLoadEventFlag == 0) {
        CurrentLoadEventFlag = 1;
    }
    EventFlag = CurrentLoadEventFlag;

    // Clear flag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Load> Clear event flag (0x%08x) begin", EventFlag);
#endif
    Rval = AmbaKAL_EventFlagClear(&LoadEventFlag, EventFlag);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Load> Clear event flag (0x%08x) end (%d)", EventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to clear event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        goto ReturnError_GiveSem;
    }

    // Initialize still decode resource
    if (AppLibStillSingle_InitStillDecResource_BeforeLoad(&StlDecRes) != 0) {
        AmbaPrint("[Applib - StillDec] <Load> Failed to initialize still decode resource!");
        goto ReturnError_GiveSem;
    }

    // Configure still decode resource
    StlDecRes.LoadStillInfo = *StillFile;
    //memcpy(&StlDecRes.LoadStillInfo, StillFile, sizeof(APPLIB_STILL_FILE_s));
    StlDecRes.LoadEndEventFlag = EventFlag;

    // Send messages to load file
    if (AppLibStillSingle_Load_Internal(&StlDecRes) != 0) {
        goto ReturnError_GiveSem;
    }

    // Set event flag validity
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Load> Set event flag validity (0x%08x) begin", EventFlag);
#endif
    Rval = AmbaKAL_EventFlagGive(&LoadEventFlagValid, EventFlag);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Load> Set event flag validity (0x%08x) end (%d)", EventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to set event flag validity (%d)!", __FUNCTION__, __LINE__, Rval);
        goto ReturnError_GiveSem;
    }

    // Return flag
    if (StillFile->OutputWaitEventID != NULL) {
        *(StillFile->OutputWaitEventID) = EventFlag;
    }

    // Don't give semaphore here. Wait until callback of "decode end".

    return 0; // Success

ReturnError_GiveSem:
    // Give semaphore
    Rval = AppLibStillSingle_SemGive();
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Load> %s:%u Failed to give semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
    }

ReturnError:
    StlDecRes.LoadEndEventFlag = APPLIB_INVALID_WAIT_EVENT_ID;
    return -1; // Error
}

int AppLibStillSingle_WaitLoad(UINT32 WaitEventID)
{
    int Rval = 0;                                   // Function call return value
    UINT32 ActualEvent;                             // Output from AmbaKAL_EventFlagTake()
    UINT32 EventTimeout = AMBA_KAL_WAIT_FOREVER;    // Maximum waiting time (in ms) for an event
    UINT32 WaitEventFlag = 0;

    // Preliminary check
    // TODO: Check validity of WaitEventID, maximum and minimum

    // Get event flag by event ID
    if (AppLibStillSingle_GetEventFlag(WaitEventID, &WaitEventFlag) != 0) {
        AmbaPrint("[Applib - StillDec] <WaitLoad> %s:%u Failed to get event flag (Event ID = %u)!", __FUNCTION__, __LINE__, WaitEventID);
        return -1; // Error
    }

    // Check validity of WaitEventFlag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitLoad> Take event flag validity (0x%08x) begin", WaitEventFlag);
#endif
    // If the "Load" function executted successfully and there's only one task waiting for this flag, this flag in LoadEventFlagValid
    // should already been set. As a result, timeout option is set to AMBA_KAL_NO_WAIT.
    Rval = AmbaKAL_EventFlagTake(&LoadEventFlagValid, WaitEventFlag, AMBA_KAL_AND_CLEAR, &ActualEvent, AMBA_KAL_NO_WAIT);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitLoad> Take event flag validity (0x%08x) end (%d)", WaitEventFlag, Rval);
#endif
    if (Rval != 0) {
        // There might be multiple tasks waiting for the same event flag in rare situation, when load command are done so fast that the
        // event flags are run out. In this case we can only make sure those waiting functions will return instead of waiting for good.
        // We cannot add protection in waiting function since it's not necessarily been called by user.
        AmbaPrint("[Applib - StillDec] <WaitLoad> %s:%u EventID is not valid or collision of EventID occurred (%d)!", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }

    // Wait for the callback of "load end"
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitLoad> Take event Flag (0x%08x) begin", WaitEventFlag);
#endif
    Rval = AmbaKAL_EventFlagTake(&LoadEventFlag, WaitEventFlag, AMBA_KAL_AND_CLEAR, &ActualEvent, EventTimeout);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitLoad> Take event Flag (0x%08x) end (%d)", WaitEventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <WaitLoad> %s:%u Failed to take event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }

    // Actions after callback
    // Do NOT access result. It's no longer valid.
    //Rval = StlDecRes.SingleStillVoutMsgOutput.ResultCode;

    return 0; // Success
}

static int AppLibStillSingle_Show_Internal(APPLIB_STILL_DECODE_RESOURCE_s *StlDecResource)
{
    // Assume the decoder is initialized

    // Check preliminary steps
    if (AppLibStillSingle_IsStillDecDecoded(StlDecResource) == 0) {
        AmbaPrint("[Applib - StillDec] <Show> Load first!");
        goto ReturnError; // Error
    }

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Show begin");
#endif

    // Allocate Vout buffer
    if (AppLibStillSingle_AllocVout() != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to alloc Vout buffer.", __FUNCTION__, __LINE__);
        goto ReturnError; // Error
    }

    // Multiple buffering: Take a free and clean Vout buffer.
    if (ApplibVoutBuffer_TakeVoutBuffer_AllChannel(&G_VoutBufMgr, &StlDecResource->VoutBufferRequestID) != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <Show> Failed to take Vout buffer!");
        goto ReturnError; // Error
    }
    // Don't have to clean Vout buffer. "TakeVoutBuffer" has done the trick.
    //ApplibVoutBuffer_CleanNextVoutBuffer_AllChannel(&G_VoutBufMgr);
#ifdef TEST_MULTI_BUFFER
AmbaPrintColor(RED, "[Applib - StillDec] <Show> LCD After  take %d/%d",G_VoutBufMgr.VoutBuffer[0].NextBufIdx,G_VoutBufMgr.VoutBuffer[0].BufferNumber);
AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> TV  After  take %d/%d",G_VoutBufMgr.VoutBuffer[1].NextBufIdx,G_VoutBufMgr.VoutBuffer[1].BufferNumber);
#endif

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
                Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, StlDecResource->VoutBufferRequestID, &Buffer);
                if (Rval != 0) {
                    AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to get Vout buffer (%d)!", __FUNCTION__, __LINE__, Rval);
                    goto ReturnError; // Error
                }

                // Clean cache after writing the data in cacheable Vout buffer
                AppLibStillSingle_CleanCache(&Buffer);
            }
        }
    }

    // Send rescale messages
    if (AppLibStillSingle_Rescale(&(StlDecResource->ShowStillInfo)) != 0) {
        goto ReturnError; // Error
    }

    // Send display messages
    if (AppLibStillSingle_DisplayVout() != 0) {
        goto ReturnError; // Error
    }

    return 0; // Success

ReturnError:
    return -1; // Error. Don't give semaphore here!
}

int AppLibStillSingle_Show(APPLIB_STILL_SINGLE_s *StillInfo)
{
    int Rval = 0;                                   // Function call return value
    UINT32 EventFlag = 0;

    // Initialize output value
    if (StillInfo->OutputWaitEventID != NULL) {
        *(StillInfo->OutputWaitEventID) = APPLIB_INVALID_WAIT_EVENT_ID;
    }

    // Check initialization
    if (isStillDecInitialized() == 0) {
        AmbaPrint("[Applib - StillDec] <Show> Init first!");
        goto ReturnError; // Error
    }

    // Start decoder
    // If seamless is enabled and the decoder is not started yet, LCD buffer will be updated by DSP command.
    if (AppLibStillSingle_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibStillSingle_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Take semaphore
    // Make sure that a StlDecRes cannot be accessed by another show command until "show end"
    Rval = AppLibStillSingle_SemTake(SINGLE_STILL_TIMEOUT);
    if (Rval != 0) {
        if (Rval == TX_NO_INSTANCE) { // TODO: Wait DSP definition
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Take semaphore timeout (over %d ms)!", __FUNCTION__, __LINE__, SINGLE_STILL_TIMEOUT);
        } else {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to take semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        }
        goto ReturnError; // Error. Don't goto ReturnError_GiveSem. There's no semaphore to give.
    }

    // TODO: Take available event ID, and get event flag from event ID
    // Get next event flag
    CurrentShowEventFlag = CurrentShowEventFlag << 1;
    if (CurrentShowEventFlag == 0) {
        CurrentShowEventFlag = 1;
    }
    EventFlag = CurrentShowEventFlag;

    // Clear flag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> Clear event flag (0x%08x) begin", EventFlag);
#endif
    Rval = AmbaKAL_EventFlagClear(&ShowEventFlag, EventFlag);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> Clear event flag (0x%08x) end (%d)", EventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to clear event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        goto ReturnError_GiveSem;
    }

    // Initialize still decode resource
    if (AppLibStillSingle_InitStillDecResource_BeforeShow(&StlDecRes) != 0) {
        AmbaPrint("[Applib - StillDec] <Show> Failed to initialize still decode resource!");
        goto ReturnError_GiveSem;
    }

    // Configure still decode resource
    StlDecRes.ShowStillInfo = *StillInfo;
    StlDecRes.ShowEndEventFlag = EventFlag;

    // Send messages to show image
    if (AppLibStillSingle_Show_Internal(&StlDecRes) != 0) {
        goto ReturnError_GiveSem;
    }

    // Set event flag validity
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> Set event flag validity (0x%08x) begin", EventFlag);
#endif
    Rval = AmbaKAL_EventFlagGive(&ShowEventFlagValid, EventFlag);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <Show> Set event flag validity (0x%08x) end (%d)", EventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to set event flag validity (%d)!", __FUNCTION__, __LINE__, Rval);
        goto ReturnError_GiveSem;
    }

    // Return flag
    if (StillInfo->OutputWaitEventID != NULL) {
        *(StillInfo->OutputWaitEventID) = EventFlag;
    }

    // NOTE: Don't give semaphore here! Wait until the callback of "show end".

    return 0; // Success

ReturnError_GiveSem:
    // Give semaphore
    Rval = AppLibStillSingle_SemGive();
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to give semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
    }

ReturnError:
    StlDecRes.ShowEndEventFlag = APPLIB_INVALID_WAIT_EVENT_ID;
    return -1;
}

int AppLibStillSingle_WaitShow(UINT32 WaitEventID)
{
    int Rval = 0;                                   // Function call return value
    UINT32 ActualEvent;                             // Output from AmbaKAL_EventFlagTake()
    UINT32 EventTimeout = AMBA_KAL_WAIT_FOREVER;    // Maximum waiting time (in ms) for an event
    UINT32 WaitEventFlag = 0;

    // Preliminary check
    // TODO: Check validity of WaitEventID, maximum and minimum

    // Get event flag by event ID
    if (AppLibStillSingle_GetEventFlag(WaitEventID, &WaitEventFlag) != 0) {
        AmbaPrint("[Applib - StillDec] <WaitShow> %s:%u Failed to get event flag! (Event ID = %u)", __FUNCTION__, __LINE__, WaitEventID);
        return -1; // Error
    }

    // Check validity of WaitEventFlag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitShow> Take Event flag validity (0x%08x) begin", WaitEventFlag);
#endif
    // If the "show" function executted successfully and there's only one task waiting for this flag, this flag in ShowEventFlagValid
    // should already been set. As a result, timeout option is set to AMBA_KAL_NO_WAIT.
    Rval = AmbaKAL_EventFlagTake(&ShowEventFlagValid, WaitEventFlag, AMBA_KAL_AND_CLEAR, &ActualEvent, AMBA_KAL_NO_WAIT);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitShow> Take Event flag validity (0x%08x) end (%d)", WaitEventFlag, Rval);
#endif
    if (Rval != 0) {
        // There might be multiple tasks waiting for the same event flag in rare situation, when show command are done so fast that the
        // event flags are run out. In this case we can only make sure those waiting functions will return instead of waiting for good.
        // We cannot add protection in waiting function since it's not necessarily been called by user.
        AmbaPrint("[Applib - StillDec] <WaitShow> %s:%u EventID is not valid or collision of EventID occurred (%d)!", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }

    // Wait for the callback of "show end"
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitShow> Take Event Flag (0x%08x) begin", WaitEventFlag);
#endif
    Rval = AmbaKAL_EventFlagTake(&ShowEventFlag, WaitEventFlag, AMBA_KAL_AND_CLEAR, &ActualEvent, EventTimeout);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] <WaitShow> Take Event Flag (0x%08x) end (%d)", WaitEventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <WaitShow> %s:%u Failed to take event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }

    // Actions after callback
    // Do NOT access result. It's no longer valid.
    //Rval = StlDecRes.SingleStillVoutMsgOutput.ResultCode;

    return 0; // Success
}

static int AppLibStillSingle_ClearScreen_Internal(APPLIB_STILL_DECODE_RESOURCE_s *StlDecResource)
{
    //int Rval = 0;                               // Function call return value

    // Assume the decoder is initialized

    // No need to load a file

#ifdef DEBUG_VOUT_TASK_CB_FLOW
    AmbaPrintColor(YELLOW, "[Applib - StillDec] <Show> Show begin");
#endif

    // Allocate Vout buffer
    if (AppLibStillSingle_AllocVout() != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to alloc Vout buffer.", __FUNCTION__, __LINE__);
        goto ReturnError; // Error
    }

    // Multiple buffering: Take a free and clean Vout buffer.
    if (ApplibVoutBuffer_TakeVoutBuffer_AllChannel(&G_VoutBufMgr, &StlDecResource->VoutBufferRequestID) != 0) {
        AmbaPrintColor(RED, "[Applib - StillDec] <Show> Failed to take Vout buffer!");
        goto ReturnError; // Error
    }

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
                Rval = ApplibVoutBuffer_GetVoutBuffer(&G_VoutBufMgr, VoutChannel, StlDecResource->VoutBufferRequestID, &Buffer);
                if (Rval != 0) {
                    AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to get Vout buffer (%d)!", __FUNCTION__, __LINE__, Rval);
                    goto ReturnError; // Error
                }

                // Clean cache after writing the data in cacheable Vout buffer
                AppLibStillSingle_CleanCache(&Buffer);
            }
        }
    }

    // Send display messages. Display an empty buffer.
    if (AppLibStillSingle_DisplayVout() != 0) {
        goto ReturnError; // Error
    }

    // NOTE: Don't give semaphore here! Wait until the callback of "show end".

    return 0; // Success

ReturnError:
    return -1; // Error
}

int AppLibStillSingle_ClearScreen(UINT32 *OutputWaitEventID)
{
    int Rval = 0;                                   // Function call return value
    UINT32 EventFlag = 0;

#define APPLIB_INVALID_WAIT_EVENT_ID (0)

    // Initialize output value
    if (OutputWaitEventID != NULL) {
        *OutputWaitEventID = APPLIB_INVALID_WAIT_EVENT_ID;
    }

    // Check initialization
    if (isStillDecInitialized() == 0) {
        AmbaPrint("[Applib - StillDec] <Show> Init first!");
        goto ReturnError; // Error
    }

    // Start decoder
    // If seamless is enabled and the decoder is not started yet, LCD buffer will be updated by DSP command.
    if (AppLibStillSingle_StillDecodeStart() != AMP_OK) {
        AmbaPrint("%s:%u AppLibStillSingle_StillDecodeStart failed", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Take semaphore
    // Make sure that a StlDecRes cannot be accessed by another show command until "show end"
    Rval = AppLibStillSingle_SemTake(SINGLE_STILL_TIMEOUT);
    if (Rval != 0) {
        if (Rval == TX_NO_INSTANCE) { // TODO: Wait DSP definition
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Take semaphore timeout (over %d ms)!", __FUNCTION__, __LINE__, SINGLE_STILL_TIMEOUT);
        } else {
            AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to take semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
        }
        goto ReturnError; // Error. Don't goto ReturnError_GiveSem. There's no semaphore to give.
    }

    // TODO: Take available event ID, and get event flag from event ID
    // Get next event flag
    CurrentShowEventFlag = CurrentShowEventFlag << 1;
    if (CurrentShowEventFlag == 0) {
        CurrentShowEventFlag = 1;
    }
    EventFlag = CurrentShowEventFlag;

    // Clear flag
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] Clear ShowEndEventFlag (0x%08x) begin", EventFlag);
#endif
    Rval = AmbaKAL_EventFlagClear(&ShowEventFlag, EventFlag);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] Clear ShowEndEventFlag (0x%08x) end (%d)", EventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to clear event flag (%d)!", __FUNCTION__, __LINE__, Rval);
        goto ReturnError_GiveSem;
    }

    // Initialize still decode resource
    if (AppLibStillSingle_InitStillDecResource_BeforeLoad(&StlDecRes) != 0) {
        AmbaPrint("[Applib - StillDec] <Show> Failed to initialize still decode resource!");
        goto ReturnError_GiveSem;
    }
    if (AppLibStillSingle_InitStillDecResource_BeforeShow(&StlDecRes) != 0) {
        AmbaPrint("[Applib - StillDec] <Show> Failed to initialize still decode resource!");
        goto ReturnError_GiveSem;
    }

    // Configure still decode resource
    StlDecRes.ShowEndEventFlag = EventFlag;

    // Send messages to show image
    if (AppLibStillSingle_ClearScreen_Internal(&StlDecRes) != 0) {
        goto ReturnError_GiveSem;
    }

    // Set event flag validity
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] Set event flag validity (0x%08x) begin", EventFlag);
#endif
    Rval = AmbaKAL_EventFlagGive(&ShowEventFlagValid, EventFlag);
#ifdef DEBUG_VOUT_TASK_EVENT_FLAG_FLOW
    AmbaPrintColor(BLUE, "[Applib - StillDec] Set event flag validity (0x%08x) end (%d)", EventFlag, Rval);
#endif
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to set event flag validity (%d)!", __FUNCTION__, __LINE__, Rval);
        goto ReturnError_GiveSem;
    }

    // Return flag
    if (OutputWaitEventID != NULL) {
        *OutputWaitEventID = EventFlag;
    }

    return 0; // Success

ReturnError_GiveSem:
    // Give semaphore
    Rval = AppLibStillSingle_SemGive();
    if (Rval != 0) {
        AmbaPrint("[Applib - StillDec] <Show> %s:%u Failed to give semaphore (%d)!", __FUNCTION__, __LINE__, Rval);
    }

ReturnError:
    StlDecRes.ShowEndEventFlag = APPLIB_INVALID_WAIT_EVENT_ID;
    return -1;
}

int AppLibStillSingle_WaitClearScreen(UINT32 WaitEventID)
{
    return AppLibStillSingle_WaitShow(WaitEventID);
}

int AppLibStillSingle_Stop(void)
{
    int ReturnValue = 0;
    if (DecPipeHdlr != NULL) {
        // Stop still decode
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - StillDec] %s:%u Failed to stop still (%d).", __FUNCTION__, __LINE__, ReturnValue);
            return -1; // Error
        }
        StillDecodeStartFlag = 0;
    } else {
        AmbaPrint("[Applib - StillDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
    }
    return ReturnValue;
}
int AppLibStillSingle_Deinit(void)
{
    AmbaPrint("[Applib - StillDec] <Deinit> Start");

    // In case this module is not completely initialized, continue the following steps even if isStillDecInitialized() == 0

    // Delete event flag
    AmbaKAL_EventFlagDelete(&LoadEventFlag);
    AmbaKAL_EventFlagDelete(&LoadEventFlagValid);
    AmbaKAL_EventFlagDelete(&ShowEventFlag);
    AmbaKAL_EventFlagDelete(&ShowEventFlagValid);

    // Delete semaphore
    AmbaKAL_SemDelete(&SingleStillVoutSem);

    StillDecodeStartFlag = 0;

    // Deinit still decoder manager
    if (DecPipeHdlr != NULL) {
        // Cannot "Stop" still decoder manager. Video only.
        //AmpDec_Stop(DecPipeHdlr);

        if (AmpDec_Remove(DecPipeHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to deactivate the still decoder manager.", __FUNCTION__, __LINE__);
        }

        if (AmpDec_Delete(DecPipeHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to deinit the still decoder manager.", __FUNCTION__, __LINE__);
        }
        DecPipeHdlr = NULL;
    }

    // Deinit still decoder
    if (StlDecHdlr != NULL) {
        if (AmpStillDec_Delete(StlDecHdlr) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to deinit the still decoder.", __FUNCTION__, __LINE__);
        }
        StlDecHdlr = NULL;
    }

    // Release raw buffer
    if (ImageRawBuf != NULL) {
        /*if (AmbaKAL_BytePoolFree(ImageRawBufOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to release the raw buffer.", __FUNCTION__, __LINE__);
        }*/
        AppLibComSvcMemMgr_FreeDSPMemory();
        ImageRawBuf = NULL;
    }

    // Don't release codec module here (AppLibStillDecModule_Deinit())

    // Don't release Vout buffer here

    // Don't delete decode task here (AppLibStillDec_DeinitTask)

    // Release cache buffer
    if (ImageCacheBufOri != NULL) {
        if (AmbaKAL_BytePoolFree(ImageCacheBufOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to release the cache buffer.", __FUNCTION__, __LINE__);
        }
        ImageCacheBufOri = NULL;
    }
    ImageCacheBuf = NULL;

    // Release main buffer
    if (ImageMainLumaAddr != NULL) {
        /*if (AmbaKAL_BytePoolFree(ImageMainBufOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Failed to release the main buffer.", __FUNCTION__, __LINE__);
        }*/
        AppLibComSvcMemMgr_FreeDSPMemory();
        ImageMainLumaAddr = NULL;
    }


    // Reset decode resource
    SET_ZERO(StlDecRes);

    // Set decoder flag
    ApplibStillDecInitFlag = 0;     // Not initialized

    AmbaPrint("[Applib - StillDec] <Deinit> End");
    return 0;
}

#ifdef CONFIG_APP_ARD
void AppLibStillSingle_VoutAr_Config(int config_param)
{
    UINT32 DispAr = 0;
    DispAr = AppLibSysLcd_GetDispAR(LCD_CH_DCHAN);
    if(config_param == 1)
    DeviceAR[0] = DispAr;
    else
    DeviceAR[0] = VAR_4x3;
}
#endif

