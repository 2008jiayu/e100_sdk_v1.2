/**
 * system/src/ui/ambsh/date.c
 *
 * History:
 *    2005/11/10 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

#include "AmbaRTC.h"

#define DAYS_PER_WEEK       7
#define MONS_PER_YEAR       12

static char const ab_weekday_name[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char const ab_month_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char format[] = "%3s %3s %02d %02d:%02d:%02d %04d";
static char result[3 + 1 + 3 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 4 + 32 + 1];

static char *tm_asctime(const AMBA_RTC_CALENDAR_TIME_s *pCalendar, char *buf)
{
    if (pCalendar == NULL)
        return NULL;

    if (sprintf(buf, format,
                ((pCalendar->DayOfWeek >= DAYS_PER_WEEK) ?
                 "???" : ab_weekday_name[pCalendar->DayOfWeek]),
                ((pCalendar->Month == 0 || pCalendar->Month > MONS_PER_YEAR) ?
                 "???" : ab_month_name[pCalendar->Month - 1]),
                pCalendar->Day, pCalendar->Hour, pCalendar->Minute,
                pCalendar->Second, pCalendar->Year) < 0)
        return NULL;

    return buf;
}

static inline void usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [-s YYYY/MM/DD HH:MM:SS]\n", argv[0]);
#if 0
    AmbaShell_Print(env, "Usage: %s [-set_alarm YYYY/MM/DD HH:MM:SS]\n", argv[0]);
    AmbaShell_Print(env, "Usage: %s [-get_alarm]\n", argv[0]);
    AmbaShell_Print(env, "Usage: %s [-clr_alarm]\n", argv[0]);
#endif
}

int ambsh_date(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AMBA_RTC_TIME_SPEC_u TimeSpec;
    AMBA_RTC_CALENDAR_TIME_s *pCalendar = &TimeSpec.Calendar;

    int i;
    char *str, tmp[10] = {0};
    char *date = NULL;
    char *time = NULL;

    if (argc == 1) {
        AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
        str = tm_asctime(pCalendar, result);
        AmbaShell_Print(env, "%s\n", str);
        goto done;
    }

    for (i = 1; i < argc; i++) {

        /* -s, --set=STRING    set time described by STRING */
        if (!strcmp(argv[i], "-s")) {
            date = argv[i + 1]; /* YYYY/MM/DD */
            time = argv[i + 2]; /* HH:MM:SS   */

            memset(tmp, 0x00, sizeof(tmp)); /* year */
            strncpy( tmp, date, 4);
            pCalendar->Year = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* month */
            strncpy( tmp, date + 5, 2);
            pCalendar->Month = atoi(tmp); /* 1 ~ 12 */

            memset(tmp, 0x00, sizeof(tmp)); /* date */
            strncpy( tmp, date + 8, 2);
            pCalendar->Day = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* hour */
            strncpy( tmp, time, 2);
            pCalendar->Hour = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* minute */
            strncpy( tmp, time + 3, 2);
            pCalendar->Minute = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* second */
            strncpy( tmp, time + 6, 2);
            pCalendar->Second = atoi(tmp);

            AmbaRTC_SetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
            goto done;
#if 0
        } else if (!strcmp(argv[i], "-set_alarm")) {
            date = argv[i + 1]; /* YYYY/MM/DD */
            time = argv[i + 2]; /* HH:MM:SS   */

            memset(tmp, 0x00, sizeof(tmp)); /* year */
            strncpy( tmp, date, 4);
            tm_alarm.tm_year = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* month */
            strncpy( tmp, date + 5, 2);
            tm_alarm.tm_mon = atoi(tmp);    /* 1 ~ 12 */

            memset(tmp, 0x00, sizeof(tmp)); /* date */
            strncpy( tmp, date + 8, 2);
            tm_alarm.tm_mday = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* hour */
            strncpy( tmp, time, 2);
            tm_alarm.tm_hour = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* minute */
            strncpy( tmp, time + 3, 2);
            tm_alarm.tm_min = atoi(tmp);

            memset(tmp, 0x00, sizeof(tmp)); /* second */
            strncpy( tmp, time + 6, 2);
            tm_alarm.tm_sec = atoi(tmp);

            rtc_get_time(&tm);
            rtc_set_time(&tm);
            rtc_set_alarm(&tm_alarm);
            goto done;
        } else if (!strcmp(argv[i], "-get_alarm")) {
            rtc_get_alarm(&tm_alarm);
            str = tm_asctime(&tm_alarm);
            AmbaShell_Print(env, "%s\n", str);
            goto done;
        } else if (!strcmp(argv[i], "-clr_alarm")) {
            rtc_clr_alarm();
            goto done;
#endif
        } else if (!strcmp(argv[i], "-h")) {
            usage(env, argc, argv);
            goto done;
        }
    }

    usage(env, argc, argv);

done:
    return 0;
}

