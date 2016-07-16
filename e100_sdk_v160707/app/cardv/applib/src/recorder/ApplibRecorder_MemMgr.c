/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_MemMgr.c
 *
 * Implementation of recorder's buffer manager
 *
 * History:
 *    2014/05/27 - [Martin Lai] created file
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

//#define DEBUG_APPLIB_RECORDER_MEMMGR
#if defined(DEBUG_APPLIB_RECORDER_MEMMGR)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#ifdef CONFIG_APP_ARD
#define APPLIB_ENC_BITSFIFO_SIZE (120*1024*1024)
#else
#define APPLIB_ENC_BITSFIFO_SIZE 90*1024*1024
#endif
static UINT32 ApplibEncBitBufSize = APPLIB_ENC_BITSFIFO_SIZE;
UINT8 *AppLibEncBitsBuf = NULL;
static void *AppLibEncBitsBufRaw = NULL;

#ifdef CONFIG_APP_ARD
#define APPLIB_ENC_DESC_SIZE (50*1024*10)
#else
#define APPLIB_ENC_DESC_SIZE 40*1024*10
#endif
static UINT32 ApplibEncDescBufSize = APPLIB_ENC_DESC_SIZE;
UINT8 *AppLibEncDescBuf = NULL;
static void *AppLibEncDescBufRaw = NULL;

static int ApplibVideoEncLiveviewInitFlag = -1;

int AppLibRecorderMemMgr_Init(void)
{
    int ReturnValue = 0;

    if (ApplibVideoEncLiveviewInitFlag == 0) {
        return 0;
    }

    ApplibVideoEncLiveviewInitFlag = 0;

    return ReturnValue;
}

int AppLibRecorderMemMgr_SetBufSize(UINT32 bitsBufSize, UINT32 descBufSize)
{
    int ReturnValue = 0;

    ApplibEncBitBufSize = bitsBufSize;
    ApplibEncDescBufSize = descBufSize;

    return ReturnValue;
}

int AppLibRecorderMemMgr_GetBufSize(UINT32 *bitsBufSize, UINT32 *descBufSize)
{
    int ReturnValue = 0;

    *bitsBufSize = ApplibEncBitBufSize;
    *descBufSize = ApplibEncDescBufSize;

    return ReturnValue;
}


int AppLibRecorderMemMgr_GetBufAddr(UINT8 **bitsBufAddr, UINT8 **descBufAddr)
{
    int ReturnValue = 0;

    *bitsBufAddr = AppLibEncBitsBuf;
    *descBufAddr = AppLibEncDescBuf;

    if (AppLibEncBitsBuf == NULL) {
        AmbaPrintColor(RED,"[Applib - RecorderMemMgr] <BufAllocate> bitsBufAddr = NULL");
    }

    if (AppLibEncDescBuf == NULL) {
        AmbaPrintColor(RED,"[Applib - RecorderMemMgr] <BufAllocate> descBufAddr = NULL");
    }


    return ReturnValue;
}



int AppLibRecorderMemMgr_BufAllocate(void)
{
    int ReturnValue = 0;

    /* Allocate bitstream buffer */
    DBGMSGc2(GREEN,"[Applib - RecorderMemMgr] <BufAllocate> !!");
    if (AppLibEncBitsBufRaw == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AppLibEncBitsBuf, &AppLibEncBitsBufRaw, ApplibEncBitBufSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - RecorderMemMgr] <BufAllocate> Out of cached memory for bitsFifo!!");
            AppLibEncBitsBuf = NULL;
            AppLibEncBitsBufRaw = NULL;
            return ReturnValue;
        }
    } else {
        AmbaPrintColor(RED,"[Applib - RecorderMemMgr] <BufAllocate> Get!!");
    }

    if (AppLibEncDescBufRaw == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AppLibEncDescBuf, &AppLibEncDescBufRaw, ApplibEncDescBufSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - RecorderMemMgr] <BufAllocate> Out of cached memory for bitsFifo!!");
            AppLibEncDescBuf = NULL;
            AppLibEncDescBufRaw = NULL;
        }
    } else {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <BufAllocate> The descrition of Get!");
    }

    return ReturnValue;
}


int AppLibRecorderMemMgr_BufFree(void)
{
    int ReturnValue = 0;

    if (AppLibEncBitsBufRaw != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AppLibEncBitsBufRaw);
        AppLibEncBitsBuf = NULL;
        AppLibEncBitsBufRaw = NULL;
    }
    if (AppLibEncDescBufRaw != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AppLibEncDescBufRaw);
        AppLibEncDescBuf = NULL;
        AppLibEncDescBufRaw = NULL;
    }

    return ReturnValue;
}


