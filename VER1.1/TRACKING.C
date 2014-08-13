// tracking.c

// Functions related to tracking on the Dunham telescope.
// Version 1.1

// Barbara Carter    October, 1995

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

//************************** function prototypes **************************

void DisplayTrackingMenu( int x, int y );

//******************************* functions *******************************

void DoZenithMenu( int *menu )
{
   int i, y=7, result=0, invalid_xy=0, invalid_ra=0;
   int sign=1, num=0, too_many=200, *p, s[255], letter, err;
   long int dummy1=0L, dummy2=0L;
   double dummy3=0.0, dummy4=0.0;
   long int x_target, y_target;
   FILE *fileptr;
   Coordinate C;
   char ch, string[201];

   clrscr();
   FrameScreen();
   gotoxy(34,y); printf("Zenith Check");
   gotoxy(8,y+2); 
   printf("It's important that the Dunham telescope ");
                                          printf("be aligned with Zenith");
   gotoxy(8,y+3); 
   printf("before proceeding-- or you may elect to slew to the ");
                                          printf("last object's");
   gotoxy(8,y+4);
   printf("position, if the telescope has NOT been moved since then.");
   gotoxy(8,y+5);
   printf("Otherwise, all calculations will be in error.\n");
   gotoxy(13,y+7); printf("1 - The Dunham telescope is at Zenith ");
                                          printf("(start tracking)");
   fileptr = fopen("DUNHAM.SAV", "r");
   if (fileptr)
   {
      result = fscanf(fileptr,"%*s %ld %*s %ld",&dummy1,&dummy2);
      if (result != 2)  // couldn't get X and Y from file.
      {
         gotoxy(13,y+8); 
         printf("2 - Can't start tracking from telescope's current position");
         invalid_xy = 1;
         gotoxy(13,y+9);
         printf("3 - Can't start tracking on last object");
         invalid_ra = 1;
         // We don't even bother to try reading RA and Dec from the file
         // if there's a problem with X and Y.  So both options (2 and 3) 
         // are automatically invalidated.  You need to know the telescope's
         // current position (X,Y) to slew to the old (RA,Dec).
      }
      else  // got X and Y from the file.
      {
         gX = dummy1;         
         gY = dummy2;
         invalid_xy=0;
         gotoxy(13,y+8); 
         printf("2 - Start tracking from telescope's current position");
//       printf(": X %ld, Y %ld", gX, gY);
         ch = fgetc(fileptr);         // need to skip to next line...
         fgets(string,200,fileptr);   // Read in the RA line from the file
         for (i=0;i<200;i++) s[i] = string[i];  // transform data
         err = GetCoordinateFromString( s, &C );
         if (!err) 
         {
            dummy3 = (double)C.hour + (double)C.minute/SIXTY 
                     + (double)C.second/(SIXTY*SIXTY);  // stores RA value
            fgets(string,200,fileptr);   // Read in the Dec line from file
            for (i=0;i<200;i++) s[i] = string[i];  // transform data
            GetSignFromString( s, &sign );
            err = GetCoordinateFromString( s, &C );
         }
         if (err) invalid_ra=1;   // couldn't read RA,Dec-- can't use. 
         else // was able to read in object's RA & Dec
         {
            dummy4 = (double)C.hour + (double)C.minute/SIXTY 
                     + (double)C.second/(SIXTY*SIXTY);  // stores Dec value
            RA_Target = dummy3;                // If got the object's RA,Dec 
            DecTarget = ((double)sign)*dummy4; // then make that the target.
            GetXY( RA_Target, DecTarget, &x_target, &y_target );
//            invalid_ra = OutOfBounds( x_target, y_target );
         }
         if (invalid_ra) // Unable to get (RA,Dec) or (RA,Dec) out of bounds.  
         {
            gotoxy(13,y+9);
            printf("3 - Can't start tracking on last object");
         }   
         else   // Got valid and useable values for RA and Dec.     
         {
            dX = x_target - gX;
            dY = y_target - gY;
            RA_Orig = RA_Target;
            DecOrig = DecTarget;
            gotoxy(13,y+9); 
            printf("3 - Slew to (and start tracking on) last object:");
            gotoxy(13,y+10);
            printf("    RA %12.8f, Dec %12.8f", RA_Target, DecTarget);
         }
      }
      fclose( fileptr );
   }
   else
   {
      gotoxy(13,y+8); 
      printf("2 - Can't start tracking from telescope's current position");
      gotoxy(13,y+9);
      printf("3 - Can't start tracking on last object");
      invalid_xy = 1;
      invalid_ra = 1;
   }
   gotoxy(13,y+11); printf("4 - Go back to main menu");
   gotoxy(29,y+13);
   GetSelection(menu,4);
   switch (*menu)
   {
      case 1:  *menu=TrackingMenu; 
               gX=0L; 
               gY=0L; 
               dX=0L;
               dY=0L;
               break;
      case 2:  if (!invalid_xy)
               {
                  *menu=TrackingMenu;     
                  dX=0L;
                  dY=0L;
               }
               else *menu=ZenithMenu;
               break;
      case 3:  if (!invalid_ra) 
               {
                  if (dX || dY) *menu=SlewingMenu;
                  else *menu=TrackingMenu;
               }
               else *menu=ZenithMenu;
               break;
      case 4:  *menu=MainMenu;     
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
         GetRADec( gX, gY, &RA_Target, &DecTarget );
         RA_Orig = RA_Target;
         DecOrig = DecTarget;
         PrevStatus = Status;
         Status = TRACKING;
         HaveBeenTracking=1;  // this shows that we know where we are.
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
                  RA_Target = RA_Orig;    // position.  However, RA and Dec
                  DecTarget = DecOrig;    // get updated.
                  break;
         case 4:  *menu=MainMenu;    // User wants to stop tracking:
                  PrevStatus = Status;
                  Status = STOPPED;  // stop tracking, and
                  SaveCoords();      // save telescope coordinates to file.
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
   gotoxy(x-8,y+7+s); printf("2 - Change jog amount (%ld arcsec)", Jog_Angle);
   gotoxy(x-8,y+8+s); printf("3 - Set current coordinates to original values");
   gotoxy(x-8,y+9+s); printf("4 - Stop tracking");
   gotoxy(x-8,y+10+s); 
   printf("Arrow Keys: %ld arcsec", Jog_Angle);
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

void SaveCoords( void )
// Called when exiting tracking, this routine saves the telescope's 
// coordinates (X and Y, and RA and Dec) in the file DUNHAM.SAV so that 
// restarting tracking is easier.
{
   FILE *fileptr;

   fileptr = fopen( "DUNHAM.SAV", "w" );
   if (fileptr)
   {
      fprintf( fileptr, "X %ld\nY %ld\nR %12.8f\nD %12.8f", 
                         gX, gY, RA_Target, DecTarget);
      fclose(fileptr);
   }
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
               TimeOfLastKey = TOD;
            }
            DisplayCoordinates(y_display,1,ShowXY,1);
            gotoxy(xx,yy);
            i=0;
            out_of_bounds = OutOfBounds(gX,gY);
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
   if ( Bits(inp(PADDLEPORT),&S,&N,&W,&E) < 4 ) // if any buttons are down...
   {                                       
      if ( PaddleSpeed() != SlowPaddleSpeed )   // and it's in fast mode...
      {
         FastPaddle();   // ...handle it in this routine until all paddle
         return;         // buttons are released.  Sort of like slewing.
      }
      else //...slow paddle doesn't require anything fancy like FastPaddle()
      {
         if (!paddleflag) paddleflag = TOD;  // we actually only activate
         paddletime = TOD - paddleflag;      // the motors in response to
         if (paddletime >= 5*PaddleSpeed())  // the paddle once in a while.
         {                        
            if (!N) dXpaddle += 5;
            if (!S) dXpaddle -= 5;           // if it's been long enough, 
            if (!W) dYpaddle += (5*E_W);     // time to add a few steps
            if (!E) dYpaddle -= (5*E_W);
            paddleflag = TOD;                // and reset the timer.
         }
      }
   }
   else paddleflag = 0;     // if no buttons are down, reset the paddle flag.
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

