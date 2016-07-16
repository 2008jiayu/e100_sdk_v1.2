/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTC.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Power and RTC (Real-Time Clock) Controller Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTC_H_
#define _AMBA_RTC_H_

typedef enum _AMBA_DAY_OF_WEEK_e_ {
    WEEK_SUNDAY = 0,
    WEEK_MONDAY,
    WEEK_TUESDAY,
    WEEK_WEDNESDAY,
    WEEK_THURSDAY,
    WEEK_FRIDAY,
    WEEK_SATURDAY,

    AMBA_NUM_DAY_OF_WEEK
} AMBA_DAY_OF_WEEK_e;

/*-----------------------------------------------------------------------------------------------*\
 * Time Standard
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_RTC_TIME_STANDARD_e_ {
    AMBA_TIME_STD_TAI = 0,          /* International Atomic Time */
    AMBA_TIME_STD_UTC,              /* Coordinated Universal Time */
    AMBA_TIME_STD_UNIX,             /* Unix Time */

    AMBA_NUM_TIME_STD
} AMBA_RTC_TIME_STANDARD_e;

/*-----------------------------------------------------------------------------------------------*\
 * Time Spec Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTC_CALENDAR_TIME_s_ {
    UINT32              Year;
    UINT32              Month;      /* month of the year */
    UINT32              Day;        /* day of the month */
    AMBA_DAY_OF_WEEK_e  DayOfWeek;  /* day of the week */
    UINT32              Hour;
    UINT32              Minute;
    UINT32              Second;
} AMBA_RTC_CALENDAR_TIME_s;

typedef union _AMBA_RTC_TIME_SPEC_u_ {
    UINT32  Seconds;
    AMBA_RTC_CALENDAR_TIME_s    Calendar;
} AMBA_RTC_TIME_SPEC_u;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTC.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTC_Init(void);

int AmbaRTC_GetSystemTime(AMBA_RTC_TIME_STANDARD_e Type, AMBA_RTC_TIME_SPEC_u *pTimeSpec);
int AmbaRTC_SetSystemTime(AMBA_RTC_TIME_STANDARD_e Type, AMBA_RTC_TIME_SPEC_u *pTimeSpec);

#endif /* _AMBA_RTC_H_ */
