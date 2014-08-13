// string.c

// Routines for parsing strings in the Dunham program.
// Version 1.2

// Barbara Carter      October, 1995

#include <math.h>
#include <stdlib.h>
#include "globals.h"
#include "string.h"
#include "utils.h"

void GetSignFromString( int *s, int *sign )
// Searches the string "s" for the minus sign (-).  If one is found,
// the sign is assumed to be negative.  If none is found, positive.
// Notice that this is not completely foolproof.  A minus sign anywhere
// in the string before the first number is assumed to indicate a negative
// number.  The string "thus-hence 6.78" will yield -6.78.
{
   int letter, *p;

   *sign = 1;
   p = s;
   do  // skip to the first numeric character, looking for a minus sign. 
   {
      letter = *p++;
      if (letter=='-') *sign = -1;
   }  while ( (letter<'0' || letter>'9') && letter!=RETURN);
}

int GetCoordinateFromString( int *s, Coordinate *C )
// This routine calls on NextNumberFromString() up to three times, to
// get the numerical value of a coordinate such as "12:42:03.5".  Any/all
// of the three numbers (hours, minutes, seconds) may include a decimal 
// point, though this is not the usual notation.  For example, "12.5:30:00" 
// would be equivalent to "13:00:00".  Notice that the routine allows any 
// non-numerical ASCII character to delineate the hours, minutes, and
// seconds, not just the ":" character.  This is meant to permit greatest 
// flexibility for the user, who may find the space or other character(s) 
// easier to type.  For example, "12h 30m" is just as valid as "12 30", 
// "12h30", and "12:30".
// Returns 1 if there was an error, 0 if all went well.
{
   double remainder, EPSILON=ONE/3600.;
   int letter, err, temp_err, *p;
   static int tempy=2, tempx=2;

   C->hour=ZERO;
   C->minute=ZERO;
   C->second=ZERO;
   p = s;
   err = NextNumberFromString( &(C->hour), &p );
   if (err) return (err);      // if user hit return with no numbers, exit.
   temp_err = NextNumberFromString( &(C->minute), &p );
   if (temp_err) C->minute = ZERO;
   else 
   {
      temp_err = NextNumberFromString( &(C->second), &p );
   }
   if (temp_err) C->second = ZERO;
   C->second = truncate(C->second);  // truncate to whole seconds.
   remainder = (C->minute) - truncate(C->minute);
   if (remainder > EPSILON)    // convert fractions of minutes to seconds.
   {
      C->minute = truncate(C->minute);
      C->second += (remainder*SIXTY);
      C->second = truncate(C->second);
   }
   remainder = (C->hour) - truncate(C->hour);
   if (remainder > EPSILON)    // convert fractions of hours to minutes. 
   {
      C->hour = truncate(C->hour);
      C->minute += (remainder*SIXTY);
      remainder = (C->minute) - truncate(C->minute);
      if (remainder > EPSILON)    // convert fractions of minutes to seconds.
      {
         C->minute = truncate(C->minute);
         C->second += (remainder*SIXTY);
         C->second = truncate(C->second);
      }
   }
   while (C->second > SIXTY) 
   {
      C->second -= SIXTY;       // add any seconds over 60 to the
      C->minute += ONE;         // minutes.
   }
   while (C->minute > SIXTY) 
   {
      C->minute -= SIXTY;       // add any minutes over 60 to the
      C->hour += ONE;           // hours.
   }
   if (C->hour > 90.0) C->hour = 90.0;  // disallow hours to be more than 90.
   return err;
}

int NextNumberFromString( double *Num, int **s )
// Parses the string pointed to by s, and finds the next "number",
// which may be any sequence of the characters 0-9.  There may be a 
// decimal point in the "number".  This function returns a code that 
// indicates if a number was found, or if the end of the string was 
// encountered (a carriage-return).  The error code is 0 if all is well.
{
   int    letter, i=0, TooMany=200, HowMany=0;
   int    Okay=0, Escape=1;  // error codes: 0=>got a valid number, 1=>didn't
   double number=ZERO, decimal=ZERO;
   static int tempx=2, tempy=22;

   *Num = ZERO;
   do  // skip to the first numeric character 
   {
      letter = *(*s)++;
      HowMany++;
   }  while ((letter<'0' || letter>'9') 
                     && letter!=RETURN && letter!='\n' && HowMany<TooMany);
   if (letter==RETURN || letter==NULL || letter=='\n') 
      return( Escape );
   while ( letter>='0' && letter<='9' && HowMany<TooMany)   
   {                  // read in numbers until the next non-numeric character
      number *= 10; 
      number += (double)(letter+CHAROFFSET);
      letter = *(*s)++;
      HowMany++;
   } 
   *Num = number;
   if (letter!='.' || HowMany>=TooMany) 
      return ( Okay );
   letter = *(*s)++;
   HowMany++;
   if (letter<'0' || letter>'9' || HowMany>=TooMany) 
      return( Okay );
   do
   {
      decimal += (double)(letter+CHAROFFSET)/pow(10,++i);
      letter = *(*s)++;
      HowMany++;
   } while ( letter>='0' && letter<='9' && HowMany<TooMany );
   *Num = number + decimal;
   return( Okay );
}
