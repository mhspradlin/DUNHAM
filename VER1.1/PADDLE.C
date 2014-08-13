// paddle.c

// Routines for the dunham program concerning the hand-paddle.
// version 1.1

// Barbara Carter     May 4, 1995


#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <time.h>
#include <stdlib.h>
#include "globals.h"
#include "tracking.h"
#include "slew.h"
#include "utils.h"
#include "isr.h"
#include "string.h"

void FastPaddle( void )
// This routine acts like a "mini-slew" routine, for fast adjustments
// using the hand paddle.  It is called when a paddle button is pressed
// and the paddle speed switch is set to the "fast" position.  It ramps
// up the speed of the pulses until the button is released, then ramps
// the pulses down to zero.  This will result in a small "overshoot"
// effect each time the button is released.  There's really no way around
// it.  This fast paddle speed is intended for coarse adjustments (like
// getting an object into the center of the finder scope).  Once the object
// is visible in the Dunham's eyepiece, the slower paddle speed should be
// used to center the object in the field of view.
{
   long int flag=0L, common, widen=0L, X=0L, Y=0L, outword;
   long int xindex, yindex;
   long int xskip, yskip, skip[MAXLEVELS*2+1];
   long int xsend, ysend, send[MAXLEVELS*2+1];
   int N, S, E, W, Xout=0, Yout=0, i;

   skip[Levels]=MaxDelay;
   send[Levels]=0;
   xskip=0; yskip=0;
   xsend=0; ysend=0;
   xindex = Levels;
   yindex = Levels;
   for (i=1;i<=Levels;i++)
   {
      common = MinSteps + (i-1)*(MaxSteps-MinSteps)/Levels;
      send[Levels+i] = common;
      send[Levels-i] = common;
      common = MaxDelay - (i-2)*(MaxDelay-MinDelay)/Levels;
      skip[Levels+i] = common;
      skip[Levels-i] = common;  // Initialize the arrays skip[] and send[].
   }
   if ( Bits(inp(PADDLEPORT),&S,&N,&W,&E) < 4 )  // if a paddle button is
      flag = 1;                                   // down...
   while (flag)                                   // ...do the routine.
   {
      if ( (xindex!=Levels) || !N || !S )  // if it's going, or button down,
      {
         if ( (--xskip)<=0 )               // need to skip a few loops...
         {
            if ( (xindex==Levels) && (!N || !S) )    // if not yet moving, 
               xindex += ((!N ? 1:0) - (!S ? 1:0));  // start moving.
// note: the above shouldn't be needed because we'll be incrementing            
// xindex anyway.  No need to increment twice!  But weird things happen
// (like no pulses are sent!) when the above is removed.  Hmmm...
            if (xindex>Levels) 
            {
               X=1;
               Xout=XPLUS;
            }
            else if (xindex<Levels) 
            {
               X=-1;
               Xout=XMINUS;
            }
            else
            {
               X=0;
               Xout=0;
            }
            if (X!=0)
            {
               if ((--xsend)<=0)                     // if time to ramp the
               {                                     // speed up or down...
                  if ((!N && S && (xindex<(Levels+MaxRange))) || 
                                              ((N==S) && (xindex<Levels)))
                     xindex++; // If North and not-South, increment xindex.
                  else if ((!S && N && (xindex>Levels-MaxRange)) || 
                                              ((N==S) && (xindex>Levels)))
                     xindex--; // If South and not-North, decrement xindex.
                  xsend = send[xindex];              // ...set the new speed
               }
            } // if (X!=0)
            else Xout=0;
//gotoxy(2,15); printf("X=%ld  ",X);
            xskip = skip[xindex];                    // and corresponding
         }                                           // delay.
         else X=0;
      }
      else X=0;
      if ( (yindex!=Levels) || !W || !E )   // And similarly for y-direction.
      {
         if ( (--yskip)<=0 )
         {
            if ( (yindex==Levels) && (!W || !E) )     // if not yet moving, 
               yindex += ((!W ? E_W:0)-(!E ? E_W:0)); // start moving.
            if (yindex>Levels) 
            {
               Y=1;
               Yout=YPLUS;
            }
            else if (yindex<Levels) 
            {
               Y=-1;
               Yout=YMINUS;
            }
            else
            {
               Y=0;
               Yout=0;
            }
            if (Y!=0)
            {
               if ((--ysend)<=0)                     // if time to ramp the
               {                                     // speed up or down...
                  if ((!W && E && (yindex<(Levels+MaxRange))) || 
                                              ((W==E) && (yindex<Levels)))
                     yindex++; // If West and not-East, increment yindex.
                  else if ((!E && W && (yindex>Levels-MaxRange)) || 
                                              ((W==E) && (yindex>Levels)))
                     yindex--; // If East and not-West, decrement yindex.
                  ysend = send[yindex];              // ...set the new speed
               }
            } // if (Y!=0)
            else Yout=0;
//gotoxy(2,16); printf("Y=%ld  ",Y);
            yskip = skip[yindex];
         }
         else Y=0;
      }
      else Y=0;
      outword = NOPULSE - Xout - Yout;
      outp(PULSEPORT,outword);           // Send the pulse(s).
      gX += X;
      gY += Y;                           // Update the telescope's position.
      X=0L; Y=0L;
      Xout = 0;
      Yout = 0;
      if (WidenPulse)
      {
         for (widen=0L;widen<WideEnough;widen++) ;
      }
      if ( ( Bits(inp(PADDLEPORT),&S,&N,&W,&E) < 4 ) || 
                                    (xindex!=Levels) || (yindex!=Levels) )
         flag = 1;
      else
         flag = 0;            // exit if no buttons down and speed is zero.
      outp(PULSEPORT,NOPULSE);
   } // while(flag)
   GetRADec( gX, gY, &RA_Target, &DecTarget );   // reset "target" coords.
}

long int PaddleSpeed( void )
// Checks whether the paddle's speed is set to "slow" or "fast" by polling
// the paddle-speed button.  1=slow, 0=fast.  Or the other way 'round.
{
   if ( inp(SLOWFASTPORT) & 1 )
      return ( SlowPaddleSpeed );
   return ( 0L );
}

int Bits( int number, int* bit3, int* bit2, int* bit1, int* bit0 )
// Sets four "bits" according to the binary value of the number (<16).
// Returns the total number of 1's (0, 1, 2, 3, or 4).
{
   *bit3 = (number & 8)/8;
   *bit2 = (number & 4)/4;
   *bit1 = (number & 2)/2;
   *bit0 = number & 1;
   return ( (*bit3) + (*bit2) + (*bit1) + (*bit0) );
}

