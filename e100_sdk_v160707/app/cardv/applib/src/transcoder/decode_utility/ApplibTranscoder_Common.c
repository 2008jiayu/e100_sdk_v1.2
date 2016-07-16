/**
 * @file src/app/connected/applib/src/player/decode_utility/ApplibPlayer_Common.c
 *
 * Common functions used by player module in application Library
 *
 * History:
 *    2014/01/06 - [phcheng] Create file
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
#include <player/decode_utility/ApplibPlayer_Common.h>
#include <player/decode_utility/ApplibPlayer_Internal.h>
#include <player/StillDec.h>
#include <comsvc/misc/util.h>
#include "../../AppLibTask_Priority.h"

#define VOUT_BUF_INVALID_REQUEST_ID (0) /** Initialize RequestID of each buffer to this value. */
#define VOUT_BUF_ALL_VOUT_CHANNEL (0xFFFFFFFF)
//#define DEBUG_VOUT_BUF_MGR_ALLOC /* Printk when Vout buffer is allocated and released */
//#define DEBUG_VOUT_BUF_MUTEX_FLOW /** Printk when mutes is taken or given */

#define SET_ARRAY_ZERO(array, size) memset((array), 0, sizeof((array)[0]) * (size))

// Gloabal variable
APPLIB_VOUT_BUFFER_MANAGER_s G_VoutBufMgr = {0}; // Vout buffer manager
//static UINT8 VoutBufMgrInitFlag = 0; // Whether the Vout buffer manager is iniialized

static int ApplibStillDecModuleInitFlag = 0;
static void *stlCodecModuleBufOri = NULL;   // Original buffer address of codec module
static void *stlCodecModuleBuf = NULL;      // Aligned buffer address of codec module

static const UINT32 VoutBufferChannel[DISP_CH_NUM] = {
    DISP_CH_DCHAN,  // LCD channel
    DISP_CH_FCHAN   // TV  channel
}; // VoutChannel (defined in "ApplibDisplay.h") of each Vout buffer

UINT32 Applib_Convert_VoutChannel_To_ChannelIdx(
    const UINT32 VoutChannel,
    UINT32 *OutputChannelIdx)
{
    int i = 0; // Counter

    if (OutputChannelIdx == NULL) {
        return -1; // Error
    }

    for (i = 0; i < DISP_CH_NUM; ++i) {
        if (VoutBufferChannel[i] == VoutChannel) {
            // Set output
            *OutputChannelIdx = i;
            return 0; // Success
        }
    }

    return -1; // Error. Cannot find corresponding ChannelIdx.
}

UINT32 Applib_Convert_ChannelIdx_To_VoutChannel(
    const UINT32 ChannelIdx,
    UINT32 *OutputVoutChannel)
{
    if (OutputVoutChannel == NULL) {
        return -1; // Error
    }

    if (ChannelIdx >= DISP_CH_NUM) {
        return -1; // Error
    }

    // Set output
    *OutputVoutChannel = VoutBufferChannel[ChannelIdx];

    return 0; // Success
}

/**
 * Get buffer size.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] ChannelIdx           Channel array index
 *
 * @return Buffer size
 */
static UINT32 GetBufferSize(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 ChannelIdx)
{
    return (ChannelIdx < DISP_CH_NUM)
           ? (VoutBufMgr->VoutBuffer[ChannelIdx].Pitch * VoutBufMgr->VoutBuffer[ChannelIdx].Height)
           : (0);
}

/**
 * An internal function checking whether the Vout buffer has been allocated.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] ChannelIdx           Channel array index
 *
 * @return 0 - Not allocated, 1 - Allocated
 */
static UINT8 ApplibVoutBuffer_IsAllocated_Internal(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 ChannelIdx)
{
    return (VoutBufMgr->VoutBuffer[ChannelIdx].IsAllocated != 0) ? (1) :(0);
}

/**
 * An internal function checking whether the Vout channel is ready.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] ChannelIdx           Channel array index
 *
 * @return 0 - Not ready, 1 - Ready
 */
static UINT8 ApplibVoutBuffer_IsVoutReady_Internal(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 ChannelIdx)
{
#if 0
    // Alloc everytime the devices are on/off. No need to check device here.
    AMP_DISP_INFO_s DispDev = { 0 }; // Display device info.
    int Rval = 0; // Function call result.

    Rval = AppLibDisp_GetDeviceInfo(VoutChannel, &DispDev);
    if (Rval != AMP_OK) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Cannot get device info!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    return ( DispDev.DeviceInfo.Enable && ApplibVoutBuffer_IsAllocated_Internal(VoutBufMgr, ChannelIdx) ) ? (1) :(0);
#endif

    return ApplibVoutBuffer_IsAllocated_Internal(VoutBufMgr, ChannelIdx);
}

/**
 * An internal function which release the buffer and reset all elements of a channel.\n
 * Assume all inputs are valid, so make sure they've been checked before.\n
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in,out] VoutBufMgr       Vout buffer manager
 * @param [in] ChannelIdx           Channel array index
 *
 * @return 0 - OK, others - Error
 */
static int AppLibVoutBuffer_ReleaseChannel_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 ChannelIdx)
{
    UINT8 *BufferAddrOri = NULL; // Start address of buffer to be released
    int ReturnValue = 0; // Return value.

    if (ApplibVoutBuffer_IsAllocated_Internal(VoutBufMgr, ChannelIdx)) {
        BufferAddrOri = VoutBufMgr->VoutBuffer[ChannelIdx].BufferAddrOri[0];
        // Release Vout buffer
        // Don't have to check "BufferAddrOri != NULL"
        if (AmbaKAL_BytePoolFree(BufferAddrOri) != AMP_OK) { // Error
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to release the buffer at 0x%08x.",
                __FUNCTION__, __LINE__, BufferAddrOri);
            ReturnValue = -1; // Error
        }

#ifdef DEBUG_VOUT_BUF_MGR_ALLOC
        AmbaPrintColor(BLUE, "[Applib - VoutBuffer] Channel(%d): Release buffer at 0x%08x.", ChannelIdx, BufferAddrOri);
#endif /* DEBUG_VOUT_BUF_MGR_ALLOC */

        // Reset all elements of the channel
        SET_ZERO(VoutBufMgr->VoutBuffer[ChannelIdx]);
    }

    return ReturnValue;
}

