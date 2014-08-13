// isr.c

// Interrupt Service Routines for the Dunham PC program.

// Barbara Carter   September 6, 1994

#include <math.h>
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include "globals.h"
#include "isr.h"
#include "tracking.h"

void interrupt newclk(void)
// "Hooks" the clock-tick interrupt, checks for a full keyboard buffer
// (which is extremely unlikely since the keyboard ISR "newkb" prevents
// any control-characters from hanging the computer) and empties it if it's
// full.  Also calls the old clock-tick ISR.
{
   int far *headptr = (int far *)1050;
   int far *tailptr = (int far *)1052;

   if (headptr == tailptr) keyboard_buffer=0;
   else { if (keyboard_buffer++ > 50) headptr=tailptr; }

   oldclk();
}

void interrupt newkb(void)
// "Hooks" the keyboard interrupt before the normal keyboard ISR is called,
// and gets rid of any potentially disruptive characters, like ^C.  A break
// command like ^C would halt the program before it has a chance to reset
// the various interrupt vectors it has changed, and would hang the computer.
// Also checks if the user has pressed the 'q' key during a slew operation,
// indicating the user wishes to abort the slew-- this routine sets a flag
// to stop the slew.
{
   int scancode, temp;
   char far *bios_keyflag;

   scancode = inp(0x60);
   if (scancode==0x10 && Status==SLEWING)   // 10Hex = 'q'
   {
      abort_slew_flag = 1;   // if user pressed 'q' or 'Q' during slewing,
      temp = inp(0x61);      // the abort-slew flag is set here,
      outp(0x61, temp|0x80);
      outp(0x61, temp);
      outp(0x20, 0x20);      // and the 'q' is erased.
      return;
   }
   if (Status==TRACKING && scancode>=72 && scancode <=80)
   {
      switch (scancode)
      {
         case 72: UpArrow=1; break;
         case 80: DnArrow=1; break;
         case 75: LfArrow=1; break;
         case 77: RtArrow=1; break;
         default: UpArrow=0; DnArrow=0; LfArrow=0; RtArrow=0;
      }
      if (UpArrow || DnArrow || LfArrow || RtArrow)
      {
         temp = inp(0x61);          // if one of the arrow keys was pressed,
         outp(0x61, temp|0x80);     // then the keyboard is cleared.
         outp(0x61, temp);          
         outp(0x20, 0x20);         
         return;
      }
   }
   bios_keyflag = MK_FP(0x40,0x17);
   if ( ((*bios_keyflag&4) && (scancode>1) && (scancode<0x53)) ||
                              ((*bios_keyflag&8) && (scancode==0x51)) ||
                              ((scancode>0x3A) && (scancode<0x45)) )
   {
      temp = inp(0x61);          // if the control-key was down, or
      outp(0x61, temp|0x80);     // if Alt-3 (numeric keypad) was pressed,
      outp(0x61, temp);          // or if any of the function keys F1-F10
      outp(0x20, 0x20);          // were pressed, the keyboard is cleared.
      return;
   }
   oldkb();
   return;
}

void interrupt onbreak(void)
// Int 23H handler-- intercepts the control-break interrupt, and does nothing.
{}

void interrupt tracking_ticks(void)
// The clock-tick ISR that runs at a faster tick rate than the normal rate
// for PC's.  It sends out pulses to the telescope stepper motors during
// tracking, and updates an internal time-of-day counter, TOD.
{
   int outword, sign, XP, XM, YP, YM;

   TOD++;

   if (Status == TRACKING)  // send out pulses during tracking, not slewing
   {
      outword = inp(PULSEPORT);  // Find out what was sent the last time.
      Bits(outword, &YM, &YP, &XM, &XP);
      XP *= XPLUS; XM *= XMINUS; YP *= YPLUS; YM *= YMINUS;
      if (!XP || !XM)                        // If sent an x-pulse last time...
         outword += (XBITS-XP-XM);  // ...clear it.
      else  // If didn't send an x-pulse last tick, can send one this time...
      {
         if (dX)                         // ...if there is a need for it.     
         {
            sign = _SIGN(dX);
            dX -= sign;
            gX += sign;
            if (sign>0) outword -= XPLUS;
            else outword -= XMINUS;
         }
      }
      if (!YP || !YM)                       // Similarly for Y.
         outword += (YBITS-YP-YM);
      else
      {
         if (dY)
         {
            sign = _SIGN(dY);
            dY -= sign;
            gY += sign;
            if (sign>0) outword -= YPLUS;
            else outword -= YMINUS;
         }
      }
      outp(PULSEPORT,outword);  // Start/clear the x and/or y pulse.
   }

   if( ++ticks >= FACTOR )
   {  // call old clock-tick ISR approx once every 18.2 seconds. 
      ticks = 0;
      newclk(); 
      return;
   }
   else outp(0x20,0x20);
   return;
}

void interrupt stop_tracking_ticks( void )
// This routine is called to stop the faster tick speed and return to the
// PC's normal clock-tick rate of 18.2 ticks per second.  It does so when
// one of the original clock-ticks occurs, to keep from losing fractions
// of clock-ticks when switching between different clock-tick speeds.
{
   if ( ++ticks >= FACTOR )  // if we're on an "old" clock tick...
   {
      newclk();                     // call the clock tick isr,
      setvect( 8, newclk );
      outp( CNTLPORT, CNTLWORD );   // and start up the old ticks again.
      outp( CHANNEL0, SLOWLOWORD );
      outp( CHANNEL0, SLOWHIWORD );
      PrevStatus = Status;
      Status = STOPPED;
      ticks = 0;
   }
   else outp( 0x20, 0x20 );
}

