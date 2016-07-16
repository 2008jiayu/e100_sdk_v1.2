/* $Id: libgpsd_core.c 6704 2009-12-04 12:56:36Z esr $ */
/* libgpsd_core.c -- direct access to GPSes on serial or USB devices. */
#include "gps_parser.h"
#include <stdarg.h>
#include <string.h>

#define MONTHSPERYEAR	12		/* months per calendar year */

void gps_clear_fix(/*@out@*/struct gps_fix_t *fixp)
/* stuff a fix structure with recognizable out-of-band values */
{
    fixp->time = GPS_NAN;
    fixp->mode = MODE_NOT_SEEN;
    fixp->latitude = fixp->longitude = GPS_NAN;
    fixp->track = GPS_NAN;
    fixp->speed = GPS_NAN;
    fixp->climb = GPS_NAN;
    fixp->altitude = GPS_NAN;
    fixp->ept = GPS_NAN;
    fixp->epx = GPS_NAN;
    fixp->epy = GPS_NAN;
    fixp->epv = GPS_NAN;
    fixp->epd = GPS_NAN;
    fixp->eps = GPS_NAN;
    fixp->epc = GPS_NAN;
}

double timestamp(void)
{
    struct gps_timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
//    (void)gettimeofday(&tv, NULL);
    /*@i1@*/return(tv.tv_sec + tv.tv_usec*1e-6);
}

time_t mkgmtime(register struct tm *t)
/* struct tm to seconds since Unix epoch */
{
    register int year;
    register time_t result;
    static const int cumdays[MONTHSPERYEAR] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    /*@ +matchanyintegral @*/
    year = 1900 + t->tm_year + t->tm_mon / MONTHSPERYEAR;
    result = (year - 1970) * 365 + cumdays[t->tm_mon % MONTHSPERYEAR];
    result += (year - 1968) / 4;
    result -= (year - 1900) / 100;
    result += (year - 1600) / 400;
    if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) &&
	(t->tm_mon % MONTHSPERYEAR) < 2)
	     result--;
    result += t->tm_mday - 1;
    result *= 24;
    result += t->tm_hour;
    result *= 60;
    result += t->tm_min;
    result *= 60;
    result += t->tm_sec;
    /*@ -matchanyintegral @*/
    return (result);
}

void clear_dop(/*@out@*/struct dop_t *dop)
{
    dop->xdop = dop->ydop = dop->vdop = dop->tdop = dop->hdop = dop->pdop = dop->gdop = GPS_NAN;
}



void gpsd_tty_init(struct gps_device_t *session)
/* to be called on allocating a device */
{
    /* mark GPS baud rate unknown */
    session->saved_baud = -1;
#ifdef NTPSHM_ENABLE
    /* mark NTPD shared memory segments as unused */
    session->shmindex = -1;
# ifdef PPS_ENABLE
    session->shmTimeP = -1;
# endif /* PPS_ENABLE */
#endif /* NTPSHM_ENABLE */
}

void gpsd_zero_satellites(/*@out@*/gps_data_t *out)
{
    memset(out->PRN,	 0, sizeof(out->PRN));
    memset(out->elevation, 0, sizeof(out->elevation));
    memset(out->azimuth,	 0, sizeof(out->azimuth));
    memset(out->ss,	 0, sizeof(out->ss));
    out->satellites_visible = 0;
}

void gpsd_init(struct gps_device_t *session)
/* initialize GPS polling */
{
    session->rtcmtime = 0;
    /*@ +temptrans @*/
    /*@ +mayaliasunique @*/
    /*@ +mustfreeonly @*/
    gps_clear_fix(&session->gpsdata.fix);
    session->gpsdata.set = 0;
    session->gpsdata.status = STATUS_NO_FIX;
    session->gpsdata.dop.hdop = GPS_NAN;
    session->gpsdata.dop.vdop = GPS_NAN;
    session->gpsdata.dop.pdop = GPS_NAN;
    session->gpsdata.dop.tdop = GPS_NAN;
    session->gpsdata.dop.gdop = GPS_NAN;
    session->gpsdata.epe = GPS_NAN;
    session->mag_var = GPS_NAN;
    session->gpsdata.dev.cycle = session->gpsdata.dev.mincycle = 1;

    /* tty-level initialization */
    gpsd_tty_init(session);

    /* necessary in case we start reading in the middle of a GPGSV sequence */
    gpsd_zero_satellites(&(session->gpsdata));

}