int AppLibVoutBuffer_Alloc(
    const UINT32 BufferNumber[DISP_CH_NUM],
    AMBA_KAL_BYTE_POOL_t *MemoryPool,
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    UINT8 BufIdx = 0; // Index of buffer in a channel.
    UINT32 VoutChannel = 0; // Channel defined in "ApplibDisplay.h".
    AMP_DISP_INFO_s DispDev; // Device info.
    UINT32 MutexTimeout = 500; // Maximum waiting time (in ms) for a mutex
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        goto ReturnError; // Error
    }
    if (MemoryPool == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Memory pool address is not valid!", __FUNCTION__, __LINE__);
        goto ReturnError; // Error
    }
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        if (BufferNumber[ChannelIdx] > MAX_VOUT_BUF_NUM) {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Channel(%d): Buffer number (%d) over the limit (> %d)!",
                __FUNCTION__, __LINE__, ChannelIdx, BufferNumber[ChannelIdx], MAX_VOUT_BUF_NUM);
            goto ReturnError; // Error
        }
        if (BufferNumber[ChannelIdx] < MIN_VOUT_BUF_NUM) {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Channel(%d): Buffer number (%d) insufficient (< %d)!",
                __FUNCTION__, __LINE__, ChannelIdx, BufferNumber[ChannelIdx], MIN_VOUT_BUF_NUM);
            goto ReturnError; // Error
        }
    }

    // Take mutex
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex begin");
#endif
    Rval = AmbaKAL_MutexTake(&VoutBufMgr->Mutex, MutexTimeout);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex end (%d)", Rval);
#endif
    if (Rval == TX_NOT_AVAILABLE) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Take mutex timeout (over %u ms).", __FUNCTION__, __LINE__, MutexTimeout);
        goto ReturnError; // Error
    }
    else if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to take mutex (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError; // Error
    }

    // Alloc all buffers in each channel
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        // Get Vout channel
        Rval = Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel);
        if (Rval != 0) { // Error
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel id (%d)!", __FUNCTION__, __LINE__, ChannelIdx);
            goto ReturnError_GiveMutex;
        }

        // Get device info
        SET_ZERO(DispDev); // Reset value
        Rval = AppLibDisp_GetDeviceInfo(VoutChannel, &DispDev);
        if (Rval != 0) { // Error
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Channel(%d): Failed to get device info.", __FUNCTION__, __LINE__, ChannelIdx);
            goto ReturnError_GiveMutex;
        }

        if (DispDev.DeviceInfo.Enable == 0) {
            // Release and reset channel when device is turned off.
            if (AppLibVoutBuffer_ReleaseChannel_Internal(VoutBufMgr, ChannelIdx) != 0) {
                goto ReturnError_GiveMutex;
            }
            continue;
        }
        else if (BufferNumber[ChannelIdx] == 0) {
            // Release the channel when BufferNumber = 0.
            if (AppLibVoutBuffer_ReleaseChannel_Internal(VoutBufMgr, ChannelIdx) != 0) {
                goto ReturnError_GiveMutex;
            }
            continue;
        }
        else {
            UINT32 BufferPitch = 0; // Pitch of a Vout buffer in a channel after alloc.
            UINT32 BufSizeBefore = 0; // Size of a Vout buffer in a channel before alloc.
            UINT32 BufSizeAfter = 0; // Size of a Vout buffer in a channel after alloc.
            UINT32 TotalBufSize = 0; // Size of all Vout buffers in a channel.
            BufSizeBefore = GetBufferSize(VoutBufMgr, ChannelIdx);
            // Make sure that all buffer begin at aligned address.
            // Align Vout buffer pitch to 64 in case that Vout width is not aligned to 64.
            BufferPitch = ALIGN_64(DispDev.DeviceInfo.VoutWidth);
            BufSizeAfter = BufferPitch * DispDev.DeviceInfo.VoutHeight;
            // Allocate another memory when buffer number changes or buffer size changes.
            if ((VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber != BufferNumber[ChannelIdx]) ||
                (BufSizeBefore != BufSizeAfter)) {
                // Release before alloc
                if (AppLibVoutBuffer_ReleaseChannel_Internal(VoutBufMgr, ChannelIdx) != 0) {
                    goto ReturnError_GiveMutex;
                }
                // Buffer settings
                VoutBufMgr->VoutBuffer[ChannelIdx].Width = DispDev.DeviceInfo.VoutWidth;
                VoutBufMgr->VoutBuffer[ChannelIdx].Height = DispDev.DeviceInfo.VoutHeight;
                VoutBufMgr->VoutBuffer[ChannelIdx].Pitch = BufferPitch;
                VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber = BufferNumber[ChannelIdx];
                VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint = 0; // Start form the first buffer
                VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint = BufferNumber[ChannelIdx] - 1; // The end of buffer
                // Allocate memory
                TotalBufSize = BufSizeAfter * BufferNumber[ChannelIdx];
                Rval = AmpUtil_GetAlignedPool(
                    MemoryPool,
                    (void**) &(VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[0]),
                    (void**) &(VoutBufMgr->VoutBuffer[ChannelIdx].BufferAddrOri[0]),
                    (TotalBufSize << 1), // Replace "* 2" by "<< 1"
                    1 << 6); // Align to 64
                if (Rval != AMP_OK) { // Error
                    AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
                    // TODO: ASSERT
                    AppLibVoutBuffer_ReleaseChannel_Internal(VoutBufMgr, ChannelIdx); // TODO: Remove this line when ASSERT is added
                    goto ReturnError_GiveMutex; // Error
                }
                VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[0] = VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[0] + TotalBufSize;

#ifdef DEBUG_VOUT_BUF_MGR_ALLOC
                AmbaPrintColor(GREEN, "[Applib - VoutBuffer] Channel(%d): Alloc entire buffer at 0x%08x.",
                    ChannelIdx, VoutBufMgr->VoutBuffer[ChannelIdx].BufferAddrOri[0]);
                AmbaPrintColor(GREEN, "[Applib - VoutBuffer] Channel(%d): Alloc buffer(0) LumaAddr = 0x%08x, ChromaAddr = 0x%08x",
                    ChannelIdx, VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[0], VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[0]);
#endif /* DEBUG_VOUT_BUF_MGR_ALLOC */

                // Set buffer to background color.
                AppLib_SetYuvBuf_Black(
                    VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[0],
                    VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[0],
                    TotalBufSize,
                    TotalBufSize);
                // Set buffer address. Start from the second element, since the first one has been set.
                for (BufIdx = 1; BufIdx < BufferNumber[ChannelIdx]; ++BufIdx) {
                    // No need to set BufferAddrOri
                    VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx]   = VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx-1] + BufSizeAfter;
                    VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[BufIdx] = VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx] + TotalBufSize;

#ifdef DEBUG_VOUT_BUF_MGR_ALLOC
                AmbaPrintColor(GREEN, "[Applib - VoutBuffer] Channel(%d): Alloc buffer(%d) LumaAddr = 0x%08x, ChromaAddr = 0x%08x",
                    ChannelIdx, BufIdx, VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx], VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[BufIdx]);
