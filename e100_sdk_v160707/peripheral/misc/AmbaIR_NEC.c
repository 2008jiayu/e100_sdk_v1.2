/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIR_NEC.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NEC's infrared remote protocol
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "AmbaDataType.h"
#include "AmbaIrDecode.h"

/*---------------------------------------------------------------------------*\
 * Pulse-Width-Coded Signals vary the length of pulses to code the information.
 * In this case if the pulse width is short (approximately 550us) it
 * corresponds to a logical zero or a low. If the pulse width is long
 * (approximately 2200us) it corresponds to a logical one or a high.
 *
 *         +--+  +--+  +----+  +----+
 *         |  |  |  |  |    |  |    |
 *      ---+  +--+  +--+    +--+    +---
 *          0     0      1       1
\*---------------------------------------------------------------------------*/

/* NEC - APEX - HITACHI - PIONEER */
#define IR_NEC_DEFAULT_FREQUENCY       36000   /* 36KHz */
#define IR_NEC_DEFAULT_SMALLER_TIME    560     /* T, 560 microseconds. */

/*---------------------------------------------------------------------------*\
 * @logical bit 0 [1120us]
 *      ---+    +----+
 *         |    |    |
 *         +----+    +---
 *           -T   +T
 *
 * @logical bit 1 [2240us]
 *      ---+    +------------+
 *         |    |            |
 *         +----+            +---
 *           -T      +3T
 *
 * @start [13.3ms]
 *      ---+                                +---------------+
 *         |                                |               |
 *         +--------------------------------+               +---
 *                     -16T(9ms)              +7.5T(4.2ms)
 *
 * @REPEAT Frame [11.3ms]
 *      ---+                                +--------+  +---
 *         |                                |        |  |
 *         +--------------------------------+        +--+
 *                     -16T(9ms)            +4T(2.2ms)
\*---------------------------------------------------------------------------*/

#define IR_NEC_HEADER_LOW_UPBOUND      123  /* default 9ms   */
#define IR_NEC_HEADER_LOW_LOWBOUND     113
#define IR_NEC_HEADER_HIGH_UPBOUND     63   /* default 4.2ms */
#define IR_NEC_HEADER_HIGH_LOWBOUND    52

#define IR_NEC_REPEAT_LOW_UPBOUND      123  /* default 9ms   */
#define IR_NEC_REPEAT_LOW_LOWBOUND     113
#define IR_NEC_REPEAT_HIGH_UPBOUND     33   /* default 2.2ms */
#define IR_NEC_REPEAT_HIGH_LOWBOUND    23

#define IR_NEC_DATA_LOW_UPBOUND        12   /* default 560us  */
#define IR_NEC_DATA_LOW_LOWBOUND       1
#define IR_NEC_DATA_0_HIGH_UPBOUND     12   /* default 560us  */
#define IR_NEC_DATA_0_HIGH_LOWBOUND    1
#define IR_NEC_DATA_1_HIGH_UPBOUND     26   /* default 1680us */
#define IR_NEC_DATA_1_HIGH_LOWBOUND    15

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrNec_PulseHeaderCode
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
static int IrNec_PulseHeaderCode(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] < IR_NEC_HEADER_LOW_UPBOUND)  &&
        (pIrRawBuf[0] > IR_NEC_HEADER_LOW_LOWBOUND) &&
        (pIrRawBuf[1] < IR_NEC_HEADER_HIGH_UPBOUND) &&
        (pIrRawBuf[1] > IR_NEC_HEADER_HIGH_LOWBOUND))
        return OK;  /* header code */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrNec_PulseRepeatCode
 *
 *  @Description:: Check the waveform data is repeat code or not
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrNec_PulseRepeatCode(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] < IR_NEC_REPEAT_LOW_UPBOUND)  &&
        (pIrRawBuf[0] > IR_NEC_REPEAT_LOW_LOWBOUND) &&
        (pIrRawBuf[1] < IR_NEC_REPEAT_HIGH_UPBOUND) &&
        (pIrRawBuf[1] > IR_NEC_REPEAT_HIGH_LOWBOUND))
        return OK;  /* repeat code */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrNec_PulseCode0
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
static int IrNec_PulseCode0(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] < IR_NEC_DATA_LOW_UPBOUND)    &&
        (pIrRawBuf[0] > IR_NEC_DATA_LOW_LOWBOUND)   &&
        (pIrRawBuf[1] < IR_NEC_DATA_0_HIGH_UPBOUND) &&
        (pIrRawBuf[1] > IR_NEC_DATA_0_HIGH_LOWBOUND))
        return OK;  /* code 0 */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrNec_PulseCode1
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
static int IrNec_PulseCode1(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] < IR_NEC_DATA_LOW_UPBOUND)    &&
        (pIrRawBuf[0] > IR_NEC_DATA_LOW_LOWBOUND)   &&
        (pIrRawBuf[1] < IR_NEC_DATA_1_HIGH_UPBOUND) &&
        (pIrRawBuf[1] > IR_NEC_DATA_1_HIGH_LOWBOUND))
        return OK;  /* code 1 */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrNec_PulseDataTranslate
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
static int IrNec_PulseDataTranslate(int DataSize, UINT16 **pIrRawBuf, UINT8 * Data)
{
    int i;

    *Data = 0;
    for (i = DataSize - 1; i >= 0; i--) {
        if (IrNec_PulseCode0(*pIrRawBuf) == OK) {

        } else if (IrNec_PulseCode1(*pIrRawBuf) == OK) {
            (*Data) |= (1 << i);
        } else {
            return NG;
        }
        (*pIrRawBuf) += 2;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIrDecode_Nec
 *
 *  @Description:: handle NEC IR Pulse/Space protocol
 *
 *  @Input      ::
 *      pDecCtrl:   pointer to the pulse/space information
 *
 *  @Output     ::
 *      pScanCode:  NEC scancode
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIrDecode_Nec(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode)
{
    UINT8 Addr, InvAddr, Data, InvData;
    UINT16 *pIrRawBuf;
    int Rval = OK;
    int i;

    *pScanCode = 0xff;

    if (pDecCtrl->IrEventEnd <= 66)
        return NG;

    /* Find header code. */
    pIrRawBuf = &pDecCtrl->pIrEventBuf[0];
    for (i = 0; i < pDecCtrl->IrEventEnd; i ++, pIrRawBuf ++) {
        if (IrNec_PulseHeaderCode(pIrRawBuf) == OK) {
            pIrRawBuf += 2;
            break;
        } else if (IrNec_PulseRepeatCode(pIrRawBuf) == OK) {
            pIrRawBuf += 2;
            *pScanCode = 0x00;
            return OK;
        }
    }

    /* decode */
    if (pIrRawBuf >= &(pDecCtrl->pIrEventBuf[pDecCtrl->IrEventEnd]))
        return NG;

    /* Then follows 32 bits of data, broken down in 4 bytes of 8 bits. */
    Rval |= IrNec_PulseDataTranslate(8, &pIrRawBuf, &Addr);
    Rval |= IrNec_PulseDataTranslate(8, &pIrRawBuf, &InvAddr);
    Rval |= IrNec_PulseDataTranslate(8, &pIrRawBuf, &Data);
    Rval |= IrNec_PulseDataTranslate(8, &pIrRawBuf, &InvData);

    *pScanCode = (UINT32)(Data << 8)| InvData;

    return Rval;
}
