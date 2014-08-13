// slew.c

// Routines for slewing the Dunham telescope.

// Barbara Carter    October 20, 1994

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "slew.h"
#include "globals.h"
#include "utils.h"
#include "tracking.h"

//*********************** local function prototypes ************************

int GetCoordinateFromString( int *s, Coordinate *C );

//***************************** slew functions *****************************

void Slew( int y_display )
// Using the global variables dX and dY as the distances to go in X and Y,
// this routine sends out pulses to the motors to move the telescope the
// required distance.  dX and dY are in "steps" of the motors.  This routine
// sends a stream of pulses to the motors, ramping them up to the cruising 
// speed, and ramping them down to zero when finished.  It also checks if the 
// flag indicating the user wants to abort the slew has been set, and if so
// ramps down the motors to zero immediately.  Although it slows down the
// process a little, this routine also continually updates the coordinates
// on the screen so the user can see how far along the slew is.
{
   long int flag=1, outword;
   long int L2=LEVELS*2, xskip[LEVELS*2+1], yskip[LEVELS*2+1]; 
   long int x_skip, y_skip;
   long int xsend[LEVELS*2+1], ysend[LEVELS*2+1], i, j, total, i2; 
   long int x_cruise_index, y_cruise_index, X, Y, X2, Y2, common;
   long int xtemp=0L, ytemp=0L;
   int quit=0, Xout, Yout;
   int xloop=5, yloop=5;

   x_sign = _SIGN(dX);        
   y_sign = _SIGN(dY);        
   cumulative[0] = MinSteps;
   for (i=0; i<LEVELS; i++)
   {
      common = MinSteps + i*(MaxSteps-MinSteps)/LEVELS;   // ramp up from 
      xsend[i] = common; xsend[L2-i] = common;            // MinSteps and  
      ysend[i] = common; ysend[L2-i] = common;            // MaxDelay...
      common = MaxDelay - i*(MaxDelay-MinDelay)/LEVELS;   
      xskip[i] = common; xskip[L2-i] = common;           
      yskip[i] = common; yskip[L2-i] = common;            // to MaxSteps
      if (i) cumulative[i] = cumulative[i-1] + xsend[i];  // and MinDelay.
   }
   
   xsend[LEVELS] = 0;
   ysend[LEVELS] = 0;
   cumulative[LEVELS] = cumulative[LEVELS-1];
   xskip[LEVELS] = MinDelay;
   yskip[LEVELS] = MinDelay;

// for (i=0;i<=L2;i++) printf(" xskip[%ld]=%ld",i,xskip[i]);

   total=0; 
   i=0; 
   X2 = labs(dX)/2;
   while ((total+xsend[i])<X2 && i<LEVELS)
      total += xsend[i++];
   x_cruise_index = i; 
// gotoxy(2,2); printf("x_cruise_index = %ld", x_cruise_index);
   i2 = 2*i;
   for (i=x_cruise_index+1; i<=i2; i++)
   {
      xskip[i] = xskip[i2-i];
      xsend[i] = xsend[i2-i];
      total += xsend[i];
   }
   if (total < labs(dX)) xsend[x_cruise_index] = labs(dX) - total;
   else xsend[x_cruise_index] = 0;
// gotoxy(2,3); printf("xsend[%ld] = %ld, xskip[%ld] = %ld", x_cruise_index,
// xsend[x_cruise_index], x_cruise_index, xskip[x_cruise_index] );

   total=0; 
   i=0; 
   Y2 = labs(dY)/2;
   while ((total+ysend[i])<Y2 && i<LEVELS)
      total += ysend[i++];
   y_cruise_index = i; 
   i2 = 2*i;
   for (i=y_cruise_index+1; i<=i2; i++)
   {
      yskip[i] = yskip[i2-i];
      ysend[i] = ysend[i2-i];
      total += ysend[i];
   }
   if (total < labs(dY)) ysend[y_cruise_index] = labs(dY) - total;
   else ysend[y_cruise_index] = 0;

   i=0; j=0;
   x_skip = xskip[0];
   y_skip = yskip[0];
   gotoxy(37,y_display);   printf("%+9ld ", gX); 
   if (!dX) printf("            ");
   gotoxy(37,y_display+1); printf("%+9ld ", gY); 
   if (!dY) printf("            ");
   while (flag)
   {
      Xout=0; Yout=0;
      if (dX)
      {
         if ((--x_skip) == 0)
         {
            X = x_sign;
            if (X<0) Xout = XMINUS;
            else Xout = X*XPLUS;
            dX -= X;
//            xtemp++;
            if ((--xsend[i]) == 0)
               ++i;
            x_skip = xskip[i];
         }
         else X = 0;
      }
      else X = 0;
      if (dY)
      {
         if ((--y_skip) == 0)
         {
            Y = y_sign;
            if (Y<0) Yout = YMINUS;
            else Yout = Y*YPLUS;
            dY -= Y;
//            ytemp++;
            if ((--ysend[j]) == 0)
               ++j;
            y_skip = yskip[j];
         }
         else Y = 0;
      }
      else Y = 0;
      outword = NOPULSE - Xout - Yout;
      outp( PULSEPORT, outword );
      gX += X;
      gY += Y;
      if (Xout || Yout)
      {
         if (Xout)
         {
            if (++xloop>2)
            {
               xloop=0;
               gotoxy(38,y_display);   
               printf("%+9ld ", gX); 
// printf("   i=%3ld, xsend[i]=%ld, xskip[i]=%ld   ", i, xsend[i], xskip[i]);
               if (!dX) printf("            ");
            }
         }
         if (Yout)
         {
            if (++yloop>2)
            {
               yloop=0;
               gotoxy(38,y_display+1); 
               printf("%+9ld ", gY); 
               if (!dY) printf("            ");
            }
         }
      }
      if (!(dX || dY)) flag=0;   // dX=0 and dY=0 => done slewing
      if (abort_slew_flag)
      {
         quit = 1;
         if (i<=x_cruise_index) 
         {
            if (i==x_cruise_index) 
               dX = x_sign * cumulative[i++];
            else
               { dX = x_sign * cumulative[i];  i = 2*x_cruise_index - i; }
         }
         if (j<=y_cruise_index) 
         {
            if (j==y_cruise_index)
               dY = y_sign * cumulative[j++];
            else
               { dY = y_sign * cumulative[j];  j = 2*y_cruise_index - j; }
         }
         abort_slew_flag = 0;
         gotoxy(46,y_display); 
         if (dX) printf(" (ramping down)"); 
         else    printf("               ");
         gotoxy(46,y_display+1); 
         if (dY) printf(" (ramping down)");
         else    printf("               ");
      }
      outp( PULSEPORT, NOPULSE );
   }  // while (flag)
   if (quit)  // if the user requested the slew be aborted, we'll take up
   {          // tracking on wherever we landed out of the slew.
      GetRADec( gX, gY, &RA_Target, &DecTarget );
   }
}

