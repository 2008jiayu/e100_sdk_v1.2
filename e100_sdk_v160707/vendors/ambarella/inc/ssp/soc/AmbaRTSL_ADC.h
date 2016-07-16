/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_ADC.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for ADC RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_ADC_H_
#define _AMBA_RTSL_ADC_H_

#include "AmbaADC_Def.h"

#define AMBA_NUM_ADC_TIME_SLOT      8       /* Total Number of ADC Time Slots */
#define AMBA_ADC_FIFO_SIZE          1024

typedef enum _AMBA_ADC_FIFO_e_ {
    AMBA_ADC_FIFO0,                         /* 0th ADC FIFO */
    AMBA_ADC_FIFO1,                         /* 1st ADC FIFO */
    AMBA_ADC_FIFO2,                         /* 2nd ADC FIFO */
    AMBA_ADC_FIFO3,                         /* 3rd ADC FIFO */

    AMBA_ADC_NUM_FIFO                       /* Number of ADC Channels */
} AMBA_ADC_FIFO_e;

typedef enum _AMBA_ADC_INT_TYPE_e_ {
    AMBA_ADC_INT_EVENT_COUNTER      = 0x01, /* Event counter interrupt */
    AMBA_ADC_INT_SLOT_PERIOD_ERR    = 0x02, /* Sample period over limit interrupt */
    AMBA_ADC_INT_INVAILD_FIFO_PARAM = 0x04, /* Invaild fifo param interrupt */
    AMBA_ADC_INT_FIFO_ERROR         = 0x08, /* FIFO full, underflow or overflow */
    AMBA_ADC_INT_DATA_EXCEPTION     = 0x10  /* ADC Channel Interrupt */
} AMBA_ADC_INT_TYPE_e;

typedef enum _AMBA_ADC_VCM_OPTION_e_ {
    AMBA_ADC_VCM_OPTION_1,
    AMBA_ADC_VCM_OPTION_2
} AMBA_ADC_VCM_OPTION_e;

typedef enum _AMBA_ADC_VCM_REFER_e_ {
    AMBA_ADC_VCM_AB_REF_REG_VAL,
    AMBA_ADC_VCM_A_REF_REG_B_REF_ADC_CHANNEL,
    AMBA_ADC_VCM_A_REF_ADC_CHANNEL_B_REF_REG,
    AMBA_ADC_VCM_AB_REF_ADC_CHANNEL
} AMBA_ADC_VCM_REFER_e;

/*-----------------------------------------------------------------------------------------------*\
 * ADC Status Structures
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_ADC_PARAM_s_ {
    UINT8   TimeSlotNumberUsed;                         /* Number of valid time slots */
    UINT16  TimeSlotPeriod;                             /* Ticks of one time slot period */
    UINT16  TimeSlotChannels[AMBA_NUM_ADC_TIME_SLOT];  /* Enabled ADC channels in round-robin scheduling */
} AMBA_RTSL_ADC_PARAM_s;

/*-----------------------------------------------------------------------------------------------*\
 * ADC VCM Structures
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_ADC_VCM_s_ {
    AMBA_ADC_VCM_OPTION_e   VcmOption;
    UINT8                   VcmSourceChannelA;      /* ADC vcm signal a source channel */
    UINT8                   VcmSourceChannelB;      /* ADC vcm signal b source channel */
    AMBA_ADC_VCM_REFER_e    VcmReferDataFrom;       /* ADC vcm refer data from select */
    UINT16                  VcmReferDataA;          /* ADC vcm signal a refer data or ADC channel */
    UINT16                  VcmReferDataB;          /* ADC vcm signal b refer data or ADC channel */
    UINT16                  VcmCounterValue;        /* ADC vcm event counter value */
    UINT16                  VcmCounterThreshold;    /* ADC vcm event counter threshold */
    UINT16                  VcmPrimeHighThreshold;  /* ADC vcm prime high threshold */
    UINT16                  VcmPrimeLowThreshold;   /* ADC vcm prime low threshold */
} AMBA_RTSL_ADC_VCM_s;

/*-----------------------------------------------------------------------------------------------*\
 * ADC Channel Structures
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ADC_CHANNEL_s_ {
    UINT16                  DataUpperBound;         /* Maximum allowed data value of the ADC channel */
    UINT16                  DataLowerBound;         /* Minimum allowed data value of the ADC channel */
    AMBA_ADC_ISR_HANDLER_f  DataHandler;            /* Handler for ADC data value interrupts */
} AMBA_ADC_CHANNEL_s;

/*-----------------------------------------------------------------------------------------------*\
 * ADC FIFO Structures
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ADC_FIFO_s_ {
    AMBA_ADC_CHANNEL_e      AdcChanNo;              /* ADC channel number */
    UINT16                  FifoSize;               /* Allocated FIFO size for ADC channel */
    AMBA_ADC_ISR_HANDLER_f  FifoHandler;            /* Handler for FIFO interrupts */
} AMBA_ADC_FIFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * RTSL ADC Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_ADC_CTRL_s_ {
    UINT32                  FifoAvailSize;                          /* ADC FIFO Unused Size */
    AMBA_ADC_FIFO_s         FifoConfig[AMBA_ADC_NUM_FIFO];          /* ADC FIFO Allocation */
    AMBA_ADC_CHANNEL_s      ChannelConfig[AMBA_NUM_ADC_CHANNEL];    /* ADC Channel Configuration */
} AMBA_RTSL_ADC_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_ADC.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_RTSL_ADC_CTRL_s AmbaRTSL_AdcCtrl;

void AmbaRTSL_AdcInit(void);
void AmbaRTSL_AdcStart(void);
void AmbaRTSL_AdcStop(void);
int  AmbaRTSL_AdcDataCollection(void);
int  AmbaRTSL_AdcConfig(AMBA_RTSL_ADC_PARAM_s *pAdcConfig);
void AmbaRTSL_AdcVcmConfig(AMBA_RTSL_ADC_VCM_s *pAdcVcmConfig);

#endif /* _AMBA_RTSL_ADC_H_ */
