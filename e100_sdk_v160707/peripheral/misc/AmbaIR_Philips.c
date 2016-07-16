/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIrPhilips_Philips.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Philips's infrared remote protocol
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "AmbaDataType.h"
#include "AmbaIrDecode.h"

/*---------------------------------------------------------------------------*\
 * Shift-Coded Signals (RC-5)vary the order of pulse space to code the
 * information. In this case if the space width is short (approximately 550us)
 * and the pulse width is long (approximately 1100us) the signal corresponds to
 * a logical one or a high. If the space is long and the pulse is short the
 * signal corresponds to a logical zero or a low.
 *
 *           |     |     |
 *        +--|  +--|--+  |  +---
 *        |  |  |  |  |  |  |
 *     ---+  |--+  |  +--|--+
 *        1  |  1  |  0  |  1
\*---------------------------------------------------------------------------*/

/* Philips (RC-5) */
#define IR_PHILIPS_DEFAULT_FREQUENCY        36000   /* 36KHz */
#define IR_PHILIPS_DEFAULT_SMALLER_TIME     1728    /* T, 1728 microseconds. */

/*---------------------------------------------------------------------------*\
 *        |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
 *     ---+-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +---
 *        | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *        | +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+
 *        |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
 *        |  AGC  |TOG|<     address     >|<       command       >|
\*---------------------------------------------------------------------------*/

#define IR_PHILIPS_HEADER_UPBOUND           0xffff  /* default don't care */
#define IR_PHILIPS_HEADER_LOWBOUND          30

#define IR_PHILIPS_SHC_SIG_CYC_UPBOUND      14      /* default 10~ 12 */
#define IR_PHILIPS_SHC_SIG_CYC_LOWBOUND     8

#define IR_PHILIPS_SHC_DOB_CYC_UPBOUND      25      /* default 12~ 23 */
#define IR_PHILIPS_SHC_DOB_CYC_LOWBOUND     20

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPhilips_ShiftHeaderCode
 *
 *  @Description:: Check the waveform data is header code or not
 *          |                 |                 |
 *       ---|--------+        +--------+        |
 *          |        |        |        |        |
 *          |        +--------+        +--------|---
 *          |                 |                 |
 *          |                AGC                |
 *        Header     | Header | Header |
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrPhilips_ShiftHeaderCode(UINT16 *pIrRawBuf)
{
    if ((pIrRawBuf[0] > IR_PHILIPS_HEADER_LOWBOUND)         &&
        (pIrRawBuf[1] < IR_PHILIPS_SHC_SIG_CYC_UPBOUND)     &&
        (pIrRawBuf[1] > IR_PHILIPS_SHC_SIG_CYC_LOWBOUND)    &&
        (pIrRawBuf[2] < IR_PHILIPS_SHC_SIG_CYC_UPBOUND)     &&
        (pIrRawBuf[2] > IR_PHILIPS_SHC_SIG_CYC_LOWBOUND))
        return OK;  /* header code */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPhilips_ShiftInvertCode
 *
 *  @Description:: Check the waveform data need invert or not
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrPhilips_ShiftInvertCode(UINT16 *pIrRawBuf)
{
    if ((*pIrRawBuf < IR_PHILIPS_SHC_DOB_CYC_UPBOUND)   &&
        (*pIrRawBuf > IR_PHILIPS_SHC_DOB_CYC_LOWBOUND))
        return OK;  /* invert code */
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IrPhilips_ShiftRepeatCode
 *
 *  @Description:: Check the waveform data need repeat or not
 *
 *  @Input      ::
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrPhilips_ShiftRepeatCode(UINT16 *pIrRawBuf)
{
    if ((*pIrRawBuf < IR_PHILIPS_SHC_SIG_CYC_UPBOUND)   &&
        (*pIrRawBuf > IR_PHILIPS_SHC_SIG_CYC_LOWBOUND))
        return OK;    /*  repeat code */
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
 *      Toggle:     bit state
 *      pIrRawBuf:  pointer to the pulse/space information
 *
 *  @Output     ::
 *      pData:      translated data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IrPhilips_ShiftDataTranslate(int DataSize, UINT16 *pToggle, UINT16 **pIrRawBuf, UINT16 *pData)
{
    int i;

    *pData = 0;
    for (i = 0; i < DataSize; i++) {
        if (IrPhilips_ShiftInvertCode(*pIrRawBuf) == OK) {
            (*pToggle) ^= 1;
        } else if (IrPhilips_ShiftRepeatCode(*pIrRawBuf) == OK) {

        } else {
            return NG;
        }
        (*pData) = ((*pData) << 1) | (*pToggle);
        (*pIrRawBuf) ++;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIrDecode_Philips
 *
 *  @Description:: handle Philips IR Pulse/Space protocol
 *
 *  @Input      ::
 *      pDecCtrl:   pointer to the pulse/space information
 *
 *  @Output     ::
 *      pScanCode:  Philips scancode
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIrDecode_Philips(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode)
{
    UINT16 Addr, Data, Toggle;
    UINT16 *pIrRawBuf;
    int Rval = OK;
    int i;

    *pScanCode = 0xff;

    if (pDecCtrl->IrEventEnd <= 15)
        return NG;

    /* Find header code. */
    pIrRawBuf = &pDecCtrl->pIrEventBuf[0];
    for (i = 0; i < pDecCtrl->IrEventEnd; i ++, pIrRawBuf ++) {
        if (IrPhilips_ShiftHeaderCode(pIrRawBuf) == OK) {
            pIrRawBuf += 3;
            break;
        }
    }

    if (pIrRawBuf >= &(pDecCtrl->pIrEventBuf[pDecCtrl->IrEventEnd]))
        return NG;

    /* Get toggle code and Initialize start Value */
    if (IrPhilips_ShiftInvertCode(pIrRawBuf)) {
        pIrRawBuf ++;
        Toggle = 1;
    } else if (IrPhilips_ShiftRepeatCode(pIrRawBuf)) {
        pIrRawBuf ++;
        Toggle = 0;
    } else {
        return NG;
    }

    /* Then follows 22 bits of data, broken down in 4 bytes of 8 bits. */
    Rval |= IrPhilips_ShiftDataTranslate(5, &Toggle, &pIrRawBuf, &Addr);
    Rval |= IrPhilips_ShiftDataTranslate(6, &Toggle, &pIrRawBuf, &Data);

    *pScanCode = (Addr << 16) | Data;

    return Rval;
}
