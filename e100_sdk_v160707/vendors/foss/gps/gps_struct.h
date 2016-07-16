/* $Id: gps.h 6692 2009-12-03 15:01:37Z esr $ */
#ifndef _GPS_STRUCT_H_
#define _GPS_STRUCT_H_

/* gps.h -- interface of the libgps library */

/* Macro for declaring function arguments unused. */
#if defined(__GNUC__)
#  define UNUSED __attribute__((unused)) /* Flag variable as unused */
#else /* not __GNUC__ */
#  define UNUSED
#endif


#if defined(__PRKERNEL_AMB__)
#include <itron.h>
#include <kutil.h>
//#include <ebcfg/stdint.h>
#include <stdint.h>
#else
#include <sys/types.h>
#include <stdbool.h>
//#include <inttypes.h>	/* stdint.h would be smaller but not all have it */
#include <stdint.h>	/* stdint.h would be smaller but not all have it */
#endif
#include <time.h>
#include <stdio.h>
/*
 * Unless otherwise noted, all function signatures and types and
 * constants and structure layouts exposed here are correct for all
 * 3.x versions up to and including this one.
 */

#define MAXTAGLEN	8	/* maximum length of sentence tag name */
#define MAXCHANNELS	20	/* maximum GPS channels (*not* satellites!) */

/* 
 * The structure describing an uncertainty volume in kinematic space.
 * This is what GPSes are meant to produce; all the other info is 
 * technical impedimenta.
 *
 * All double values use NAN to indicate data not available.
 *
 * Usually all the information in this structure was considered valid
 * by the GPS at the time of update.  This will be so if you are using
 * a GPS chipset that speaks SiRF binary, Garmin binary, or Zodiac binary.
 * This covers over 80% of GPS products in early 2005.
 *
 * If you are using a chipset that speaks NMEA, this structure is updated
 * in bits by GPRMC (lat/lon, track, speed), GPGGA (alt, climb), GPGLL 
 * (lat/lon), and GPGSA (eph, epv).  Most NMEA GPSes take a single fix
 * at the beginning of a 1-second cycle and report the same timestamp in
 * GPRMC, GPGGA, and GPGLL; for these, all info is guaranteed correctly
 * synced to the time member, but you'll get different stages of the same 
 * update depending on where in the cycle you poll.  A very few GPSes, 
 * like the Garmin 48, take a new fix before more than one of of 
 * GPRMC/GPGGA/GPGLL during a single cycle; thus, they may have different 
 * timestamps and some data in this structure can be up to 1 cycle (usually
 * 1 second) older than the fix time.
 *
 * Error estimates are at 95% confidence.
 */
struct gps_fix_t {
    double time;	/* Time of update, seconds since Unix epoch */
    int    mode;	/* Mode of fix */
#define MODE_NOT_SEEN	0	/* mode update not seen yet */
#define MODE_NO_FIX	1	/* none */
#define MODE_2D  	2	/* good for latitude/longitude */
#define MODE_3D  	3	/* good for altitude/climb too */
    double ept;		/* Expected time uncertainty */
    double latitude;	/* Latitude in degrees (valid if mode >= 2) */
    double epy;  	/* Latitude position uncertainty, meters */
    double longitude;	/* Longitude in degrees (valid if mode >= 2) */
    double epx;  	/* Longitude position uncertainty, meters */
    double altitude;	/* Altitude in meters (valid if mode == 3) */
    double epv;  	/* Vertical position uncertainty, meters */
    double track;	/* Course made good (relative to true north) */
    double epd;		/* Track uncertainty, degrees */
    double speed;	/* Speed over ground, meters/sec */
    double eps;		/* Speed uncertainty, meters/sec */
    double climb;       /* Vertical speed, meters/sec */
    double epc;		/* Vertical speed uncertainty */
};

