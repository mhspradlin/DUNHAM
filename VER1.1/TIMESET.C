// timeset.c

// Dunham telescope routines for resetting the computer's system time.
// Version 1.1

// Barbara Carter       October, 1995


#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <time.h>
#include <stdlib.h>
#include "globals.h"
#include "timeset.h"
#include "utils.h"
#include "string.h"
#include "prefs.h"

void DoTimeMenu( int *menu )
{
   int i, y=7, letter;
   int xx, yy, xl, yl, xd, yd, xu, yu; 
   int firstxx, menuset=0, choice;

   *menu = MainMenu;
   for (i=2;i<25;i++) EraseLine(i);
   FrameScreen();
   gotoxy(36,y); printf("Set Time");
   gotoxy(7,y+2); 
   printf("The time and Daylight/Standard Time setting must be set ");
   printf("correctly,");
   gotoxy(7,y+3);
   printf("or the telescope won't be able to track properly.");
   gotoxy(13,y+5); printf("1 - Set local time:  "); 
   xl=wherex(); yl=wherey();
   gotoxy(13,y+6); printf("2 - Change daylight-savings setting:  "); 
   if (DaylightSavings) printf("Daylight Savings Time");
                        else printf("Standard Time");
   gotoxy(13,y+7); printf("3 - Set local date:  "); 
   xd=wherex(); yd=wherey();
   gotoxy(13,y+8); printf("4 - Set universal time: "); 
   xu=wherex(); yu=wherey();
   gotoxy(13,y+9); printf("5 - Go back to Main Menu");
   gotoxy(29,y+11); printf("Enter your selection: ");
   xx=wherex(); yy=wherey(); firstxx=xx; 
   do
   {
      ClockKeyboard( xl, yl, xd, yd, xu, yu, xx, yy, &letter );
                // continuously updates the time and date until a key is hit. 
      switch(letter)
      {
         case '\r'     : gotoxy(2,yy); 
                         for (i=2;i<29;i++) printf(" ");
                         gotoxy(29,yy); printf("Enter your selection: ");    
                         for (i=wherex();i<80;i++) printf(" ");
                         for (i=yy+1;i<24;i++) EraseLine(i); 
                         FrameScreen();
                         xx=firstxx;
                         gotoxy(xx,yy);
                         break;
         case BACKSPACE: 
                         if (xx > firstxx) 
                         { 
                            if (menuset==xx) menuset=0;
                            gotoxy(--xx,yy);
                            delay(1000);
                            putchar(' ');
                            delay(1000);
                         } 
                         break;
         default       : xx++;
                         if (letter>='1' && letter<='5')
                         {
                            if (!menuset)
                            {
                               choice = letter + CHAROFFSET;
                               menuset = xx;
                            }
                         }
      }
   }  while ( !(letter==RETURN && menuset) );
   menuset=0;
   switch (choice)
   {
      case 1: *menu=LocalTimeMenu; break;
      case 2: *menu=DaylightMenu; break;
      case 3: *menu=LocalDateMenu; break;
      case 4: *menu=UniversalTimeMenu; break;
      case 5: *menu=MainMenu; break;
      default: *menu=SetTimeMenu; break;
   }
}

