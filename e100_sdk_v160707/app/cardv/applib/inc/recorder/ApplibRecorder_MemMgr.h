/**
 * @file src/app/connected/applib/inc/recorder/ApplibRecorder_MemMgr.h
 *
 * Header of recorder's buffer manager
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
#ifndef APPLIB_RECORD_MEMMGR_H_
#define APPLIB_RECORD_MEMMGR_H_
/**
* @defgroup ApplibRecorder_MemMgr
* @brief recorder's buffer manager
*
*
*/

/**
 * @addtogroup ApplibRecorder_MemMgr
 * @ingroup Recorder
 * @{
 */
#include <applib.h>
__BEGIN_C_PROTO__

/**
 *
 * Allocate buffer for recorder
 *
 * @return >=0 success, <0 failure.
 */
extern int AppLibRecorderMemMgr_BufAllocate(void);

/**
 *
 * Free buffer for recorder
 *
 * @return >=0 success, <0 failure.
 */
extern int AppLibRecorderMemMgr_BufFree(void);

/**
 *  Set the buffer size
 *
 *  @param [in] bitsBufSize Bits fifo buffer size
 *  @param [in] descBufSize Fifo description buffer size
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibRecorderMemMgr_SetBufSize(UINT32 bitsBufSize, UINT32 descBufSize);

/**
 *
 *Get buffer address for recorder
 *
 * @return >=0 success, <0 failure.
 */
extern int AppLibRecorderMemMgr_GetBufAddr(UINT8 **bitsBufAddr, UINT8 **descBufAddr);


/**
 *
 *Get buffer size for recorder
 *
 * @return >=0 success, <0 failure.
 */
 extern int AppLibRecorderMemMgr_GetBufSize(UINT32 *bitsBufSize, UINT32 *descBufSize);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_RECORD_MEMMGR_H_ */

