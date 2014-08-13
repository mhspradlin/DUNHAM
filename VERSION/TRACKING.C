// tracking.c

// Functions related to tracking on the Dunham telescope.

// Barbara Carter    October 20, 1994

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

//************************** function prototypes **************************

void DisplayTrackingMenu( int x, int y );

//******************************* functions *******************************

void DoZenithMenu( int *menu )
{
   int i,y=8, result=0;
   long int dummy1=0L, dummy2=0L;
   FILE *fileptr;

   clrscr();
   FrameScreen();
   gotoxy(34,y); printf("Zenith Check");
   gotoxy(8,y+2); 
   printf("It's important that the Dunham telescope ");
   printf("be aligned with Zenith");
   gotoxy(8,y+3); 
   printf("before proceeding-- or you may elect to resume tracking on its ");
   printf("last");
   gotoxy(8,y+4);
   printf("position, if the telescope has NOT been moved since then.");
   gotoxy(8,y+5);
   printf("Otherwise, all calculations will be in error.\n");
   gotoxy(21,y+7); printf("1 - The Dunham telescope is at Zenith");
   gotoxy(21,y+8); printf("2 - Start tracking on last position");
   gotoxy(21,y+9); printf("3 - Go back to main menu");
   gotoxy(29,y+11);
   GetSelection(menu,3);
   switch (*menu)
   {
      case 1:  *menu=TrackingMenu; 
               gX=0L; 
               gY=0L; 
               break;
      case 2:  fileptr = fopen("DUNHAM.SAV", "r");
               if (fileptr)
               {
                  result = fscanf(fileptr,"%*s %ld %*s %ld",&dummy1,&dummy2);
                  if (result != 2) 
                     *menu=ZenithMenu;
                  else
                  {
                     *menu=TrackingMenu;
                     gX = dummy1;
                     gY = dummy2;
                  }
                  fclose( fileptr );
               }
               else *menu=ZenithMenu;
               break;
      case 3:  *menu=MainMenu;     
               break;
      default: *menu=SorryMenu;    
               break;
   }
}