void DoRA_DecSlewMenu( int *menu )
{
   int      escape=0, acceptable=0, s[255], *p;
   double   oldRA, oldDec, Alt, Az, HA;
   long int X_Target, Y_Target;
   int      letter, firstxx, xx, yy, x=25; 
   int      w=TRACKINGDISPLAY+5, y=w+4+2*ShowXY, i, sign;
   double   temp, hr, min, sec;

   *menu = TrackingMenu;              
   while (!escape && !acceptable)
   {
      for (i=0;i<255;i++) s[i]=NULL;
      gotoxy(x,y); 
      for (i=y; i<25; i++) EraseLine(i);
      FrameScreen();
      gotoxy(x,y+0); printf("Enter coordinates to slew to...");
      gotoxy(x,y+1); printf("     (return to escape)");
      gotoxy(x,y+2); printf(" Enter RA: ");
      firstxx=wherex(); xx=firstxx; yy=wherey();
      p = s;
      do 
      {                                      // Continuously update coord's
         TrackingKeyboard(w,xx,yy,&letter);  // (runs until a key is hit).
         if (letter==BACKSPACE)              // If that key was backspace,
         {
            if (xx > firstxx)
            {
               *(--p) = NULL;                // delete last entry from string
               gotoxy(--xx,yy); 
               printf(" ");                  // and erase screen character.
            }
         }
         else                                // Otherwise,
         {
            *p++ = letter;                   // add that key to the string
            xx++;                            // and advance the screen cursor
         }
      } while (letter!=RETURN);
      p = s;
      RA.hour=ZERO; RA.minute=ZERO; RA.second=ZERO;
      escape = GetCoordinateFromString(s, &RA);
      if (escape) return;

      temp = RA.hour + (RA.minute + RA.second/SIXTY)/SIXTY;
      hr = _HOURS(temp);
      min = _MINUTES(temp);
      sec = _SECONDS(temp);

      for (i=0;i<255;i++) s[i]=NULL;
      gotoxy(x,y); 
      for (i=y; i<23; i++) EraseLine(i);
      gotoxy(x,y);   printf("Enter coordinates to slew to...");
      gotoxy(x,y+1); printf("     (return to escape)");
      gotoxy(x,y+2); printf("       RA: %2.0f:%02.0f:%02.0f", hr, min, sec);
      gotoxy(x,y+3); printf("Enter Dec: ");
      xx=wherex(); firstxx=xx; yy=wherey();
      p = s;
      do 
      {                                      // Continuously update coord's 
         TrackingKeyboard(w,xx,yy,&letter);  // (runs until a key is hit). 
         if (letter==BACKSPACE)              // If that key was backspace,
         {
            if (xx > firstxx)
            {
               *(--p) = NULL;                // delete last entry from string
               gotoxy(--xx,yy); printf(" "); // and erase screen character.
            }
         }
         else                                // Otherwise,
         {
            *p++ = letter;                   // add that key to the string
            ++xx;                            // and advance the screen cursor
         }
      } 
      while (letter!=RETURN);
      p = s;
      Dec.hour=ZERO; Dec.minute=ZERO; Dec.second=ZERO;
      GetSignFromString(s, &sign);
      escape = GetCoordinateFromString(s, &Dec);
      if (escape) return;

      oldRA = RA_Target;
      oldDec = DecTarget;
      RA_Target = RA.hour + (RA.minute + RA.second/SIXTY)/SIXTY;
      DecTarget = Dec.hour + (Dec.minute + Dec.second/SIXTY)/SIXTY;
      DecTarget *= sign;
      GetXY( RA_Target, DecTarget, &X_Target, &Y_Target );
      if ((pow(X_Target,2)+pow(Y_Target,2)) > 
                                 pow(MAXZENITHANGLE*STEPSPERDEGREE,2) )
      {
         escape = 0; acceptable=0;  // If user's requested coordinates would
         dX = 0; dY = 0;            // put the telescope too far over, user
         RA_Target = oldRA;         // is so informed and permitted to try
         DecTarget = oldDec;        // again.
         gotoxy(x,y); 
         for (i=y; i<25; i++) EraseLine(i);
         FrameScreen();
         gotoxy(10,y); 
         printf("     The values you typed will put ");
         printf("the telescope outside its");
         gotoxy(10,y+1); 
         printf("allowed bounds.  It is not allowed lower than a ");
         printf("%ld degree angle.", MAXZENITHANGLE);
         gotoxy(10,y+2);
         printf("               (press any key to continue)");
         xx=wherex(); yy=wherey();
         TrackingKeyboard(10,xx,yy,&letter); // waits for user to press a key
      }
      else 
      {
         acceptable=1;
         *menu=SlewingMenu;
      }
   }
   dX = X_Target - gX;
   dY = Y_Target - gY;
   RA_Orig = RA_Target;
   DecOrig = DecTarget;
   *menu=SlewingMenu;
}

