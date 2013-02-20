/***************************************************************************
			sun.cpp  -  Sun Rise and Set Calculations
                             -------------------
    begin                : Friday July 11 2003
    copyright            : (C) 2003 by John Ratke
    email                : jratke@comcast.net

    history:
        Written as DAYLEN.C, 1989-08-16
        Modified to SUNRISET.C, 1992-12-01
        (c) Paul Schlyter, 1989, 1992
        Released to the public domain by Paul Schlyter, December 1992
        Portions Modified to SUNDOWN.NLM by Cliff Haas 98-05-22
	Converted to C++ and modified by John Ratke

***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <kdebug.h>

#include <math.h>
#include "sun.h"

/* A function to compute the number of days elapsed since 2000 Jan 0.0 */
/* (which is equal to 1999 Dec 31, 0h UT)                           */

static inline double days_since_2000_Jan_0(int y, int m, int d)
{
	return (367L*(y)-((7*((y)+(((m)+9)/12)))/4)+((275*(m))/9)+(d)-730530L);
}

/* Some conversion factors between radians and degrees */

static const double PI = 3.14159265358979323846;

static const double RADEG  = ( 180.0 / PI );
static const double DEGRAD = ( PI / 180.0 );

/* The trigonometric functions in degrees */
static inline double sind(double x) { return sin( x * DEGRAD ); }
static inline double cosd(double x) { return cos( x * DEGRAD ); }
static inline double tand(double x) { return tan( x * DEGRAD ); }
static inline double atand(double x) { return RADEG * atan(x); }
static inline double asind(double x) { return RADEG * asin(x); }
static inline double acosd(double x) { return RADEG * acos(x); }
static inline double atan2d(double y, double x) { return RADEG * atan2(y, x); }

/* Other local functions */
static double latitudeToDouble( const QString &latitude );
static double longitudeToDouble( const QString &longitude );
static int __sunriset__( int year, int month, int day, double lon, double lat,
			 double altit, int upper_limb, double &trise, double &tset );
static void sunpos( double d, double &lon, double &r );
static void sun_RA_dec( double d, double &RA, double &dec, double &r );
static inline double revolution( const double x );
static inline double rev180( const double x );
static inline double GMST0( const double d );


/*
 * This function computes times for sunrise/sunset.
 * Sunrise/set is considered to occur when the Sun's upper limb is 
 * 35 arc minutes below the horizon (this accounts for the refraction
 * of the Earth's atmosphere).
 */
static inline int sun_rise_set(int year, int month, int day, double lon, double lat, double &rise, double &set)
{
	return __sunriset__( year, month, day, lon, lat, -35.0/60.0, 1, rise, set );
}

/*
 * This function computes the start and end times of civil twilight.
 * Civil twilight starts/ends when the Sun's center is 6 degrees below
 * the horizon. 
 */
static inline int civil_twilight(int year, int month, int day, double lon, double lat, double &start, double &end)
{
	return __sunriset__( year, month, day, lon, lat, -6.0, 0, start, end );
}


Sun::Sun(const QString &latitude, const QString &longitude, QDate date, const int localUTCOffset) :
	m_date(date), 
	m_lat(latitudeToDouble(latitude)), m_lon(longitudeToDouble(longitude)),
	m_localUTCOffset(localUTCOffset)
{
}


QTime Sun::computeRiseTime()
{
	double riseTime;
	double setTime;

	sun_rise_set( m_date.year(), m_date.month(), m_date.day(), m_lon, m_lat, riseTime, setTime );

	QTime  result = convertDoubleToLocalTime( riseTime );

	if ( ! result.isValid() )
		result.setHMS( 6, 0, 0 );
	
	return result;
}


QTime Sun::computeSetTime()
{
	double riseTime;
	double setTime;

	sun_rise_set( m_date.year(), m_date.month(), m_date.day(), m_lon, m_lat, riseTime, setTime );

	QTime  result = convertDoubleToLocalTime( setTime );

	if ( ! result.isValid() )
		result.setHMS( 19, 0, 0 );

	return result;
}


QTime Sun::computeCivilTwilightStart()
{
	double start;
	double end;

	civil_twilight( m_date.year(), m_date.month(), m_date.day(), m_lon, m_lat, start, end );

	QTime  result = convertDoubleToLocalTime( start );

	if ( ! result.isValid() )
		result.setHMS( 6, 0, 0 );
	
	return result;
}


QTime Sun::computeCivilTwilightEnd()
{
	double start;
	double end;

	civil_twilight( m_date.year(), m_date.month(), m_date.day(), m_lon, m_lat, start, end );

	QTime  result = convertDoubleToLocalTime( end );

	if ( ! result.isValid() )
		result.setHMS( 19, 0, 0 );

	return result;
}


/*
 * Converts latitude in format DD-MMH, where DD is degrees, MM is minutes, 
 * and H is Hemisphere (N for North, or S for South) to a floating point number.
 *
 * For example: 27-00S to -27.0
 *
 * Does not currently handle seconds.
 */