void DoTrackingMenu( int *menu )
{
   int      x=27, y=TRACKINGDISPLAY; 
   int      xx, yy, firstxx, i;
   int      letter, menuset=0, choice;
   long int tempX, tempY;

   if (ShowXY) y--;
   do   // while (*menu == TrackingMenu); 
   {
      if (Status != TRACKING) 
      {
         delay(0);
         StartTracking();
         delay(0);
      }
      DisplayTrackingMenu(x,y);
      xx=wherex(); yy=wherey(); firstxx=xx;
      do
      {
         TrackingKeyboard(y+5,xx,yy,&letter);
         switch(letter)
         {
            case 0        : menuset=xx;
                            if (ScreenDark) // if was in screensaver mode,
                            {               // go back to tracking.
                               letter = RETURN;
                               choice=5;  
                            }
                            else  // otherwise telescope is out-of-bounds.
                               choice=0;
                            break;
            case '\r'     : gotoxy(2,yy); 
                            for (i=2;i<x;i++) printf(" ");
                            gotoxy(x,yy); printf("Enter your selection: ");    
                            for (i=wherex();i<80;i++) printf(" ");
                            for (i=yy+1;i<24;i++) EraseLine(i); 
                            FrameScreen();
                            xx=firstxx;
                            gotoxy(xx,yy);
                            break;
            case BACKSPACE: if (xx > firstxx) 
                            { 
                               if (menuset==xx) menuset=0;
                               gotoxy(--xx,yy); printf(" "); 
                            } 
                            break;
            default       : xx++;
                            if (letter>='1' && letter<='4')
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
         case 0:  *menu=OutOfBoundsMenu;  // Telescope is too far from zenith.
                  break;
         case 1:  *menu=RA_DecSlewMenu;   // User wants to slew.
                  break;
         case 2:  *menu=ChangeJogMenu;    // User wants to change jog amount.
                  break;
         case 3:  *menu=TrackingMenu;     // User wants to update coordinates:
                  GetXY( RA_Orig, DecOrig, &tempX, &tempY );
                  Xoffset += (tempX-gX);  // gX,gY remain unchanged-- they
                  Yoffset += (tempY-gY);  // reflect the telescope's actual
                  RA_Target = RA_Orig;    // position.  However, the RA,Dec
                  DecTarget = DecOrig;    // get updated.
                  break;
         case 4:  *menu=MainMenu;  // User wants to stop tracking:
                  StopTracking();  // returns clock-ticks to the normal speed.
                  break;
         default: *menu=TrackingMenu;   // If anything weird gets through,
                  break;                // we'll just assume we want to keep
      }                                 // tracking.
   } while (*menu == TrackingMenu); 
}

void DisplayTrackingMenu( int x, int y )
{
   int s=2*ShowXY+3;

   clrscr();
   FrameScreen();
   DisplayCoordinates(y+5,1,ShowXY,1);
   gotoxy(36,y); printf("Tracking"); 
   gotoxy(17,y+3); printf("Original");
   gotoxy(14,y+5); printf("RA: %4d:%02d:%02d", _HOURS(RA_Orig), 
                            _MINUTES(RA_Orig), _SECONDS(RA_Orig) );
   gotoxy(14,y+6); printf("Dec:%+4d:%02d:%02d",_SIGN(DecOrig)*_HOURS(DecOrig),  
                            _MINUTES(DecOrig), _SECONDS(DecOrig) );
   gotoxy(36,y+3); printf("Current");
   gotoxy(52,y+3); printf("Difference");
   gotoxy(x-8,y+6+s); printf("1 - Slew to new RA/Dec coordinates");
   gotoxy(x-8,y+7+s); printf("2 - Change jog amount (%ld arcsec)", 
                                                               Jog_Angle);
   gotoxy(x-8,y+8+s); printf("3 - Set current coordinates to original values");
   gotoxy(x-8,y+9+s); printf("4 - Stop tracking");
   gotoxy(x-8,y+10+s); printf("Arrow keys - jog telescope by %ld arcsec", 
                                                               Jog_Angle);
   gotoxy(x,y+12+s); printf("Enter your selection: ");
}

void DoChangeJogMenu( int *menu )
{
   int               y=TRACKINGDISPLAY, xx, yy, firstxx, str[255], *p, i;
   int               letter, err, x=22, s2=2*ShowXY;
   double            number=ZERO;

   *menu = TrackingMenu;
   do
   {
      p = str;
      for (i=0;i<255;i++) str[i]=NULL;
      clrscr();
      FrameScreen();
      DisplayCoordinates(y+5,1,ShowXY,1);
      gotoxy(33,y); printf("Change Jog Value"); 
      gotoxy(17,y+3); printf("Original");
      gotoxy(14,y+5); printf("RA: %4d:%02d:%02d", _HOURS(RA_Orig), 
                               _MINUTES(RA_Orig), _SECONDS(RA_Orig) );
      gotoxy(14,y+6); printf("Dec:%+4d:%02d:%02d",_SIGN(DecOrig)*_HOURS(DecOrig),  
                               _MINUTES(DecOrig), _SECONDS(DecOrig) );
      gotoxy(36,y+3); printf("Current");
      gotoxy(52,y+3); printf("Difference");

      gotoxy(x+4,y+9+s2); printf("Current jog value: %ld arcsec", Jog_Angle);
      gotoxy(x+2,y+11+s2); printf("Enter new jog value in arcseconds");
      gotoxy(x,y+12+s2); printf("(hit Return to leave value unchanged): ");
      xx=wherex(); yy=wherey(); firstxx=xx;
      do
      {
         TrackingKeyboard(y+5,xx,yy,&letter);
         switch(letter)
         {
            case '\r'     : for (i=yy;i<24;i++) EraseLine(i);
                            FrameScreen();
                            gotoxy(x,yy); 
                            printf("(hit Return to leave value unchanged): ");    
                            xx=firstxx;
                            *p = letter;
                            break;
            case BACKSPACE: if (xx > firstxx) 
                            { 
                               gotoxy(--xx,yy); printf(" "); 
                               *(--p) = NULL;
                            } 
                            break;
            default       : xx++;
                            *p++ = letter;
                            break;
         }

      }  while ( letter!=RETURN );
      p = str; // reset ptr to start of string.
      err = NextNumberFromString( &number, &p ); 
      if (!err) 
      {
         Jog_Angle = round(number);
         *menu = TrackingMenu;
      }
   } while (*menu == ChangeJogMenu); 
}

void StartTracking( void )
// Starts the faster clock ticks, sets up the timing, and assigns 
// "tracking_ticks" to be the clock-tick ISR.
{
   double JD, S, T;
   time_t timer, *dummy;
   struct tm *t_;
   int temp;
   double Alt, Az, HA;

   if (Status != TRACKING)
   {
      CoordinatesInitialized=1;  // this shows that we know where we are.
      timer = time( dummy );
      t_ = localtime( &timer );
//gotoxy(1,1);
//printf("local time: %d year, %d month, %d day, %d hour, %d min, %d sec",
//t_->tm_year, t_->tm_mon, t_->tm_mday, t_->tm_hour, t_->tm_min, t_->tm_sec);
      JD = JulianDate( (t_->tm_year)+1900, (t_->tm_mon)+1, t_->tm_mday );
      S = JD - 2451545.0;
      T = S / 36525.0;
      T0 = 6.697374558 + (T*2400.051336) + (pow(T,2)*0.000025862);
      while (T0 > 24.0) T0 -= 24.0;
      while (T0 < 0.0)  T0 += 24.0;
      StartingUT = (double)(t_->tm_hour + 6 - DaylightSavings);
      StartingUT += ( (double)(t_->tm_min) 
                    + (double)(t_->tm_sec)/SIXTY )/SIXTY;
      ticks=0;
      TOD=0L;
      PrevStatus = Status;
      Status = TRACKING;
      disable();
      outp( CNTLPORT, CNTLWORD );
      outp( CHANNEL0, TRACKLOWORD );
      outp( CHANNEL0, TRACKHIWORD );
      setvect(8,tracking_ticks); 
      enable();
   }

   if (PrevStatus == STOPPED)
   {
      GetRADec( gX, gY, &RA_Target, &DecTarget );
      RA_Orig = RA_Target;
      DecOrig = DecTarget;
   }
}

void StopTracking( void )
// Called before exiting the Dunham control program, this routine initiates
// the "stop_tracking_ticks" ISR, which will gracefully stop the tracking.
{
   int i;

   FrameScreen();
   for (i=2;i<23;i++) EraseLine(i);
   setvect(8,stop_tracking_ticks);
   delay(200);
//   delay(150);
}

void TrackingKeyboard( int y_display, int xx, int yy, int *letter)
// Continuously updates the coordinates and re-calculates the target location.
// Runs until the user presses a key: if user does not press any keys for a
// while, it goes into "screen-saver" mode (screen goes mostly blank), and 
// the next key the user presses is not passed to the calling function but
// is simply used to "wake up" from screen-saver mode.  Tracking continues
// uninterrupted in screen-saver mode.  Arrow keys do jog the telescope in
// screen-saver mode, and pressing one also wakes up from the screen saver.
{
   int i=DisplayDelay, out_of_bounds=0;
   int xscreen, yscreen, x=27, y=TRACKINGDISPLAY;
   long int R;

   gotoxy(xx,yy);
   randomize();
   TimeOfLastKey = TOD;
   ScreenDark=0;
   do
   {
      TrackCalc();
      if ((++i) > DisplayDelay)    // only re-display every so often. 
      {
         if ( TimeToDarken && ((TOD-TimeOfLastKey)>TimeToDarken) )
         {
            if ( !ScreenDark )
            {
               clrscr();
               ScreenDark = 1;   // indicates screensaver mode
               xscreen = random(48) + 1;
               yscreen = random(24) + 1;
               gotoxy(xscreen,yscreen);
               printf("Screen saver mode. Press any key.");
            }
         }
         else
         {
            if (ScreenDark)
            {
               DisplayTrackingMenu(x,y);
               ScreenDark = 0;
            }
            DisplayCoordinates(y_display,1,ShowXY,1);
            gotoxy(xx,yy);
            i=0;
            R = round(sqrt( pow(gX,2) + pow(gY,2) ));
            if (R > MAXZENITHANGLE*STEPSPERDEGREE)
            {
               out_of_bounds = 1;
            }
         }
      }
   }  while ( !kbhit() && !out_of_bounds );
   if (!out_of_bounds) 
   {
      if ( ScreenDark ) 
      {
         *letter = getche(); 
         *letter = 0;
      }
      else
         *letter = getche();
   }
   else *letter = 0;
}

void TrackCalc( void )
// This function gets called repeatedly from TrackingKeyboard().  It updates
// the "target" coordinates RA_Target, DecTarget, by updating dX and dY 
// (dX and dY are the distance, in motor steps, from (gX,gY) to (RA_Target,
// DecTarget)).  It first checks whether any of the paddle buttons are down, 
// and if so incorporates that into its target calculation.  The paddle 
// effectively moves the "target".  It also checks for arrow-keys, which
// the user uses to "jog" the telescope by a pre-set distance.
{
   double          HA, Alt, Az, f_X_Target, f_Y_Target;
   int             dXpaddle=0, dYpaddle=0;
   long int        X_Target, Y_Target;
   static long int paddleflag=0L;
   static long int paddletime=0L;
   int             N,S,E,W;

   // first we check the paddle... 
   if ( Bits(inp(PADDLEPORT),&S,&N,&W,&E) < 4 ) // if any buttons are down,
   {                                       
      if (!paddleflag) paddleflag = TOD;
      paddletime = TOD - paddleflag;
      if (paddletime >= 5*PaddleSpeed())  // check how long they have been so,
      {                        
         if (!N) dXpaddle += 5;
         if (!S) dXpaddle -= 5;           // and if long enough, 
         if (!W) dYpaddle += 5;           // time to add a step...
         if (!E) dYpaddle -= 5;
         paddleflag = TOD;                // and reset the timer.
      }
   }
   else paddleflag = 0;     // if no buttons are down, reset the paddle flag.
   
//   gotoxy(2,2); printf("paddle: ");
//   if (!N) printf("N"); else printf(" ");
//   if (!S) printf("S"); else printf(" ");
//   if (!E) printf("E"); else printf(" ");
//   if (!W) printf("W"); else printf(" ");
//   gotoxy(2,3); printf("dXpaddle:%3.0f", (double)dXpaddle);
//   gotoxy(2,4); printf("dYpaddle:%3.0f", (double)dYpaddle);

   if (dXpaddle || dYpaddle || UpArrow || DnArrow || LfArrow || RtArrow)
   {
      if (UpArrow) printf("%c", 24);
      if (DnArrow) printf("%c", 25);
      if (RtArrow) printf("%c", 16);  // SHOULD be 26, but 26 doesn't work!
      if (LfArrow) printf("%c", 27);
      f_GetXY( RA_Target, DecTarget, &f_X_Target, &f_Y_Target );
      f_X_Target += (double)dXpaddle;
      f_X_Target += ( (double)(UpArrow-DnArrow)*(double)Jog_Angle )
                                 *(double)STEPSPERDEGREE/3600.;
      f_Y_Target += (double)dYpaddle;
      f_Y_Target += ( (double)(LfArrow-RtArrow)*(double)Jog_Angle )
                                 *(double)STEPSPERDEGREE/3600.;
      f_GetRADec( f_X_Target, f_Y_Target, &RA_Target, &DecTarget );
      X_Target = round(f_X_Target);
      Y_Target = round(f_Y_Target);
      UpArrow=0; DnArrow=0; LfArrow=0; RtArrow=0;
      dXpaddle=0; dYpaddle=0;
   }
   else
   {                       
      GetXY( RA_Target, DecTarget, &X_Target, &Y_Target ); 
   }
   dX = X_Target - gX;   // dX,dY tell the tracking routine how
   dY = Y_Target - gY;   // many pulses to send to the motors.
}

long int PaddleSpeed( void )
// Checks whether the paddle's speed is set to "slow" or "fast" by polling
// the paddle-speed button.  1=slow, 0=fast.  Or the other way 'round.
{
   if ( inp(SLOWFASTPORT) & 1 )
      return ( SlowPaddleSpeed );
   return ( FastPaddleSpeed );
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

