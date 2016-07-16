/**
 * @file src/app/connected/applib/inc/player/decode_utility/ApplibPlayer_Common.h
 *
 * Common functions used by player module in application Library
 *
 * History:
 *    2013/12/09 - [phcheng] Create file
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

/**
 * @defgroup ApplibPlayer_Common        ApplibPlayer_Common
 * @brief Common functions and definitions used by other modules.
 *
 * Composed of the following.\n
 * 1. General parameters such as limitations of magnification factor.\n
 * 2. Vout buffer.\n
 * 3. Still decode module.
 */

/**
 * @addtogroup ApplibPlayer_Common
 * @ingroup DecodeUtility
 * @{
 */

#ifndef _APPLIB_PLYR_COMMON_H_
#define _APPLIB_PLYR_COMMON_H_

#include <applib.h>
#include <display/ApplibDisplay.h>
//#include <mw.h>
//#include <common/common.h>
//#include <display/Display.h>

#define MAGNIFICATION_FACTOR_BASE   100     ///< Value of magnification factor indicating no magnification.
#define MAGNIFICATION_FACTOR_MIN    100     ///< Minimum of magnification factor.
#define MAGNIFICATION_FACTOR_MAX    1000    ///< Maximum of magnification factor.

#define MIN_VOUT_BUF_NUM (2)    ///< Define how many buffers at least can be used to hold Vout data in a Vout channel.
#define MAX_VOUT_BUF_NUM (16)   ///< Define how many buffers at most can be used to hold Vout data in a Vout channel.

/**
 * Still Display Descriptor.                            \n
 * Containing information needed to display an image
 * Including location, size, luma, and layer of
 * the window in which the image displayed.
 */
typedef struct _APPLIB_STILL_DISP_DESC_s_ {
    /** Display location and area of the window. */
    AMP_AREA_s Area;
    /**
     * Luma gain. Adjust the luminance values (brightness) of the image.    \n
     * LumaGain =   1 ~ 127, image become darker.                           \n
     * LumaGain = 128,       keep original luminance.                       \n
     * LumaGain = 129 ~ 255, image become brighter.
     */
    UINT8 LumaGain;
    /**
     * Layer of the window. Control the front-to-back layering of windows.  \n
     * Windows with smaller layers will be covered up by others
     * when overlapping occurs.
     */
    UINT8 Layer;
} APPLIB_STILL_DISP_DESC_s;

/**
 * Vout buffer array for multiple buffering.
 */
typedef struct _APPLIB_VOUT_BUFFER_ARRAY_s_ {
    /**
     * Number of Vout buffers holding Vout frame data (for multiple buffering). \n
     * MIN_VOUT_BUF_NUM <= BufferNumber <= MAX_VOUT_BUF_NUM
     */
    UINT8 BufferNumber;
    /**
     * Array of buffer address with (BufferNumber) elements.    \n
     * Address of Vout buffer before alignment.
     */
    UINT8* BufferAddrOri[MAX_VOUT_BUF_NUM];
    /**
     * Array of buffer address with (BufferNumber) elements.    \n
     * Address of Vout buffer after alignment.                  \n
     * It's also the luma address of Vout buffer.
     */
    UINT8* LumaAddr[MAX_VOUT_BUF_NUM];
    /**
     * Array of buffer address with (BufferNumber) elements.    \n
     * The chroma address of Vout buffer.
     */
    UINT8* ChromaAddr[MAX_VOUT_BUF_NUM];
    /**
     * Width (number of pixels) of Vout screen.     \n
     * Its also the width of each Vout buffer.
     */
    UINT32 Width;
    /**
     * Height (number of pixels) of Vout screen.    \n
     * Its also the height of each Vout buffer.
     */
    UINT32 Height;
    /**
     * Pitch of each Vout buffer.
     */
    UINT32 Pitch;
    /**
     * Vout buffer has been allocated.  \n
     * 0: Not allocated, 1: Allocated
     */
    UINT8 IsAllocated;
    /**
     * Unique ID of each buffer for identifying request.
     */
    UINT32 RequestID[MAX_VOUT_BUF_NUM];
    /**
     * Read point. The index of curently displayed buffer.  \n
     * 0 <= ReadPoint < BufferNumber
     */
    UINT8 ReadPoint;
    /**
     * Write point. The index of next available buffer.     \n
     * 0 <= WritePoint < BufferNumber
     */
    UINT8 WritePoint;
} APPLIB_VOUT_BUFFER_ARRAY_s;

