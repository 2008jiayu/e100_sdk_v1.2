/* $Id: driver_nmea.c 6703 2009-12-04 12:42:36Z esr $ */
#include <sys/types.h>
#include <stdio.h>
#ifndef S_SPLINT_S
#include <unistd.h>
#endif /* S_SPLINT_S */
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

#include "gps_struct.h"
#include "gps_parser.h"
#include "AmbaSSP.h"


#ifdef NMEA_ENABLE
/**************************************************************************
 *
 * Parser helpers begin here
 *
 **************************************************************************/

static void do_lat_lon(char *field[], gps_data_t *out)
/* process a pair of latitude/longitude fields starting at field index BEGIN */
{
    double lat, lon, d, m;
    char str[20], *p;
    int updated = 0;

    if (*(p = field[0]) != '\0') {
	strncpy(str, p, 20);
	(void)sscanf(p, "%lf", &lat);
	m = 100.0 * modf(lat / 100.0, &d);
	lat = d + m / 60.0;
	p = field[1];
	if (*p == 'S')
	    lat = -lat;
	if (out->fix.latitude != lat)
	    out->fix.latitude = lat;
	updated++;
    }
    if (*(p = field[2]) != '\0') {
	strncpy(str, p, 20);
	(void)sscanf(p, "%lf", &lon);
	m = 100.0 * modf(lon / 100.0, &d);
	lon = d + m / 60.0;

	p = field[3];
	if (*p == 'W')
	    lon = -lon;
	if (out->fix.longitude != lon)
	    out->fix.longitude = lon;
	updated++;
    }
}

/**************************************************************************
 *
 * Scary timestamp fudging begins here
 *
 * Four sentences, GGA and GLL and RMC and ZDA, contain timestamps.
 * GGA/GLL/RMC timestamps look like hhmmss.ss, with the trailing .ss part
 * optional.  RMC has a date field, in the format ddmmyy.  ZDA has
 * separate fields for day/month/year, with a 4-digit year.  This
 * means that for RMC we must supply a century and for GGA and GLL we
 * must supply a century, year, and day.  We get the missing data from
 * a previous RMC or ZDA; century in RMC is supplied by a constant if
 * there has been no previous ZDA.
 *
 **************************************************************************/

#define DD(s)	((int)((s)[0]-'0')*10+(int)((s)[1]-'0'))

static void merge_ddmmyy(char *ddmmyy, struct gps_device_t *session)
/* sentence supplied ddmmyy, but no century part */
{
    if (session->driver.nmea.date.tm_year == 0) {
    	session->driver.nmea.date.tm_year = (CENTURY_BASE + DD(ddmmyy+4)) - 1900;
    	gpsd_report(LOG_DATA, "merge_ddmmyy(ddmmyy) sets year %d from %s\n",
    		    session->driver.nmea.date.tm_year,
    		    ddmmyy);
    }

    session->driver.nmea.date.tm_mon = DD(ddmmyy+2)-1;
    session->driver.nmea.date.tm_mday = DD(ddmmyy);
}

static void merge_hhmmss(char *hhmmss, struct gps_device_t *session)
/* update from a UTC time */
{
    int old_hour = session->driver.nmea.date.tm_hour;

    session->driver.nmea.date.tm_hour = DD(hhmmss);

    if (session->driver.nmea.date.tm_hour < old_hour)	/* midnight wrap */
	    session->driver.nmea.date.tm_mday++;

    session->driver.nmea.date.tm_min = DD(hhmmss+2);
    session->driver.nmea.date.tm_sec = DD(hhmmss+4);
    session->driver.nmea.subseconds = atof(hhmmss+4) - session->driver.nmea.date.tm_sec;
}

static void register_fractional_time(const char *tag, const char *fld,
				     struct gps_device_t *session)
{
    if (fld[0]!='\0') {
    	session->driver.nmea.last_frac_time=session->driver.nmea.this_frac_time;
    	session->driver.nmea.this_frac_time = atof(fld);
    	session->driver.nmea.latch_frac_time = 1;
    	gpsd_report(LOG_DATA, "%s: registers fractional time %.2f\n",
    		    tag, session->driver.nmea.this_frac_time);
    }
}

