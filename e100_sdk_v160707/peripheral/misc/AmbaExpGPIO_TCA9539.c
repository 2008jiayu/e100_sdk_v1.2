/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaExpGPIO_TCA9539.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of TCA9539 GPIO expander with I2C interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaGPIO.h"
#include "AmbaI2C.h"

#include "AmbaExpGPIO_TCA9539.h"

#define TCA9539_SLAVE_ADDR      0x74

AMBA_EXP_GPIO_CTRL_s AmbaExpGPIO_Ctrl;  /* TCA9539 Management Structure */

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TCA9539_WriteRegister
 *
 *  @Description:: Write TCA9539 Register
 *
 *  @Input      ::
 *      RegIdx: Index of the register
 *      RegVal: Value of the register
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TCA9539_WriteRegister(AMBA_TCA953X_REG_IDX_e RegIdx, UINT8 RegVal)
{
    UINT8 WorkUINT8[2];

    WorkUINT8[0] = RegIdx;
    WorkUINT8[1] = RegVal;
    return AmbaI2C_Write(AMBA_I2C_CHANNEL2, AMBA_I2C_SPEED_FAST,
                         (TCA9539_SLAVE_ADDR << 1), 2, WorkUINT8, 500);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TCA9539_ReadRegister
 *
 *  @Description:: Read TCA9539 Register
 *
 *  @Input      ::
 *      RegIdx: Index of the register
 *      Size:   number of registers to read
 *
 *  @Output     ::
 *      pRegVal: pointer to the Register Value
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TCA9539_ReadRegister(AMBA_TCA953X_REG_IDX_e RegIdx, int Size, UINT8 *pRegVal)
{
    UINT16 WorkUINT16[3];

    WorkUINT16[0] = TCA9539_SLAVE_ADDR << 1;    /* Slave Address + r/w (0) */
    WorkUINT16[1] = RegIdx;                     /* Command Byte */

    /* Restart, Slave Address + r/w (1) */
    WorkUINT16[2] = AMBA_I2C_RESTART_FLAG | (TCA9539_SLAVE_ADDR << 1) | 0x01;
    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL2, AMBA_I2C_SPEED_FAST,
                                  3, WorkUINT16, Size, pRegVal, 500);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaExpGPIO_Init
 *
 *  @Description:: TCA9539 Initializations
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaExpGPIO_Init(AMBA_EXP_GPIO_CONFIG_s *pExpGpioPortConfig)
{
    if (AmbaKAL_MutexCreate(&AmbaExpGPIO_Ctrl.Mutex) != OK)
        return NG; /* should never happen */

    memcpy(&AmbaExpGPIO_Ctrl.PortConfig, pExpGpioPortConfig, sizeof(AMBA_EXP_GPIO_CONFIG_s));

    TCA9539_WriteRegister(TCA953X_REG_IDX_CONFIG_PORT0, pExpGpioPortConfig->Port[AMBA_EXP_GPIO_PORT0].IoDirection);
    TCA9539_WriteRegister(TCA953X_REG_IDX_CONFIG_PORT1, pExpGpioPortConfig->Port[AMBA_EXP_GPIO_PORT1].IoDirection);

    TCA9539_WriteRegister(TCA953X_REG_IDX_OUTPUT_PORT0, pExpGpioPortConfig->Port[AMBA_EXP_GPIO_PORT0].PinLevel);
    TCA9539_WriteRegister(TCA953X_REG_IDX_OUTPUT_PORT1, pExpGpioPortConfig->Port[AMBA_EXP_GPIO_PORT1].PinLevel);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaExpGPIO_ConfigInput
 *
 *  @Description:: Configure an I/O pin as input
 *
 *  @Input      ::
 *      ExGpioNo:       Pin ID
 *      PinPolarity:    Pin polarity. 0 = Original incoming logic level, 1 = Inverted incoming logic level
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaExpGPIO_ConfigInput(AMBA_EXP_GPIO_PIN_ID_e ExGpioNo, UINT32 PinPolarity)
{
    AMBA_EXP_GPIO_CONFIG_s *pPortConfig = &AmbaExpGPIO_Ctrl.PortConfig;
    UINT8 BitOffset;
    UINT8 PortID;

    if (ExGpioNo >= NUM_EX_GPIO_PIN)
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&AmbaExpGPIO_Ctrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;  /* should never happen */

    switch (ExGpioNo) {
    case EX_GPIO_PIN_00:
    case EX_GPIO_PIN_01:
    case EX_GPIO_PIN_02:
    case EX_GPIO_PIN_03:
    case EX_GPIO_PIN_04:
    case EX_GPIO_PIN_05:
    case EX_GPIO_PIN_06:
    case EX_GPIO_PIN_07:
        BitOffset = ExGpioNo - EX_GPIO_PIN_00;
        PortID = AMBA_EXP_GPIO_PORT0;

        pPortConfig->Port[PortID].IoDirection |= (1 << BitOffset);

        if (!PinPolarity)
            pPortConfig->Port[PortID].PinPolarity &= ~(1 << BitOffset);
        else
            pPortConfig->Port[PortID].PinPolarity |= (1 << BitOffset);

        TCA9539_WriteRegister(TCA953X_REG_IDX_CONFIG_PORT0, pPortConfig->Port[PortID].IoDirection);
        TCA9539_WriteRegister(TCA953X_REG_IDX_POLARITY_INV_PORT0, pPortConfig->Port[PortID].PinPolarity);
        break;

    case EX_GPIO_PIN_10:
    case EX_GPIO_PIN_11:
    case EX_GPIO_PIN_12:
    case EX_GPIO_PIN_13:
    case EX_GPIO_PIN_14:
    case EX_GPIO_PIN_15:
    case EX_GPIO_PIN_16:
    case EX_GPIO_PIN_17:
        BitOffset = ExGpioNo - EX_GPIO_PIN_10;
        PortID = AMBA_EXP_GPIO_PORT1;

        pPortConfig->Port[PortID].IoDirection |= (1 << BitOffset);

        if (!PinPolarity)
            pPortConfig->Port[PortID].PinPolarity &= ~(1 << BitOffset);
        else
            pPortConfig->Port[PortID].PinPolarity |= (1 << BitOffset);

        TCA9539_WriteRegister(TCA953X_REG_IDX_CONFIG_PORT1, pPortConfig->Port[PortID].IoDirection);
        TCA9539_WriteRegister(TCA953X_REG_IDX_POLARITY_INV_PORT1, pPortConfig->Port[PortID].PinPolarity);
        break;
    default:
	break;
    }

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&AmbaExpGPIO_Ctrl.Mutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaExpGPIO_ConfigOutput
 *
 *  @Description:: Configure an I/O pin as output
 *
 *  @Input      ::
 *      ExGpioNo:       Pin ID
 *      PinLevel:       Outgoing logic level. 0 = Logic Zero, 1 = Logic One
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaExpGPIO_ConfigOutput(AMBA_EXP_GPIO_PIN_ID_e ExGpioNo, UINT32 PinLevel)
{
    AMBA_EXP_GPIO_CONFIG_s *pPortConfig = &AmbaExpGPIO_Ctrl.PortConfig;
    UINT8 BitOffset;
    UINT8 PortID;

    if (ExGpioNo >= NUM_EX_GPIO_PIN)
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&AmbaExpGPIO_Ctrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;  /* should never happen */

    switch (ExGpioNo) {
    case EX_GPIO_PIN_00:
    case EX_GPIO_PIN_01:
    case EX_GPIO_PIN_02:
    case EX_GPIO_PIN_03:
    case EX_GPIO_PIN_04:
    case EX_GPIO_PIN_05:
    case EX_GPIO_PIN_06:
    case EX_GPIO_PIN_07:
        BitOffset = ExGpioNo - EX_GPIO_PIN_00;
        PortID = AMBA_EXP_GPIO_PORT0;

        pPortConfig->Port[PortID].IoDirection &= ~(1 << BitOffset);

        if (!PinLevel)
            pPortConfig->Port[PortID].PinLevel &= ~(1 << BitOffset);
        else
            pPortConfig->Port[PortID].PinLevel |= (1 << BitOffset);

        TCA9539_WriteRegister(TCA953X_REG_IDX_CONFIG_PORT0, pPortConfig->Port[PortID].IoDirection);
        TCA9539_WriteRegister(TCA953X_REG_IDX_OUTPUT_PORT0, pPortConfig->Port[PortID].PinLevel);
        break;

    case EX_GPIO_PIN_10:
    case EX_GPIO_PIN_11:
    case EX_GPIO_PIN_12:
    case EX_GPIO_PIN_13:
    case EX_GPIO_PIN_14:
    case EX_GPIO_PIN_15:
    case EX_GPIO_PIN_16:
    case EX_GPIO_PIN_17:
        BitOffset = ExGpioNo - EX_GPIO_PIN_10;
        PortID = AMBA_EXP_GPIO_PORT1;

        pPortConfig->Port[PortID].IoDirection &= ~(1 << BitOffset);

        if (!PinLevel)
            pPortConfig->Port[PortID].PinLevel &= ~(1 << BitOffset);
        else
            pPortConfig->Port[PortID].PinLevel |= (1 << BitOffset);

        TCA9539_WriteRegister(TCA953X_REG_IDX_CONFIG_PORT1, pPortConfig->Port[PortID].IoDirection);
        TCA9539_WriteRegister(TCA953X_REG_IDX_OUTPUT_PORT1, pPortConfig->Port[PortID].PinLevel);
        break;
    default:
	break;
    }

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&AmbaExpGPIO_Ctrl.Mutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaExpGPIO_GetLevel
 *
 *  @Description:: Get logic level of an I/O pin
 *
 *  @Input      ::
 *      ExGpioNo:       Pin ID
 *      pPinLevel:      Incoming/Outgoing logic level. 0 = Logic Zero, 1 = Logic One
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaExpGPIO_GetLevel(AMBA_EXP_GPIO_PIN_ID_e ExGpioNo, UINT32 *pPinLevel)
{
    UINT8 BitOffset;
    UINT8 PinLevel;
    int RetStatus;

    if (ExGpioNo >= NUM_EX_GPIO_PIN || pPinLevel == NULL)
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&AmbaExpGPIO_Ctrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;  /* should never happen */

    switch (ExGpioNo) {
    case EX_GPIO_PIN_00:
    case EX_GPIO_PIN_01:
    case EX_GPIO_PIN_02:
    case EX_GPIO_PIN_03:
    case EX_GPIO_PIN_04:
    case EX_GPIO_PIN_05:
    case EX_GPIO_PIN_06:
    case EX_GPIO_PIN_07:
        BitOffset = ExGpioNo - EX_GPIO_PIN_00;
        RetStatus = TCA9539_ReadRegister(TCA953X_REG_IDX_INPUT_PORT0, 1, &PinLevel);
        break;

    case EX_GPIO_PIN_10:
    case EX_GPIO_PIN_11:
    case EX_GPIO_PIN_12:
    case EX_GPIO_PIN_13:
    case EX_GPIO_PIN_14:
    case EX_GPIO_PIN_15:
    case EX_GPIO_PIN_16:
    case EX_GPIO_PIN_17:
        BitOffset = ExGpioNo - EX_GPIO_PIN_10;
        RetStatus = TCA9539_ReadRegister(TCA953X_REG_IDX_INPUT_PORT1, 1, &PinLevel);
        break;
    default:
	break;
    }

    if (RetStatus == OK) {
        *pPinLevel = (PinLevel >> BitOffset) & 01;
    }

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&AmbaExpGPIO_Ctrl.Mutex);

    return OK;
}