const char *gpsd_maskdump(unsigned int set)
{
    static char buf[173];
    const struct {
	unsigned int      mask;
	const char      *name;
    } *sp, names[] = {
	{ONLINE_SET,	"ONLINE"},
	{TIME_SET,	"TIME"},
	{TIMERR_SET,	"TIMERR"},
	{LATLON_SET,	"LATLON"},
	{ALTITUDE_SET,	"ALTITUDE"},
	{SPEED_SET,	"SPEED"},
	{TRACK_SET,	"TRACK"},
	{CLIMB_SET,	"CLIMB"},
	{STATUS_SET,	"STATUS"},
	{MODE_SET,	"MODE"},
	{DOP_SET,	"DOP"},
	{VERSION_SET,	"VERSION"},
	{HERR_SET,	"HERR"},
	{VERR_SET,	"VERR"},
	{PERR_SET,	"PERR"},
	{POLICY_SET,	"POLICY"},
	{SATELLITE_SET,	"SATELLITE"},
	{RAW_SET,	"RAW"},
	{USED_SET,	"USED"},
	{SPEEDERR_SET,	"SPEEDERR"},
	{TRACKERR_SET,	"TRACKERR"},
	{CLIMBERR_SET,	"CLIMBERR"},
	{DEVICE_SET,	"DEVICE"},
	{DEVICELIST_SET,	"DEVICELIST"},
	{DEVICEID_SET,	"DEVICEID"},
	{ERROR_SET,	"ERROR"},
	{RTCM2_SET,	"RTCM2"},
	{RTCM3_SET,	"RTCM3"},
	{AIS_SET,	"AIS"},
	{PACKET_SET,	"PACKET"},
	{CLEAR_SET,	"CLEAR"},
	{REPORT_SET,	"REPORT"},
    };

    memset(buf, '\0', sizeof(buf));

    buf[0] = '{';
    for (sp = names; sp < names + sizeof(names)/sizeof(names[0]); sp++)
	if ((set & sp->mask)!=0) {
	    (void)strncat(buf, sp->name, sizeof(buf));
	    (void)strncat(buf, "|", sizeof(buf));
	}
    if (buf[1] != '\0')
	buf[strlen(buf)-1] = '\0';
    (void)strncat(buf, "}", sizeof(buf));
    return buf;
}

static double bilinear(double x1, double y1, double x2, double y2, double x, double y, double z11, double z12, double z21, double z22)
{
 double delta;

 if (y1 == y2 && x1 == x2 ) return (z11);
 if (y1 == y2 && x1 != x2 ) return (z22*(x-x1)+z11*(x2-x))/(x2-x1);
 if (x1 == x2 && y1 != y2 ) return (z22*(y-y1)+z11*(y2-y))/(y2-y1);

 delta=(y2-y1)*(x2-x1);

 return (z22*(y-y1)*(x-x1)+z12*(y2-y)*(x-x1)+z21*(y-y1)*(x2-x)+z11*(y2-y)*(x2-x))/delta;
}