/**************************************************************************
 *
 * Compare GPS timestamps for equality.  Depends on the fact that the
 * timestamp granularity of GPS is 1/100th of a second.  Use this to avoid
 * naive float comparisons.
 *
 **************************************************************************/

#define GPS_TIME_EQUAL(a, b) (fabs((a) - (b)) < 0.01)

/**************************************************************************
 *
 * NMEA sentence handling begins here
 *
 **************************************************************************/
#define S_ABS(num)                  ((num>0)?(num):-(num))  
static unsigned int processGPRMC(int count, char *field[], struct gps_device_t *session)
/* Recommend Minimum Course Specific GPS/TRANSIT Data */
{
    /*
	RMC,225446.33,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E,A*68
     1     225446.33    Time of fix 22:54:46 UTC
     2     A	        Status of Fix: A = Autonomous, valid;
                        D = Differential, valid; V = invalid
     3,4   4916.45,N    Latitude 49 deg. 16.45 min North
     5,6   12311.12,W   Longitude 123 deg. 11.12 min West
     7     000.5	Speed over ground, Knots
     8     054.7	Course Made Good, True north
     9     181194       Date of fix  18 November 1994
     10,11 020.3,E      Magnetic variation 20.3 deg East
     12    A	    FAA mode indicator (NMEA 2.3 and later)
			A=autonomous, D=differential, E=Estimated,
			N=not valid, S=Simulator, M=Manual input mode
	   *68	  mandatory nmea_checksum

     * SiRF chipsets don't return either Mode Indicator or magnetic variation.
     */
     static int loop_c=0;
     int speed_temp=0;
    unsigned int mask = 0;
    const char *str;
  
    if (strcmp(field[2], "V")==0) {
        // if(session->gpsdata.fix.mode >= MODE_2D && loop_c++<5)
        // {
        //     return 0;
        // }    
        
        	/* copes with Magellan EC-10X, see below */
        if (session->gpsdata.status != STATUS_NO_FIX) {
        	    session->gpsdata.status = STATUS_NO_FIX;
        	    mask |= STATUS_SET;
        }
        if (session->gpsdata.fix.mode >= MODE_2D) {
        	    session->gpsdata.fix.mode = MODE_NO_FIX;
        	    mask |= MODE_SET;
    	}
        session->gpsdata.fix.speed=0;
        mask &= ~(TRACK_SET | SPEED_SET);
        // AmbaPrint("STATUS_NO_FIX");    
    	/* set something nz, so it won't look like an unknown sentence */
    	mask |= ONLINE_SET;
        // loop_c=0;
        
    } else if (strcmp(field[2], "A")==0) {
    	/*
    	 * The MKT3301, Royaltek RGM-3800, and possibly other
    	 * devices deliver bogus time values when the navigation
    	 * warning bit is set.
    	 */
    	if (count > 9 && field[1][0]!='\0' && field[9][0]!='\0') {
    	    merge_hhmmss(field[1], session);
    	    merge_ddmmyy(field[9], session);
    	    mask |= TIME_SET;
    	    register_fractional_time(field[0], field[1], session);
            // if(session->driver.nmea.date.tm_hour>=11&&session->driver.nmea.date.tm_hour<=22)
            // {
            //     session->gpsdata.night_flg=1;
            // }
            // else
            // {
            //     session->gpsdata.night_flg=0;
            // }
            // AmbaPrint("%d-%d-%d %d:%d:%d",
            // session->driver.nmea.date.tm_year,
            // session->driver.nmea.date.tm_mon,
            // session->driver.nmea.date.tm_mday,
            // session->driver.nmea.date.tm_hour,
            // session->driver.nmea.date.tm_min,
            // session->driver.nmea.date.tm_sec);
	    }
    	do_lat_lon(&field[3], &session->gpsdata);
    	mask |= LATLON_SET;
        // speed_temp=atof(field[7]) * KNOTS_TO_MPS;
        // if(session->gpsdata.fix.speed>10&&S_ABS(session->gpsdata.fix.speed-speed_temp)>10)
        // {
        //     // session->gpsdata.fix.speed=session->gpsdata.fix.speed;
        // }
        // else
        // {
            session->gpsdata.fix.speed = atof(field[7]) * KNOTS_TO_MPS;;
        // }    
        // AmbaPrint("sPEED[%d]" ,session->gpsdata.fix.speed);
        // loop_c=0;
    	session->gpsdata.fix.track = atof(field[8]);
    	mask |= (TRACK_SET | SPEED_SET);
    	/*
    	 * This copes with GPSes like the Magellan EC-10X that *only* emit
    	 * GPRMC. In this case we set mode and status here so the client
    	 * code that relies on them won't mistakenly believe it has never
    	 * received a fix.
    	 */
    	if (session->gpsdata.status == STATUS_NO_FIX) {
    	    session->gpsdata.status = STATUS_FIX;	/* could be DGPS_FIX, we can't tell */
    	    mask |= STATUS_SET;
    	}

        if (session->gpsdata.fix.mode < MODE_2D) {
    	    session->gpsdata.fix.mode = MODE_2D;
    	    mask |= MODE_SET;
    	}
    }

    str = gpsd_maskdump(mask);

    gpsd_report(LOG_DATA,
		"RMC: ddmmyy=%s hhmmss=%s lat=%.2f lon=%.2f "
		"speed=%.2f track=%.2f mode=%d status=%d mask=%s\n",
		field[9], field[1],
		session->gpsdata.fix.latitude,
		session->gpsdata.fix.longitude,
		session->gpsdata.fix.speed,
		session->gpsdata.fix.track,
		session->gpsdata.fix.mode,
		session->gpsdata.status,
		str);
//		gpsd_maskdump(mask));

    return mask;
}

