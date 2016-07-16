/**
 * @file src/app/connected/applib/inc/graphics/DirectDraw/ApplibGraphics_DirectDraw.h
 *
 * ApplibGraphics_DirectDraw include Amba DirectDraw related
 *
 * History:
 *    2015/02/05 - [Jamie Cheng] created file
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

#ifndef _APPLIB_GRAPHICS_DirectDraw_H_
#define _APPLIB_GRAPHICS_DirectDraw_H_

#include <display/Osd.h>
#include <msgqueue.h>

__BEGIN_C_PROTO__ 
/*************************************************************************
 * DD Enums
 ************************************************************************/
/**
 * DirectDraw Action Type
 */
typedef enum _APPLIB_DIRECTDRAW_ACTION_TYPE_e_ {
    APPLIB_DIRECTDRAW_ACTION_GET_BUFFER = 0x00,
    APPLIB_DIRECTDRAW_ACTION_SHOW_ON_SCREEN,
} APPLIB_DIRECTDRAW_ACTION_TYPE_e;

/**
 * DirectDraw Task Message
 */
typedef enum _APPLIB_DIRECTDRAW_TASK_MSG_e_ {
    APPLIB_DIRECTDRAW_TASK_MSG_DRAW = 0,
	APPLIB_DIRECTDRAW_TASK_MSG_HIDE,
    APPLIB_DIRECTDRAW_TASK_MSG_NUM
} APPLIB_DIRECTDRAW_TASK_MSG_e;

/*************************************************************************
 * DIRECTDRAW Structures
 ************************************************************************/
/**
 * Attribute Format for DIRECTDRAW GUI Object
 */
typedef struct _APPLIB_GRAPHIC_DIRECTDRAW_CNT_s_ {
	APPLIB_DIRECTDRAW_ACTION_TYPE_e Act;        /**< Action of DirectDraw obj       */
    UINT32 Width;                               /**< Width of GUI obj				*/
    UINT32 Height;                              /**< Height of GUI obj				*/
    UINT32 Pitch;                               /**< Picth of GUI obj				*/
    UINT32 Ptr;                                 /**< Buffer point of DirectDraw obj	*/
	UINT32 VoutChn;                             /**< What vout channel to draw      */
	UINT8  ShowFlg;                             /**< Show flag of DirectDraw obj    */
} APPLIB_GRAPHIC_DIRECTDRAW_CNT_s;

typedef struct _APPLIB_DIRECTDRAW_TASK_MSG_s_ {
    APPLIB_DIRECTDRAW_TASK_MSG_e MessageType;
    void *Parameter;
	void (*Refresh)(void *parameter);
	void (*Clean)(void *parameter);
} APPLIB_DIRECTDRAW_TASK_MSG_s;

/**
 * Resource for DirectDraw task.
 */
typedef struct _APPLIB_DIRECTDRAW_TASK_RESOURCE_s_{
    UINT8 IsInit;
    AMBA_KAL_TASK_t ddTask;
    AMP_MSG_QUEUE_HDLR_s ddMsgQueue;
    APPLIB_DIRECTDRAW_TASK_MSG_s *ddMsgPool;
} APPLIB_DIRECTDRAW_TASK_RESOURCE_s;

/*************************************************************************
 * DD Functions
 ************************************************************************/
/**
* To init DirectDraw Task
*
* @return 0 - OK, others - failure
*/
extern int AppLibDirectDraw_TaskInit(void);

/**
* To Destroy DirectDraw Task
*
* @return 0 - OK, others - failure
*/
extern int AppLibDirectDraw_DeinitTask(void);

/**
* To Send message to DirectDraw Task execute
*
* @return 0 - OK, others - failure
*/
extern int AppLibDirectDraw_SendMsg(const APPLIB_DIRECTDRAW_TASK_MSG_s *ddMsg, const UINT32 Timeout);

__END_C_PROTO__
#endif /* _APPLIB_GRAPHICS_DirectDraw_H_ */
