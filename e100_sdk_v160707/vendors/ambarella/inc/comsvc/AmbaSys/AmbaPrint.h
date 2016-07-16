/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPrint.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants used for Ambarella System
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PRINT_H_
#define _AMBA_PRINT_H_

typedef int (*AMBA_PRINT_PUT_CHAR_f)(int TxDataSize, char *pTxDataBuf, UINT32 TimeOut);

typedef struct _AMBA_PRINT_CONFIG_s_ {
    int     Priority;                           /* Priority */
    int     PrintBufSize;                       /* Byte size of print buffer */
    UINT8   *pPrintBuf;                         /* pointer to the print buffer */
    int     DspDebugLogAreaSize;                /* Byte size of DSP debug log buffer */
    UINT8   *pDspDebugLogDataArea;              /* pointer to the base address of DSP debug log buffer */

    AMBA_PRINT_PUT_CHAR_f   PutCharCallBack;    /* pointer to the function of character output */
	UINT32  SmpCoreinclusionMap;
} AMBA_PRINT_CONFIG_s;

int AmbaPrint_Init(AMBA_PRINT_CONFIG_s *pPrintConfig);
int AmbaPrint_SetConfig(int RtosMsgEnable, int DspMsgEnable);
int AmbaPrint_GetConfig(int *pRtosMsgEnable, int *pDspMsgEnable);
int AmbaPrint(const char *pFmt, ...);
void AmbaPrint_Flush(void);

#endif  /* _AMBA_PRINT_H_ */
