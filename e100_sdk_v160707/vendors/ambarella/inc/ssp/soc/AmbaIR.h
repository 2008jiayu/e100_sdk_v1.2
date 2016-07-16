/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIR.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for InfraRed Remote Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IR_H_
#define _AMBA_IR_H_

/*-----------------------------------------------------------------------------------------------*\
 * IR Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_IR_CTRL_s_ {
    AMBA_KAL_MUTEX_t                Mutex;
    AMBA_KAL_EVENT_FLAG_t           EventFlag;
} AMBA_IR_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaIR.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_IR_CTRL_s AmbaIR_Ctrl; /* IR Management Structure */

int AmbaIR_Init(void);
int AmbaIR_Enable(void);
int AmbaIR_Disable(void);
int AmbaIR_Read(UINT32 *pDataSize, UINT16 **pDataBuf);

#endif /* _AMBA_IR_H_ */
