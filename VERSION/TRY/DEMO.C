// demo.c 

// Code for testing PC-based user interface Dunham telescope control stuff.  
// In C:\DUNHAM\DEMO.

// Barbara Carter  September 6, 1994 

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <math.h>
#include "globals.h"
#include "isr.h"
#include "tracking.h"
#include "slew.h"
#include "prefs.h"
#include "motor.h"
#include "utils.h"

void main(void)
// The Dunham PC-based user interface program.  It is menu-driven, allowing
// the user several choices at all times.  Emphasis is placed on permitting
// the user to back out of a choice at any time, and easily starting and
// stopping any function.
{
   int menu=MainMenu;

   Initialize();
   while ( menu >= 0 )
   {
      DoMenu( &menu );
   }
}

void DoMenu( int* menu )
// The heart of the PC user interface, this routine calls the appropriate
// subroutine to perform whatever function the user has chosen.
{
   switch (*menu)
   {
      case ExitProgram:     DoExitProgram(menu);     break;
      case MainMenu:        DoMainMenu(menu);        break;
      case ZenithMenu:      DoZenithMenu(menu);      break;
      case TrackingMenu:    DoTrackingMenu(menu);    break;
      case ChangeJogMenu:   DoChangeJogMenu(menu);   break;
      case RA_DecSlewMenu:  DoRA_DecSlewMenu(menu);  break;
      case MotorMenu:       DoMotorMenu(menu);       break;
      case MotorTestMenu:   DoMotorTestMenu(menu);   break;
      case SlewingMenu:     DoSlewingMenu(menu);     break;
      case OutOfBoundsMenu: DoOutOfBoundsMenu(menu); break;
      case PreferencesMenu: DoPreferencesMenu(menu); break;
      case ShowXYMenu:      DoShowXYMenu(menu);      break;
      case SlewSpeedMenu:   DoSlewSpeedMenu(menu);   break;
      case FastPaddleMenu:  DoFastPaddleMenu(menu);  break;
      case SlowPaddleMenu:  DoSlowPaddleMenu(menu);  break;
      case AccelerationMenu: DoAccelerationMenu(menu); break;
      case AccuracyMenu:    DoAccuracyMenu(menu);    break;
      case MaxDelayMenu:    DoMaxDelayMenu(menu);    break;
      case MinDelayMenu:    DoMinDelayMenu(menu);    break;
      case MinStepsMenu:    DoMinStepsMenu(menu);    break;
      case MaxStepsMenu:    DoMaxStepsMenu(menu);    break;
      case LevelsMenu:      DoLevelsMenu(menu);      break;
      case DarkTimeMenu:    DoDarkTimeMenu(menu);    break;
      default:              DoSorryMenu(menu);       break;
   }
}

void DoMainMenu( int *menu )
// This menu is displayed when the program starts up.  It allows the user
// to choose between exiting, testing the motors (by slewing to user-entered
// X and Y coordinates), or starting tracking ("Start Program").
{  
   int letter, y=8, i;

   for (i=2;i<23;i++) EraseLine(i);
   FrameScreen();
   gotoxy(32,y);   printf("Dunham Telescope");
   gotoxy(32,y+1); printf(" User Interface ");
   gotoxy(31,y+4); 
   if (CoordinatesInitialized) printf("1 - Resume Tracking"); 
                          else printf("1 - Start Tracking"); 
   gotoxy(31,y+5);             printf("2 - Test Motors");
   gotoxy(31,y+6);             printf("3 - Set Preferences");
   gotoxy(31,y+7);             printf("4 - Exit Program"); 
   gotoxy(29,y+9);
   GetSelection(menu,4);
   switch(*menu)
   {
      case 1: if (CoordinatesInitialized) 
                 *menu=TrackingMenu;  
              else *menu=ZenithMenu;
              break;
      case 2: *menu=MotorMenu;       break;
      case 3: *menu=PreferencesMenu; break;
      case 4: *menu=ExitProgram;     break;
      default: *menu=SorryMenu;      break;
   }
}

void DoOutOfBoundsMenu( int *menu )
{
   double A, R, R_new;
   int xx, yy;

   clrscr();
   FrameScreen();
   gotoxy(33,10); printf("Out Of Bounds");
   gotoxy(15,15); 
   printf("The telescope has moved beyond its allowed limits.");
   gotoxy(13,16);
   printf("It is not allowed lower than %ld degrees from zenith.", 
                                                            MAXZENITHANGLE);
   gotoxy(15,18); printf("Press any key to retreat to a safe position.");
   gotoxy(17,18); printf("(From there you may slew to new coordinates)");
   xx=wherex(); yy=wherey();
   while (!kbhit())
   {
      DisplayCoordinates(12,1,0,0);
      gotoxy(xx,yy);
      delay(100);
   }
   R = sqrt( pow(gX,2) + pow(gY,2) );
   R_new = (double)((MAXZENITHANGLE-5L)*STEPSPERDEGREE);
   A = ONE - R/R_new;
   dX = -A*gX;
   dY = -A*gY;
   PrevStatus = Status;
   Status = SLEWING;
   Slew(37,12);
   if (PrevStatus == TRACKING)
   {
      PrevStatus = Status;
      Status = TRACKING;
      *menu = TrackingMenu;
   }
   else
   {
      PrevStatus = Status;
      Status = STOPPED;
      *menu = MainMenu;
   }
}

