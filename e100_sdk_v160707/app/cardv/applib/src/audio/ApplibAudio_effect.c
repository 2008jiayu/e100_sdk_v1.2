/**
 * @file 
 *
 * Implementation of beep sound.
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

#include <applib.h>
#include "audio/ApplibAudio_effect.h"

//#define DEBUG_APPLIB_AUDIOEFFECT
#if defined(DEBUG_APPLIB_AUDIOEFFECT)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static AMBA_AUDIO_PLUGIN_EFFECT_CS_s AppLibAudioEffectControl[APPLIB_AUDIO_EFFECT_MAX]; //main control structure
static AMBA_AUDIO_PLUGIN_EFFECT_CS_s AppLibAudioEffectControlShadow[APPLIB_AUDIO_EFFECT_MAX]; //shadow of the "main control structure"
static APPLIB_AUDIO_EFFECT_VOLUME_PROC_t AppLibAudioEffectVolumeConfig[APPLIB_AUDIO_EFFECT_MAX]; //self control structure
static APPLIB_AUDIO_EFFECT_VOLUME_PROC_t AppLibAudioEffectVolumeConfigShadow[APPLIB_AUDIO_EFFECT_MAX]; //shadow of the "self control structure"

/* Install effect */
static int AppLibAudio_EffectInstall(APPLIB_AUDIO_EFFECT_e type,UINT32 *pHdlr,UINT8 effectId,void* config,UINT32 config_size,T_fPROC proc)
{
    int ReturnValue = 0;
    
    if ((effectId < AMBA_AUDIO_DEFAULT_INPUT_EFFECT_NUM) ||(effectId >= MAX_AUDIO_EFFECT) ||(type >= APPLIB_AUDIO_EFFECT_MAX)) {
        AmbaPrintColor(RED,"Wrong EffectID(%d %d) %u", effectId,type,__LINE__);
        return -1;
    }

	memset(&AppLibAudioEffectControl[type], 0x0, sizeof(AMBA_AUDIO_PLUGIN_EFFECT_CS_s));
	memset(&AppLibAudioEffectControlShadow[type], 0x0, sizeof(AMBA_AUDIO_PLUGIN_EFFECT_CS_s));
	AppLibAudioEffectControl[type].proc = proc;
	AppLibAudioEffectControl[type].size_of_self = config_size;
	AppLibAudioEffectControl[type].self = config;
	AppLibAudioEffectControl[type].dest_auto_assign = 1;
	AppLibAudioEffectControl[type].dest_ch_auto_assign = 1;
	AppLibAudioEffectControl[type].dest_size_auto_assign = 1;

    if (pHdlr) {
	if(type == APPLIB_AUDIO_EFFECT_INPUT){
		AmbaAudio_InputPluginEffectInstall(pHdlr, effectId, &AppLibAudioEffectControl[type]);
	}else{
		AmbaAudio_OutputPluginEffectInstall(pHdlr, effectId, &AppLibAudioEffectControl[type]);
	}
    }else{
    	ReturnValue = -1;
    }

    return ReturnValue;
}

/* Enable effect*/
static int AppLibAudioEnc_EffectEnable(APPLIB_AUDIO_EFFECT_e type,UINT32 *pHdlr,UINT8 effectId, UINT8 enable)
{
	int ReturnValue = 0;

	if ((effectId < AMBA_AUDIO_DEFAULT_INPUT_EFFECT_NUM) ||(effectId >= MAX_AUDIO_EFFECT) ||(type >= APPLIB_AUDIO_EFFECT_MAX)) {
		AmbaPrintColor(RED,"Wrong EffectID(%d %d) %u", effectId,type,__LINE__);
		return -1;
	}

	if (enable) {
		if(type == APPLIB_AUDIO_EFFECT_INPUT){
			ReturnValue = AmbaAudio_InputPluginEffectEnable(pHdlr, effectId);
		}else{
			ReturnValue = AmbaAudio_OutputPluginEffectEnable(pHdlr, effectId);
		}
	} else {
		if(type == APPLIB_AUDIO_EFFECT_INPUT){
			ReturnValue = AmbaAudio_InputPluginEffectDisable(pHdlr, effectId);
		}else{
			ReturnValue = AmbaAudio_OutputPluginEffectDisable(pHdlr, effectId);
		}
	}

	return ReturnValue;
}