void DoLocalTimeMenu( int *menu )
{
   int i, y=8, s[255], *p, number, letter, n, escape;
   int hour, min, sec, temp;
   int xx, yy, xl=-1, yl=-1, xd=-1, yd=-1, xu=-1, yu=-1, firstxx;
   double Num=ZERO;
   time_t timer, t;
   struct tm *t_;

   *menu = SetTimeMenu;
   for (i=0;i<255;i++) s[i]=NULL;
   for (i=2;i<25;i++) EraseLine(i);
   FrameScreen();
   gotoxy(34,y); printf("Set Local Time");
   gotoxy(8,y+2); 
   gotoxy(20,y+3); printf("Current Local time:  "); 
   xl=wherex(); yl=wherey();
   gotoxy(20,y+4); 
   if (DaylightSavings) printf("Daylight Savings time");
   else printf("Standard time (not Daylight Savings)");
   gotoxy(15,y+7); 
   printf("Using 24-hour notation (i.e., 23:00 not 11pm),");
   gotoxy(15,y+8);
   printf("enter new local time (return to exit): ");
   firstxx=wherex(); xx=firstxx; yy=wherey();
   p=s;   // p points to the 1st element of array s[].
   n=0;   // number of array elements.
   do
   {
      ClockKeyboard( xl, yl, xd, yd, xu, yu, xx, yy, &letter );
                  // continuously updates the time until a key is hit. 
      if (letter==BACKSPACE)
      {
         if (xx>firstxx)
         {
            *(--p) = NULL;
            gotoxy(--xx,yy);
            putchar(' ');
         }
      }
      else
      {
         *p++ = letter;
         xx++;
      }
      n = xx - firstxx;
   } while ((letter!=RETURN) && (n<254));
   p=s;  // reset p to point to 1st array element in s[].
   if (n>0) escape = NextNumberFromString( &Num, &p );
   else escape=1;
   if (escape) return;
   else      // first number is the hours.
   {
      hour = (int)Num;
      min = 0; sec = 0;
      temp = ( Num - (double)hour ) * SIXTY;
      if (temp>0)
      {
         min = (int)temp;
         sec = ( temp - (double)min ) * SIXTY;
         if (sec<0) sec = 0;
      }
      escape = NextNumberFromString( &Num, &p );
   }
   if (!escape)  // second number is the minutes.
   {
      min += (int)Num;
      temp = ( Num - (double)min ) * SIXTY;
      sec += (int)temp;
      escape = NextNumberFromString( &Num, &p );
   }
   if (!escape)  // third number is the seconds.
   {
      sec += round(Num);
   }
   if (sec<0) sec=0;
   while (sec>60) 
   {
      sec-=60;
      min++;
   }
   if (min<0) min=0;
   while (min>60) 
   {
      min-=60;
      hour++;
   }
   while (hour<0) hour+=24;
   while (hour>24) hour-=24;
   timer = time( NULL );
   t_ = localtime( &timer );
   t_->tm_hour = hour;
   t_->tm_min = min;
   t_->tm_sec = sec;
   t = mktime( t_ ); // Converts time to calendar (seconds since 1970) format.
   stime( &t );      // Sets system date and time.
   *menu = SetTimeMenu;
}

void DoDaylightMenu( int *menu )
{
   int i, y=8, menuset=0, choice, letter, prevDaylightSavings;
   int xx, yy, xl=-1, yl=-1, xd=-1, yd=-1, xu=-1, yu=-1, firstxx;
   time_t timer, t;
   struct tm *t_;

   *menu = SetTimeMenu;
   for (i=2;i<25;i++) EraseLine(i);
   FrameScreen();
   gotoxy(26,y); printf("Daylight-Savings Time Setting");
   gotoxy(18,y+2); 
   printf("Currently, the local time is: ");
   xl=wherex(); yl=wherey();
   gotoxy(18,y+3);
   if (DaylightSavings) printf("Daylight Savings time");
   else printf("Standard time (not Daylight)");
   gotoxy(10,y+5); printf("1 - Switch "); 
   if (DaylightSavings) printf("from");
   else printf("to");
   printf(" Daylight Savings Time, and change the time by 1 hour");
   gotoxy(10,y+6); printf("2 - Switch setting but don't change the time"); 
   gotoxy(10,y+7); printf("3 - Go back to Set Time Menu"); 
   gotoxy(29,y+9); printf("Enter your selection: ");
   firstxx=wherex(); xx=firstxx; yy=wherey();
   do
   {
      ClockKeyboard( xl, yl, xd, yd, xu, yu, xx, yy, &letter );
                // continuously updates the time and date until a key is hit. 
      switch(letter)
      {
         case '\r'     : gotoxy(2,yy); 
                         for (i=2;i<29;i++) printf(" ");
                         gotoxy(29,yy); printf("Enter your selection: ");    
                         for (i=wherex();i<80;i++) printf(" ");
                         for (i=yy+1;i<24;i++) EraseLine(i); 
                         FrameScreen();
                         xx=firstxx;
                         gotoxy(xx,yy);
                         break;
         case BACKSPACE: if (xx > firstxx) 
                         { 
                            if (menuset==xx) menuset=0;
                            gotoxy(--xx,yy); putchar(' '); 
                         } 
                         break;
         default       : xx++;
                         if (letter>='1' && letter<='3')
                         {
                            if (!menuset)
                            {
                               choice = letter + CHAROFFSET;
                               menuset = xx;
                            }
                         }
      }
   }  while ( !(letter==RETURN && menuset) );
   menuset=0;
   prevDaylightSavings = DaylightSavings;
   switch (choice)
   {
      case 1: if (DaylightSavings) DaylightSavings=0;
              else DaylightSavings=1;
              timer = time( NULL );
              t_ = localtime( &timer );
              t_->tm_hour += (DaylightSavings? 1 : -1);
              t_->tm_isdst = !(t_->tm_isdst);
              t = mktime( t_ );
              stime( &t );
              break;
      case 2: if (DaylightSavings) DaylightSavings=0;
              else DaylightSavings=1;
              t_->tm_isdst = !(t_->tm_isdst);
              break;
      case 3: break;
      default: *menu=DaylightMenu; break;
   }
   if (prevDaylightSavings != DaylightSavings) SavePrefsFile();
}

