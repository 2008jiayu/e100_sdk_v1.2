/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_DebugPort.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Debug Port RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_DEBUG_PORT_H_
#define _AMBA_RTSL_DEBUG_PORT_H_

#define AmbaRTSL_DebugPortResetSyncCounter(SyncCounter) AmbaRTSL_DebugPortSetSyncCounter(SyncCounter, 0)

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_DebugPort.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTSL_DebugPortSetDspPC(UINT32 MainDspPC, UINT32 Sub0DspPC, UINT32 Sub1DspPC);
int AmbaRTSL_DebugPortEnableDSP(void);
int AmbaRTSL_DebugPortSuspendDSP(void);
int AmbaRTSL_DebugPortResetDSP(void);
int AmbaRTSL_DebugPortSetSyncCounter(int SyncCounter, UINT16 Value);

#endif /* _AMBA_RTSL_I2C_H_ */
