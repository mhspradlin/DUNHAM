// calibr8.c 

// Routines for setting MaxDelay and MinDelay based on how fast the computer
// is that's running the program.

// Barbara Carter   1994

#include <stdio.h>
#include "calibr8.h"
#include "prefs.h"

void Calibrate( void )
{
   int result=0;
   FILE *fileptr;

   clrscr();
   FrameScreen();
   gotoxy(30,8); printf("Initializing...");
   result = DoQuickTest();
   if (result)
   {
      fileptr = fopen("DUNHAM.DAT","r");
      if (fileptr) 
         fclose(fileptr);
      else 
         SavePrefsFile();
      return;
   }
   SetOptimumDelay( &MaxDelay, 300 );
   SetOptimumDelay( &MinDelay, 5000 );
   SavePrefsFile();
}

int DoQuickTest( void )
{
   int      result=1;
   long int number;

   number = TimeSlew( MaxDelay, 150 );
   if (number>11 || number<7) result=0;
   number = TimeSlew( MinDelay, 2500 );
   if (number>11 || number<7) result=0;
   return( result );
}

void SetOptimumDelay( int *delayLoops, int numLoops )
{
   int number=1, loop=0;
   while ((number<=0 || number>=36) && loop<100)
   {
      number = TimeSlew( *delayLoops, numLoops );
      loop++;
   }
   while (number>19 && *delayLoops>1)
   {
      if (*delayLoops>6) *delayLoops-=5;
      else *delayLoops--;
      loop=0;
      do
      {
         number = TimeSlew( *delayLoops, numLoops );
         loop++;
      } while ((number<=0 || number>=36) && loop<100);
   }
   while (number<17)
   {
      *delayLoops++;
      loop = 0;
      do
      {
         number = TimeSlew( *delayLoops, numLoops );
         loop++;
      } while ((number<=0 || number>=36) && loop<100);
   }
}

long int TimeSlew( int delayLoops, int numLoops )
{
   long int ggX=0L, ggY=0L, widen;
   int flag;

   dX = delayLoops;
   dY = delayLoops/2;
   
   
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
            if ((--ysend[j]) == 0)
               ++j;
            y_skip = yskip[j];
         }
         else Y = 0;
      }
      else Y = 0;
      outword = NOPULSE - Xout - Yout;
      outp( PULSEPORT, outword );
      ggX += X;
      ggY += Y;
      if (WidenPulse)
      {
         for (widen=0L;widen<WideEnough;widen++) ;
      }
      if (screen && ShowXY)
      {
         if (Xout)
         {
            if (++xloop>2)
            {
               xloop=0;
               gotoxy(38,y_display);   
               printf("%+9ld ", gX); 
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
      } // if (screen && ShowXY)
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
         if (ShowXY)
         {
            gotoxy(46,y_display); 
            if (dX) printf(" (ramping down)"); 
            else    printf("               ");
            gotoxy(46,y_display+1); 
            if (dY) printf(" (ramping down)");
            else    printf("               ");
         }
      }
      outp( PULSEPORT, NOPULSE );
   }  // while (flag)
}
