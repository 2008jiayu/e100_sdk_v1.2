/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIR_Panasonic.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Panasonic's old infrared remote protocol
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

#define IR_PANASONIC_DEFAULT_FREQUENCY      38000   /* 38KHz */

/* T = 420 us to approx 424 us in the USA and Canada */
/* T = 454 us to approx 460 us in Europe and others  */
#define IR_PANASONIC_DEFAULT_SMALLER_TIME   454     /* T, 450 microseconds. */

/*---------------------------------------------------------------------------*\
 * @logical 0 bit
 *         +----+    +---
 *         |    |    |
 *      ---+    +----+
 *           2T   2T
 *
 * @logical 1 bit
 *         +----+            +---
 *         |    |            |
 *      ---+    +------------+
 *           2T       6T
 *
 * @start
 *         +--------------------------------+                +---
 *         |                                |                |
 *      ---+                                +----------------+
 *                         16T                      8T
\*---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPanasonic_PulseHeaderCode
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
static int IrPanasonic_PulseHeaderCode(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] > 42) && (pIrRawBuf[0] < 50) &&
        (pIrRawBuf[1] > 18) && (pIrRawBuf[1] < 26))
        return OK;  /* header code */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPanasonic_PulseCode0
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
static int IrPanasonic_PulseCode0(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] > 1) && (pIrRawBuf[0] < 9) &&
        (pIrRawBuf[1] > 1) && (pIrRawBuf[1] < 9))
        return OK;    /* code 0 */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPanasonic_PulseCode1
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
static int IrPanasonic_PulseCode1(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] > 1)  && (pIrRawBuf[0] < 9) &&
        (pIrRawBuf[1] > 12) && (pIrRawBuf[1] < 20))
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
static int IrPanasonic_PulseDataTranslate(int DataSize, UINT16 **pIrRawBuf, UINT32 *pData)
{
    int i;

    *pData = 0;
    for (i = DataSize - 1; i >= 0; i--) {
        if (IrPanasonic_PulseCode0(*pIrRawBuf) == OK) {

        } else if (IrPanasonic_PulseCode1(*pIrRawBuf) == OK) {
            (*pData) |= (1 << i);
        } else {
            return NG;
        }
        (*pIrRawBuf) += 2;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIrDecode_Panasonic
 *
 *  @Description:: handle Panasonic IR Pulse/Space protocol
 *
 *  @Input      ::
 *      pDecCtrl:   pointer to the pulse/space information
 *
 *  @Output     ::
 *      pScanCode:  Panasonic scancode
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIrDecode_Panasonic(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode)
{
    UINT32 Addr;
    UINT32 Data;
    UINT16 *pIrRawBuf;
    int i;

    *pScanCode = 0xff;

    if (pDecCtrl->IrEventEnd < (49 * 2))
        return NG;

    /* Find header code. */
    pIrRawBuf = pDecCtrl->pIrEventBuf;
    for (i = 0; i < pDecCtrl->IrEventEnd; i ++, pIrRawBuf ++) {
        if (IrPanasonic_PulseHeaderCode(pIrRawBuf) == OK) {
            pIrRawBuf += 2;
            break;
        }
    }

    if ((pDecCtrl->IrEventEnd - i - 2) < (48 * 2)) {
        return NG;
    }

    if (pIrRawBuf >= &(pDecCtrl->pIrEventBuf[pDecCtrl->IrEventEnd]))
        return NG;

    /* Then follows 22 bits of data, broken down in 4 bytes of 8 bits. */
    if (IrPanasonic_PulseDataTranslate(16, &pIrRawBuf, &Addr) == NG)
        return NG;

    if (IrPanasonic_PulseDataTranslate(32, &pIrRawBuf, &Data) == NG)
        return NG;

    *pScanCode = Data;

    return OK;
}

