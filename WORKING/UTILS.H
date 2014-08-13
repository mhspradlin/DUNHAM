// utils.h

#include <time.h>

void GetRADec( long X, long Y, double *RA, double *Dec );
void GetXY( double RA, double Dec, long *X, long *Y );
void f_GetRADec( double f_X, double f_Y, double *RA, double *Dec );
void f_GetXY( double RA, double Dec, double *f_X, double *f_Y );

double LocalSiderealTime( void );
double UniversalTime( void );
double JulianDate( int year, int month, int day );
double Radians( double *degrees );
double Degrees( double *radians );
long round( double number );
