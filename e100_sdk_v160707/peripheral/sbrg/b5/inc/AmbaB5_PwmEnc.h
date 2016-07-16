/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_PwmEnc.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 PWMENC Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_PWMENC_H_
#define _AMBA_B5_PWMENC_H_

typedef enum _AMBA_B5_PWN_ENC_CHUNK_SIZE_e_ {
    CHUNK_SIZE_4_BYTES,
    CHUNK_SIZE_8_BYTES,
    CHUNK_SIZE_16_BYTES,
    CHUNK_SIZE_32_BYTES,
    CHUNK_SIZE_64_BYTES
} AMBA_B5_PWN_ENC_CHUNK_SIZE_e;

typedef struct _AMBA_B5_PWN_ENC_CREDIT_CONFIG_s_ {
    UINT8 Credit[4];
    AMBA_B5_PWN_ENC_CHUNK_SIZE_e ChunkSize;
} AMBA_B5_PWN_ENC_CREDIT_CONFIG_s;

typedef struct _AMBA_B5_PWM_ENC_CONFIG_s_ {
    UINT8                               ReplayTimes;            /* [0] */
    AMBA_B5_PWN_ENC_CREDIT_CONFIG_s     CreditConfig;           /* [1] */
    UINT8                               WriteFifoTh;
    UINT8                               RFifoNearFullTh[4];
    UINT16                              ChannelDelayTime[4];
    UINT16                              ChannelTimeOut[4];
} AMBA_B5_PWM_ENC_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_PWMENC.c
\*---------------------------------------------------------------------------*/
int AmbaB5_PwmEncInit(void);
int AmbaB5_PwmEncEnable(void);
int AmbaB5_PwmSetReplayTimes(UINT8 ReplayTimes);
int AmbaB5_PwmEncSetCreditforCh0(UINT8 Credit);
int AmbaB5_PwmEncSetCreditforCh1(UINT8 Credit);
int AmbaB5_PwmEncSetCreditforCh2(UINT8 Credit);
int AmbaB5_PwmEncSetCreditforCh3(UINT8 Credit);
int AmbaB5_PwmEncSetChunkSize(AMBA_B5_PWN_ENC_CHUNK_SIZE_e ChunkSize);
int AmbaB5_PwmEncResetAll(void);
int AmbaB5_PwmEncSetWriteFifoTh(UINT8 Threshold);
int AmbaB5_PwmEncSetRFifoNearFullTh(UINT8 Ch0Th,UINT8 Ch1Th,UINT8 Ch2Th,UINT8 Ch3Th);
int AmbaB5_PwmEncDelayMeterEnable(void);
int AmbaB5_PwmEncSetChDelayTime(UINT16 Ch0DelayTime, UINT16 Ch1DelayTime,UINT16 Ch2DelayTime,UINT16 Ch3DelayTime);
int AmbaB5_PwmEncSetChTimeOut(UINT16 Ch0TimeOut, UINT16 Ch1TimeOut,UINT16 Ch2TimeOut,UINT16 Ch3TimeOut);
int AmbaB5_PwmEncResetChannelStatus(UINT8 Channel);
int AmbaB5_PwmEncGetCtrlStatus(UINT32 *DataBuf);

#endif  /* _AMBA_B5_PWMENC_H_ */
