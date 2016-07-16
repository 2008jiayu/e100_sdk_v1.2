/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_PWM.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Stepper Motor Controller RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_STEPPER_H_
#define _AMBA_RTSL_STEPPER_H_

typedef enum _AMBA_STEPPER_CHANNEL_e_ {
    AMBA_STEPPER_CHANNELA,              /* Stepper Channel-A */
    AMBA_STEPPER_CHANNELB,              /* Stepper Channel-B */
    AMBA_STEPPER_CHANNELC,              /* Stepper Channel-C */
    AMBA_STEPPER_CHANNELD,              /* Stepper Channel-D */
    AMBA_STEPPER_CHANNELE,              /* Stepper Channel-E */

    AMBA_NUM_STEPPER_CHANNEL            /* Number of Stepper Channels */
} AMBA_STEPPER_CHANNEL_e;

typedef enum _AMBA_MICRO_STEPPER_CHANNEL_e_ {
    AMBA_MICRO_STEPPER_CHANNEL0,        /* Micro Stepper Channel-A */
    AMBA_MICRO_STEPPER_CHANNEL1,        /* Micro Stepper Channel-B */
    AMBA_MICRO_STEPPER_CHANNEL2,        /* Micro Stepper Channel-C */
    AMBA_MICRO_STEPPER_CHANNEL3,        /* Micro Stepper Channel-D */

    AMBA_NUM_MICRO_STEPPER_CHANNEL      /* Number of Micro Stepper Channels */
} AMBA_MICRO_STEPPER_CHANNEL_e;

/*-----------------------------------------------------------------------------------------------*\
 * Configuration Parameter Structure
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_NUM_STEPPER_CHANNEL_PIN    4
#define AMBA_NUM_STEPPER_TIME_SLICE     64

typedef struct _AMBA_STEPPER_CONFIG_s_ {
    UINT32  EventFreq;
    UINT8   EventList[AMBA_NUM_STEPPER_CHANNEL_PIN][AMBA_NUM_STEPPER_TIME_SLICE];
    UINT32  NumTimeSlices;  /* Effective time slices */
    UINT32  StepSize;       /* in number of event time slice */
} AMBA_STEPPER_CONFIG_s;

typedef struct _AMBA_STEPPER_BURST_CONFIG_s_ {
    UINT32  EventFreq;
    UINT32  EventFreq0;     /* For Acc/Dec */
    UINT32  EventFreq1;
    UINT32  EventFreq2;
    UINT32  EventFreq3;
    UINT32  AccType;
    UINT32  AccCount;
} AMBA_STEPPER_BURST_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_Stepper.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_StepperInit(void);
int AmbaRTSL_StepperForward(AMBA_STEPPER_CHANNEL_e StepperChanNo, UINT32 NumSteps, AMBA_STEPPER_CONFIG_s *pStepperConfig);
int AmbaRTSL_StepperBackward(AMBA_STEPPER_CHANNEL_e StepperChanNo, UINT32 NumSteps, AMBA_STEPPER_CONFIG_s *pStepperConfig);
int AmbaRTSL_StepperSetConfig(AMBA_STEPPER_CHANNEL_e StepperChanNo, AMBA_STEPPER_CONFIG_s *pStepperConfig);
int AmbaRTSL_StepperGetRunningCounter(AMBA_STEPPER_CHANNEL_e StepperChanNo);

int AmbaRTSL_StepperSetBurstConfig(AMBA_STEPPER_CHANNEL_e StepperChanNo, AMBA_STEPPER_BURST_CONFIG_s *pStepperBurstConfig);


#endif /* _AMBA_RTSL_STEPPER_H_ */