static unsigned int processGPGLL(int count, char *field[], struct gps_device_t *session)
/* Geographic position - Latitude, Longitude */
{
    /* Introduced in NMEA 3.0.

       $GPGLL,4916.45,N,12311.12,W,225444,A,A*5C

       1,2: 4916.46,N    Latitude 49 deg. 16.45 min. North
       3,4: 12311.12,W   Longitude 123 deg. 11.12 min. West
       5:   225444       Fix taken at 22:54:44 UTC
       6:   A            Data valid
       7:   A            Autonomous mode
       8:   *5C          Mandatory NMEA checksum

     1,2 Latitude, N (North) or S (South)
     3,4 Longitude, E (East) or W (West)
     5 UTC of position
     6 A=Active, V=Void
     7 Mode Indicator
       A = Autonomous mode
       D = Differential Mode
       E = Estimated (dead-reckoning) mode
       M = Manual Input Mode
       S = Simulated Mode
       N = Data Not Valid

     I found a note at <http://www.secoh.ru/windows/gps/nmfqexep.txt>
     indicating that the Garmin 65 does not return time and status.
     SiRF chipsets don't return the Mode Indicator.
     This code copes gracefully with both quirks.

     Unless you care about the FAA indicator, this sentence supplies nothing
     that GPRMC doesn't already.  But at least one Garmin GPS -- the 48
     actually ships updates in GPLL that aren't redundant.
     */
    char *status = field[7];
    unsigned int mask = ERROR_SET;

    if (field[5][0]!='\0') {
    	merge_hhmmss(field[5], session);
    	register_fractional_time(field[0], field[5], session);
    	if (session->driver.nmea.date.tm_year == 0){
    	    gpsd_report(LOG_WARN, "can't use GLL time until after ZDA or RMC has supplied a year.\n");
        }
    	else {
    	    mask = TIME_SET;
    	}
    }
    if (strcmp(field[6], "A")==0 && (count < 8 || *status != 'N')) {
    	int newstatus = session->gpsdata.status;

    	mask &=~ ERROR_SET;
    	do_lat_lon(&field[1], &session->gpsdata);
    	mask |= LATLON_SET;
    	if (count >= 8 && *status == 'D')
    	    newstatus = STATUS_DGPS_FIX;	/* differential */
    	else
    	    newstatus = STATUS_FIX;
    	/*
    	 * This is a bit dodgy.  Technically we shouldn't set the mode
    	 * bit until we see GSA.  But it may be later in the cycle,
    	 * some devices like the FV-18 don't send it by default, and
    	 * elsewhere in the code we want to be able to test for the
    	 * presence of a valid fix with mode > MODE_NO_FIX.
    	 */
    	if (session->gpsdata.fix.mode < MODE_2D) {
    	    session->gpsdata.fix.mode = MODE_2D;
    	    mask |= MODE_SET;
    	}
    	session->gpsdata.status = newstatus;
    	mask |= STATUS_SET;
    }

    gpsd_report(LOG_DATA,
		"GLL: hhmmss=%s lat=%.2f lon=%.2f mode=%d status=%d mask=%s\n",
		field[5],
		session->gpsdata.fix.latitude,
		session->gpsdata.fix.longitude,
		session->gpsdata.fix.mode,
		session->gpsdata.status,
		gpsd_maskdump(mask));
    return mask;
}

