/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaI2S_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for I2S APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_I2S_DEF_H_
#define _AMBA_I2S_DEF_H_

typedef enum _AMBA_I2S_CHANNEL_e_ {
    AMBA_I2S_CHANNEL0,                  /* I2S Channel-0 for I2S1 in A12*/
    AMBA_NUM_I2S_CHANNEL                /* Total Number of I2S Channels */
} AMBA_I2S_CHANNEL_e;

typedef enum _AMBA_I2S_MODE_e_ {
    AMBA_I2S_MODE_LEFT_JUSTIFIED = 0,   /* Left-Justified data format */
    AMBA_I2S_MODE_RIGHT_JUSTIFIED,      /* Right-Justified data format */
    AMBA_I2S_MODE_MSB_EXTENDED,         /* MSB extended data format */
    AMBA_I2S_MODE_I2S = 4,              /* Philips I2S data format */
    AMBA_I2S_MODE_DSP = 6
} AMBA_I2S_MODE_e;

typedef enum _AMBA_I2S_MASTER_SLAVE_e_ {
    AMBA_I2S_SLAVE = 0,                 /* Slave clock */
    AMBA_I2S_MASTER                     /* Master clock */
} AMBA_I2S_MASTER_SLAVE_e;

typedef enum _AMBA_I2S_BIT_ORDER_e_ {
    AMBA_I2S_MSB_FIRST = 0,             /* Data word is MSB first */
    AMBA_I2S_LSB_FIRST                  /* Data word is LSB first */
} AMBA_I2S_BIT_ORDER_e;

typedef enum _AMBA_I2S_CLK_EDGE_e_ {
    AMBA_I2S_CLK_EDGE_RISING = 0,       /* Rising edge of serial clock */
    AMBA_I2S_CLK_EDGE_FALLING           /* Falling edge of serial clock */
} AMBA_I2S_CLK_EDGE_e;

typedef enum _AMBA_I2S_WORD_SELECT_SIZE_e_ {
    AMBA_I2S_WS_16SCK = 0,              /* 16 bit clock cycles */
    AMBA_I2S_WS_32SCK,                  /* 32 bit clock cycles */

    AMBA_I2S_NUM_WS_SIZE
} AMBA_I2S_WORD_SELECT_SIZE_e;

typedef enum _AMBA_I2S_CLK_FREQ_e_ {
    AMBA_I2S_CLK_FREQ_128FS,
    AMBA_I2S_CLK_FREQ_256FS,
    AMBA_I2S_CLK_FREQ_384FS,
    AMBA_I2S_CLK_FREQ_512FS,
    AMBA_I2S_CLK_FREQ_768FS,
    AMBA_I2S_CLK_FREQ_1024FS,
    AMBA_I2S_CLK_FREQ_1152FS,
    AMBA_I2S_CLK_FREQ_1536FS,
    AMBA_I2S_CLK_FREQ_2304FS,

    AMBA_I2S_NUM_CLK_FREQ
} AMBA_I2S_CLK_FREQ_e;

typedef struct _AMBA_I2S_TX_SETTING_s_ {
    UINT32  Mono;                       /* Transmitter mono */
    UINT32  Mute;                       /* Transmitter mute: 0 - Normal, 1 - Mute */
    AMBA_I2S_BIT_ORDER_e    Order;      /* Transmitter bit order: 0 - MSB is first, 1 - LSB is first */
    UINT32  Loopback;                   /* Transmitter loop back test */
    UINT32  Shift;                      /* Transmitter shift: 0 - Disable, 1 - Enable */
} AMBA_I2S_TX_SETTING_s;

typedef struct _AMBA_I2S_RX_SETTING_s_ {
    AMBA_I2S_BIT_ORDER_e    Order;      /* Receiver bit order: 0 - MSB is first, 1 - LSB is first */
    UINT32  Loopback;                   /* Receiver loop back test */
    UINT32  Shift;                      /* Receiver shift: 0 - Disable, 1 - Enable */
    UINT32  Echo;                       /* Receiver echo: 0 - Disable, 1 - Enable */
} AMBA_I2S_RX_SETTING_s;

typedef struct _AMBA_I2S_CTRL_INFO_s_ {
    AMBA_I2S_MODE_e             Mode;               /* I2S modes */
    AMBA_I2S_MASTER_SLAVE_e     ClkDir;             /* I2S master/slave clock */
    AMBA_I2S_WORD_SELECT_SIZE_e WsPulseWidth;       /* WordSelect pulse width */
    UINT8                       WordLength;         /* Word length (bits) */
    AMBA_I2S_CLK_FREQ_e         OverSampleIdx;      /* I2S oversample rate index */
    UINT32                      DspModeSlots;       /* Slots number for DSP mode */
    UINT32                      NumAudioChannels;   /* Number of audio channels */
    AMBA_I2S_TX_SETTING_s       TxSet;              /* Tx special setting */
    AMBA_I2S_RX_SETTING_s       RxSet;              /* Rx special setting */
} AMBA_I2S_CTRL_INFO_s;

#endif /* _AMBA_I2S_DEF_H_ */
