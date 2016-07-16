/* This file is generated.  Do not hand-hack it! */
/* gpsd.h -- fundamental types and structures for the gpsd library */

/* $Id: gpsd.h-head 6566 2009-11-20 03:51:06Z esr $ */
#ifndef _GPS_PARSER_H_
#define _GPS_PARSER_H_

#include <stdio.h>
#include "gps_struct.h"

#ifndef GPSD_CONFIG_H
/* Feature configuration switches begin here */
/* gpsd_config.h.  Generated from gpsd_config.h.in by configure.  */
/* gpsd_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* MTK-3301 requires NMEA support */
#define NMEA_ENABLE 1

/* NTP time hinting support */
#define NTPSHM_ENABLE 1

/* PPS time syncing support */
#define PPS_ENABLE 1

#define GPS_NAN	0

#define GPSD_CONFIG_H

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
/* Feature configuration switches end here */
#endif /* GPSD_CONFIG_H */

/*
 * For NMEA-conforming receivers this is supposed to be 82, but
 * some receivers (TN-200, GSW 2.3.2) emit oversized sentences.
 * The current hog champion is the Trimble BX-960 receiver, which
 * emits a 91-character GGA message.
 */
#define NMEA_MAX	91		/* max length of NMEA sentence */

/*
 * The packet buffers need to be as long than the longest packet we
 * expect to see in any protocol, because we have to be able to hold
 * an entire packet for checksumming...
 * First we thought it had to be big enough for a SiRF Measured Tracker
 * Data packet (188 bytes). Then it had to be big enough for a UBX SVINFO
 * packet (206 bytes). Now it turns out that a couple of ITALK messages are
 * over 512 bytes. I know we like verbose output, but this is ridiculous.
 */
#define MAX_PACKET_LENGTH	516	/* 7 + 506 + 3 */

struct gps_timeval {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};

struct gps_device_t {
/* session object, encapsulates all global state */
    gps_data_t gpsdata;
    double rtcmtime;	/* timestamp of last RTCM104 correction to GPS */
    int saved_baud;
#ifdef NTPSHM_ENABLE
    int shmindex;
# ifdef PPS_ENABLE
    int shmTimeP;
# endif /* PPS_ENABLE */
#endif /* NTPSHM_ENABLE */
    double mag_var;			/* magnetic variation in degrees */
    int cycle_end_reliable;		/* does driverr signal REPORT_SET */
    /*
     * The rest of this structure is driver-specific private storage.
     * Because the Garmin driver uses a long buffer, you can have
     * up to 4096+12 bytes of private storage in your own union member
     * without making this structure larger or changing the API at all.
     */
    union {
#ifdef NMEA_ENABLE
	struct {
	    int part, await;		/* for tracking GSV parts */
	    struct tm date;		/* date part of last sentence time */
	    double subseconds;		/* subsec part of last sentence time */
	    char *field[NMEA_MAX];
	    unsigned char fieldcopy[NMEA_MAX+1];
	    /*
	     * State for the cycle-tracking machinery.
	     * The reason these timestamps are separate from the
	     * general sentence timestamps is that we can
	     * use the minutes and seconds part of a sentence
	     * with an incomplete timestasmp (like GGA) for
	     * end-cycle recognition, even if we don't have a previous
	     * RMC or ZDA that lets us get full time from it.
	     */
	    double this_frac_time, last_frac_time;
	    int latch_frac_time;
	    unsigned int lasttag;
	    unsigned int cycle_enders;
	} nmea;
#endif /* NMEA_ENABLE */
    } driver;
};

/* logging levels */
#define LOG_ERROR 	0	/* errors, display always */
#define LOG_SHOUT	0	/* not an error but we should always see it */
#define LOG_WARN	1	/* not errors but may indicate a problem */
#define LOG_INF 	2	/* key informative messages */
#define LOG_DATA	3	/* log data management messages */
#define LOG_PROG	4	/* progress messages */
#define LOG_IO  	5	/* IO to and from devices */
#define LOG_RAW 	6	/* raw low-level I/O */

extern unsigned int gps_parser(char *, struct gps_device_t *);
extern const char *gpsd_maskdump(unsigned int);
extern void clear_dop(/*@out@*/struct dop_t *);
/* application interface */
extern void gpsd_init(struct gps_device_t *);
extern void gpsd_zero_satellites(/*@out@*/gps_data_t *sp)/*@modifies sp@*/;

//extern void gpsd_report(int errlevel UNUSED, const char *fmt, ... );
#define gpsd_report(... )

#endif /* _GPS_PARSER_H_ */