static double latitudeToDouble( const QString &latitude )
{
	double result;
	
	double dd = latitude.left(2).toDouble();
	double mm = latitude.mid(3, 2).toDouble();
	
	result = dd + (mm / 60);

	if (latitude.contains("S"))
		result *= -1;

	return result;
}


static double longitudeToDouble( const QString &longitude )
{
	double result;
	
	double ddd = longitude.left(3).toDouble();
	double mm  = longitude.mid(4, 2).toDouble();
	
	result = ddd + (mm / 60);

	if (longitude.contains("W"))
		result *= -1;

	return result;
}


QTime Sun::convertDoubleToLocalTime( const double time )
{
	QTime result;
	
	// Example:  say time is 17.7543  Then hours = 17 and minutes = 0.7543 * 60 = 45.258
        // We need to convert the time to CORRECT local hours
	int hours   = (int)floor(time);
	int localhours = hours + (m_localUTCOffset / 60);

	// We need to convert the time to CORRECT local minutes
	int minutes = (int)floor((time - hours) * 60);
	int localminutes = minutes + (m_localUTCOffset % 60);

	// We now have to adjust the time to be within the 60m boundary
	if (localminutes < 0)
	{
		//As minutes is less than 0, we need to
		//reduce a hour and add 60m to minutes.
		localminutes += 60;
		localhours--;
	}
	if (localminutes >= 60)
  {
		//As minutes are more than 60, we need to
		//add one more hour and reduce the minutes to
		//a value between 0 and 59.
		localminutes -= 60;
		localhours++;
	}

	// Round up or down to nearest second.
	// Use rint instead of nearbyint because rint is in FreeBSD
	int seconds = (int)rint( fabs( minutes - ((time - hours) * 60) ) * 60 );

	// We now have to adjust the time to be within the 24h boundary
	if (localhours < 0) { localhours += 24; }
	if (localhours >= 24) { localhours -= 24; }

	// Try to set the hours, minutes and seconds for the local time.
	// If this doesn't work, then we will return the invalid time.
	result.setHMS( localhours, localminutes, seconds );

	return result;	
}


/*
 * Note: year,month,date = calendar date, 1801-2099 only.
 *       Eastern longitude positive, Western longitude negative
 *       Northern latitude positive, Southern latitude negative
 *       The longitude value IS critical in this function!
 *       altit = the altitude which the Sun should cross
 *               Set to -35/60 degrees for rise/set, -6 degrees
 *               for civil, -12 degrees for nautical and -18
 *               degrees for astronomical twilight.
 *       upper_limb: non-zero -> upper limb, zero -> center
 *               Set to non-zero (e.g. 1) when computing rise/set
 *               times, and to zero when computing start/end of
 *               twilight.
 *       trise = the rise time gets stored here
 *       tset  = the set  time gets stored here
 *               Both times are relative to the specified altitude,
 *               and thus this function can be used to comupte
 *               various twilight times, as well as rise/set times
 *
 * Return value:  0 = sun rises/sets this day, times stored in
 *                    trise and tset.
 *               +1 = sun above the specified "horizon" 24 hours.
 *                    trise set to time when the sun is at south,
 *                    minus 12 hours while tset is set to the south
 *                    time plus 12 hours. "Day" length = 24 hours
 *               -1 = sun is below the specified "horizon" 24 hours
 *                    "Day" length = 0 hours, trise and tset are
 *                    both set to the time when the sun is at south.
 *
 */
static int __sunriset__( int year, int month, int day, double lon, double lat,
			 double altit, int upper_limb, double &trise, double &tset )
{
      double  d;          /* Days since 2000 Jan 0.0 (negative before) */
      double  sr;         /* Solar distance, astronomical units */
      double  sRA;        /* Sun's Right Ascension */
      double  sdec;       /* Sun's declination */
      double  sradius;    /* Sun's apparent radius */
      double  t;          /* Diurnal arc */
      double  tsouth;     /* Time when Sun is at south */
      double  sidtime;    /* Local sidereal time */

      int     rc = 0;     /* Return code from function - usually 0 */

      /* Compute d of 12h local mean solar time */
      d = days_since_2000_Jan_0(year, month, day);

      d = days_since_2000_Jan_0(year, month, day) + 0.5 - lon / 360.0;

      /* Compute local sideral time of this moment */
      sidtime = revolution( GMST0(d) + 180.0 + lon );

      /* Compute Sun's RA + Decl at this moment */
      sun_RA_dec( d, sRA, sdec, sr );

      /* Compute time when Sun is at south - in hours UT */
      tsouth = 12.0 - rev180(sidtime - sRA) / 15.0;

      /* Compute the Sun's apparent radius, degrees */
      sradius = 0.2666 / sr;

      /* Do correction to upper limb, if necessary */
      if ( upper_limb )
            altit -= sradius;

      /* Compute the diurnal arc that the Sun traverses to reach */
      /* the specified altitide altit: */
      double cost;
      cost = ( sind(altit) - sind(lat) * sind(sdec) ) /
              ( cosd(lat) * cosd(sdec) );
      if ( cost >= 1.0 )
      {
            rc = -1;
	    t = 0.0;       /* Sun always below altit */
      }
      else if ( cost <= -1.0 )
      {
            rc = +1;
	    t = 12.0;      /* Sun always above altit */
      }
      else
            t = acosd(cost) / 15.0; /* The diurnal arc, hours */

      /* Store rise and set times - in hours UT */
      trise = tsouth - t;
      tset  = tsouth + t;

      return rc;
}


