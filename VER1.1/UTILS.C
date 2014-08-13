// utils.c

// Utility routines used in the Dunham program.  Most are used for 
// converting between different coordinate systems, like (RA,Dec), 
// (X,Y), and (Alt,Az).  The (X,Y) coordinate system is in units of
// "steps" of the Dunham telescope's stepper motors and is intrinsic
// to this particular telescope.  The other two coordinate systems
// are universally used in astronomy.
// Also includes a rounding-off routine ("round") and a routine that
// checks if a particular location is below the horizon and thus
// cannot be seen ("OutOfBounds").

// Barbara Carter   August 9, 1994

#include "utils.h"
#include "globals.h"
#include <math.h>
#include <time.h>
#include <stdio.h>

#define Pi 3.1415926536
#define EPSILON 0.00001
#define STEPS_PER_DEGREE 16000

//****************** prototypes for local functions only *******************

void XYtoAltAz( long *X, long *Y, double *Alt, double *Az );
void AltAzToXY( double *Alt, double *Az, long *X, long *Y );
void f_XYtoAltAz( double *X, double *Y, double *Alt, double *Az );
void f_AltAzToXY( double *Alt, double *Az, double *X, double *Y );

void ConvertCoordinates( double *HA, double *Dec, double *Az, double *Alt );
double RightAscension( double *HA );
double HourAngle( double *RA );

//*************************** utility functions ****************************

void GetRADec( long X, long Y, double *RA, double *Dec )
// Calculates the RA,Dec coordinates of the given X,Y point.
{
   double HA, Alt, Az;
   long int tempX, tempY;

   tempX = X + Xoffset;
   tempY = Y + Yoffset;
   XYtoAltAz( &tempX, &tempY, &Alt, &Az );
   ConvertCoordinates( &Az, &Alt, &HA, Dec );
   HA = HA/FIFTEEN;
   *RA = RightAscension( &HA );
}


void GetXY( double RA, double Dec, long *X, long *Y )
// Calculates the X,Y coordinates of the given RA,Dec.
{
   double HA, Alt, Az;

   HA = HourAngle( &RA ) * FIFTEEN;
   ConvertCoordinates( &HA, &Dec, &Az, &Alt );
   AltAzToXY( &Alt, &Az, X, Y);
   *X -= Xoffset;
   *Y -= Yoffset;
}


void f_GetRADec( double f_X, double f_Y, double *RA, double *Dec )
// Calculates the RA,Dec coordinates of the given (X,Y) point.
// f_X and f_Y are (X,Y) in double-precision floating-point.
{
   double HA, Alt, Az;
   double f_tempX, f_tempY;

   f_tempX = f_X + (double)Xoffset;
   f_tempY = f_Y + (double)Yoffset;
   f_XYtoAltAz( &f_tempX, &f_tempY, &Alt, &Az );
   ConvertCoordinates( &Az, &Alt, &HA, Dec );
   HA = HA/FIFTEEN;
   *RA = RightAscension( &HA );
}


void f_GetXY( double RA, double Dec, double *f_X, double *f_Y )
// Calculates the floating-point X,Y coordinates of the given RA,Dec.
{
   double HA, Alt, Az;

   HA = HourAngle( &RA ) * FIFTEEN;
   ConvertCoordinates( &HA, &Dec, &Az, &Alt );
   f_AltAzToXY( &Alt, &Az, f_X, f_Y);
   *f_X -= (double)Xoffset;
   *f_Y -= (double)Yoffset;
}


void XYtoAltAz( long *X, long *Y, double *Alt, double *Az )
{
   double sineAlt, cosAlt, Xangle, Yangle, temp, arg;

   temp = (double)( *X / (double)STEPS_PER_DEGREE );
   Xangle = Radians( &temp );
   temp = (double)( *Y / (double)STEPS_PER_DEGREE );
   Yangle = Radians( &temp );
   sineAlt = cos( Xangle ) * cos( Yangle );
   if (sineAlt>1) 
      sineAlt = ONE;
   else if (sineAlt<(-1)) 
      sineAlt = -ONE;
   *Alt = asin( sineAlt );
   cosAlt = cos( *Alt );
   if (cosAlt > 0.00001)
   {
      arg = cos(Xangle) * sin(-Yangle) / cosAlt;
      if (arg > 1) 
         arg = ONE;
      else if (arg < -1) 
         arg = -ONE;
      *Az = asin( arg );
   }
   else
   {
      *Az = ZERO;
   }
   *Alt = Degrees( Alt );
   *Az = Degrees( Az );
   if (*X >= 0)
   {
      while ((*Az) < 0.0)   *Az += 360.0;
      while ((*Az) > 360.0) *Az -= 360.0;
   }
   else
      *Az = (double)180.0 - *Az;
}


