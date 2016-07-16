/**
 * @file 
 *
 * Header of beep sound implementation
 *
 * History:
 *    2015/3/11 - [Qiang Su] created file
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

#ifndef APPLIB_EFFECT_H_
#define APPLIB_EFFECT_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaUtility.h>

__BEGIN_C_PROTO__ 

#define APPLIB_AUDIO_MAX_VOLUME_LEVLE (MAX_AUDIO_VOLUME_LEVLE)

#define APPLIB_AUDIO_EFFECT_VOLUME_ID (AMBA_AUDIO_DEFAULT_INPUT_EFFECT_NUM+1)

typedef void (*T_fPROC)(struct _AMBA_AUDIO_PLUGIN_EFFECT_CS_s_ *);

/* AppLib Audio Effect Volume structure */
typedef struct _AUDIO_EFFECT_PROC_s {
	UINT16 volume; 
} APPLIB_AUDIO_EFFECT_VOLUME_PROC_t;

typedef enum{
	APPLIB_AUDIO_EFFECT_INPUT,
	APPLIB_AUDIO_EFFECT_OUTPUT,
	APPLIB_AUDIO_EFFECT_MAX	
}APPLIB_AUDIO_EFFECT_e;

extern int AppLibAudio_EffectVolumeUpdate(APPLIB_AUDIO_EFFECT_e type,UINT32 *pHdlr,UINT16 volume);
extern int AppLibAudio_EffectVolumeInstall(APPLIB_AUDIO_EFFECT_e type,UINT32 *pHdlr);

__END_C_PROTO__

#endif /* APPLIB_BEEP_H_ */
