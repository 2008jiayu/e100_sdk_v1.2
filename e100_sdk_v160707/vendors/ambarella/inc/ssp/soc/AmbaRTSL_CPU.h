/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_CPU.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for CPU RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_CPU_H_
#define _AMBA_RTSL_CPU_H_

/*-----------------------------------------------------------------------------------------------*\
 * CPU ID Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_CPU_ID_s_ {
    UINT16  PartNo;         /* Primary part number */
    UINT8   MajorRevNo;     /* Major revision number */
    UINT8   MinorRevNo;     /* Minor revision number */
    UINT8   CpuID;          /* CPU number in the MPCore configuration */

} AMBA_CPU_ID_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_CPU.c
\*-----------------------------------------------------------------------------------------------*/
int  AmbaRTSL_CpuGetCpuID(AMBA_CPU_ID_REG_s *pCpuID);

#endif /* _AMBA_RTSL_CPU_H_ */