void DoSorryMenu( int *menu )
// This routine runs when the menu choice the user entered has not yet been
// fully implemented.  Should not be needed in the final product.
{
   clrscr();
   FrameScreen();
   gotoxy(20,20); printf("Sorry, that feature is not implemented.");
   gotoxy(20,21); printf("(hit <return> to continue) ");
   while ( getchar() != '\n' ) ;
   *menu = MainMenu;
}

void DoExitProgram( int *menu )
// Like the opposite of "Initialize", this routine resets all interrupts to
// their original ISR's, stops the telescope if it's still moving, and then
// exits the program gracefully.  Resetting the old ISR's is particularly
// necessary, as otherwise the computer would be hung and would require a
// hardware reset (like pressing the reset switch, or turning it off and on).
{
   int i;
   FILE *fileptr;

   if (Status==SLEWING)            // If the telescope is slewing, 
   {                             
      abort_slew_flag = 1;         // tell it to ramp down now,
      clrscr();
      FrameScreen();
      gotoxy(30,10); 
      printf("Ending slew...");
      while (Status==SLEWING) ;    // and then wait til it's done.
   }
   disable();
   setvect(8,oldclk);
   setvect(9,oldkb);
   enable();
   outp(PADDLEPORT,0);
   outp(PULSEPORT,0);
   FrameScreen();
   for (i=2;i<23;i++) EraseLine(i);
   gotoxy(10,12);
   printf("Thank you for using the Dunham telescope PC user interface.");
   gotoxy(1,24);
   *menu=-1;
   fileptr = fopen( "DUNHAM.SAV", "w" );
   if (fileptr)
   {
      fprintf( fileptr, "X %ld\nY %ld\n", gX, gY);
      fclose(fileptr);
   }
}

void Initialize( void )
// Called once, at the beginning of the program, this routine sets up all
// the ports and ISR's that will be needed.
{
   int dummy=0, err=0, result=0;
   long int ldummy;
   FILE *fileptr;

   printf(" Initializing...");
   PrevStatus = STOPPED;
   Status = STOPPED;
//   gX=0L;
//   gY=0L;
   fileptr = fopen( "DUNHAM.DAT", "r" );
   if (fileptr)
   {
//      printf("Successfully opened DUNHAM.DAT...\n");
      result = fscanf( fileptr, "%*s %d", &dummy );       //ShowXY
//      printf("First fscanf result=%d, got value dummy=%d\n", result, dummy);
      if (result != 1) err=1;
      else ShowXY = dummy;
//      if (err) printf("err=%d: Error happened on first line.\n", err);
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %d", &dummy );    //FastSlew
         if (result != 1) err=1;
         else FastSlew = dummy;
//       printf("2nd fscanf result=%d, got value dummy=%d\n", result, dummy);
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy ); // FastPaddleSpeed
         if (result != 1) err=1;
         else FastPaddleSpeed = ldummy;
//       printf("3rd fscanf result=%d, got value ldummy=%ld\n", result, ldummy);
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy ); // SlowPaddleSpeed
         if (result != 1) err=1;
         else SlowPaddleSpeed = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy ); // CloseEnough
         if (result != 1) err=1;
         else CloseEnough = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy ); // TimeToDarken
         if (result != 1) err=1;
         else TimeToDarken = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy );  // MaxDelay
         if (result != 1) err=1;
         else MaxDelay = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy );  // MinDelay
         if (result != 1) err=1;
         else MinDelay = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy );  // MinSteps
         if (result != 1) err=1;
         else MinSteps = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy );  // MaxSteps
         if (result != 1) err=1;
         else MaxSteps = ldummy;
      }
      if (!err) 
      {
         result = fscanf( fileptr, "%*s %ld", &ldummy );  // Levels
         if (result != 1) err=1;
         else Levels = ldummy;
      }
//      if (err) printf("error occurred during read.\n");
      fclose( fileptr );
   }
//   else printf("Cannot open DUNHAM.DAT\n");
//   delay(3000);
//   TimeToDarken = round( 18.2*(double)SecsToDarken*(double)FACTOR );
   SecsToDarken = (double)TimeToDarken / ( (double)FACTOR * 18.2 );
   oldShowXY=ShowXY;
   oldFastSlew=FastSlew;
   oldFastPaddleSpeed=FastPaddleSpeed;
   oldSlowPaddleSpeed=SlowPaddleSpeed;
   oldCloseEnough=CloseEnough;
   oldTimeToDarken=TimeToDarken;
   oldMaxDelay=MaxDelay;
   oldMinDelay=MinDelay;
   oldMinSteps=MinSteps;
   oldMaxSteps=MaxSteps;
   oldLevels=Levels;
   outp( IOCNTLPORT, A_OUTPUT + B_OUTPUT + C_INPUT ); 
      // initialize the I/O board
   outp( PULSEPORT, NOPULSE );       // initialize the motors
   outp( PADDLEPORT, PADDLEWORD );   // initialize the Paddle
   setvect(0x1B,onbreak);   // swallow break interrupts (like ^C)
   setvect(0x23,onbreak);   // without allowing them to halt the program.
   oldkb = getvect(9);
   setvect(9,newkb);        // hook the keyboard ISR.
   oldclk = getvect(8);
   setvect(8,newclk);       // hook the clock-tick ISR.
   clrscr();
}