void DoLocalDateMenu( int *menu )
{
   int i, y=8, s[255], *p, number, letter, n, escape;
   int year=0, month=0, day=0;
   int xx, yy, xl=-1, yl=-1, xd=-1, yd=-1, xu=-1, yu=-1, firstxx;
   double Num=ZERO;
   time_t timer, t;
   struct tm *t_;

   *menu = SetTimeMenu;
   for (i=0;i<255;i++) s[i]=NULL;
   for (i=2;i<25;i++) EraseLine(i);
   FrameScreen();
   gotoxy(34,y); printf("Set Local Date");
   gotoxy(8,y+2); 
   gotoxy(20,y+3); printf("Current local date:  "); 
   xd=wherex(); yd=wherey();
   gotoxy(11,y+6); 
   printf("Using MM/DD/YYYY notation (i.e., 1/31/2010 not Jan. 31, 2010),");
   gotoxy(11,y+7);
   printf("enter date (return to exit): ");
   firstxx=wherex(); xx=firstxx; yy=wherey();
   p=s;   // p points to the 1st element of array s[].
   n=0;   // number of array elements.
   do
   {
      ClockKeyboard( xl, yl, xd, yd, xu, yu, xx, yy, &letter );
                  // continuously updates the time until a key is hit. 
      if (letter==BACKSPACE)
      {
         if (xx>firstxx)
         {
            *(--p) = NULL;
            gotoxy(--xx,yy);
            putchar(' ');
         }
      }
      else
      {
         *p++ = letter;
         xx++;
      }
      n = xx - firstxx;
   } while ((letter!=RETURN) && (n<254));
   p=s;  // reset p to point to 1st array element in s[].
   escape = NextNumberFromString( &Num, &p );
   if (escape) return;    // Escape -> goes back to the Set Time Menu.
   else      // first number is the month.
   {
      month = (int)Num;
      escape = NextNumberFromString( &Num, &p );
   }
   if (!escape)  // second number is the day of the month.
   {
      day = (int)Num;
      escape = NextNumberFromString( &Num, &p );
   }
   if (!escape)  // third number is the year.
   {
      year = (int)Num;
   }
   if ( (year>1970) && (month>=1) && (month<=12) && (day>=1) && (day<=31) )
   {
      timer = time( NULL );
      t_ = localtime( &timer );
      t_->tm_mon = month-1;
      t_->tm_mday = day;
      t_->tm_year = year-1900;
      t = mktime( t_ );
      stime( &t );
      *menu = SetTimeMenu;
   }
   else *menu = LocalDateMenu;
}