#endif /* DEBUG_VOUT_BUF_MGR_ALLOC */

                }
                VoutBufMgr->VoutBuffer[ChannelIdx].IsAllocated = 1; // Allocated
            }
        }
    }

    // Give mutex
    Rval = AmbaKAL_MutexGive(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Give mutex (%d)", Rval);
#endif
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to give mutex (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    return 0; // Success

ReturnError_GiveMutex:
    // Give mutex
    Rval = AmbaKAL_MutexGive(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Give mutex (%d)", Rval);
#endif
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to give mutex (%d).", __FUNCTION__, __LINE__, Rval);
    }

ReturnError:
    return -1; // Error
}

UINT8 ApplibVoutBuffer_IsVoutReady(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return 0; // Not ready
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return 0; // Not ready
    }

    switch (VoutChannel) {
        case DISP_CH_DCHAN:
        case DISP_CH_FCHAN:
            return ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx);
        default:
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
            // TODO: ASSERT
            return 0; // Not ready
    }
}

/**
 * An internal function which search the activated buffer for identical RequestID in a specific channel.\n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in] VoutBufMgr               Vout buffer manager
 * @param [in] ChannelIdx               Channel array index
 * @param [in] RequestID                Request ID
 * @param [in,out] OutputBufferIdx      Buffer ID
 *
 * @return 0 - OK, -1 - RequestID not found
 */
static int ApplibVoutBuffer_GetActiveBufferIdx_Internal(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 ChannelIdx,
    const UINT32 RequestID,
    UINT8 *OutputBufferIdx)
{
    const UINT8 BufferNumber = VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber;
    const UINT8 WritePoint = VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint;
    const UINT8 ReadPoint = VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint;
    UINT8 BufIdx = 0; // Index of buffer.
    UINT8 i = 0; // Counter

    // Initialize output value
    *OutputBufferIdx = 0;

    if (RequestID == VOUT_BUF_INVALID_REQUEST_ID) {
        return -1; // RequestID not found
    }

    BufIdx = ReadPoint + 1; // Start from (ReadPoint + 1)
    // Search all activated buffer for identical RequestID
    // Activated buffers are distributed from (ReadPoint + 1) to (WritePoint - 1)
    for (i = 0; i < BufferNumber; ++i) {
        if (BufIdx >= BufferNumber) {
            BufIdx = 0; // Wrap search
        }
        if (BufIdx == WritePoint) { // End of search. Cannot overstep write point.
            return -1; // RequestID not found in activated buffers
        }
        if (VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[BufIdx] == RequestID) {
            // Set output value
            *OutputBufferIdx = BufIdx;
            return 0; // Success
        }
        ++BufIdx;
    }
    return -1; // RequestID not found
}

int ApplibVoutBuffer_GetVoutLumaAddr(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    UINT8 **OutputLumaAddr)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    UINT8 BufIdx = 0; // Index of buffer
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputLumaAddr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Search activated buffers for RequestID
    if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &BufIdx) != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u RequestID (%d) not found!", __FUNCTION__, __LINE__, RequestID);
        return -1; // Not found
    } else {
        // Set output value
        *OutputLumaAddr = VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx];
        return 0; // Success
    }
}

int ApplibVoutBuffer_GetVoutChromaAddr(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    UINT8 **OutputChromaAddr)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    UINT8 BufIdx = 0; // Index of buffer
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputChromaAddr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Search activated buffers for RequestID
    if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &BufIdx) != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u RequestID (%d) not found!", __FUNCTION__, __LINE__, RequestID);
        return -1; // Not found
    } else {
        // Set output value
        *OutputChromaAddr = VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[BufIdx];
        return 0; // Success
    }
}

int ApplibVoutBuffer_GetVoutColorFormat(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    AMP_COLOR_FORMAT_e *OutputColorFormat)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputColorFormat == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Set output value
    *OutputColorFormat = AMP_YUV_422; // DSP Vout buffer is always 4:2:2

    return 0; // Success
}

int ApplibVoutBuffer_GetVoutWidth(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32 *OutputWidth)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputWidth == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer (%d) is not ready. Allocate first!", __FUNCTION__, __LINE__, ChannelIdx);
        return -1; // Error
    }

    // Set output value
    *OutputWidth = VoutBufMgr->VoutBuffer[ChannelIdx].Width;

    return 0; // Success
}

int ApplibVoutBuffer_GetVoutHeight(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32 *OutputHeight)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputHeight == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Set output value
    *OutputHeight = VoutBufMgr->VoutBuffer[ChannelIdx].Height;

    return 0; // Success
}

int ApplibVoutBuffer_GetVoutPitch(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32 *OutputPitch)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputPitch == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Set output value
    *OutputPitch = VoutBufMgr->VoutBuffer[ChannelIdx].Pitch;

    return 0; // Success
}

int ApplibVoutBuffer_GetVoutBuffer(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    AMP_YUV_BUFFER_s *OutputBuffer)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    UINT8 BufIdx = 0; // Index of buffer
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputBuffer == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Abscence of output!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Search activated buffers for RequestID
    if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &BufIdx) != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u RequestID (%d) not found!", __FUNCTION__, __LINE__, RequestID);
        return -1; // Not found
    } else {
        // Set output value
        OutputBuffer->ColorFmt = AMP_YUV_422; // DSP Vout buffer is always 4:2:2.
        OutputBuffer->Width = VoutBufMgr->VoutBuffer[ChannelIdx].Width;
        OutputBuffer->Height = VoutBufMgr->VoutBuffer[ChannelIdx].Height;
        OutputBuffer->Pitch = VoutBufMgr->VoutBuffer[ChannelIdx].Pitch;
        OutputBuffer->LumaAddr = VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx];
        OutputBuffer->ChromaAddr = VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[BufIdx];
        return 0; // Success
    }
}

int ApplibVoutBuffer_CleanVoutBuffer(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 RequestID,
    const UINT32 VoutChannel)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    UINT8 BufIdx = 0; // Index of buffer
    UINT32 BufSize = 0; // Size of the next Vout buffer.
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get buffer channel index
    Rval = Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx);
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        // TODO: ASSERT
        return -1; // Error
    }

    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Search activated buffers for RequestID
    if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &BufIdx) != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u RequestID (%d) not found!", __FUNCTION__, __LINE__, RequestID);
        return -1; // Not found
    } else {
        // Get buffer size
        BufSize = GetBufferSize(VoutBufMgr, ChannelIdx);
        // Clean the buffer
        AppLib_SetYuvBuf_Black(
            VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx],
            VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[BufIdx],
            BufSize,
            BufSize);
        return 0; // Success
    }
}

int ApplibVoutBuffer_CleanVoutBuffer_AllChannel(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 RequestID)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    UINT32 VoutChannel = 0; // Channel defined in "ApplibDisplay.h".
    UINT8 BufIdx = 0; // Index of buffer
    UINT32 BufSize = 0; // Size of the next Vout buffer.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Do for each channel
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        // Get VoutChannel
        Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannel);

        if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
            continue;
        }

        // Search activated buffers for RequestID
        if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &BufIdx) != 0) {
            continue;
        }

        // Get buffer size
        BufSize = GetBufferSize(VoutBufMgr, ChannelIdx);
        // Clean the buffer
        AppLib_SetYuvBuf_Black(
            VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[BufIdx],
            VoutBufMgr->VoutBuffer[ChannelIdx].ChromaAddr[BufIdx],
            BufSize,
            BufSize);
    }

    return 0; // Success
}

