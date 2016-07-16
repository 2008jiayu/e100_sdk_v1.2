/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_H_
#define _AMBA_B5_H_

typedef enum _AMBA_B5_CHIP_INTERFACE_e_ {
    AMBA_B5_CHIP_INTERFACE_SPI = 0,
    AMBA_B5_CHIP_INTERFACE_I2C
} AMBA_B5_CHIP_INTERFACE_e;

typedef enum _AMBA_B5_COMPRESS_RATIO_e_ {
    AMBA_B5_COMPRESS_NONE = 0,
    AMBA_B5_COMPRESS_4P75,      /* 4.75 bits per pixel */
    AMBA_B5_COMPRESS_5P5,       /* 5.5 bits per pixel */
    AMBA_B5_COMPRESS_5P75,      /* 5.75 bits per pixel */
    AMBA_B5_COMPRESS_6P5,       /* 6.5 bits per pixel */
    AMBA_B5_COMPRESS_6P75,      /* 6.75 bits per pixel */
    AMBA_B5_COMPRESS_7P5,       /* 7.5 bits per pixel */
    AMBA_B5_COMPRESS_7P75,      /* 7.75 bits per pixel */
    AMBA_B5_COMPRESS_8P5,       /* 8.5 bits per pixel */
    AMBA_B5_COMPRESS_8P75,      /* 8.75 bits per pixel */
    AMBA_B5_COMPRESS_9P5,       /* 9.5 bits per pixel */
    AMBA_B5_COMPRESS_9P75,      /* 9.75 bits per pixel */
    AMBA_B5_COMPRESS_10P5,      /* 10.5 bits per pixel */
} AMBA_B5_COMPRESS_RATIO_e;

/*---------------------------------------------------------------------------*\
 * Channel Configuration
\*---------------------------------------------------------------------------*/
typedef union _AMBA_B5_CHIP_ID_u_ {
    UINT8   Data;                       /* this is an 8-bit data */

    struct {
        UINT8   Chan0B5F:       1;      /* B5F connected to B5N Channel 0 */
        UINT8   Chan1B5F:       1;      /* B5F connected to B5N Channel 1 */
        UINT8   Chan2B5F:       1;      /* B5F connected to B5N Channel 2 */
        UINT8   Chan3B5F:       1;      /* B5F connected to B5N Channel 3 */
        UINT8   Reserved:       4;
    } Bits;
} AMBA_B5_CHIP_ID_u;

typedef enum _AMBA_B5_CHANNEL_e_ {
    AMBA_B5_CHANNEL0 = 0,               /* B5 Channel 0 */
    AMBA_B5_CHANNEL1,                   /* B5 Channel 1 */
    AMBA_B5_CHANNEL2,                   /* B5 Channel 2 */
    AMBA_B5_CHANNEL3,                   /* B5 Channel 3 */

    AMBA_NUM_B5_CHANNEL                 /* Total number of B5 channels */
} AMBA_B5_CHANNEL_e;

typedef enum _AMBA_B5_CHANNEL_CONFIG_e_ {
    AMBA_B5_CHANNEL_DISABLED = 0,       /* Not used */
    AMBA_B5_CHANNEL_INTERNAL,           /* Internal generated pattern */
    AMBA_B5_CHANNEL_NEAR_END,           /* Near end sensor input */
    AMBA_B5_CHANNEL_FAR_END             /* Far end sensor input */
} AMBA_B5_CHANNEL_CONFIG_e;

typedef struct _AMBA_B5_CHANNEL_s_ {
    AMBA_B5_CHANNEL_CONFIG_e    Active[AMBA_NUM_B5_CHANNEL];
    AMBA_B5_CHANNEL_CONFIG_e    Inactive[AMBA_NUM_B5_CHANNEL];
    UINT32  SensorID;
} AMBA_B5_CHANNEL_s;

#define AMBA_B5N_VIN_CHANNEL0   AMBA_B5_CHANNEL0
#define AMBA_B5N_VIN_CHANNEL1   AMBA_B5_CHANNEL1
#define AMBA_B5N_VIN_CHANNEL2   AMBA_B5_CHANNEL2
#define AMBA_B5N_VIN_CHANNEL3   AMBA_B5_CHANNEL3
#define AMBA_B5F_VIN_CHANNEL    AMBA_B5_CHANNEL2