void AltAzToXY( double *Alt, double *Az, long *X, long *Y )
{
   double tanY, cosY, Xangle, Yangle, tanAlt, arg;

   tanAlt = tan( Radians(Alt) );
   if (tanAlt > EPSILON)
   {
      tanY = sin( -Radians(Az) ) / tanAlt;
      Yangle = atan( tanY );
      Yangle = Degrees( &Yangle );
   }
   else
   {
      if ( (0.0<=(*Az)) && ((*Az)<180.0)) Yangle = (double)(180.0);
      else                                Yangle = (double)(-180.0);
   }
   *Y = round( Yangle * (double)STEPS_PER_DEGREE );
   cosY = cos( Radians(&Yangle) );
   if (cosY > EPSILON)
   {
      arg = sin( Radians(Alt) ) / cosY;
      if (arg>1) 
         arg=(double)1;
      else 
         if (arg<-1) arg=(double)(-1);
      Xangle = acos( arg );
      Xangle = Degrees( &Xangle );
   }
   else
   {
      if ( *Az >= 180.0 ) Xangle = (*Az) - (double)270.0;
      else                Xangle = (double)270.0 - (*Az);
   }
   *X = round( Xangle * (double)STEPS_PER_DEGREE );
   if ( (90.0 < (*Az)) && ((*Az) < 270.0) )
      *X = -(*X);
}


void f_XYtoAltAz( double *X, double *Y, double *Alt, double *Az )
{
   double sineAlt, cosAlt, Xangle, Yangle, temp, arg;

   temp = *X / (double)STEPS_PER_DEGREE;
   Xangle = Radians( &temp );
   temp = *Y / (double)STEPS_PER_DEGREE;
   Yangle = Radians( &temp );
   sineAlt = cos( Xangle ) * cos( Yangle );
   *Alt = asin( sineAlt );
   cosAlt = cos( *Alt );
   if (cosAlt > 0.00001)
   {
      arg = cos(Xangle) * sin(-Yangle) / cosAlt;
      if (arg > 1) 
         arg = (double)1;
      else if (arg < -1) 
         arg = (double)(-1);
      *Az = asin( arg );
   }
   else
      *Az = (double)0.0;
   *Alt = Degrees( Alt );
   *Az = Degrees( Az );
   if (*X >= 0)
   {
      while ((*Az) < 0.0)   *Az += 360.0;
      while ((*Az) > 360.0) *Az -= 360.0;
   }
   else
      *Az = (double)180.0 - *Az;
}


void f_AltAzToXY( double *Alt, double *Az, double *X, double *Y )
{
   double tanY, cosY, Xangle, Yangle, tanAlt, arg;

   tanAlt = tan( Radians(Alt) );
   if (tanAlt > EPSILON)
   {
      tanY = sin( -Radians(Az) ) / tanAlt;
      Yangle = atan( tanY );
      Yangle = Degrees( &Yangle );
   }
   else
   {
      if ( (0.0<=(*Az)) && ((*Az)<180.0)) Yangle = (double)(180.0);
      else                                Yangle = (double)(-180.0);
   }
   *Y = Yangle * (double)STEPS_PER_DEGREE;
   cosY = cos( Radians(&Yangle) );
   if (cosY > EPSILON)
   {
      arg = sin( Radians(Alt) ) / cosY;
      if (arg > 1) 
         arg = (double)1;
      else if (arg < -1) 
         arg = (double)(-1);
      Xangle = acos( arg );
      Xangle = Degrees( &Xangle );
   }
   else
   {
      if ( *Az >= 180.0 ) Xangle = (*Az) - (double)270.0;
      else                Xangle = (double)270.0 - (*Az);
   }
   *X = Xangle * (double)STEPS_PER_DEGREE;
   if ( (90.0 < (*Az)) && ((*Az) < 270.0) )
      *X = -(*X);
}