/* return geoid separtion (MSL - WGS84) in meters, given a lat/lot in degrees */
/*@ +charint @*/
double wgs84_separation(double lat, double lon)
{
#define GEOID_ROW	19
#define GEOID_COL	37
//    const char geoid_delta[GEOID_COL*GEOID_ROW]={
    const float geoid_delta[GEOID_COL*GEOID_ROW]={
	/* 90S */ -30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, -30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,
	/* 80S */ -53,-54,-55,-52,-48,-42,-38,-38,-29,-26,-26,-24,-23,-21,-19,-16,-12, -8, -4, -1,  1,  4,  4,  6,  5,  4,   2, -6,-15,-24,-33,-40,-48,-50,-53,-52,-53,
	/* 70S */ -61,-60,-61,-55,-49,-44,-38,-31,-25,-16, -6,  1,  4,  5,  4,  2,  6, 12, 16, 16, 17, 21, 20, 26, 26, 22,  16, 10, -1,-16,-29,-36,-46,-55,-54,-59,-61,
	/* 60S */ -45,-43,-37,-32,-30,-26,-23,-22,-16,-10, -2, 10, 20, 20, 21, 24, 22, 17, 16, 19, 25, 30, 35, 35, 33, 30,  27, 10, -2,-14,-23,-30,-33,-29,-35,-43,-45,
	/* 50S */ -15,-18,-18,-16,-17,-15,-10,-10, -8, -2,  6, 14, 13,  3,  3, 10, 20, 27, 25, 26, 34, 39, 45, 45, 38, 39,  28, 13, -1,-15,-22,-22,-18,-15,-14,-10,-15,
	/* 40S */  21,  6,  1, -7,-12,-12,-12,-10, -7, -1,  8, 23, 15, -2, -6,  6, 21, 24, 18, 26, 31, 33, 39, 41, 30, 24,  13, -2,-20,-32,-33,-27,-14, -2,  5, 20, 21,
	/* 30S */  46, 22,  5, -2, -8,-13,-10, -7, -4,  1,  9, 32, 16,  4, -8,  4, 12, 15, 22, 27, 34, 29, 14, 15, 15,  7,  -9,-25,-37,-39,-23,-14, 15, 33, 34, 45, 46,
	/* 20S */  51, 27, 10,  0, -9,-11, -5, -2, -3, -1,  9, 35, 20, -5, -6, -5,  0, 13, 17, 23, 21,  8, -9,-10,-11,-20, -40,-47,-45,-25,  5, 23, 45, 58, 57, 63, 51,
	/* 10S */  36, 22, 11,  6, -1, -8,-10, -8,-11, -9,  1, 32,  4,-18,-13, -9,  4, 14, 12, 13, -2,-14,-25,-32,-38,-60, -75,-63,-26,  0, 35, 52, 68, 76, 64, 52, 36,
	/* 00N */  22, 16, 17, 13,  1,-12,-23,-20,-14, -3, 14, 10,-15,-27,-18,  3, 12, 20, 18, 12,-13, -9,-28,-49,-62,-89,-102,-63, -9, 33, 58, 73, 74, 63, 50, 32, 22,
	/* 10N */  13, 12, 11,  2,-11,-28,-38,-29,-10,  3,  1,-11,-41,-42,-16,  3, 17, 33, 22, 23,  2, -3, -7,-36,-59,-90, -95,-63,-24, 12, 53, 60, 58, 46, 36, 26, 13,
	/* 20N */   5, 10,  7, -7,-23,-39,-47,-34, -9,-10,-20,-45,-48,-32, -9, 17, 25, 31, 31, 26, 15,  6,  1,-29,-44,-61, -67,-59,-36,-11, 21, 39, 49, 39, 22, 10,  5,
	/* 30N */  -7, -5, -8,-15,-28,-40,-42,-29,-22,-26,-32,-51,-40,-17, 17, 31, 34, 44, 36, 28, 29, 17, 12,-20,-15,-40, -33,-34,-34,-28,  7, 29, 43, 20,  4, -6, -7,
	/* 40N */ -12,-10,-13,-20,-31,-34,-21,-16,-26,-34,-33,-35,-26,  2, 33, 59, 52, 51, 52, 48, 35, 40, 33, -9,-28,-39, -48,-59,-50,-28,  3, 23, 37, 18, -1,-11,-12,
	/* 50N */  -8,  8,  8,  1,-11,-19,-16,-18,-22,-35,-40,-26,-12, 24, 45, 63, 62, 59, 47, 48, 42, 28, 12,-10,-19,-33, -43,-42,-43,-29, -2, 17, 23, 22,  6,  2, -8,
	/* 60N */   2,  9, 17, 10, 13,  1,-14,-30,-39,-46,-42,-21,  6, 29, 49, 65, 60, 57, 47, 41, 21, 18, 14,  7, -3,-22, -29,-32,-32,-26,-15, -2, 13, 17, 19,  6,  2,
	/* 70N */   2,  2,  1, -1, -3, -7,-14,-24,-27,-25,-19,  3, 24, 37, 47, 60, 61, 58, 51, 43, 29, 20, 12,  5, -2,-10, -14,-12,-10,-14,-12, -6, -2,  3,  6,  4,  2,
	/* 80N */   3,  1, -2, -3, -3, -3, -1,  3,  1,  5,  9, 11, 19, 27, 31, 34, 33, 34, 33, 34, 28, 23, 17, 13,  9,  4,   4,  1, -2, -2,  0,  2,  3,  2,  1,  1,  3,
	/* 90N */  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13};
/*@ -charint @*/
    int	ilat, ilon;
    int	ilat1, ilat2, ilon1, ilon2;

//    ilat=(int)floor(( 90+lat)/10);
//    ilon=(int)floor((180+lon)/10);
    ilat=(int)(( 90+lat)/10);
    ilon=(int)((180+lon)/10);

    /* sanity checks to prevent segfault on bad data */
    if ( ( ilat > 90 ) || ( ilat < -90 ) ) {
	return 0.0;
    }
    if ( ( ilon > 180 ) || ( ilon < -180 ) ) {
	return 0.0;
    }

    ilat1=ilat;
    ilon1=ilon;
    ilat2=(ilat < GEOID_ROW-1)? ilat+1:ilat;
    ilon2=(ilon < GEOID_COL-1)? ilon+1:ilon;

    return bilinear(
	ilon1*10.-180.,ilat1*10.-90.,
	ilon2*10.-180.,ilat2*10.-90.,
	lon,           lat,
	(double)geoid_delta[ilon1+ilat1*GEOID_COL],
	(double)geoid_delta[ilon2+ilat1*GEOID_COL],
	(double)geoid_delta[ilon1+ilat2*GEOID_COL],
	(double)geoid_delta[ilon2+ilat2*GEOID_COL]
	);
}

void gpsd_reportp(int errlevel UNUSED, const char *fmt, ... )
/* stub logger for clients that don't supply one */
{

    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

}