/**
 * An internal function that move write point in "VoutChannel" to the next position.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [in] ChannelIdx           Channel array index
 * @param [in] RequestID            Request ID
 *
 * @return 0 - OK, 1 - Buffer is not ready, 2 - Buffer full, others - Error
 */
static int ApplibVoutBuffer_AddWritePoint_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 ChannelIdx,
    const UINT32 RequestID)
{
    const UINT8 CurrentWritePoint = VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint;
    UINT8 NextWritePoint; // Next write point

    // Check whether the buffer is ready
    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        return 1; // Do nothing and return
    }

    // Check BufferNumber
    if (VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber == 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Illegal buffer number (%d).", __FUNCTION__, __LINE__,
                VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber);
        return -1; // Theoretically, this error should not happen as long as the buffer is ready.
    }

    // Get next write point
    NextWritePoint = CurrentWritePoint + 1;
    if (NextWritePoint >= VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber) {
        NextWritePoint = 0; // Ring buffer
    }

    // Check number of available buffers
    if (VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint == VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint) {
        return 2; // Buffer full
    }

    // Set request command ID
    VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[CurrentWritePoint] = RequestID;

    // Set write point
    VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint = NextWritePoint;

    return 0; // Success
}

/**
 * An internal function that move write point in "VoutChannel" to the previous position.
 * This action will remove the last taken buffer.
 * Only used when failed to take buffers in multiple channels and needed to resume early modified channels.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [in] ChannelIdx           Channel array index
 * @param [in] RequestID            Request ID (for double check)
 *
 * @return 0 - OK, 1 - Buffer is not ready, 2 - Buffer locked, others - Error
 */
static int ApplibVoutBuffer_RollbackWritePoint_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 ChannelIdx,
    const UINT32 RequestID)
{
    UINT8 PrevWritePoint; // Previous write point

    // Check whether the buffer is ready
    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        return 1; // Do nothing and return
    }

    // Check BufferNumber
    if (VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber == 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Illegal buffer number (%d).", __FUNCTION__, __LINE__,
                VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber);
        return -1; // Unexpected error. BufferNumber should be larger than 0 as long as the buffer is ready.
    }

    // Get previous write point
    if (VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint == 0) {
        PrevWritePoint = VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber - 1;
    } else {
        PrevWritePoint = VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint - 1;
    }

    // Check previous write point
    if (PrevWritePoint == VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint) {
        return 2; // Cannot rollback. Buffer is currently displayed on-screen and locked.
    }

    // Check RequestID
    if (VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[PrevWritePoint] != RequestID) {
        return -1; // Unexpected error.
    }

    // Rollback write point
    VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint = PrevWritePoint;

    return 0; // Success
}

/**
 * An internal function that take a free Vout buffer in "VoutChannel" if it's ready.
 * Assume all inputs are valid, so make sure they've been checked before.
 * This function would enter critical section.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [in] ChannelIdx           Channel array index
 * @param [in] RequestID            Request ID
 * @param [in] Timeout              Maximum waiting time (in ms) for a Vout buffer
 *
 * @return 0 - OK, 1 - Buffer is not ready, others - Error
 */
static int ApplibVoutBuffer_TakeVoutBuffer_SingleChannel_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 ChannelIdx,
    const UINT32 RequestID,
    const UINT32 Timeout)
{
    const UINT32 TimeLimit = MAX(AmbaSysTimer_GetTickCount() + Timeout, Timeout); // Use "MAX" in case of overflow
    int ReturnValue = 0; // Return value
    int Rval = 0; // Function call result.
    int AddWP_Rval = 0; // Return value of ApplibVoutBuffer_AddWritePoint_Internal

#define TAKE_BUFFER_SLEEP_TIME (1) /** Sleep for a while when buffer's full to avoid busy waiting. */

    while (1) {
        UINT32 TimeLeft; // Time remained (in ms)
        UINT32 MutexTimeout; // Maximum waiting time (in ms) for a mutex
        // Set timeout
        if (Timeout == AMBA_KAL_WAIT_FOREVER) {
            TimeLeft = AMBA_KAL_WAIT_FOREVER;
        } else if (TimeLimit >= AmbaSysTimer_GetTickCount()) {
            TimeLeft = TimeLimit - AmbaSysTimer_GetTickCount();
        } else {
            TimeLeft = 0;
        }
        MutexTimeout = MIN(Timeout, TimeLeft);
        // Take mutex
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
        AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex begin");
#endif
        Rval = AmbaKAL_MutexTake(&VoutBufMgr->Mutex, MutexTimeout);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
        AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex end (%d)", Rval);
#endif
        if (Rval == TX_NOT_AVAILABLE) {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Take mutex timeout (over %u ms).", __FUNCTION__, __LINE__, MutexTimeout);
            ReturnValue = -1; // Error
            break;
        }
        else if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to take mutex (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            break;
        }
        // Add write point
        AddWP_Rval = ApplibVoutBuffer_AddWritePoint_Internal(VoutBufMgr, VoutChannel, ChannelIdx, RequestID);
        // Give mutex
        Rval = AmbaKAL_MutexGive(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
        AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Give mutex (%d)", Rval);
#endif
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to give mutex (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            break;
        }
        // Check whether the buffer is ready
        if (AddWP_Rval == 0) { // Success
            ReturnValue = 0;
            break;
        } else if (AddWP_Rval == 1) { // Not ready
            //AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
            ReturnValue = 1; // Not ready
            break;
        } else if (AddWP_Rval == 2) { // Buffer full
            // Check timeout
            if ((Timeout == AMBA_KAL_WAIT_FOREVER) || (AmbaSysTimer_GetTickCount() < TimeLimit)) {
                // Sleep for a while to avoid busy waiting.
                AmbaKAL_TaskSleep(TAKE_BUFFER_SLEEP_TIME);
            } else {
                AmbaPrint("[Applib - VoutBuffer] %s:%u Take Vout buffer timeout (over %u ms)!", __FUNCTION__, __LINE__, Timeout);
                ReturnValue = -1; // Error
                break;
            }
        } else {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to add write point!", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            break;
        }
    }

    return ReturnValue;
}

/**
 * An internal function that take a free Vout buffer in specified channel(s).
 * Assume all inputs are valid, so make sure they've been checked before.
 * This function would enter critical section.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Which Vout channel to take. Take all channels when VoutChannel equals to 0xFFFFFFFF
 * @param [out] OutputRequestID     Request ID
 *
 * @return 0 - OK, others - Error
 */
static int ApplibVoutBuffer_TakeVoutBuffer_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32* OutputRequestID)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer. Used to parse all channels.
    UINT32 VoutChannelCurrent; // Vout channel. Used to parse all channels.
    UINT32 Timeout = 1000; // Maximum waiting time (in ms) for taking a Vout buffer in each channel
    UINT8 IsBufferTaken[DISP_CH_NUM] = { 0 }; // whether the buffer in each channel is taken. Used when undoing changes.
    int Rval = 0; // Function call result.
    static UINT32 CurrentRequestID = 1; // Current request ID. TODO: Add multi-access protection

    // Do for each channel
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        // Get VoutChannel
        Rval = Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannelCurrent);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel ID (%d)!", __FUNCTION__, __LINE__, ChannelIdx);
            // TODO: ASSERT
            goto ReturnError_Undo; // Error TODO: Remove this line when ASSERT is applied
        }

        // Match Vout channel
        if ((VoutChannelCurrent & VoutChannel) == 0) {
            continue;
        }

        // Take a Vout buffer
        Rval = ApplibVoutBuffer_TakeVoutBuffer_SingleChannel_Internal(VoutBufMgr, VoutChannelCurrent, ChannelIdx, CurrentRequestID, Timeout);
        if (Rval == 0) { // Success
            IsBufferTaken[ChannelIdx] = 1;
        } else if (Rval < 0) { // Exclude situation that the channel is not ready (Rval = 1)
            AmbaPrint("[Applib - VoutBuffer] %s:%u Channel(%d) Failed to take Vout buffer!", __FUNCTION__, __LINE__, ChannelIdx);
            goto ReturnError_Undo; // Undo changes of previous channels.
        }
    }

    // Clean the taken Vout buffer(s)
    if (ApplibVoutBuffer_CleanVoutBuffer_AllChannel(VoutBufMgr, CurrentRequestID) != 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to clean buffer!", __FUNCTION__, __LINE__);
        goto ReturnError_Undo; // Undo changes of previous channels.
    }

    // Set output value
    *OutputRequestID = CurrentRequestID;

    // Add CurrentRequestID
    ++CurrentRequestID; // TODO: Add multi-access protection
    if (CurrentRequestID == VOUT_BUF_INVALID_REQUEST_ID) {
        ++CurrentRequestID;
    }

    return 0; // Success