static unsigned int processGPGGA(int c UNUSED, char *field[], struct gps_device_t *session)
/* Global Positioning System Fix Data */
{
    /*
	GGA,123519,4807.038,N,01131.324,E,1,08,0.9,545.4,M,46.9,M, , *42
	   1     123519       Fix taken at 12:35:19 UTC
	   2,3   4807.038,N   Latitude 48 deg 07.038' N
	   4,5   01131.324,E  Longitude 11 deg 31.324' E
	   6	 1            Fix quality: 0 = invalid, 1 = GPS, 2 = DGPS,
	        	      3=PPS (Precise Position Service),
	                      4=RTK (Real Time Kinematic) with fixed integers,
	                      5=Float RTK, 6=Estimated, 7=Manual, 8=Simulator
	   7     08	      Number of satellites being tracked
	   8     0.9	      Horizontal dilution of position
	   9,10  545.4,M      Altitude, Metres above mean sea level
	   11,12 46.9,M       Height of geoid (mean sea level) above WGS84
			ellipsoid, in Meters
	   (empty field) time in seconds since last DGPS update
	   (empty field) DGPS station ID number (0000-1023)
    */
    unsigned int mask;

    session->gpsdata.status = atoi(field[6]);
    mask = STATUS_SET;

    if (session->gpsdata.status > STATUS_NO_FIX) {
    	char *altitude;

    	merge_hhmmss(field[1], session);
    	register_fractional_time(field[0], field[1], session);

        if (session->driver.nmea.date.tm_year == 0){
    	    gpsd_report(LOG_WARN, "can't use GGA time until after ZDA or RMC has supplied a year.\n");
    	} else {
    	    mask |= TIME_SET;
    	}
    	do_lat_lon(&field[2], &session->gpsdata);
    	mask |= LATLON_SET;
    	session->gpsdata.satellites_used = atoi(field[7]);
    	altitude = field[9];
    	/*
    	 * SiRF chipsets up to version 2.2 report a null altitude field.
    	 * See <http://www.sirf.com/Downloads/Technical/apnt0033.pdf>.
    	 * If we see this, force mode to 2D at most.
    	 */
    	if (altitude[0] == '\0') {
    	    if (session->gpsdata.fix.mode == MODE_3D) {
    		session->gpsdata.fix.mode = session->gpsdata.status ? MODE_2D : MODE_NO_FIX;
    		mask |= MODE_SET;
    	    }
    	} else {
    	    session->gpsdata.fix.altitude = atof(altitude);
    	    mask |= ALTITUDE_SET;
    	    /*
    	     * This is a bit dodgy.  Technically we shouldn't set the mode
    	     * bit until we see GSA.  But it may be later in the cycle,
    	     * some devices like the FV-18 don't send it by default, and
    	     * elsewhere in the code we want to be able to test for the
    	     * presence of a valid fix with mode > MODE_NO_FIX.
    	     */
    	    if (session->gpsdata.fix.mode < MODE_3D) {
    		session->gpsdata.fix.mode = MODE_3D;
    		mask |= MODE_SET;
    	    }
    	}
    	if (strlen(field[11]) > 0) {
    	   session->gpsdata.separation = atof(field[11]);
    	} else {
    	   session->gpsdata.separation = wgs84_separation(session->gpsdata.fix.latitude,session->gpsdata.fix.longitude);
    	}
    }
    gpsd_report(LOG_DATA,
		"GGA: hhmmss=%s lat=%.2f lon=%.2f alt=%.2f mode=%d status=%d mask=%s\n",
		field[1],
		session->gpsdata.fix.latitude,
		session->gpsdata.fix.longitude,
		session->gpsdata.fix.altitude,
		session->gpsdata.fix.mode,
		session->gpsdata.status,
		gpsd_maskdump(mask));
    return mask;
}

