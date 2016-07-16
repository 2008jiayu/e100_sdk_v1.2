/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIrSony_SONY.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Sony's infrared remote protocol
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "AmbaDataType.h"
#include "AmbaIrDecode.h"

/*---------------------------------------------------------------------------*\
 * Space-Coded Signals (REC-80) vary the length of the spaces between pulses
 * to code the information. In this case if the space width is short
 * (approximately 550us) it corresponds to a logical zero or a low. If the
 * space width is long (approximately 1650us) it corresponds to a logical one
 * or a high.
 *
 *      ---+  +--+  +--+    +--+    +---
 *         |  |  |  |  |    |  |    |
 *         +--+  +--+  +----+  +----+
 *          0     0      1       1
\*---------------------------------------------------------------------------*/

/* Sony */
#define IR_SONY_DEFAULT_FREQUENCY       36000   /* 36KHz */
#define IR_SONY_DEFAULT_SMALLER_TIME    600     /* T, 600 microseconds. */

/*---------------------------------------------------------------------------*\
 * @bit code 0 [1200us]
 *      ---+    +----+
 *         |    |    |
 *         +----+    +---
 *           -T   +T
 *
 * @bit code 1 [1800us]
 *      ---+    +--------+
 *         |    |        |
 *         +----+        +---
 *           -T    +2T
 *
 * @start code [1800us]
 *      ---+                              +---
 *         |                              |
 *         +------------------------------+
 *                   -3T(1800us)
\*---------------------------------------------------------------------------*/

/**
 * If you hold the remote button pressed, the whole transmited frame
 * repeats every 25ms.
 */

#define IR_SONY_HEADER_LOW_UPBOUND      33      /* default 1800us */
#define IR_SONY_HEADER_LOW_LOWBOUND     28

#define IR_SONY_DATA_LOW_UPBOUND        9       /* default 560us  */
#define IR_SONY_DATA_LOW_LOWBOUND       4
#define IR_SONY_DATA_0_HIGH_UPBOUND     9       /* default 560us  */
#define IR_SONY_DATA_0_HIGH_LOWBOUND    4
#define IR_SONY_DATA_1_HIGH_UPBOUND     18      /* default 1680us */
#define IR_SONY_DATA_1_HIGH_LOWBOUND    12

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrSony_SpaceHeaderCode
 *
 *  @Description:: Check the waveform data is header code or not
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrSony_SpaceHeaderCode(UINT16 *pIrRawBuf)
{
    if ((*pIrRawBuf < IR_SONY_HEADER_LOW_UPBOUND)   &&
        (*pIrRawBuf > IR_SONY_HEADER_LOW_LOWBOUND))
        return OK;  /* header code */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrSony_SpaceCode0
 *
 *  @Description:: Check the waveform data is logic 0 or not
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrSony_SpaceCode0(UINT16 *pIrRawBuf)
{
    /* 500us of Silence + 700us of IR for bits ZERO, */
    if ((pIrRawBuf[0] < IR_SONY_DATA_LOW_UPBOUND)       &&
        (pIrRawBuf[0] > IR_SONY_DATA_LOW_LOWBOUND)      &&
        (pIrRawBuf[1] < IR_SONY_DATA_0_HIGH_UPBOUND)    &&
        (pIrRawBuf[1] > IR_SONY_DATA_0_HIGH_LOWBOUND))
        return OK;    /* code 0 */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrSony_SpaceCode1
 *
 *  @Description:: Check the waveform data is logic 1 or not
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrSony_SpaceCode1(UINT16 *pIrRawBuf)
{
    /* 500us of Silence + 1300us of IR for bits ONE. */
    if ((pIrRawBuf[0] < IR_SONY_DATA_LOW_UPBOUND)       &&
        (pIrRawBuf[0] > IR_SONY_DATA_LOW_LOWBOUND)      &&
        (pIrRawBuf[1] < IR_SONY_DATA_1_HIGH_UPBOUND)    &&
        (pIrRawBuf[1] > IR_SONY_DATA_1_HIGH_LOWBOUND))
        return OK;    /* code 1 */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPanasonic_PulseDataTranslate
 *
 *  @Description:: Translate 1 byte waveform data to useful message
 *
 *  @Input      ::
 *      DataSize:   number of bits
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     ::
 *      pData:      translated data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrSony_SpaceDataTranslate(int DataSize, UINT16 **pIrRawBuf, UINT16 *pData)
{
    int i;

    *pData = 0;
    for (i = 0; i < DataSize; i++) {
        if (IrSony_SpaceCode0(*pIrRawBuf) == OK) {

        } else if (IrSony_SpaceCode1(*pIrRawBuf) == OK) {
            (*pData) |= (1 << i);
        } else {
            return NG;
        }
        (*pIrRawBuf) += 2;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIrDecode_Sony
 *
 *  @Description:: handle Sony IR Pulse/Space protocol
 *
 *  @Input      ::
 *      pDecCtrl:   pointer to the pulse/space information
 *
 *  @Output     ::
 *      pScanCode:  Sony scancode
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIrDecode_Sony(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode)
{
    UINT16 Addr, Data;
    UINT16 *pIrRawBuf;
    int Rval = OK;
    int i;

    *pScanCode = 0xff;

    if (pDecCtrl->IrEventEnd <= 25)
        return NG;

    /* Find header code. */
    pIrRawBuf = &pDecCtrl->pIrEventBuf[0];
    for (i = 0; i < pDecCtrl->IrEventEnd; i ++, pIrRawBuf ++) {
        if (IrSony_SpaceHeaderCode(pIrRawBuf) == OK) {
            pIrRawBuf ++;
            break;
        }
    }

    if (pIrRawBuf >= &(pDecCtrl->pIrEventBuf[pDecCtrl->IrEventEnd]))
        return NG;

    /* Then follows 22 bits of data, broken down in 4 bytes of 8 bits. */
    Rval |= IrSony_PulseDataTranslate(7, &pIrRawBuf, &Addr);    /* command - 7 bits*/
    Rval |= IrSony_PulseDataTranslate(5, &pIrRawBuf, &Data);    /* device address - 5 bits */

    *pScanCode = (UINT32)(Addr << 16) | Data;

    return Rval;
}
