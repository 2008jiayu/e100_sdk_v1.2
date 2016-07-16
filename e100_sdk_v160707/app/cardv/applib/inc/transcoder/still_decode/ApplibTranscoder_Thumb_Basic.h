/**
 * @file src/app/connected/applib/inc/transcoder/still_decode/ApplibTranscoder_Thumb_Basic.h
 *
 * The apis provide basic function for retrieving IDR frame
 *
 * History:
 *    2015/03/17 - [cichen] Create file
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
 * @defgroup ApplibTranscoder_Thumb_Basic       ApplibTranscoder_Thumb_Basic
 * @brief to retrieve IDR frame.
 *
 */

/**
 * @addtogroup ApplibTranscoder_Thumb_Basic
 * @ingroup StillDecode
 * @{
 */

#ifndef APPLIB_TRANSCODER_THUMB_BASIC_H_
#define APPLIB_TRANSCODER_THUMB_BASIC_H_

/**
 * Image source type
 */
typedef enum _TRANS_STILL_IMAGE_SOURCE_TYPE_e_{
    TRANS_STILL_IMAGE_SOURCE_FULL,         /**< image source type for full resolution */
    TRANS_STILL_IMAGE_SOURCE_THUMBNAIL,    /**< image source type for thumbnail */
    TRANS_STILL_IMAGE_SOURCE_SCREENNAIL,   /**< image source type for screennail */
    TRANS_STILL_IMAGE_SOURCE_NUM           /**< image source type boundary */
} TRANS_STILL_IMAGE_SOURCE_TYPE_e;

/**
 * Applib Data Buffer Structure Definitions
 */
typedef struct _TRANS_STILL_DATA_BUF_s_ {
    UINT8 *Buf;             /**< buffer address*/
    UINT32 BufSize;         /**< size of buffer */
    UINT32 RetDataSize;    /**< size of data that are actually written into Buf */
} TRANS_STILL_DATA_BUF_s;

/**
 *  @brief Initialize basic thumbnail module
 *
 *  @return 0 success, <0 failure
 */
int AppLibTranscoderThmBasic_Init(void);

/**
 * Un-initialize basic thumbnail module.
 *
 * @return 0 - success, others - failure
 */
int AppLibTranscoderThmBasic_Uninit(void);

/**
 * Get IDR frame from specified file
 *
 * @param [in] Filename    File name
 * @param [in] DataBuf     Information of data buffer
 *
 * @return 0 - OK, success - failure
 */
int AppLibTranscoderThmBasic_GetIdrFrame(char *Filename, TRANS_STILL_DATA_BUF_s *DataBuf);

/**
 * Get image (full image/thumbnail/screennail) from specified file
 *
 * @param [in] Filename File name
 * @param [in] Type         Image source selction
 * @param [in] DataBuf  Information of data buffer
 *
 * @return 0 - OK, success - failure
 */
int AppLibTranscoderThmBasic_GetImage(char *Filename, TRANS_STILL_IMAGE_SOURCE_TYPE_e Type, TRANS_STILL_DATA_BUF_s *DataBuf);

#endif /* APPLIB_TRANSCODER_THUMB_BASIC_H_ */

/**
 * @}
 */     // End of group ApplibTranscoder_Thumb_Basic
