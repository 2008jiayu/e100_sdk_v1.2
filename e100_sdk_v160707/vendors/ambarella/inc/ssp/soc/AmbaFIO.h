/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFIO.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Flash IO APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FIO_H_
#define _AMBA_FIO_H_

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for EventFlag.
\*-----------------------------------------------------------------------------------------------*/
#define FIO_CMD_DONE_FLAG       0x1
#define FIO_DMA_DONE_FLAG       0x2

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFIO.h (MW for Flash IO)
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaFio_CmdDoneCallBack)(void);
extern void (*AmbaFio_DmaDoneCallBack)(void);

int AmbaFIO_Lock(int Module);
void AmbaFIO_Unlock(int Module);
void AmbaFIO_CmdDoneIsr(void);
void AmbaFIO_DmaDoneIsr(void);
int AmbaFIO_Init(void);

#endif  /* _AMBA_FIO_H_ */