ReturnError_Undo:
    // Take mutex
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex begin");
#endif
    Rval = AmbaKAL_MutexTake(&VoutBufMgr->Mutex, AMBA_KAL_WAIT_FOREVER);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex end (%d)", Rval);
#endif
    if (Rval == TX_NOT_AVAILABLE) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Take mutex timeout (over %u ms).", __FUNCTION__, __LINE__, AMBA_KAL_WAIT_FOREVER);
        return -1;
    }
    else if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to take mutex (%d).", __FUNCTION__, __LINE__, Rval);
        return -1;
    }
    // Undo changes of taken channel(s)
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        if (IsBufferTaken[ChannelIdx]) {
            // Get VoutChannel
            Rval = Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannelCurrent);
            if (Rval != 0) {
                continue;
            }
            // Check whether the buffer is ready
            if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
                continue;
            }
            AmbaPrint("[Applib - VoutBuffer] Channel(%d) Undo changes since an error occurred. Rollback WritePoint.", ChannelIdx);
            // Roll back WritePoint
            Rval = ApplibVoutBuffer_RollbackWritePoint_Internal(VoutBufMgr, VoutChannelCurrent, ChannelIdx, CurrentRequestID);
            if (Rval != 0) {
                AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Channel(%d) Failed to rollback WritePoint!", __FUNCTION__, __LINE__, ChannelIdx);
            }
        }
    }
    // Give mutex
    Rval = AmbaKAL_MutexGive(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Give mutex (%d)", Rval);
#endif
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to give mutex (%d).", __FUNCTION__, __LINE__, Rval);
        return -1;
    }

    return -1;
}

int ApplibVoutBuffer_TakeVoutBuffer(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    UINT32* OutputRequestID)
{
    UINT32 RequestID; // Request ID
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputRequestID == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Invalid address!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Take Vout buffer
    Rval = ApplibVoutBuffer_TakeVoutBuffer_Internal(VoutBufMgr, VoutChannel, &RequestID);
    if (Rval != 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to take Vout buffer!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Set output value
    *OutputRequestID = RequestID;

    return 0; // Success
}

int ApplibVoutBuffer_TakeVoutBuffer_AllChannel(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    UINT32* OutputRequestID)
{
    UINT32 RequestID; // Request ID
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (OutputRequestID == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Invalid address!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Take Vout buffer
    Rval = ApplibVoutBuffer_TakeVoutBuffer_Internal(VoutBufMgr, VOUT_BUF_ALL_VOUT_CHANNEL, &RequestID);
    if (Rval != 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to take Vout buffer!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Set output value
    *OutputRequestID = RequestID;

    return 0; // Success
}

/**
 * An internal function which search the activated buffers of all channels and report the occurrence of specified RequestID.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel. Search all channels when VoutChannel equals to 0xFFFFFFFF
 * @param [in] RequestID            Request ID given in "TakeVoutBuffer" function
 * @param [out] OutputBufferIdx     Buffer ID
 *
 * @return 0 - OK, -1 - RequestID not found
 */
static int ApplibVoutBuffer_GetRequestIDCount_Internal(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID,
    UINT8 *OutputCount)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer. Used to parse all channels.
    UINT32 VoutChannelCurrent = 0; // Vout channel
    UINT8 BufIdx = 0; // Index of buffer.
    int Rval = 0; // Function call result.

    // Initialize output value
    *OutputCount = 0;

    if (RequestID == VOUT_BUF_INVALID_REQUEST_ID) {
        return -1; // Not found
    }

    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        // Get VoutChannel
        Rval = Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannelCurrent);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel ID (%d)!", __FUNCTION__, __LINE__, ChannelIdx);
            // TODO: ASSERT
            return -1; // Error TODO: Remove this line when ASSERT is applied
        }

        // Match Vout channel
        if ((VoutChannelCurrent & VoutChannel) == 0) {
            continue; // Do nothing
        }

        // Check whether the buffer is ready
        if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
            continue; // Do nothing
        }

        // Search every activated buffers for RequestID
        if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &BufIdx) != 0) {
            continue; // Not found
        } else {
            // Set output value
            *OutputCount += 1;
        }
    }

    return 0; // Success
}

/**
 * An internal function that invoke callback when a RequestID is complete in a channel.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] ChannelIdx           Channel array index
 * @param [in] RequestID            RequestID that is about to be finished
 * @param [in] BufferIdx            Buffer index of RequestID
 *
 * @return 0 - OK, others - Error
 */
static int ApplibVoutBuffer_DisplayEndCB_Internal(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 ChannelIdx,
    const UINT32 RequestID,
    const UINT8 BufferIdx)
{
    UINT32 EventInfo[2] = {ChannelIdx, RequestID};
    // Double check
    if (RequestID != VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[BufferIdx]) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Inconsistent input!", __FUNCTION__, __LINE__);
        return -1; // Error. Inconsistent input
    }

    // TODO: Invoke callback
    if (VoutBufMgr->DisplayEndCB != NULL) {
        if (VoutBufMgr->DisplayEndCB(NULL, 0, &EventInfo) != OK) {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Callback failed!", __FUNCTION__, __LINE__);
        }
    }

    return 0; // Success
}

