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
      if (Status != TRACKING) StartTracking();
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