void DoSlewingMenu( int *menu )
// Shows slew progress in X and Y.  Called to do the slew from within
// the tracking routine, and returns to tracking when done.
{
   long int X_Target, Y_Target;
   double HA, Alt, Az;
   int x=25, y=TRACKINGDISPLAY, w=y+5;

   clrscr();
   FrameScreen();
   *menu = TrackingMenu;
   gotoxy(37,y); printf("Slewing");
   DisplayCoordinates(w,1,1,0);   
   gotoxy(x,w+0); printf("(Target RA: %4d:%02d:%02d )", _HOURS(RA_Target), 
                          _MINUTES(RA_Target), _SECONDS(RA_Target));
   gotoxy(x,w+1); printf("(Target Dec:%+4d:%02d:%02d )", 
                              _SIGN(DecTarget)*_HOURS(DecTarget), 
                              _MINUTES(DecTarget), _SECONDS(DecTarget));
   gotoxy(18,w+6); printf("Hit q to stop slewing and return to tracking.");
   PrevStatus = Status;
   Status = SLEWING;
   do
   {
      Slew(w+2);
      GetXY( RA_Target, DecTarget, &X_Target, &Y_Target );
      dX = X_Target - gX;
      dY = Y_Target - gY;
   } while ( (pow(dX,2)+pow(dY,2)) > pow(CLOSE_ENOUGH,2) );
   PrevStatus = Status;
   Status = TRACKING;
}