static unsigned int processGPGSA(int count, char *field[], struct gps_device_t *session)
/* GPS DOP and Active Satellites */
{
    /*
	eg1. $GPGSA,A,3,,,,,,16,18,,22,24,,,3.6,2.1,2.2*3C
	eg2. $GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35
	1    = Mode:
	       M=Manual, forced to operate in 2D or 3D
	       A=Automatic, 3D/2D
	2    = Mode: 1=Fix not available, 2=2D, 3=3D
	3-14 = PRNs of satellites used in position fix (null for unused fields)
	15   = PDOP
	16   = HDOP
	17   = VDOP
     */
    unsigned int mask;

    /*
     * One chipset called the i.Trek M3 issues GPGSA lines that look like
     * this: "$GPGSA,A,1,,,,*32" when it has no fix.  This is broken
     * in at least two ways: it's got the wrong number of fields, and
     * it claims to be a valid sentence (A flag) when it isn't.
     * Alarmingly, it's possible this error may be generic to SiRFstarIII.
     */
    if (count < 17) {
    	gpsd_report(LOG_DATA, "GPGSA: malformed, setting ONLINE_SET only.\n");
    	mask = ONLINE_SET;
    } else {
    	int i;
    	session->gpsdata.fix.mode = atoi(field[2]);
    	/*
    	 * The first arm of this conditional ignores dead-reckoning
    	 * fixes from an Antaris chipset. which returns E in field 2
    	 * for a dead-reckoning estimate.  Fix by Andreas Stricker.
    	 */
    	if (session->gpsdata.fix.mode == 0 && field[2][0] == 'E')
    	    mask = 0;
    	else
    	    mask = MODE_SET;
    	gpsd_report(LOG_PROG, "GPGSA sets mode %d\n", session->gpsdata.fix.mode);
    	clear_dop(&session->gpsdata.dop);
    	session->gpsdata.dop.pdop = atof(field[15]);
    	session->gpsdata.dop.hdop = atof(field[16]);
    	session->gpsdata.dop.vdop = atof(field[17]);
    	session->gpsdata.satellites_used = 0;
    	memset(session->gpsdata.used,0,sizeof(session->gpsdata.used));
    	/* the magic 6 here counts the tag, two mode fields, and the DOP fields */
    	for (i = 0; i < count - 6; i++) {
    	    int prn = atoi(field[i+3]);
    	    if (prn > 0)
    		session->gpsdata.used[session->gpsdata.satellites_used++] = prn;
    	}
    	mask |= DOP_SET | USED_SET;

        /* FIXME: perhaps dump the satellite vector here? */
    	gpsd_report(LOG_DATA,
    		    "GPGSA: mode=%d used=%d pdop=%.2f hdop=%.2f vdop=%.2f mask=%s\n",
    		    session->gpsdata.fix.mode,
    		    session->gpsdata.satellites_used,
    		    session->gpsdata.dop.pdop,
    		    session->gpsdata.dop.hdop,
    		    session->gpsdata.dop.vdop,
    		    gpsd_maskdump(mask));
    }
    return mask;
}