/**
 * An internal function that invoke callback when a RequestID is complete in all channels.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] RequestID            RequestID that is about to be finished
 * @param [in] BufferIdx            Buffer index of RequestID
 *
 * @return 0 - OK, others - Error
 */
static int ApplibVoutBuffer_DisplayAllChanEndCB_Internal(
    const APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 RequestID)
{
    UINT32 EventInfo = RequestID;

    // Invoke callback
    if (VoutBufMgr->DisplayAllChanEndCB != NULL) {
        if (VoutBufMgr->DisplayAllChanEndCB(NULL, 0, &EventInfo) != OK) {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Callback failed!", __FUNCTION__, __LINE__);
        }
    }

    return 0; // Success
}

/**
 * An internal function that move read point in "VoutChannel" to the next position.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 * NOTE: VoutChannel should be "Single" channel
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          "Single" Vout channel.
 * @param [in] ChannelIdx           Channel array index
 *
 * @return 0 - OK, 1 - Buffer is not ready, 2 - Buffer empty, others - Error
 */
static int ApplibVoutBuffer_AddReadPoint_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 ChannelIdx)
{
    const UINT8 CurrentReadPoint = VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint;
    UINT8 NextReadPoint; // Next read point
    UINT32 NextRequestID;
    UINT8 RequestIDCount = 0; // Occurrence of RequestID in all channels except this channel (VoutChannel)
    UINT32 OtherVoutChannel = VOUT_BUF_ALL_VOUT_CHANNEL ^ VoutChannel;

    // Get the next read point
    NextReadPoint = CurrentReadPoint + 1;
    if (NextReadPoint >= VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber) {
        NextReadPoint = 0; // Ring buffer
    }

    // Check number of active buffers
    if (VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint == NextReadPoint) {
        return 2; // Buffer empty. Cannot add read point.
    }

    // Get request ID of the next read point
    NextRequestID = VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[NextReadPoint];

    // Invoke callback of the next read point
    ApplibVoutBuffer_DisplayEndCB_Internal(VoutBufMgr, ChannelIdx, NextRequestID, NextReadPoint);

    // Invoke callback if this is the last buffer displayed with RequestID in all channels.
    // Search active buffers in all channels except this channel (VoutChannel) for RequestID.
    if (ApplibVoutBuffer_GetRequestIDCount_Internal(VoutBufMgr, OtherVoutChannel, NextRequestID, &RequestIDCount) == 0) {
        // Invoke callback if there's no RequestID in other channels.
        if (RequestIDCount == 0) {
            // Invoke callback
            ApplibVoutBuffer_DisplayAllChanEndCB_Internal(VoutBufMgr, NextRequestID);
        }
    }

    // Don't reset request ID of current read point. This buffer may need to resume changes if some error occurred.
    //VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[CurrentReadPoint] = VOUT_BUF_INVALID_REQUEST_ID;

    // Add read point
    VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint = NextReadPoint;

    return 0; // Success
}

/**
 * An internal function that move read point in "VoutChannel" to the previous position.
 * Only used when failed to give buffers in multiple channels and needed to resume early changes.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [in] ChannelIdx           Channel array index
 * @param [in] OriginalReadPoint    Original read point
 *
 * @return 0 - OK, 1 - Buffer is not ready, 2 - Buffer locked, others - Error
 */
static int ApplibVoutBuffer_RollbackReadPoint_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 ChannelIdx,
    const UINT32 OriginalReadPoint)
{
    UINT8 PrevReadPoint; // Previous read point
    UINT32 i; // Counter

    // Check whether the buffer is ready
    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        return 1; // Do nothing and return
    }

    // Check BufferNumber
    if (VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber == 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Illegal buffer number (%d).", __FUNCTION__, __LINE__,
                VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber);
        return -1; // Unexpected error. BufferNumber should be larger than 0 as long as the buffer is ready.
    }

    // Move read point backwards until the original read point
    for (i = 0; i < VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber; ++i) {
        // Check current read point
        if (VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint == OriginalReadPoint) {
            return 0; // Success
        }

        // Check read point limit
        if (VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint == VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint) {
            return -1; // Unexpected error.
        }

        // Get previous read point
        if (VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint == 0) {
            PrevReadPoint = VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber - 1;
        } else {
            PrevReadPoint = VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint - 1;
        }

        // Move read point backwards
        VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint = PrevReadPoint;
    }

    return -1; // Error. OriginalReadPoint not found.
}

/**
 * An internal function that give Vout buffer in "VoutChannel". Move Read point to the buffer with RequestID.
 * Assume all inputs are valid, so make sure they've been checked before.
 * Assume this function is in a critical section and there's no need to take precautions.
 * NOTE: VoutChannel should be "Single" channel
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          "Single" Vout channel
 * @param [in] ChannelIdx           Channel array index
 * @param [in] RequestID            Request ID given in "TakeVoutBuffer" function
 *
 * @return 0 - OK, 1 - Buffer is not ready, 2 - RequestID not found, others - Error
 */
static int ApplibVoutBuffer_GiveVoutBuffer_SingleChannel_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 ChannelIdx,
    const UINT32 RequestID)
{
    UINT8 RequestIDCount = 0; // Occurrence of RequestID in VoutChannel
    UINT8 TargetBufIdx; // Buffer ID with RequestID
    int Rval = 0; // Function call result.

    // Check whether the buffer is ready
    if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
        return 1; // Do nothing and return
    }

    // Search active buffers in VoutChannel for RequestID.
    if (ApplibVoutBuffer_GetRequestIDCount_Internal(VoutBufMgr, VoutChannel, RequestID, &RequestIDCount) != 0) {
        // Don't move RP if RequestID is not found.
        return 2; // Do nothing and return
    }

    // Check RequestIDCount
    if (RequestIDCount == 0) {
        // RequestID is not found
        return 2; // Do nothing and return
    } else if (RequestIDCount > 1) {
        // RequestID should be unique in a channel
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Illegal occurrence(%d) of RequestID(%d) in Vout channel(0x%x)",
                __FUNCTION__, __LINE__, RequestIDCount, RequestID, VoutChannel);
        return -1; // Unexpected error
    }

    // Search every activated buffers in VoutChannel for RequestID
    if (ApplibVoutBuffer_GetActiveBufferIdx_Internal(VoutBufMgr, ChannelIdx, RequestID, &TargetBufIdx) != 0) {
        return -1; // Unexpected error
    }

    while (VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint != TargetBufIdx) {
        // Add read point
        Rval = ApplibVoutBuffer_AddReadPoint_Internal(VoutBufMgr, VoutChannel, ChannelIdx);
        if (Rval == 1) { // Not ready
            AmbaPrint("[Applib - VoutBuffer] %s:%u Vout buffer not ready. Allocate first!", __FUNCTION__, __LINE__);
            return 1; // Not ready
        } else if (Rval == 2) { // Buffer empty
            // Add read point until buffer's empty. This could happen when there's no RequestID in this channel.
            // This case should be handled in previous steps. Read point should be recoverd to original location.
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected abscence of RequestID(%d) in Vout channel(0x%x)",
                    __FUNCTION__, __LINE__, RequestID, VoutChannel);
            return -1; // Unexpected error
        } else if (Rval != 0) {
            AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to add read point!", __FUNCTION__, __LINE__);
            return -1; // Error
        }
    }

    return 0; // Success
}

