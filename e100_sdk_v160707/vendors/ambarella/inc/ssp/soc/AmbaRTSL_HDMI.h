/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_HDMI.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for HDMI Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_HDMI_H_
#define _AMBA_RTSL_HDMI_H_

#include "AmbaHDMI_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for IEC 60958 Channel Status
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_HDMI_AUDIO_CHANNEL_STATUS_s_ {
    UINT8 Byte[24];

    struct {
        UINT32  Use:                        1;      /* [0] 0 = Consumer use (S/PDIF), 1 = Professional use (AES/EBU) */
        UINT32  LinearPCM:                  1;      /* [1] 0 = Audio sample word represents linear PCM samples */
        UINT32  Copyright:                  1;      /* [2] 0 = Copy restrict, 1 = Copy permit */
        UINT32  Preemphasis:                3;      /* [5:3] 0 = No pre-emphasis */
        UINT32  Mode:                       2;      /* [7:6] Subsequent bytes, always zero */

        UINT32  Category:                   7;      /* [14:8] Audio source category (general, CD-DA, DVD, etc.) */
        UINT32  L:                          1;      /* [15] L-bit, original or copy (see text) */

        UINT32  SourceNo:                   4;      /* [19:16] Source number, 0 = Unspecified */
        UINT32  ChannelNo:                  4;      /* [23:20] Channel number, 0 = Unspecified */
        UINT32  SampleFreq:                 4;      /* [27:24] 1 = Not indicated */
        UINT32  ClockAccuracy:              2;      /* [29:28] 0 = Level II, 1 = Level I, 2 = Level III */
        UINT32  Reserved:                   2;      /* [31:30] */

        UINT32  MaxWordLength:              1;      /* [32] 0 = 20 bits, 1 = 24 bits word length at maximum */
        UINT32  WordLength:                 3;      /* [35:33] Sample word length */
        UINT32  OriSampleFreq:              4;      /* [39:36] Original sampling frequency */

        UINT32  CGMSA:                      2;      /* [41:40] CGMS-A information */
        UINT32  Reserved1:                  22;     /* [63:42] */
        UINT32  Reserved2[4];                       /* [191:64] */
    } Consumer;
} AMBA_HDMI_AUDIO_CHANNEL_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 * RTSL HDMI Audio Clock Regeneration Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_HDMI_ACR_PARAM_s_ {
    UINT32  N;              /* N parameter */
    UINT32  CTS;            /* CTS paramter */
} AMBA_RTSL_HDMI_ACR_PARAM_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_HDMI.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_HdmiIsrCallBack)(UINT32);

#define AmbaRTSL_HdmiIsrRegister(pIsr)  AmbaRTSL_HdmiIsrCallBack = (pIsr)

void AmbaRTSL_HdmiInit(void);
void AmbaRTSL_HdmiSetPowerCtrl(UINT32 EnableFlag);
void AmbaRTSL_HdmiSetPhyCtrl(AMBA_HDMI_SINK_CURRENT_INCREMENT_e SinkCurrentInc, AMBA_HDMI_SINK_PREEMPHASIS_MODE_e SinkPreEmphasisMode);
void AmbaRTSL_HdmiCheckConnection(int *pHotPlug, int *pRxSense);
void AmbaRTSL_HdmiStart(AMBA_HDMI_CABLE_DETECT_e HdmiCableState);
void AmbaRTSL_HdmiStop(void);
void AmbaRTSL_HdmiPacketUpdate(void);
int  AmbaRTSL_HdmiDataIslandPacketCtrl(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket);
void AmbaRTSL_HdmiReset(void);
void AmbaRTSL_HdmiConfigVideo(AMBA_VIDEO_TIMING_s *pVideoTiming);
void AmbaRTSL_HdmiConfigAudio(UINT32 TmdsClock, AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);
UINT32 AmbaRTSL_HdmiGetAudioChannelCount(HDMI_AUDIO_CHANNEL_ALLOC_e SpeakerAlloc);
void AmbaRTSL_HdmiSendAVMUTE(int Flag);
void AmbaRTSL_HdmiDisableAudioChannel(int AudChanNo);
void AmbaRTSL_HdmiEnableAudioChannel(int AudChanNo);

/*-----------------------------------------------------------------------------------------------*\
 * HDMI CEC module
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_HdmiCecSetClkFreq(void);
void AmbaRTSL_HdmiCecEnable(void);
void AmbaRTSL_HdmiCecDisable(void);
int AmbaRTSL_HdmiCecSetLogicalAddress(UINT8 LogicalAddress);
int AmbaRTSL_HdmiCecTransmit(UINT8 *pMessage, UINT32 MsgSize);
int AmbaRTSL_HdmiCecReceive(UINT8 *pMessage, UINT32 *pMsgSize);

#endif /* _AMBA_RTSL_HDMI_H_ */
