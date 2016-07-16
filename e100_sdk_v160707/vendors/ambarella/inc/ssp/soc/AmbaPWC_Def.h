/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPWC_Def.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for PWC Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PWC_DEF_H_
#define _AMBA_PWC_DEF_H_

typedef enum _AMBA_PWC_WAKEUP_REASON_e_ {
    AMBA_PWC_WAKEUP_ALARM = 0,              /* Wake-Up by Alarm */
    AMBA_PWC_WAKEUP_POR,                    /* Wake-Up by POR_L pin */
    AMBA_PWC_WAKEUP_PWC_WKUP,               /* Wake-Up by PWC_WKUP pin */
    AMBA_PWC_WAKEUP_PWC_WKUP1,              /* Wake-Up by PWC_WKUP1 pin */
    AMBA_PWC_WAKEUP_PWC_WKUP2,              /* Wake-Up by PWC_WKUP2 pin */
    AMBA_PWC_WAKEUP_PWC_WKUP3               /* Wake-Up by PWC_WKUP3 pin */
} AMBA_PWC_WAKEUP_REASON_e;

typedef union _AMBA_PWC_STATUS_u_ {
    UINT32  Data;

    struct {
        UINT32  DdrFreq_1:              1;      /* [0]: 1 = Select DDR frequency_1 */
        UINT32  LostPower:              1;      /* [1]: 1 = Power Lost */

        UINT32  Pseq3State:             1;      /* [2]: 1 = PSEQ3 State (for H/W control) */
        UINT32  EnableWakeupByAlarm:    1;      /* [3]: 1 = Enable Wake-Up by Alarm (for H/W control) */

        UINT32  WakeupByAlarmState:     1;      /* [4]: 1 = The system is Waked Up by Alarm */
        UINT32  UserDefined:            3;      /* [7:5]: Defined by user */
        UINT32  Reserved:               24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PWC_STATUS_u;

#endif /* _AMBA_PWC_H_ */