/**
 * Vout buffer manager
 */
typedef struct _APPLIB_VOUT_BUFFER_MANAGER_s_ {
    /**
     * Array of "Vout buffer array" of each channel.
     */
    APPLIB_VOUT_BUFFER_ARRAY_s VoutBuffer[DISP_CH_NUM];
    /**
     * Mutex for protecting the buffer during multiple access.
     */
    AMBA_KAL_MUTEX_t Mutex;
    /**
     * Whether the buffer has been initialized.
     */
    UINT8 IsInit;
    /**
     * Callback function that is invoked right after receiving
     * the DSP callback that the display request is done.
     */
    int (*DisplayEndCB)(void *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after receiving
     * the last DSP callback of all channels that the display
     * request is done.
     */
    int (*DisplayAllChanEndCB)(void *Hdlr, UINT32 EventID, void *Info);
} APPLIB_VOUT_BUFFER_MANAGER_s;

// Gloabal variable
extern APPLIB_VOUT_BUFFER_MANAGER_s G_VoutBufMgr; /**< Vout buffer manager*/

/**
 * Convert the channel defined in "ApplibDisplay.h" to
 * the index of Vout buffer in a Vout buffer manager.
 *
 * @param [in] VoutChannel          Vout channel.
 * @param [out] OutputChannelIdx    Channel array index.
 *
 * @return 0 - OK, others - Error
 */
extern UINT32 Applib_Convert_VoutChannel_To_ChannelIdx(const UINT32 VoutChannel, UINT32 *OutputChannelIdx);

/**
 * Convert the index of Vout buffer in a Vout buffer manager to
 * the channel defined in "ApplibDisplay.h".
 *
 * @param [in] ChannelIdx           Channel array index
 * @param [out] OutputVoutChannel   Vout channel.
 *
 * @return 0 - OK, others - Error
 */
extern UINT32 Applib_Convert_ChannelIdx_To_VoutChannel(const UINT32 ChannelIdx, UINT32 *OutputVoutChannel);

/**
 * Allocate memory for Vout buffer in "available" (not all) Vout channel.   \n
 * Do nothing when the memory has already been allocated.                   \n
 * The number of buffers in a channel is determined by "BufferNumber".      \n
 * The elements of "VoutBufMgr" receive return values.                      \n
 * If BufferNumber = 0, return error.
 *
 * @param [in] BufferNumber     Array with size of DISP_CH_NUM storing number of buffers in each channel
 * @param [in] MemoryPool       Memory byte pool
 * @param [out] VoutBufMgr      Vout buffer manager
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVoutBuffer_Alloc(
    const UINT32 BufferNumber[DISP_CH_NUM],
    AMBA_KAL_BYTE_POOL_t *MemoryPool,
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr);

/**
 * Handle the event AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT.
 * Update common buffer data.
 * Common buffer is allocated by DSP and shared by encoder and decoder when seamless is enabled.
 * If common buffer data is valid, use it instead.
 * Also note that common buffer cannot be accessed during video decode
 *
 * @param [in] EventData        Common buffer data
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVoutBuffer_UpdateCommonBuffer(void* EventData);

/**
 * Whether the buffer in "VoutChannel" is ready to work.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 *
 * @return 0: Not ready, 1: Ready
 */
extern UINT8 ApplibVoutBuffer_IsVoutReady(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel);

/**
 * Get the Vout buffer luma address in "VoutChannel" with specified RequestID.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [in] RequestID        Request ID
 * @param [out] OutputLumaAddr  Luma address.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutLumaAddr(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    UINT8 **OutputLumaAddr);

/**
 * Get the Vout buffer chroma address in "VoutChannel" with specified RequestID.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [in] RequestID            Request ID
 * @param [out] OutputChromaAddr    Chroma address.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutChromaAddr(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    UINT8 **OutputChromaAddr);

/**
 * Get the Vout buffer color format in "VoutChannel".
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [out] OutputColorFormat   Color format.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutColorFormat(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    AMP_COLOR_FORMAT_e *OutputColorFormat);

/**
 * Get the Vout buffer width in "VoutChannel".
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [out] OutputWidth     Buffer width.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutWidth(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32 *OutputWidth);

/**
 * Get the Vout buffer height in "VoutChannel".
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [out] OutputHeight    Buffer height.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutHeight(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32 *OutputHeight);

/**
 * Get the Vout buffer pitch in "VoutChannel".
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [out] OutputPitch     Buffer pitch.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutPitch(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32 *OutputPitch);

/**
 * Get the Vout buffer in "VoutChannel" with specified RequestID and custom AOI.\n
 * All elements except AOI of "OutputBuffer" would be changed.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [in] RequestID        Request ID
 * @param [out] OutputBuffer    Buffer with custom AOI.
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_GetVoutBuffer(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    AMP_YUV_BUFFER_s *OutputBuffer);

/**
 * Set the Vout buffer in "VoutChannel" with specified RequestID to the background color (i.e. black).
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] RequestID        Request ID
 * @param [in] VoutChannel      Vout channel
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_CleanVoutBuffer(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 RequestID,
    const UINT32 VoutChannel);

/**
 * Set the Vout buffer in "all channel" with specified RequestID to the background color (i.e. black).
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] RequestID        Request ID
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_CleanVoutBuffer_AllChannel(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 RequestID);

/**
 * Take a free and clean (ie. black) Vout buffer in "VoutChannel".\n
 * The process of giving buffer is done in ApplibVoutBuffer_DisplayVoutBuffer.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [out] OutputRequestID       Request ID
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_TakeVoutBuffer(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32* OutputRequestID);

/**
 * Take a free and clean (ie. black) Vout buffer in "all channel".\n
 * The process of giving buffer is done in ApplibVoutBuffer_DisplayVoutBuffer.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [out] OutputRequestID       Request ID
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_TakeVoutBuffer_AllChannel(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    UINT32* OutputRequestID);

/**
 * Display a buffer in "VoutChannel" and release buffers that are no longer displayed.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] VoutChannel      Vout channel
 * @param [in] LumaAddr         Luma address of the displayed buffer
 * @param [in] ChromaAddr       Chroma address of the displayed buffer
 *
 * @return 0 - OK, others - Error
 */
extern int ApplibVoutBuffer_DisplayVoutBuffer(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT8 *LumaAddr,
    const UINT8 *ChromaAddr);

/**
 * Initialize Vout buffer manager.
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 * @param [in] DisplayEndCB
 * @param [in] DisplayAllChanEndCB
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVoutBuffer_Init(
        APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
        int (*DisplayEndCB)(void *Hdlr, UINT32 EventID, void *Info),
        int (*DisplayAllChanEndCB)(void *Hdlr, UINT32 EventID, void *Info));

/**
 * Release all Vout buffers in all Vout channel.\n
 * Reset all elements of "VoutBufMgr"
 *
 * @param [in] VoutBufMgr       Vout buffer manager
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVoutBuffer_Release(APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr);

/**
 * Initialize still decode module.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillDecModule_Init(void);

/**
 * Release still decode module.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillDecModule_Deinit(void);

#endif /* _APPLIB_PLYR_COMMON_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_Common