void GetSignFromString( int *s, int *sign )
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
{
   double remainder, EPSILON=ONE/3600.;
   int letter, err, temp_err, *p;
   static int tempy=2, tempx=2;

   p = s;
   err = NextNumberFromString( &(C->hour), &p );
   if (err) return (err);      // if user hit return with no numbers, exit.
//gotoxy(tempx,tempy); printf("minute=%2.0f ", C->minute);
//tempx=wherex(); tempy=wherey();
   temp_err = NextNumberFromString( &(C->minute), &p );
//gotoxy(tempx,tempy); printf("minute=%2.0f ", C->minute);   
//tempx=wherex(); tempy=wherey();
   if (temp_err) 
   {
      C->minute = ZERO;
   }
   else 
   {
      temp_err = NextNumberFromString( &(C->second), &p );
   }
   if (temp_err)
      C->second = ZERO;
   C->second = (double)((int)(C->second));  // truncate to whole seconds.
   remainder = (C->minute) - (double)(round(C->minute));
   if (remainder > EPSILON)    // convert fractions of minutes to seconds.
   {
      C->minute = (double)((int)(C->minute));
      C->second += (remainder*SIXTY);
   }
   remainder = (C->hour) - (double)(round(C->hour));
   if (remainder > EPSILON)    // convert fractions of hours to minutes. 
   {
      C->hour = (double)((int)(C->hour));
      C->minute += (remainder*SIXTY);
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
   if (C->hour>90.0) C->hour = 90.0;  // disallow hours to be more than 90.
//gotoxy(tempx,tempy); printf("minute=%2.0f ", C->minute);   
//tempx=wherex(); tempy=wherey();
   return err;
}

int NextNumberFromString( double *Num, int **s )
{
   int    letter, i=0, Okay=0, Escape=1, TooMany=200, HowMany=0;
   double number=ZERO, decimal=ZERO;
   static int tempx=2, tempy=22;

   *Num = ZERO;
   do  // skip to the first numeric character 
   {
      letter = *(*s)++;
      HowMany++;
   }  while ((letter<'0' || letter>'9') 
                     && letter!=RETURN && letter!='\n' && HowMany<TooMany);
//gotoxy(tempx,tempy);
//printf("skipped... ");
//tempx=wherex(); tempy=wherey();
   if (letter==RETURN || letter==NULL || letter=='\n') 
      return( Escape );
   while ( letter>='0' && letter<='9' && HowMany<TooMany)   
   {                  // read in numbers until the next non-numeric character
      number *= 10; 
      number += (double)(letter+CHAROFFSET);
//gotoxy(tempx,tempy);
//printf("Got: %c ", letter);
//tempx=wherex(); tempy=wherey();
      letter = *(*s)++;
      HowMany++;
   } 
   *Num = number;
//gotoxy(tempx,tempy);
//printf("Num = %2.0f. ", *Num);
//tempx=wherex(); tempy=wherey();
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
//gotoxy(tempx,tempy);
//printf("Num = %5.2f. ", *Num);
//tempx=wherex(); tempy=wherey();
   return( Okay );
}

