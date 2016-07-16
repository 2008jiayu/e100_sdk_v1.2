/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIrDecode.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Sensor Driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IR_DECODE_H_
#define _AMBA_IR_DECODE_H_

typedef struct _AMBA_IR_DECODE_CTRL_s_ {
    UINT16 *pIrEventBuf;
    UINT32 IrEventBufSize;
    UINT32 IrEventEnd;
} AMBA_IR_DECODE_CTRL_s;

int AmbaIrDecode_Nec(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode);
int AmbaIrDecode_Panasonic(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode);
int AmbaIrDecode_Sony(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode);
int AmbaIrDecode_Philips(AMBA_IR_DECODE_CTRL_s *pDecCtrl, UINT32 *pScanCode);

#endif  /* _AMBA_IR_DECODE_H_ */
