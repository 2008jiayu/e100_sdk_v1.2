/**
 * @file src/app/connected/applib/src/system/ApplibComSvc_MemMgr.c
 *
 *  Implementation of Application Memory manager
 *
 * History:
 *    2013/08/12 - [Martin Lai] created file
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


#include <comsvc/ApplibComSvc_MemMgr.h>

//#define DEBUG_APPLIB_MEM_MGR
#if defined(DEBUG_APPLIB_MEM_MGR)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

AMBA_KAL_BYTE_POOL_t *APPLIB_G_MMPL = NULL;
AMBA_KAL_BYTE_POOL_t *APPLIB_G_NC_MMPL = NULL;
AMBA_KAL_BYTE_POOL_t *APPLIB_FIFO_MMPL = NULL;
AMBA_KAL_BYTE_POOL_t APPLIB_TEMP_MMPL = {0};
UINT8 *ApplibDspWorkAreaResvStart = NULL;
UINT8 *ApplibDspWorkAreaResvLimit = NULL;
UINT8 *ApplibDspWorkAreaResvLimitAlign = NULL;
UINT32 ApplibDspWorkAreaResvSize = 0;

UINT32 ApplibDspWorkAreaAllocatedSize = 0;

static UINT32 FIFOBufsize;
static UINT8 *AppLibFIFOMem;
static void *AppLibFIFOMemBufRaw;
static int FIFObufallocateFlag = 0;
/**
 *  @brief To set the memory pool for applib.
 *
 *  To set the memory pool for applib.
 *
 *  @param [in] pMPL The memory address of Cache memory pool.
 *  @param [in] pNcMPL The memory address of Non-Cache memory pool.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcMemMgr_Init(AMBA_KAL_BYTE_POOL_t *pMPL, AMBA_KAL_BYTE_POOL_t *pNcMPL)
{
    int ReturnValue = 0;
    DBGMSGc2(BLUE, "[Applib - MemMgr] <%s> start",  __FUNCTION__);

    APPLIB_G_MMPL = pMPL;
    APPLIB_G_NC_MMPL = pNcMPL;

    DBGMSGc2(BLUE, "[Applib - MemMgr] <%s> end",  __FUNCTION__);

    return ReturnValue;
}

/**
 *  @brief To set the DSP memory address
 *
 *  To set the DSP memory address
 *
 *  @param [in] resvStart The start address
 *  @param [in] resvLimit The end address
 *  @param [in] resvSize The size
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcMemMgr_SetDspMemory(UINT8 *resvStart , UINT8 *resvLimit, UINT32 resvSize)
{
    int ReturnValue = 0;
    DBGMSGc2(BLUE, "[Applib - MemMgr] <%s> SetDspMemory start",  __FUNCTION__);

    ApplibDspWorkAreaResvStart = resvStart;
    ApplibDspWorkAreaResvLimit = resvLimit;
    ApplibDspWorkAreaResvSize = resvSize;
    DBGMSGc2(BLUE, "[Applib - MemMgr] <%s> SetDspMemory end",  __FUNCTION__);

    return ReturnValue;
}

int AppLibComSvcMemMgr_FIFOMemInit(UINT32 BufSize)
{
    int ReturnValue = 0;
    FIFOBufsize = BufSize;
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AppLibFIFOMem, &AppLibFIFOMemBufRaw, BufSize, 32);
    if (ReturnValue != OK) {
        FIFObufallocateFlag = -1;
    }
    return ReturnValue;

}

int AppLibComSvcMemMgr_FIFOBufAllocate(void)
{
    int ReturnValue = 0;
    if (FIFObufallocateFlag == 0) {
        ReturnValue = AmbaKAL_BytePoolCreate(&APPLIB_TEMP_MMPL, AppLibFIFOMem, FIFOBufsize);
        if (ReturnValue != OK) {
            DBGMSG("[Applib - MemMgr] <%s> Create Byte Pool Fail",__FUNCTION__);
            return ReturnValue;
        }
        APPLIB_FIFO_MMPL = &APPLIB_TEMP_MMPL;
        FIFObufallocateFlag = 1;
    } else if (FIFObufallocateFlag == -1) {
        DBGMSG("[Applib - MemMgr] <FIFOMemInit> Get Buff Fail");
    }
    return ReturnValue;
}

int AppLibComSvcMemMgr_AllocateDSPMemory(UINT8 **Addr, UINT32 Size)
{
    int ReturnValue = 0;
    DBGMSGc2(BLUE, "[Applib - MemMgr] <%s> AllocateDSPMemory start",  __FUNCTION__);
    Size = ALIGN_64(Size);
    if (ApplibDspWorkAreaAllocatedSize == 0) {
        ApplibDspWorkAreaResvLimitAlign = (UINT8*)ALIGN_64((UINT32)ApplibDspWorkAreaResvLimit-64);
    }
    (*Addr) = ApplibDspWorkAreaResvLimitAlign - Size - ApplibDspWorkAreaAllocatedSize;
    ApplibDspWorkAreaAllocatedSize = ApplibDspWorkAreaAllocatedSize + Size;
    return ReturnValue;

}

int AppLibComSvcMemMgr_FreeDSPMemory(void)
{
    ApplibDspWorkAreaAllocatedSize = 0;
    return 0;
}
