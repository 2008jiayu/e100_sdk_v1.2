/**
 * @file src/app/connected/applib/inc/storage/ApplibStorage_Message.h
 *
 * Header of Storage message.
 *
 * History:
 *    2013/12/10 - [Martin Lai] created file
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
#ifndef APPLIB_STORAGE_MSG_H_
#define APPLIB_STORAGE_MSG_H_

/**
* @defgroup ApplibStorage_Message
* @brief Message definition for storage
*
*
*/

/**
 * @addtogroup ApplibStorage_Message
 * @ingroup Storage
 * @{
 */
#include <applibhmi.h>

__BEGIN_C_PROTO__


/**********************************************************************/
/* MDL_APPLIB_STORAGE_ID messsages                                        */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APPLIB_STORAGE_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: module or interface type ID
*   |15 -  8|: Self-defined
*   | 7 -  0|: Self-defined
* Note:
*   bit 0-15 could be defined in the module itself (individual
*   header files). However, module ID should be defined here for arrangement
**/
#define HMSG_STORAGE_MODULE(x)  MSG_ID(MDL_APPLIB_STORAGE_ID, MSG_TYPE_HMI, (x))
/** Sub-group:type of app library & interface events */
#define HMSG_STORAGE_MODULE_ID_COMMON   (0x01)
#define HMSG_STORAGE_MODULE_ID_ASYNCOP   (0x02)

#define HMSG_STORAGE_MODULE_COMMON(x)   HMSG_STORAGE_MODULE(((UINT32)HMSG_STORAGE_MODULE_ID_COMMON << 16) | (x))
#define HMSG_STORAGE_BUSY               HMSG_STORAGE_MODULE_COMMON(0x0001)
#define HMSG_STORAGE_IDLE               HMSG_STORAGE_MODULE_COMMON(0x0002)


/**async op msg*/
#define HMSG_STORAGE_MODULE_ASYNC_OP(x)  HMSG_STORAGE_MODULE(((UINT32)HMSG_STORAGE_MODULE_ID_ASYNCOP << 16) | (x))
/**The status about loop encode*/
#define HMSG_LOOP_ENC_DONE                HMSG_STORAGE_MODULE_ASYNC_OP(0x0001)
#define HMSG_LOOP_ENC_ERROR                HMSG_STORAGE_MODULE_ASYNC_OP(0x0002)
#define HMSG_LOOP_ENC_START             HMSG_STORAGE_MODULE_ASYNC_OP(0x0003)
/**emergency record */
#define HMSG_EM_RECORD_MERGE2NEW             HMSG_STORAGE_MODULE_ASYNC_OP(0x0004)
#define HMSG_EM_RECORD_MERGE2FIRST             HMSG_STORAGE_MODULE_ASYNC_OP(0x0005)
#define HMSG_EM_RECORD_CROP2NEW             HMSG_STORAGE_MODULE_ASYNC_OP(0x0006)
#define HMSG_EM_RECORD_RETURN             HMSG_STORAGE_MODULE_ASYNC_OP(0x0007)
#ifdef CONFIG_APP_ARD
/**For handle the empty track clip*/
#define HMSG_EMPTY_TRACK_HDLR_DONE                HMSG_STORAGE_MODULE_ASYNC_OP(0x0008)
#define HMSG_EMPTY_TRACK_HDLR_DEL             HMSG_STORAGE_MODULE_ASYNC_OP(0x0009)
#endif
__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_STORAGE_MSG_H_ */