void ConvertCoordinates( double *HA, double *Dec, double *Az, double *Alt )
/* Converts from the given coordinates (the first two) to the second two. */
/* This works as shown, with (HA,Dec,Az,Alt), but it also works the other */
/* way around, for (Az,Alt,HA,Dec).  ALL COORDINATES ARE IN DEGREES...    */
/* this means you have to convert the usual HourAngle (hours) units       */
/* to degrees before calling this routine.                                */
{
   double sineLat, cosLat, sineDec, cosDec, lat=LATITUDE;
   double sineHA, cosHA, sineAlt, cosAlt, arg;

   lat = Radians(&lat);
   sineLat = sin( lat );         cosLat = cos( lat );
   sineDec = sin(Radians(Dec));  cosDec = cos(Radians(Dec));
   sineHA = sin(Radians(HA));    cosHA = cos(Radians(HA));
   sineAlt = ( sineDec * sineLat ) + ( cosDec * cosLat * cosHA );
   *Alt = asin( sineAlt );
   cosAlt = cos( *Alt );
   arg = (sineDec-sineLat*sineAlt) / (cosLat * cosAlt);
   if (arg>1) 
      arg=(double)1;
   else 
      if (arg<-1) arg=(double)(-1);
   *Az = acos( arg );
   *Alt = Degrees( Alt );
   *Az = Degrees( Az );
   if (sineHA > 0.0)  *Az = (double)360.0-(*Az);
}


double RightAscension( double *HA )
{
   double RA;

   RA = LocalSiderealTime() - (*HA);
   while (RA > 24.0) RA -= 24.0;
   while (RA < 0.0)  RA += 24.0;
   return RA;
}


double HourAngle( double *RA )
{
   double HA;

   HA = LocalSiderealTime() - *RA;
   while (HA > 24.0) HA -= 24.0;
   while (HA < 0.0)  HA += 24.0;
   return HA;
}


double LocalSiderealTime( void )
{
   double GST, LST;

   GST = T0 + (UniversalTime() * 1.002737909);
   LST = GST - (LONGITUDE/15.0);
   while (LST > 24.0) LST -= 24.0;
   while (LST < 0.0)  LST += 24.0;
   return( LST );
}

double UniversalTime( void )
{
   return ( StartingUT + TRACKTKTIME*(double)TOD/((double)3600) );
}


double JulianDate( int year, int month, int day )
/* This returns the Julian Date of the date given, for midnight (hour 0).*/
/* To get the JD of Feb 17th at 6am, "day" would have to be 17.25. */
{
   long int A, B=0L, C, D;

   if ( month<=2 )
   {
      year--;
      month += 12;
   }
   if ( (year>1582) || (year==1582 && month>10) || (year==1582 && month==10 && day>15) )
   {
      A = (long int)(year/100.0);
      B = 2 - A + (A/4);
   }
   if ( year < 0 )
      C = (long)(((float)year*365.25)-0.75);
   else
      C = (long)((float)year*365.25);
   D = (long)(30.6001*(month+1));
   return (double)(B+C+D+day+1720994.5);
}


double Radians( double *degrees )
{
   return (*degrees)*Pi/(double)180.0;     /* converts degrees to radians */
}


double Degrees( double *radians )
{
   return (*radians)*(double)180.0/Pi;     /* converts radians to degrees */
}


long round( double number )
// Returns the rounded (long) value of the number.  
// Handles negative numbers as well as positive.
{
   int remainder = (number - (long)number) * 10;

   return ( number + (remainder>4) - (remainder<(-4)) );
}


double truncate( double number )
// Returns the truncated (double) value of the number, that is,  
// the "rounded-down" (towards zero) value, but not in integer form.
// Handles negative numbers as well as positive.
{
   return ( (double)(long)number );
}


int OutOfBounds( long x, long y )
// Checks if the coordinates (x,y) (in Dunham stepper motor "steps")
// are "too far over" or too close to the horizon.  Or if it's too
// close to running into the telescope's support structure.
{
   int farout=0;

   if (x > TOO_FAR_NORTH ) farout=1;
   else
   {
      if ((pow(x,2)+pow(y,2)) > pow(MAXZENITHANGLE*STEPSPERDEGREE,2) ) 
         farout=1;
   }
   return( farout );
}