static unsigned int processGPGSV(int count, char *field[], struct gps_device_t *session)
/* GPS Satellites in View */
{
    /*
	GSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75
	   2	   Number of sentences for full data
	   1	   Sentence 1 of 2
	   08	   Total number of satellites in view
	   01	   Satellite PRN number
	   40	   Elevation, degrees
	   083	   Azimuth, degrees
	   46	   Signal-to-noise ratio in decibels
	   <repeat for up to 4 satellites per sentence>
		There my be up to three GSV sentences in a data packet
     */
    int n, fldnum;
    if (count <= 3) {
    	gpsd_zero_satellites(&session->gpsdata);
    	session->gpsdata.satellites_visible = 0;
    	return ERROR_SET;
    }
    if (count % 4 != 0) {
    	gpsd_report(LOG_WARN, "malformed GPGSV - fieldcount %d %% 4 != 0\n", count);
    	gpsd_zero_satellites(&session->gpsdata);
    	session->gpsdata.satellites_visible = 0;
    	return ERROR_SET;
    }

    session->driver.nmea.await = atoi(field[1]);
    if (sscanf(field[2], "%d", &session->driver.nmea.part) < 1) {
    	gpsd_zero_satellites(&session->gpsdata);
    	return ERROR_SET;
    } else if (session->driver.nmea.part == 1)
	    gpsd_zero_satellites(&session->gpsdata);

    for (fldnum = 4; fldnum < count; ) {
    	if (session->gpsdata.satellites_visible >= MAXCHANNELS) {
    	    gpsd_report(LOG_ERROR, "internal error - too many satellites!\n");
    	    gpsd_zero_satellites(&session->gpsdata);
    	    break;
    	}
    	session->gpsdata.PRN[session->gpsdata.satellites_visible]       = atoi(field[fldnum++]);
    	session->gpsdata.elevation[session->gpsdata.satellites_visible] = atoi(field[fldnum++]);
    	session->gpsdata.azimuth[session->gpsdata.satellites_visible]   = atoi(field[fldnum++]);
    	session->gpsdata.ss[session->gpsdata.satellites_visible]	= (float)atoi(field[fldnum++]);
    	/*
    	 * Incrementing this unconditionally falls afoul of chipsets like
    	 * the Motorola Oncore GT+ that emit empty fields at the end of the
    	 * last sentence in a GPGSV set if the number of satellites is not
    	 * a multiple of 4.
    	 */
    	if (session->gpsdata.PRN[session->gpsdata.satellites_visible] != 0)
    	    session->gpsdata.satellites_visible++;
    }

    if (session->driver.nmea.part == session->driver.nmea.await && atoi(field[3]) != session->gpsdata.satellites_visible)
    	gpsd_report(LOG_WARN, "GPGSV field 3 value of %d != actual count %d\n",
    		atoi(field[3]), session->gpsdata.satellites_visible);

    /* not valid data until we've seen a complete set of parts */
    if (session->driver.nmea.part < session->driver.nmea.await) {
        gpsd_report(LOG_PROG, "Partial satellite data (%d of %d).\n",
            session->driver.nmea.part, session->driver.nmea.await);
        return ERROR_SET;
    }

    /*
     * This sanity check catches an odd behavior of SiRFstarII receivers.
     * When they can't see any satellites at all (like, inside a
     * building) they sometimes cough up a hairball in the form of a
     * GSV packet with all the azimuth entries 0 (but nonzero
     * elevations).  This behavior was observed under SiRF firmware
     * revision 231.000.000_A2.
     */
    for (n = 0; n < session->gpsdata.satellites_visible; n++)
	    if (session->gpsdata.azimuth[n] != 0)
	        goto sane;
    gpsd_report(LOG_WARN, "Satellite data no good (%d of %d).\n",
		session->driver.nmea.part, session->driver.nmea.await);
    gpsd_zero_satellites(&session->gpsdata);
    return ERROR_SET;
  sane:
    session->gpsdata.skyview_time = GPS_NAN;
    gpsd_report(LOG_DATA, "GSV: Satellite data OK (%d of %d).\n",
		session->driver.nmea.part, session->driver.nmea.await);
    // FIXME: Dump satellite state at LOG_DATA level on final sentence
    return SATELLITE_SET;
}

static unsigned int processGPZDA(int c UNUSED, char *field[], struct gps_device_t *session)
/* Time & Date */
{
    /*
      $GPZDA,160012.71,11,03,2004,-1,00*7D
      1) UTC time (hours, minutes, seconds, may have fractional subsecond)
      2) Day, 01 to 31
      3) Month, 01 to 12
      4) Year (4 digits)
      5) Local zone description, 00 to +- 13 hours
      6) Local zone minutes description, apply same sign as local hours
      7) Checksum

      Note: some devices, like the uBlox ANTARIS 4h, are known to ship ZDAs
      with some fields blank under poorly-understood circumstances (probably
      when they don't have satellite lock yet).
     */
    unsigned int mask;

    if (field[1][0]=='\0' || field[2][0]=='\0' || field[3][0]=='\0' || field[4][0]=='\0') {
    	gpsd_report(LOG_WARN, "malformed ZDA\n");
    	mask = ERROR_SET;
    } else {
	    merge_hhmmss(field[1], session);
    	/*
    	 * We don't register fractional time here because want to leave
    	 * ZDA out of end-of-cycle detection. Some devices sensibly emit it only
    	 * when they have a fix, so watching for it can make them look
    	 * like they have a variable fix reporting cycle.
    	 */
    	session->driver.nmea.date.tm_year = atoi(field[4]) - 1900;
    	session->driver.nmea.date.tm_mon = atoi(field[3])-1;
    	session->driver.nmea.date.tm_mday = atoi(field[2]);
    	mask = TIME_SET;
    };
    gpsd_report(LOG_DATA, "ZDA: mask=%s\n",
	gpsd_maskdump(mask));
    return mask;
}