/* 
 * Is an MMSI number that of an auxiliary associated with a mother ship?
 * We need to be able to test this for decoding AIS Type 24 messages.
 * According to <http://www.navcen.uscg.gov/marcomms/gmdss/mmsi.htm#format>,
 * auxiliary-craft MMSIs have the form 98MIDXXXX, where MID is a country 
 * code and XXXX the vessel ID.
 */
struct dop_t {
    /* Dilution of precision factors */
    double xdop, ydop, pdop, hdop, vdop, tdop, gdop;
};

/* following structures are for representing new-protocol responses */

#define GPS_PATH_MAX		64	/* dev files usually have short names */

struct devconfig_t {
    char path[GPS_PATH_MAX];
    int flags;
#define SEEN_GPS 	0x01
#define SEEN_RTCM2	0x02
#define SEEN_RTCM3	0x04
#define SEEN_AIS 	0x08
    char driver[64];
    char subtype[64];
    double activated;
    unsigned int baudrate, stopbits;	/* RS232 link parameters */
    char parity;			/* 'N', 'O', or 'E' */
    double cycle, mincycle;     	/* refresh cycle time in seconds */
    int driver_mode;    		/* is driver in native mode or not? */
};

/* this is the main structure that includes all previous substructures */

struct gps_data_s {
    unsigned int set;	/* has field been set since this was last cleared? */
#define ONLINE_SET	0x00000001u
#define TIME_SET	0x00000002u
#define TIMERR_SET	0x00000004u
#define LATLON_SET	0x00000008u
#define ALTITUDE_SET	0x00000010u
#define SPEED_SET	0x00000020u
#define TRACK_SET	0x00000040u
#define CLIMB_SET	0x00000080u
#define STATUS_SET	0x00000100u
#define MODE_SET	0x00000200u
#define DOP_SET  	0x00000400u
#define VERSION_SET	0x00000800u	/* only used in client library */
#define HERR_SET	0x00001000u
#define VERR_SET	0x00002000u
#define PERR_SET	0x00004000u
#define POLICY_SET	0x00008000u	/* only used in client library */
#define ERR_SET		(HERR_SET|VERR_SET|PERR_SET)
#define SATELLITE_SET	0x00010000u
#define RAW_SET		0x00020000u
#define USED_SET	0x00040000u
#define SPEEDERR_SET	0x00080000u
#define TRACKERR_SET	0x00100000u
#define CLIMBERR_SET	0x00200000u
#define DEVICE_SET	0x00400000u
#define DEVICELIST_SET	0x00800000u
#define DEVICEID_SET	0x01000000u
#define ERROR_SET	0x02000000u
#define RTCM2_SET	0x04000000u
#define RTCM3_SET	0x08000000u
#define AIS_SET 	0x10000000u
#define PACKET_SET	0x20000000u	/* only used in the daemon */
#define CLEAR_SET	0x40000000u	/* sentence starts a reporting cycle */
#define REPORT_SET	0x80000000u	/* sentence ends a reporting cycle */
#define DATA_SET	~(ONLINE_SET|PACKET_SET|CLEAR_SET|REPORT_SET)
    struct gps_fix_t	fix;	/* accumulated PVT data */

    double separation;		/* Geoidal separation, MSL - WGS84 (Meters) */

    /* GPS status -- always valid */
    int    status;		/* Do we have a fix? */
#define STATUS_NO_FIX	0	/* no */
#define STATUS_FIX	1	/* yes, without DGPS */
#define STATUS_DGPS_FIX	2	/* yes, with DGPS */

    /* precision of fix -- valid if satellites_used > 0 */
    int satellites_used;	/* Number of satellites used in solution */
    int used[MAXCHANNELS];	/* PRNs of satellites used in solution */
    struct dop_t dop;

    /* redundant with the estimate elements in the fix structure */
    double epe;  /* spherical position error, 95% confidence (meters)  */

    /* satellite status -- valid when satellites_visible > 0 */
    double skyview_time;	/* skyview timestamp */
    int satellites_visible;	/* # of satellites in view */
    int PRN[MAXCHANNELS];	/* PRNs of satellite */
    int elevation[MAXCHANNELS];	/* elevation of satellite */
    int azimuth[MAXCHANNELS];	/* azimuth */
    double ss[MAXCHANNELS];	/* signal-to-noise ratio (dB) */

