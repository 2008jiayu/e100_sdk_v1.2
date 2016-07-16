/**
 * @file app/connected/applib/src/audio/beep.h
 *
 * Header of beep sound implementation
 *
 * History:
 *    2014/12/15 - [Jamie Cheng] created file
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

#ifndef APPLIB_BEEP_H_
#define APPLIB_BEEP_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <player/Decode.h>
#include <player/AudioDec.h>
#include <player/Beep.h>
#include <AmbaUtility.h>
#include <msgqueue.h>

__BEGIN_C_PROTO__
/*************************************************************************
 * Beep Sound Definitions
 ************************************************************************/
/**
 * Beep sound Structure Definitions
 */
typedef struct _APPLIB_BEEP_s_ {
    char Fn[64]; /**< file name of beep sound */
    UINT8 BeepType; /**< type of beep sound */
#define BEEP_FROM_FILE 0
#define BEEP_FROM_INDEX 1
#define BEEP_FROM_USER 2
    UINT8 BeepIndex; /**< index of beep sound */
    UINT8 *RawPcm; /**< raw buffer of beep sound */
    UINT32 RawSize; /**< size of beep sound */
    UINT32 SrcSampleRate; /**< sample rate of beep */
    UINT32 DstSampleRate; /**< sample rate of beep */
    UINT32 SrcChannelMode; /**< channel mode of beep */
} APPLIB_BEEP_s;

/**
 * The type of Vout message in "Still Task"
 */
typedef enum _APPLIB_AUDIO_BEEP_TASK_MSG_e_ {
    APPLIB_AUDIO_BEEP_TASK_MSG_PLAY_BEEP = 0,
#ifdef CONFIG_APP_ARD
    /*Avoid MACRO name conflict in ApplibAudio_beep.c*/
    APPLIB_AUDIO_BEEP_TASK_MSG_NUMBER
#else
    APPLIB_AUDIO_BEEP_TASK_MSG_NUM
#endif
} APPLIB_AUDIO_BEEP_TASK_MSG_e;

/**
 * Message as an input to "Audio Beep Task"
 * It can be either a feed message, a decode message,
 * a rescale message, or a display mesage.
 */
typedef struct _APPLIB_AUDIO_BEEP_TASK_MSG_s_ {
    APPLIB_AUDIO_BEEP_TASK_MSG_e MessageType;
    APPLIB_BEEP_s beepInfo;
} APPLIB_AUDIO_BEEP_TASK_MSG_s;

/**
 * Resource for Audio beep task.
 */
typedef struct _APPLIB_AUDIO_BEEP_TASK_RESOURCE_s_{
    UINT8 IsInit;
    AMBA_KAL_TASK_t beepTask;
    AMP_MSG_QUEUE_HDLR_s beepMsgQueue;
    APPLIB_AUDIO_BEEP_TASK_MSG_s *beepMsgPool;
} APPLIB_AUDIO_BEEP_TASK_RESOURCE_s;

/**
* To init beep module
*
* @return 0 - OK, others - failure
*/
extern int AppLibAudio_BeepInit(void);

/**
* To output beep sound
*
* @param [in] beep - beep sound info
*
* @return 0 - OK, others - failure
*/
extern int AppLibAudio_BeepPlay(APPLIB_BEEP_s beep);

/**
* To Send message to beep task execute
*
* @param [in] audioBeepMsg - beep task infomation include beep message type and beep infomation
* @param [in] timeOut - timeout of send mesage time

* @return 0 - OK, others - failure
*/
extern int AppLibAudio_SendMsg(const APPLIB_AUDIO_BEEP_TASK_MSG_s *audioBeepMsg, const UINT32 timeOut);

#ifdef CONFIG_APP_ARD
extern void AppLibAudio_Beep_Wait_Done(UINT32 TimeoutMs);
extern void AppLibAudio_BeepSetPlayFlag(void);
#endif
__END_C_PROTO__

#endif /* APPLIB_BEEP_H_ */