void DoUniversalTimeMenu( int *menu )
{
   int i, y=8, s[255], *p, number, letter, n, escape;
   int hour, min, sec, temp;
   int xx, yy, xl=-1, yl=-1, xd=-1, yd=-1, xu=-1, yu=-1, firstxx;
   double Num=ZERO;
   time_t timer, t;
   struct tm *t_;

   *menu = SetTimeMenu;
   for (i=0;i<255;i++) s[i]=NULL;
   for (i=2;i<25;i++) EraseLine(i);
   FrameScreen();
   gotoxy(32,y); printf("Set Universal Time");
   gotoxy(8,y+2); 
   gotoxy(17,y+3); printf("Current Universal time:  "); 
   xu=wherex(); yu=wherey();
   gotoxy(17,y+6); 
   printf("Using 24-hour notation (i.e., 23:00 not 11pm),");
   gotoxy(17,y+7);
   printf("enter new UT (return to exit): ");
   firstxx=wherex(); xx=firstxx; yy=wherey();
   p=s;   // p points to the 1st element of array s[].
   n=0;   // number of array elements.
   do
   {
      ClockKeyboard( xl, yl, xd, yd, xu, yu, xx, yy, &letter );
                  // continuously updates the time until a key is hit. 
      if (letter==BACKSPACE)
      {
         if (xx>firstxx)
         {
            *(--p) = NULL;
            gotoxy(--xx,yy);
            putchar(' ');
         }
      }
      else
      {
         *p++ = letter;
         xx++;
      }
      n = xx - firstxx;
   } while ((letter!=RETURN) && (n<254));
   p=s;  // reset p to point to 1st array element in s[].
   escape = NextNumberFromString( &Num, &p );
   if (escape) return;
   else      // first number is the hours.
   {
      hour = (int)Num;
      min = 0; sec = 0;
      temp = ( Num - (double)hour ) * SIXTY;
      if (temp>0)
      {
         min = (int)temp;
         sec = ( temp - (double)min ) * SIXTY;
         if (sec<0) sec = 0;
      }
      escape = NextNumberFromString( &Num, &p );
   }
   if (!escape)  // second number is the minutes.
   {
      min += (int)Num;
      temp = ( Num - (double)min ) * SIXTY;
      sec += (int)temp;
      escape = NextNumberFromString( &Num, &p );
   }
   if (!escape)  // third number is the seconds.
   {
      sec += round(Num);
   }
   if (sec<0) sec=0;
   while (sec>60) 
   {
      sec-=60;
      min++;
   }
   if (min<0) min=0;
   while (min>60) 
   {
      min-=60;
      hour++;
   }
   while (hour<0) hour+=24;
   while (hour>24) hour-=24;
   timer = time( NULL );
   t_ = localtime( &timer );
   t_->tm_hour = hour -6 + DaylightSavings;
   t_->tm_min = min;
   t_->tm_sec = sec;
   t = mktime( t_ );
   stime( &t );
   *menu = SetTimeMenu;
}

void StartTiming( void )
// Sets up the timing (the global variable "TOD" -- Time Of Day) needed 
// for tracking.  Relies on the computer's internal clock.  May be called 
// at any time during the program to reset the timing (clear TOD).
{
   double JD, S, T, newUT;
   time_t timer;
   struct tm *t_;
   int temp;
   double Alt, Az, HA;

   timer = time( NULL );
   t_ = localtime( &timer );
   JD = JulianDate( (t_->tm_year)+1900, (t_->tm_mon)+1, t_->tm_mday );
   S = JD - 2451545.0;
   T = S / 36525.0;
   T0 = 6.697374558 + (T*2400.051336) + (pow(T,2)*0.000025862);
   while (T0 > 24.0) T0 -= 24.0;
   while (T0 < 0.0)  T0 += 24.0;
   newUT = (double)(t_->tm_hour + 6 - DaylightSavings);
   newUT += ( (double)(t_->tm_min) 
                 + (double)(t_->tm_sec)/SIXTY )/SIXTY;
   disable();
   StartingUT = newUT;
   TOD=0L;
   enable();
}

void ClockKeyboard( int xl, int yl, int xd, int yd, int xu, int yu, 
     int xx, int yy, int *letter )
{
   time_t timer;
   struct tm *t_;
   int UT_hour;
   long int loopdelay=(long)(50*DisplayDelay), loop=loopdelay;

   do
   {
      if ((++loop) > loopdelay)
      {
         loop = 0L;
         timer = time( NULL );
         t_ = localtime( &timer );
         if (xl>=0) 
         {
            gotoxy(xl,yl); 
            printf("%2d:%02d:%02d  (24hr)", t_->tm_hour, 
                                            t_->tm_min, t_->tm_sec);
         }
         if (xd>=0) 
         {
            gotoxy(xd,yd); 
            printf("%2d/%2d/%4d (MM/DD/YYYY)", t_->tm_mon+1, 
                                               t_->tm_mday, t_->tm_year+1900);
         }
         if (xu>=0) 
         {
            UT_hour = t_->tm_hour + 6 - DaylightSavings;
            if (UT_hour < 0) UT_hour += 24;
            if (UT_hour > 24) UT_hour -= 24;
            gotoxy(xu,yu); 
            printf("%2d:%02d:%02d  (24hr)", UT_hour, t_->tm_min, t_->tm_sec);
         }
         gotoxy(xx,yy);
      }
   } while( !kbhit() );
   *letter = getche();
}