#define AmbaB5_SetB5nChannelEnable(ChipID,Chan)     ((ChipID) | (0x10 << (Chan)))
#define AmbaB5_SetB5fChannelEnable(ChipID,Chan)     ((ChipID) | (0x01 << (Chan)))
#define AmbaB5_SetB5nChannelDisable(ChipID,Chan)    ((ChipID) & ~(0x10 << (Chan)))
#define AmbaB5_SetB5fChannelDisable(ChipID,Chan)    ((ChipID) & ~(0x01 << (Chan)))

#define AmbaB5_GetB5nChannelStatus(ChipID)          (((ChipID) & 0xf0) >> 4)
#define AmbaB5_GetB5fChannelStatus(ChipID)          ((ChipID) & 0x0f)

#define B5_PWM_POLLING_START 6          /* for replay_times=6 and symbol_clk=400KHz */
#define B5_PWM_POLLING_TIMEOUT 100

/*---------------------------------------------------------------------------*\
 * PIN Configuration
\*---------------------------------------------------------------------------*/
typedef enum _AMBA_B5_PIN_B5N_CTRL_e_ {
    AMBA_B5_PIN_B5N_CTRL_SPI = 0,
    AMBA_B5_PIN_B5N_CTRL_I2C
} AMBA_B5_PIN_B5N_CTRL_e;

typedef enum _AMBA_B5_PIN_B5F_CTRL_e_ {
    AMBA_B5_PIN_B5F_CTRL_NONE = 0,          /* No B5F */
    AMBA_B5_PIN_B5F_CTRL_PWM_DIFFERENTIAL,  /* High Speed */
    AMBA_B5_PIN_B5F_CTRL_PWM_SINGLE_ENDED   /* Low Speed */
} AMBA_B5_PIN_B5F_CTRL_e;

typedef enum _AMBA_B5_SLVS_EC_TYPE_e_ {
    AMBA_B5_SLVS_EC_1LANE = 0,  /* Use 1-lane SLVS-EC */
    AMBA_B5_SLVS_EC_2LANE,      /* Use 2-lane SLVS-EC */
    AMBA_B5_SLVS_EC_3LANE       /* Use 3-lane SLVS-EC */
} AMBA_B5_SLVS_EC_TYPE_e;

typedef enum _AMBA_B5_PIN_SENSOR_CTRL_e_ {
    AMBA_B5_PIN_SENSOR_CTRL_SPI = 0,
    AMBA_B5_PIN_SENSOR_CTRL_I2C0,
    AMBA_B5_PIN_SENSOR_CTRL_I2C1,
    AMBA_B5_PIN_SENSOR_CTRL_I2C_BRDIGE
} AMBA_B5_PIN_SENSOR_CTRL_e;

typedef enum _AMBA_B5_PIN_VIDEO_SYNC_CTRL_e_ {
    AMBA_B5_PIN_VIDEO_SYNC_NONE = 0,
    AMBA_B5_PIN_VIDEO_SYNC_HORIZONTAL,
    AMBA_B5_PIN_VIDEO_SYNC_VERTICAL,
    AMBA_B5_PIN_VIDEO_SYNC_HORIZONTAL_VERTICAL
} AMBA_B5_PIN_VIDEO_SYNC_CTRL_e;

typedef struct _AMBA_B5_PIN_CONFIG_s_ {
    AMBA_B5_PIN_B5N_CTRL_e          B5nPinMux;          /* Access B5N registers */
    AMBA_B5_PIN_B5F_CTRL_e          B5fPinMux;          /* Access B5F registers */
    AMBA_B5_PIN_SENSOR_CTRL_e       SensorPinMux;       /* Access sensor registers */
    AMBA_B5_PIN_VIDEO_SYNC_CTRL_e   VideoSyncPinMux;    /* Generated from VIN to sensor */
    AMBA_B5_SLVS_EC_TYPE_e          SlvsEcType;         /* SLVS-EC type between B5N and B5F */
} AMBA_B5_PIN_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5.c
\*---------------------------------------------------------------------------*/
extern const UINT8 AmbaB5_ChanCount[16];

int AmbaB5_Init(AMBA_B5_CHANNEL_s *pSysB5Chan, AMBA_B5_CHANNEL_s *pCurB5Chan, AMBA_B5_PIN_CONFIG_s *pPinConfig);
int AmbaB5_Enable(AMBA_B5_CHANNEL_s *pB5Chan, UINT32 Width, UINT32 Height, UINT32 SensorPixelBit, AMBA_B5_COMPRESS_RATIO_e Ratio);
int AmbaB5_GetNumActiveChannel(AMBA_B5_CHANNEL_s *pB5Chan);
void AmbaB5_PwmReset(AMBA_B5_CHIP_ID_u ChipID);

#endif /* _AMBA_B5_H_ */