static void AppLibAudio_EffectVolumeProc(AMBA_AUDIO_PLUGIN_EFFECT_CS_s *audio_cs)
{
	UINT32 i = 0,j = 0;
	UINT32 *signal= (UINT32 *)audio_cs->src;
	UINT32 *result = (UINT32*)audio_cs->dest;
	UINT32 procsize = (UINT32)audio_cs->src_size;
	UINT32 channal = audio_cs->src_ch;
	APPLIB_AUDIO_EFFECT_VOLUME_PROC_t *EffectBypassCfg = audio_cs->self;

	if (EffectBypassCfg->volume == 0) {
		for(i = 0 ; i < channal; i++) {
			for(j = 0; j <procsize; j++) {
				*result++ = 0;
			}
		}
	}else if (EffectBypassCfg->volume == APPLIB_AUDIO_MAX_VOLUME_LEVLE) {
		for(i = 0 ; i < channal; i++) {
			for(j = 0; j <procsize; j++) {
				*result++ = *signal++;
			}
		}
	}else {
		INT32 *pSrc = (INT32*)signal;
		INT32 *pDst = (INT32*)result;

		for(i = 0 ; i < channal; i++) {
			for(j = 0; j <procsize; j++) {
				*pDst++ = ((*pSrc++)/APPLIB_AUDIO_MAX_VOLUME_LEVLE)*EffectBypassCfg->volume;
			}
		}
	}
}

/* Change effect parameters*/
int AppLibAudio_EffectVolumeUpdate(APPLIB_AUDIO_EFFECT_e type,UINT32 *pHdlr,UINT16 volume)
{
	int ReturnValue = 0;

	if(volume > APPLIB_AUDIO_MAX_VOLUME_LEVLE){
		volume = APPLIB_AUDIO_MAX_VOLUME_LEVLE;
	}

	AppLibAudioEffectVolumeConfigShadow[type].volume= volume;
	AppLibAudioEffectControlShadow[type].self = &AppLibAudioEffectVolumeConfigShadow[type];

	if (pHdlr) {
		if(type == APPLIB_AUDIO_EFFECT_INPUT){
			ReturnValue = AmbaAudio_InputPluginEffectUpdate(pHdlr, APPLIB_AUDIO_EFFECT_VOLUME_ID, &AppLibAudioEffectControlShadow[type]);
		}else{
			ReturnValue = AmbaAudio_OutputPluginEffectUpdate(pHdlr, APPLIB_AUDIO_EFFECT_VOLUME_ID, &AppLibAudioEffectControlShadow[type]);		
		}
	}else{
		ReturnValue = -1;
	}
	
	return ReturnValue;
}

int AppLibAudio_EffectVolumeInstall(APPLIB_AUDIO_EFFECT_e type,UINT32 *pHdlr)
{
	int ReturnValue = 0;
	UINT32 config_size = sizeof(APPLIB_AUDIO_EFFECT_VOLUME_PROC_t);

	AppLibAudioEffectVolumeConfig[type].volume = APPLIB_AUDIO_MAX_VOLUME_LEVLE;
	ReturnValue = AppLibAudio_EffectInstall(type,pHdlr,APPLIB_AUDIO_EFFECT_VOLUME_ID,(void*)&AppLibAudioEffectVolumeConfig[type],config_size,AppLibAudio_EffectVolumeProc);
	ReturnValue = AppLibAudioEnc_EffectEnable(type,pHdlr,APPLIB_AUDIO_EFFECT_VOLUME_ID,1);

	return ReturnValue;
}