void DisplayCoordinates( int y, int show_RADec, int show_XY, 
                                                int show_offsets )
{
   double   RA, Dec, RA_Offset, DecOffset;
   int      sign=0;
   int      x1=33, x2=52;

   if (show_XY)
   {
      gotoxy(x1,y+2); printf("X:   %+9ld", gX);
      gotoxy(x1,y+3); printf("Y:   %+9ld", gY);
      if (show_offsets)
      {
         gotoxy(x2,y+2); printf("%9ld", Xoffset);
         gotoxy(x2,y+3); printf("%9ld", Yoffset);
      }   
   }
   if (show_RADec)
   {
      GetRADec( gX, gY, &RA, &Dec );
      gotoxy(x1,y+0); printf("RA: %4d:%02d:%02d", _HOURS(RA), _MINUTES(RA), 
                                                   _SECONDS(RA) );
      gotoxy(x1,y+1); printf("Dec: ");
      sign = _SIGN(Dec);
      if (sign<0) printf("-");
      else printf("+");
      printf("%2d:%02d:%02d", _HOURS(Dec), _MINUTES(Dec), _SECONDS(Dec) );
      if (show_offsets)
      {
         RA_Offset = RA  - RA_Orig;
         if (RA_Offset > 12)  RA_Offset -= 24.0;
         if (RA_Offset < -12) RA_Offset += 24.0;
         sign=_SIGN(RA_Offset);
         gotoxy(x2,y+0); 
         if (sign<0) printf("-");
         else printf("+");
         printf("%2d:%02d:%02d", _HOURS(RA_Offset), _MINUTES(RA_Offset), 
                                                    _SECONDS(RA_Offset) );
         DecOffset = Dec - DecOrig;
         sign=_SIGN(DecOffset);
         gotoxy(x2,y+1); 
         if (sign<0) printf("-"); 
         else printf("+");
         printf("%2d:%02d:%02d", _HOURS(DecOffset), _MINUTES(DecOffset), 
                                                    _SECONDS(DecOffset) );
      }
   }
}

void GetSelection( int *menu, int max )
// Waits for the user to choose an item from the menu on the screen.  It
// disallows any choice other than the options on the screen.
{
   int x=wherex(), y=wherey(), letter, i, xx;

   do 
   {
      FrameScreen();
      gotoxy(2,y);
      for (i=2;i<x;i++) printf(" ");
      gotoxy(x,y); printf("Enter your selection: ");
      xx = wherex();
      for (i=xx;i<80;i++) printf(" ");
      for (i=y+1;i<24;i++) EraseLine(i);
      gotoxy(xx,y);
      letter=getchar(); // this waits until the user hits the return key.
   } while ( (letter<'1') || (letter>(max-CHAROFFSET)) );
   *menu = letter+CHAROFFSET;
   ClearKeyboard();
}

void ClearKeyboard( void )
// Empties the keyboard buffer.  Used after reading the user's input, like 
// new coordinates, etc.
{
   while ( (getchar()) != '\n') ;
}

void FrameScreen( void )
// Puts a decorative frame around the edge of the screen, so it's easy to
// see what program is running.
{
   int i, title_x=37, x, y;

   x = wherex();
   y = wherey();
   gotoxy(1,1); printf("%c", 0xC9);   // put the special bendy corner
   gotoxy(1,24); printf("%c", 0xC8);  // dinguses in the four corners,
   gotoxy(80,1); printf("%c", 0xBB);
   gotoxy(80,24); printf("%c", 0xBC);
   for (i=2;i<80;i++) 
   {
      gotoxy(i,24); printf("%c", 0xCD); // draw the bottom line,
   }
   for (i=2;i<title_x;i++)
   {
      gotoxy(i,1); printf("%c", 0xCD); // and the top line,
   }
   printf("DUNHAM");                    // and put in the program's title,
   for (i=wherex();i<80;i++)
   {
      gotoxy(i,1); printf("%c", 0xCD); // then finish the top line,
   }
   for (i=2;i<24;i++)
   {
      gotoxy(1,i); printf("%c", 0xBA);   // and do the left line,
      gotoxy(80,i); printf("%c", 0xBA);  // and the right.
   }
   gotoxy(x,y);
}

void EraseLine( int linenumber )
{
   gotoxy(2,linenumber);
   printf("                                       ");
   printf("                                       "); 
}
