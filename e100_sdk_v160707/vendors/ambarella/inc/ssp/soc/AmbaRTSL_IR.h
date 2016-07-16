/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_IR.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for IR RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_IR_H_
#define _AMBA_RTSL_IR_H_

#define AMBA_IR_MAX_FIFO_ENTRY      64

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_IR.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_IrIsrCallBack)(void);

void AmbaRTSL_IrInit(void);
int AmbaRTSL_IrEnable(void);
int AmbaRTSL_IrDisable(void);
int AmbaRTSL_IrRead(UINT32 *pDataSize, UINT16 **pDataBuf);

#endif /* _AMBA_RTSL_IR_H_ */
