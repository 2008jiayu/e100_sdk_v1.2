/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaExpGPIO_TCA9539.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants TCA9539 GPIO expander APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_EXP_GPIO_TCA9539_H_
#define _AMBA_EXP_GPIO_TCA9539_H_

typedef enum _AMBA_EXP_GPIO_PIN_ID_e_ {
    /* Port 0 */
    EX_GPIO_PIN_00 = 0,
    EX_GPIO_PIN_01,
    EX_GPIO_PIN_02,
    EX_GPIO_PIN_03,
    EX_GPIO_PIN_04,
    EX_GPIO_PIN_05,
    EX_GPIO_PIN_06,
    EX_GPIO_PIN_07,

    /* Port 1 */
    EX_GPIO_PIN_10,
    EX_GPIO_PIN_11,
    EX_GPIO_PIN_12,
    EX_GPIO_PIN_13,
    EX_GPIO_PIN_14,
    EX_GPIO_PIN_15,
    EX_GPIO_PIN_16,
    EX_GPIO_PIN_17,

    NUM_EX_GPIO_PIN
} AMBA_EXP_GPIO_PIN_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 * GPIO Expander Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_EXP_GPIO_PORT_e_ {
    AMBA_EXP_GPIO_PORT0,       /* I/O port 0 */
    AMBA_EXP_GPIO_PORT1,       /* I/O port 1 */

    AMBA_NUM_EXP_GPIO_PORT
} AMBA_EXP_GPIO_PORT_e;

typedef struct _AMBA_EXP_GPIO_CONFIG_s_ {
    struct {
        UINT8   IoDirection;    /* 0 = output, 1 = Input */
        UINT8   PinPolarity;    /* 0 = Original pin polarity, 1 = Inverted pin polarity */
        UINT8   PinLevel;       /* 0 = Logic low level, 1 = Logic high level */
    } Port[AMBA_NUM_EXP_GPIO_PORT];
} AMBA_EXP_GPIO_CONFIG_s;

typedef struct _AMBA_EXP_GPIO_CTRL_s_ {
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
    AMBA_EXP_GPIO_CONFIG_s  PortConfig;
} AMBA_EXP_GPIO_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * TCA9539 Register definitions
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_TCA953X_REG_IDX_e_ {
    TCA953X_REG_IDX_INPUT_PORT0 = 0,
    TCA953X_REG_IDX_INPUT_PORT1,
    TCA953X_REG_IDX_OUTPUT_PORT0,
    TCA953X_REG_IDX_OUTPUT_PORT1,
    TCA953X_REG_IDX_POLARITY_INV_PORT0,
    TCA953X_REG_IDX_POLARITY_INV_PORT1,
    TCA953X_REG_IDX_CONFIG_PORT0,
    TCA953X_REG_IDX_CONFIG_PORT1
} AMBA_TCA953X_REG_IDX_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaExpGPIO_TCA9539.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaExpGPIO_Init(AMBA_EXP_GPIO_CONFIG_s *pExpGpioPortConfig);
int AmbaExpGPIO_ConfigInput(AMBA_EXP_GPIO_PIN_ID_e ExGpioNo, UINT32 PinPolarity);
int AmbaExpGPIO_ConfigOutput(AMBA_EXP_GPIO_PIN_ID_e ExGpioNo, UINT32 PinLevel);
int AmbaExpGPIO_GetLevel(AMBA_EXP_GPIO_PIN_ID_e ExGpioNo, UINT32 *pPinLevel);

#endif /* _AMBA_EXP_GPIO_TCA9539_H_ */