/* This function computes the Sun's position at any instant
 *
 * Computes the Sun's ecliptic longitude and distance
 * at an instant given in d, number of days since
 * 2000 Jan 0.0.  The Sun's ecliptic latitude is not
 * computed, since it's always very near 0.
 */
static void sunpos( double d, double &lon, double &r )
{
      double M;         /* Mean anomaly of the Sun */
      double w;         /* Mean longitude of perihelion */
                        /* Note: Sun's mean longitude = M + w */
      double e;         /* Eccentricity of Earth's orbit */
      double E;         /* Eccentric anomaly */
      double x;
      double y;         /* x, y coordinates in orbit */
      double v;         /* True anomaly */

      /* Compute mean elements */
      M = revolution( 356.0470 + 0.9856002585 * d );
      w = 282.9404 + 4.70935E-5 * d;
      e = 0.016709 - 1.151E-9 * d;

      /* Compute true longitude and radius vector */
      E = M + e * RADEG * sind(M) * ( 1.0 + e * cosd(M) );
      x = cosd(E) - e;
      y = sqrt( 1.0 - e*e ) * sind(E);
      r = sqrt( x*x + y*y );               /* Solar distance */
      v = atan2d( y, x );                  /* True anomaly */
      lon = v + w;                         /* True solar longitude */
      
      if ( lon >= 360.0 )
            lon -= 360.0;                 /* Make it 0..360 degrees */
}


static void sun_RA_dec( double d, double &RA, double &dec, double &r )
{
      double lon;
      double obl_ecl;
      double x;
      double y;
      double z;

      /* Compute Sun's ecliptical coordinates */
      sunpos( d, lon, r );

      /* Compute ecliptic rectangular coordinates (z=0) */
      x = r * cosd(lon);
      y = r * sind(lon);

      /* Compute obliquity of ecliptic (inclination of Earth's axis) */
      obl_ecl = 23.4393 - 3.563E-7 * d;

      /* Convert to equatorial rectangular coordinates - x is unchanged */
      z = y * sind(obl_ecl);
      y = y * cosd(obl_ecl);

      /* Convert to spherical coordinates */
      RA = atan2d( y, x );
      dec = atan2d( z, sqrt(x*x + y*y) );
}


static const double INV360 = 1.0 / 360.0;

/*
 * This function reduces any angle to within the first revolution
 * by subtracting or adding even multiples of 360.0 until the
 * result is >= 0.0 and < 360.0
 */
static inline double revolution( const double x )
{
      return ( x - 360.0 * floor( x * INV360 ) );
}

/*
 * Reduce angle to within +180..+180 degrees
 */
static inline double rev180( const double x )
{
      return ( x - 360.0 * floor( x * INV360 + 0.5 ) );
}


/*
 * This function computes GMST0, the Greenwhich Mean Sidereal Time
 * at 0h UT (i.e. the sidereal time at the Greenwhich meridian at
 * 0h UT).  GMST is then the sidereal time at Greenwich at any
 * time of the day.  I've generelized GMST0 as well, and define it
 * as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at
 * other times than 0h UT as well.  While this sounds somewhat
 * contradictory, it is very practical:  instead of computing
 * GMST like:
 *
 *  GMST = (GMST0) + UT * (366.2422/365.2422)
 *
 * where (GMST0) is the GMST last time UT was 0 hours, one simply
 * computes:
 *
 *  GMST = GMST0 + UT
 *
 * where GMST0 is the GMST "at 0h UT" but at the current moment!
 * Defined in this way, GMST0 will increase with about 4 min a
 * day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)
 * is equal to the Sun's mean longitude plus/minus 180 degrees!
 * (if we neglect aberration, which amounts to 20 seconds of arc
 * or 1.33 seconds of time)
 *
 */
static inline double GMST0( const double d )
{
      double sidtim0;

      /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
      /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
      /* add these numbers, I'll let the C compiler do it for me.  */
      /* Any decent C compiler will add the constants at compile   */
      /* time, imposing no runtime or code overhead.               */
      sidtim0 = revolution( ( 180.0 + 356.0470 + 282.9404 ) +
                          ( 0.9856002585 + 4.70935E-5 ) * d );
      return sidtim0;
}