    struct devconfig_t dev;	/* device that shipped last update */

    char tag[MAXTAGLEN+1];	/* tag of last sentence processed */
    // char night_flg;
    /* pack things never reported together to reduce structure size */ 
#define UNION_SET	(RTCM2_SET|RTCM3_SET|AIS_SET|VERSION_SET|DEVICELIST_SET|ERROR_SET)
#if 0
    union {
	struct {
	    double time;
	    int ndevices;
	} devices;
	char error[80];
    };
#endif
};

typedef struct gps_data_s gps_data_t;

extern void gps_clear_fix(/*@ out @*/struct gps_fix_t *);
extern time_t mkgmtime(register struct tm *);
extern double timestamp(void);
extern double wgs84_separation(double, double);

/* some multipliers for interpreting GPS output */
#define METERS_TO_FEET	3.2808399	/* Meters to U.S./British feet */
#define METERS_TO_MILES	0.00062137119	/* Meters to miles */
#define KNOTS_TO_MPH	1.1507794	/* Knots to miles per hour */
#define KNOTS_TO_KPH	1.852		/* Knots to kilometers per hour */
#define KNOTS_TO_MPS	0.51444444	/* Knots to meters per second */
#define MPS_TO_KPH	3.6		/* Meters per second to klicks/hr */
#define MPS_TO_MPH	2.2369363	/* Meters/second to miles per hour */
#define MPS_TO_KNOTS	1.9438445	/* Meters per second to knots */
/* miles and knots are both the international standard versions of the units */

/* angle conversion multipliers */
#define GPS_PI      3.1415926535897932384626433832795029
#define RAD_2_DEG	57.2957795130823208767981548141051703
#define DEG_2_RAD	0.0174532925199432957692369076848861271

/*
 * This is used only when an NMEA device issues a two-digit year in a GPRMC
 * and there has been no previous ZDA to set the year.  We used to
 * query the system clock for this,  but there's no good way to cope 
 * with the mess if the system clock has been zeroed.
 */
#define CENTURY_BASE	2000

// #ifdef CONFIG_ECL_RADAR_MODEL

typedef struct tagGPSINFO
{
    double UTC;
    char status;
    double Latitude;
    double Longitude;
    double Speed;
}K_GPS_INFO;

typedef struct 
{
    unsigned short cpu_used;
    unsigned short wave_shift;
    unsigned short op_mode;
    unsigned short radar_fs;
    unsigned short radar_bn;
    unsigned short radar_ph;
    unsigned short status;
    K_GPS_INFO  gps_info;
    unsigned short radar_lib_version;
}K_RADAR_STATUS_INFO;

typedef struct 
{
    unsigned short flag;
    unsigned short size;
    unsigned short index;
}K_ADC_PACKAGE;

typedef struct
{
    unsigned short s;//distance
    short v;//speed
    short h;//hor distance
    unsigned short p;//power
    unsigned short r;
}K_RADAR_OBJ;

#define RADAR_MAX_OBJ_NUM 8
typedef struct 
{
    // K_ADC_PACKAGE     radar_data_info;
    short             abs_speed;// m/s
    short             abs_acc;
    unsigned short    obj_num;
    K_RADAR_OBJ       objs[RADAR_MAX_OBJ_NUM];
    short             acc_x;
    short             acc_y;
    short             acc_z;
    short             acc_std;
}K_RADAR_OBJ_INFO;

typedef struct {
  K_RADAR_STATUS_INFO  status_info;
  K_RADAR_OBJ_INFO     obj_info;
}RADAR_DET_DATA_T;

typedef struct {
    K_ADC_PACKAGE cmd_pack;
    unsigned short cmd;
}K_RADAR_CMD_T;
// #endif
/* gps.h ends here */
#endif /* _GPS_STRUCT_H_ */