/**
 * An internal function that set a Vout buffer with RequestID free and also all of the active buffers prior to it.
 * Move read point to the Vout buffer with RequestID.
 * Assume all inputs are valid, so make sure they've been checked before.
 * This function would enter critical section.
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Which Vout channel to take. Take all channels when VoutChannel equals to 0xFFFFFFFF
 * @param [in] RequestID            Request ID given in "TakeVoutBuffer" function
 *
 * @return 0 - OK, 1 - RequestID not found, others - Error
 */
static int ApplibVoutBuffer_GiveVoutBuffer_Internal(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer. Used to parse all channels.
    UINT32 MutexTimeout = 1000; // Maximum waiting time (in ms) for taking a mutex
    UINT32 VoutChannelCurrent; // Vout channel corresponding to ChannelIdx
    UINT8 IsBufferGiven[DISP_CH_NUM] = { 0 }; // whether the buffer in each channel is given. Used when undoing changes.
    UINT8 OriginalReadPoint[DISP_CH_NUM] = { MAX_VOUT_BUF_NUM }; // Original RP of each channel before "Give". Used when undoing changes.
    int Rval = 0; // Function call result.

    // Take mutex
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex begin");
#endif
    Rval = AmbaKAL_MutexTake(&VoutBufMgr->Mutex, MutexTimeout);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Take mutex end (%d)", Rval);
#endif
    if (Rval == TX_NOT_AVAILABLE) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Take mutex timeout (over %u ms).", __FUNCTION__, __LINE__, MutexTimeout);
        goto ReturnError; // Error
    }
    else if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to take mutex (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError; // Error
    }

    // Do for each channel
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        // Get VoutChannel
        Rval = Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannelCurrent);
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel ID (%d)!", __FUNCTION__, __LINE__, ChannelIdx);
            // TODO: ASSERT
            goto ReturnError_Undo; // Error TODO: Remove this line when ASSERT is applied
        }

        // Match Vout channel
        if ((VoutChannelCurrent & VoutChannel) == 0) {
            continue; // Do nothing
        }

        // Get current read point
        OriginalReadPoint[ChannelIdx] = VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint;

        // Give Vout buffer
        Rval = ApplibVoutBuffer_GiveVoutBuffer_SingleChannel_Internal(VoutBufMgr, VoutChannelCurrent, ChannelIdx, RequestID);
        if (Rval == 0) { // Success
            IsBufferGiven[ChannelIdx] = 1;
        } else if (Rval < 0) { // Exclude situation that the channel is not ready (Rval = 1) and that RequestID is not found (Rval = 2)
            AmbaPrint("[Applib - VoutBuffer] %s:%u Channel(%d) Failed to give Vout buffer!", __FUNCTION__, __LINE__, ChannelIdx);
            goto ReturnError_Undo; // Undo changes of previous channels.
        }
    }

    // Give mutex
    Rval = AmbaKAL_MutexGive(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Give mutex (%d)", Rval);
#endif
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to give mutex (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    return 0; // Success

ReturnError_Undo:
    // Undo changes of given channel(s)
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        if (IsBufferGiven[ChannelIdx]) {
            // Get VoutChannel
            Rval = Applib_Convert_ChannelIdx_To_VoutChannel(ChannelIdx, &VoutChannelCurrent);
            if (Rval != 0) {
                continue;
            }
            // Check whether the buffer is ready
            if (ApplibVoutBuffer_IsVoutReady_Internal(VoutBufMgr, ChannelIdx) == 0) {
                continue;
            }
            // Check original read point
            if (OriginalReadPoint[ChannelIdx] >= VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber) {
                // Unexpected error
                AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Channel(%d) Unexpected ReadPoint (%d)!", __FUNCTION__, __LINE__, OriginalReadPoint[ChannelIdx]);
                continue;
            }
            AmbaPrint("[Applib - VoutBuffer] Channel(%d) Undo changes since an error occurred. Rollback ReadPoint.", ChannelIdx);
            // Roll back ReadPoint
            Rval = ApplibVoutBuffer_RollbackReadPoint_Internal(VoutBufMgr, VoutChannelCurrent, ChannelIdx, OriginalReadPoint[ChannelIdx]);
            if (Rval != 0) {
                AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Channel(%d) Failed to rollback ReadPoint!", __FUNCTION__, __LINE__, ChannelIdx);
            }
        }
    }

    // Give mutex
    Rval = AmbaKAL_MutexGive(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
    AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Give mutex (%d)", Rval);
#endif
    if (Rval != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to give mutex (%d).", __FUNCTION__, __LINE__, Rval);
    }

ReturnError:
    return -1;
}

/**
 * Release every buffers prior to the one with RequestID (included) in "VoutChannel".
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] VoutChannel          Vout channel
 * @param [in] RequestID            Request ID given in "TakeVoutBuffer" function
 *
 * @return 0 - OK, others - Error
 */
int ApplibVoutBuffer_GiveVoutBuffer(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT32 RequestID)
{
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Give Vout buffer
    Rval = ApplibVoutBuffer_GiveVoutBuffer_Internal(VoutBufMgr, VoutChannel, RequestID);
    if (Rval != 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to give Vout buffer!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    return 0; // Success
}

/**
 * Release every buffers prior to the one with RequestID (included) in "all channel".
 *
 * @param [in] VoutBufMgr           Vout buffer manager
 * @param [in] RequestID            Request ID given in "TakeVoutBuffer" function
 *
 * @return 0 - OK, others - Error
 */
int ApplibVoutBuffer_GiveVoutBuffer_AllChannel(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 RequestID)
{
    int Rval = 0; // Function call result.

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Give Vout buffer
    Rval = ApplibVoutBuffer_GiveVoutBuffer_Internal(VoutBufMgr, VOUT_BUF_ALL_VOUT_CHANNEL, RequestID);
    if (Rval != 0) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Failed to give Vout buffer!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    return 0; // Success
}

int ApplibVoutBuffer_DisplayVoutBuffer(
    APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
    const UINT32 VoutChannel,
    const UINT8 *LumaAddr,
    const UINT8 *ChromaAddr)
{
    UINT32 ChannelIdx; // Index of the channel in VoutBuffer.
    UINT8 TargetBufIdx = 0; // Index of the buffer that match the displayed channel and addresses.
    UINT8 BufferNumber = 0;
    UINT8 WritePoint = 0;
    UINT8 ReadPoint = 0;
    UINT32 BufSize = 0; // Size of a buffer in a channel

    // Preliminary check
    if (VoutBufMgr == NULL) {
        AmbaPrint("[Applib - VoutBuffer] %s:%u Initialize VoutBufMgr first!", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    // Get VoutChannel
    if (Applib_Convert_VoutChannel_To_ChannelIdx(VoutChannel, &ChannelIdx) != 0) {
        AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Unexpected channel (0x%x)!", __FUNCTION__, __LINE__, VoutChannel);
        return -1; // Error
    }

    // Get buffer info
    BufferNumber = VoutBufMgr->VoutBuffer[ChannelIdx].BufferNumber;
    WritePoint = VoutBufMgr->VoutBuffer[ChannelIdx].WritePoint;
    ReadPoint = VoutBufMgr->VoutBuffer[ChannelIdx].ReadPoint;
    BufSize = GetBufferSize(VoutBufMgr, ChannelIdx);

    // Find currently displayed buffer and the buffer prior to it
    {
        UINT8 i = 0; // Counter
        UINT8 *CurrentLumaAddr = NULL;
        TargetBufIdx = ReadPoint; // Start from ReadPoint
        // Search all activated buffers distributed from (ReadPoint) to (WritePoint - 1)
        // In most of the cases we should find the displayed buffer at ReadPoint. Unless the displayed buffer is not from this Vout buffer manager
        // or some of the display messages are lost.
        for (i = 0; i < BufferNumber; ++i) {
            // Get current address
            CurrentLumaAddr = VoutBufMgr->VoutBuffer[ChannelIdx].LumaAddr[TargetBufIdx];
            // Match buffer address
            if ((LumaAddr >= CurrentLumaAddr) &&
                (LumaAddr < CurrentLumaAddr + BufSize)) {
                break; // Buffer found
            }
            // Move TargetBufIdx forwards
            ++TargetBufIdx;
            if (TargetBufIdx >= BufferNumber) {
                TargetBufIdx = 0; // Wrap search
            }
            if (TargetBufIdx == WritePoint) { // End of search. Cannot overstep write point.
                // Do nothing because the currently displayed buffer is not activated or not controlled by this VoutBufMgr.
                return 0; // Success. Buffer not found. Do NOT return error.
            }
        }
        if (i >= BufferNumber) {
            // In this case, ReadPoint equals to WritePoint and the displayed buffer is not from this Vout buffer manager
            return 0; // Success. Buffer not found. Do NOT return error.
        }
    }

    // Give every buffers prior to currently displayed buffer (included).
    {
        UINT32 RequestID = VoutBufMgr->VoutBuffer[ChannelIdx].RequestID[TargetBufIdx];
        // Give buffer(s)
        if (ApplibVoutBuffer_GiveVoutBuffer(VoutBufMgr, VoutChannel, RequestID) != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Channel(%d) Failed to give Vout buffer.", __FUNCTION__, __LINE__, ChannelIdx);
            return -1; // Error
        }
    }
    return 0; // Success
}

int AppLibVoutBuffer_Init(
        APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr,
        int (*DisplayEndCB)(void *Hdlr, UINT32 EventID, void *Info),
        int (*DisplayAllChanEndCB)(void *Hdlr, UINT32 EventID, void *Info))
{
    int Rval = 0; // Function call result.

    if (VoutBufMgr == NULL) {
        return -1; // Error
    }

    VoutBufMgr->DisplayEndCB = DisplayEndCB;
    VoutBufMgr->DisplayAllChanEndCB = DisplayAllChanEndCB;

    if (VoutBufMgr->IsInit == 0) {
        // Create mutex
        Rval = AmbaKAL_MutexCreate(&VoutBufMgr->Mutex);
#ifdef DEBUG_VOUT_BUF_MUTEX_FLOW
        AmbaPrintColor(YELLOW, "[Applib - VoutBuffer] Create mutex (%d)", Rval);
#endif
        if (Rval != 0) {
            AmbaPrintColor(RED, "[Applib - VoutBuffer] %s:%u Failed to create mutex (%d).", __FUNCTION__, __LINE__, Rval);
            return -1; // Error
        }
        VoutBufMgr->IsInit = 1;
    }

    return 0; // Success
}

int AppLibVoutBuffer_Release(APPLIB_VOUT_BUFFER_MANAGER_s *VoutBufMgr)
{
    UINT32 ChannelIdx = 0; // Index of the channel in VoutBuffer.
    int ReturnValue = 0; // Return value.

    if (VoutBufMgr == NULL) {
        return -1; // Error
    }

    // Do for each channel
    for (ChannelIdx = 0; ChannelIdx < DISP_CH_NUM; ++ChannelIdx) {
        // Release and reset the channel.
        if (AppLibVoutBuffer_ReleaseChannel_Internal(VoutBufMgr, ChannelIdx) != 0) {
            ReturnValue = -1; // Error
        }
    }

    return ReturnValue;
}

int AppLibStillDecModule_Init()
{
    AMP_STILLDEC_INIT_CFG_s codecInitCfg;   // Still codec module config
    if (ApplibStillDecModuleInitFlag == 0) {
        // Get the default still codec module settings
        if (AmpStillDec_GetInitDefaultCfg(&codecInitCfg) != AMP_OK) {
            AmbaPrint("[Applib - StillDecModule] %s:%u Cannot get default still codec module settings.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
        codecInitCfg.BufSize = 0x4000;
        // Allocate memory for codec module
        if (stlCodecModuleBufOri == NULL) {
            if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &stlCodecModuleBuf, &stlCodecModuleBufOri, codecInitCfg.BufSize, 1 << 5) != AMP_OK) {
                AmbaPrint("[Applib - StillDecModule] %s:%u Cannot allocate memory.", __FUNCTION__, __LINE__);
                goto ReturnError;
            }
        }
        codecInitCfg.Buf = stlCodecModuleBuf;
        codecInitCfg.TaskInfo.Priority = APPLIB_STILL_DEC_TASK_PRIORITY;
        // Configure the initial settings
        if (AmpStillDec_Init(&codecInitCfg) != AMP_OK) {
            AmbaPrint("[Applib - StillDecModule] %s:%u Cannot configure the initial settings.", __FUNCTION__, __LINE__);
            goto ReturnError;
        }
    }
    ApplibStillDecModuleInitFlag = 1; // Initialized
    return 0; // Success

ReturnError:
    ApplibStillDecModuleInitFlag = 0; // Not initialized
    return -1; // Error
}

int AppLibStillDecModule_Deinit()
{
    if (stlCodecModuleBufOri != NULL) {
        if (AmbaKAL_BytePoolFree(stlCodecModuleBufOri) != AMP_OK) {
            AmbaPrint("[Applib - StillDec] %s:%u Cannot release the codec module.", __FUNCTION__, __LINE__);
            return -1; // Error
        }
        stlCodecModuleBufOri = NULL;
    }
    stlCodecModuleBuf = NULL;
    ApplibStillDecModuleInitFlag = 0; // Not initialized
    return 0; // Success
}