/**************************************************************************
 *
 * Entry points begin here
 *
 **************************************************************************/

/*@ -mayaliasunique @*/
unsigned int gps_parser(char *sentence, struct gps_device_t *session)
/* parse an NMEA sentence, unpack it into a session structure */
{
    typedef unsigned int (*nmea_decoder)(int count, char *f[], struct gps_device_t *session);
    static struct {
	char *name;
	int nf;			/* minimum number of fields required to parse */
	nmea_decoder decoder;
    } nmea_phrase[] = {
    	/*@ -nullassign @*/
    	{"PGRMC", 0,	NULL},		/* ignore Garmin Sensor Config */
    	{"PGRMI", 0,	NULL},		/* ignore Garmin Sensor Init */
    	{"PGRMO", 0,	NULL},		/* ignore Garmin Sentence Enable */
    	/*
    	 * Basic sentences must come after the PG* ones, otherwise
    	 * Garmins can get stuck in a loop that looks like this:
             *
    	 * 1. A Garmin GPS in NMEA mode is detected.
             *
    	 * 2. PGRMC is sent to reconfigure to Garmin binary mode.
             *    If successful, the GPS echoes the phrase.
             *
    	 * 3. nmea_parse() sees the echo as RMC because the talker ID is
             *    ignored, and fails to recognize the echo as PGRMC and ignore it.
             *
    	 * 4. The mode is changed back to NMEA, resulting in an infinite loop.
    	 */
    	{"GPRMC", 8,	processGPRMC},
    	{"GGA", 13,	processGPGGA},
    	{"GLL", 7, 	processGPGLL},
    	{"GSA", 17,	processGPGSA},
    	{"GSV", 0,	processGPGSV},
    	{"VTG", 0, 	NULL},		/* ignore Velocity Track made Good */
    	{"ZDA", 7, 	processGPZDA},
    	/*@ +nullassign @*/
    };

    int count;
    unsigned int retval = 0;
    unsigned int i, thistag;
    char *p, *s, *e;
    volatile char *t;

    /*
     * We've had reports that on the Garmin GPS-10 the device sometimes
     * (1:1000 or so) sends garbage packets that have a valid checksum
     * but are like 2 successive NMEA packets merged together in one
     * with some fields lost.  Usually these are much longer than the
     * legal limit for NMEA, so we can cope by just tossing out overlong
     * packets.  This may be a generic bug of all Garmin chipsets.
     */
    if (strlen(sentence) > NMEA_MAX) {
    	gpsd_report(LOG_WARN, "Overlong packet rejected.\n");
    	return ONLINE_SET;
    }

    /*@ -usedef @*//* splint 3.1.1 seems to have a bug here */
    /* make an editable copy of the sentence */
    strncpy((char *)session->driver.nmea.fieldcopy, sentence, NMEA_MAX);
    /* discard the checksum part */
    for (p = (char *)session->driver.nmea.fieldcopy; (*p!='*') && (*p >=' '); )
	    ++p;
    if (*p == '*')
	    *p++ = ',';	/* otherwise we drop the last field */
    *p = '\0';
    e = p;

    /* split sentence copy on commas, filling the field array */
    count = 0;
    t = p;  /* end of sentence */
    p = (char *)session->driver.nmea.fieldcopy + 1; /* beginning of tag, 'G' not '$' */
    /* while there is a search string and we haven't run off the buffer... */
    while((p != NULL) && (p <= t)) {
    	session->driver.nmea.field[count] = p; /* we have a field. record it */
    	/*@ -compdef @*/
    	if ((p = strchr(p, ',')) != NULL) { /* search for the next delimiter */
    	    *p = '\0'; /* replace it with a NUL */
    	    count++; /* bump the counters and continue */
    	    p++;
    	}
    	/*@ +compdef @*/
    }

    /* point remaining fields at empty string, just in case */
    for (i = (unsigned int)count;
	 i < (unsigned)(sizeof(session->driver.nmea.field)/sizeof(session->driver.nmea.field[0]));
	 i++)
	    session->driver.nmea.field[i] = e;

    /* sentences handlers will tell us whren they have fractional time */
    session->driver.nmea.latch_frac_time = 0;

    /* dispatch on field zero, the sentence tag */
    for (thistag = i = 0; i < (unsigned)(sizeof(nmea_phrase)/sizeof(nmea_phrase[0])); ++i) {
    	s = session->driver.nmea.field[0];
    	if (strlen(nmea_phrase[i].name) == 3)
    	    s += 2;	/* skip talker ID */
    	if (strcmp(nmea_phrase[i].name, s) == 0) {
    	    if (nmea_phrase[i].decoder!=NULL && (count >= nmea_phrase[i].nf)) {
        		retval = (nmea_phrase[i].decoder)(count, session->driver.nmea.field, session);
        		strncpy(session->gpsdata.tag, nmea_phrase[i].name, MAXTAGLEN);
        		/*
        		 * Must force this to be nz, as we're gong to rely on a zero
        		 * value to mean "no previous tag" later.
        		 */
        		thistag = i+1;
    	    } else
    		    retval = ONLINE_SET;		/* unknown sentence */
    	    break;
    	}
    }

    /* timestamp recording for fixes happens here */
    if ((retval & TIME_SET)!=0) {
    	session->gpsdata.fix.time = (double)mkgmtime(&session->driver.nmea.date)+session->driver.nmea.subseconds;
    	gpsd_report(LOG_DATA, "%s computed time is %2f = %s\n",
    		    session->driver.nmea.field[0],
    		    session->gpsdata.fix.time,
    		    asctime(&session->driver.nmea.date));
    }

    /*
     * The end-of-cycle detector.  This code depends on just one
     * assumption: if a sentence with a timestamp occurs just before
     * start of cycle, then it is always good to trigger a reort on
     * that sentence in the future.  For devices with a fixed cycle
     * this should work perfectly, locking in detection after one
     * cycle.  Most split-cycle devices (Garmin 48, for example) will
     * work fine.  Problems will only arise if a a sentence that
     * occurs just befiore timestamp increments also occurs in
     * mid-cycle, as in the Garmin eXplorist 210; those might jitter.
     */
    if (session->driver.nmea.latch_frac_time)
    {
    	gpsd_report(LOG_PROG,
    		    "%s reporting cycle started on %.2f.\n",
    		    session->driver.nmea.field[0], session->driver.nmea.this_frac_time);

        if (!GPS_TIME_EQUAL(session->driver.nmea.this_frac_time, session->driver.nmea.last_frac_time)) {
    	    unsigned int lasttag = session->driver.nmea.lasttag;
    	    retval |= CLEAR_SET;
    	    gpsd_report(LOG_PROG,
    			"%s starts a reporting cycle.\n",
    			session->driver.nmea.field[0]);
    	    /*
    	     * Have we seen a previously timestamped NMEA tag?
    	     * If so, designate as end-of-cycle marker.
    	     */
    	    if (lasttag > 0 && (session->driver.nmea.cycle_enders & (1 << lasttag))==0) {
    		session->driver.nmea.cycle_enders |= (1 << lasttag);
    		gpsd_report(LOG_PROG,
    			    "tagged %s as a cycle ender.\n",
    			    nmea_phrase[lasttag-1].name);
    	    }
	    }
    	/* here's where we check for end-of-cycle */
    	if (session->driver.nmea.cycle_enders & (1 << thistag)) {
    	    gpsd_report(LOG_PROG,
    			"%s ends a reporting cycle.\n",
    			session->driver.nmea.field[0]);
    	    retval |= REPORT_SET;
    	}
    	session->driver.nmea.lasttag = thistag;
    }

    /* we might have a reliable end-of-cycle */
    if (session->driver.nmea.cycle_enders != 0)
	    session->cycle_end_reliable = 1;

    return retval;
}
/*@ +mayaliasunique @*/
#endif /* NMEA_ENABLE */

