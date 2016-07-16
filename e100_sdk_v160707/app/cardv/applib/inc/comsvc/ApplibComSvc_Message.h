/**
 * @file src/app/connected/applib/inc/comsvc/ApplibComSvc_Message.h
 *
 * Header of common services' message.
 *
 * History:
 *    2014/04/15 - [Martin Lai] created file
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
#ifndef APPLIB_COMSVC_MSG_H_
#define APPLIB_COMSVC_MSG_H_
/**
* @defgroup ApplibComSvc_Message
* @brief common services' message.
*
*
*/

/**
 * @addtogroup ApplibComSvc_Message
 * @ingroup CommonService
 * @{
 */
#include <applibhmi.h>

__BEGIN_C_PROTO__

/**********************************************************************/
/* MDL_APPLIB_COMSVC_ID messsages                                        */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APPLIB_COMSVC_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: module or interface type ID
*   |15 -  8|: Self-defined
*   | 7 -  0|: Self-defined
* Note:
*   bit 0-15 could be defined in the module itself (individual
*   header files). However, module ID should be defined here
*   for arrangement
**/
#define HMSG_COMSVC_MODULE(x)  MSG_ID(MDL_APPLIB_COMSVC_ID, MSG_TYPE_HMI, (x))

#define HMSG_COMSVC_MODULE_ID_ASYNC            (0x01) /**< Sub-group:type of app library & interface events */
#define HMSG_COMSVC_MODULE_ID_TIMER            (0x02) /**< Sub-group:type of app library & interface events */

/** Async op module events */
/** Details in ApplibComSvc_AsyncOp.h */
#define HMSG_COMSVC_MODULE_ASYNC(x)    HMSG_COMSVC_MODULE(((UINT32)HMSG_COMSVC_MODULE_ID_ASYNC << 16) | (x))

/** Timer module events */
/** Details in ApplibComSvc_Timer.h */
#define HMSG_COMSVC_MODULE_TIMER(x)    HMSG_COMSVC_MODULE(((UINT32)HMSG_COMSVC_MODULE_ID_TIMER << 16) | (x))


__END_C_PROTO__

#endif /* APPLIB_COMSVC_MSG_H_ */

