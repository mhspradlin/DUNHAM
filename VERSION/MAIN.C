// main.c 

// The Dunham's PC-based user interface.  Normally you would use the  
// Macintosh user interface program to run the Dunham Telescope, but  
// the PC also has its own (rudimentary) user interface for emergency use.    

// Barbara Carter  July 28, 1994 

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <math.h>
#include "globals.h"
#include "tracking.h"
#include "slew.h"
#include "utils.h"

#define ExitProgram    0
#define MainMenu       1
#define ZenithMenu     6 
#define TrackingMenu   7
#define SlewMenu       8
#define SlewingMenu   13
#define MotorMenu     14
#define MotorTestMenu 15
#define SorryMenu     20

#define CHAROFFSET -48
#define BACKSPACE    8

#define NoNumber          2 
#define NumberAtEndOfLine 1
#define MoreToCome        0

#define DELAYFACTOR 20 // number of times to run the loop in DoTrackingMenu
// before calling DisplayCoordinates, because printf takes too long to do
// every single time.

typedef struct
{
   double hour;
   double minute;
   double second;
} Coordinate;

void DoMenu( int *menu );
void DoMainMenu( int *menu );
void DoZenithMenu( int *menu );
void DoTrackingMenu( int *menu );
void DoSlewMenu( int *menu );
void DoSlewingMenu( int *menu );
void DoMotorMenu( int *menu );
void DoMotorTestMenu( int *menu );
void DoSorryMenu( int *menu );
void DoExitProgram( int *menu );
void StartSlewing( void );

void Initialize( void );
void GetSelection( int *menu, int max );
void DisplayCoordinates( int y );
void GetCoordinate( Coordinate *C );
int GetSign( int *sign );
int GetNextNumber( double* Num );
void GoToNextNumber( int *letter );
void ClearKeyboard( void );

Coordinate  RA, Dec;

void main(void)
{
   int menu=MainMenu;

   Initialize();
   while ( menu >= 0 )
   {
      DoMenu( &menu );
   }
}

void DoMenu( int* menu )
{
   switch (*menu)
   {
      case ExitProgram:   DoExitProgram(menu);   break;
      case MainMenu:      DoMainMenu(menu);      break;
      case MotorMenu:     DoMotorMenu(menu);     break;
      case MotorTestMenu: DoMotorTestMenu(menu); break;
      case ZenithMenu:    DoZenithMenu(menu);    break;
      case TrackingMenu:  DoTrackingMenu(menu);  break;     
      case SlewMenu:      DoSlewMenu(menu);      break;
      case SlewingMenu:   DoSlewingMenu(menu);   break;   
      default:            DoSorryMenu(menu);     break;
   }
}

void DoMainMenu( int *menu )
{  
   int letter, y=8;

   clrscr();
   gotoxy(32,y);   printf("Dunham Telescope");
   gotoxy(32,y+1); printf(" User Interface ");
   gotoxy(31,y+4); printf("1 - Start Program");
   gotoxy(31,y+5); printf("2 - Test Motors");
   gotoxy(31,y+6); printf("3 - Exit Program");
   gotoxy(29,y+8);
   GetSelection(menu,3);
   switch(*menu)
   {
      case 1: *menu=ZenithMenu;  break;
      case 2: *menu=MotorMenu;   break;
      case 3: *menu=ExitProgram; break;
      default: *menu=SorryMenu;  break;
   }
}

void DoMotorMenu( int *menu )
{
   clrscr();
   gotoxy(35,8); printf("Motor Test");
   gotoxy(29,10); printf("X: %7d",gX);
   gotoxy(29,11); printf("Y: %7d",gY);
   gotoxy(29,14); printf("1 - Test Motors");
   gotoxy(29,15); printf("2 - Return to Main Menu");
   gotoxy(29,17);
   GetSelection(menu,2);
   switch (*menu)
   {
      case 1:  *menu=MotorTestMenu; break;
      case 2:  *menu=MainMenu;      break;
      default: *menu=SorryMenu;     break;
   }
}

void DoMotorTestMenu( int *menu )
{
   int okay=0, escape=0, sign=1, err;
   double number;

   *menu = MotorMenu;
   clrscr();
   gotoxy(35,8); printf("Motor Test");
   gotoxy(29,10); printf("X: %7d",gX);
   gotoxy(29,11); printf("Y: %7d",gY);
   while (!escape && !okay)
   {
      gotoxy(29,15); printf("Enter distance to go in X and Y...");
      gotoxy(29,16); printf("(return to escape)");
      gotoxy(29,17); printf("Enter distance to go in X: ");
      number=0;
      if ( !(err = GetSign( &sign )) ) 
      {
         err = GetNextNumber( &number );
         if (err!=NoNumber) 
         {
            okay=1;
            dX = (long)sign * number;
         }
      }
      if (err==NoNumber) escape=1;
   }
   okay=0;
   while (!escape && !okay)
   {
      gotoxy(29,15); printf("Enter distance to go in X and Y...");
      gotoxy(29,16); printf("(return to escape)");
      gotoxy(29,17); printf("      Distance to go in X: %d", dX);
      gotoxy(29,18); printf("Enter distance to go in Y: ");
      number=0;
      if ( !(err = GetSign( &sign )) ) 
      {
         err = GetNextNumber( &number );
         if (err!=NoNumber) 
         {
            okay=1;
            dY = (long)sign * number;
         }
      }
      if (err==NoNumber) escape=1;
   }
   printf("dX=%ld, dY=%ld.", dX, dY);
   if (!escape) Slew();
}

void DoZenithMenu( int *menu )
{
   int i,y=8;

   clrscr();
   gotoxy(34,y); printf("Zenith Check");
   gotoxy(8,y+2); printf("It's important that the Dunham telescope be aligned with Zenith\n");
   gotoxy(8,y+3); printf("before proceeding.  Otherwise, all calculations will be in error.\n");
   gotoxy(21,y+6); printf("1 - The Dunham telescope is at Zenith");
   gotoxy(21,y+7); printf("2 - Go back to main menu");
   gotoxy(29,y+9);
   GetSelection(menu,2);
   switch (*menu)
   {
      case 1:  *menu=TrackingMenu; break;
      case 2:  *menu=MainMenu;     break;
      default: *menu=SorryMenu;    break;
   }
}

void DoTrackingMenu( int *menu )
{
   int x=28, y=8, xx, yy, firstxx, letter, menuset=0, Return='\r';
   int i=DELAYFACTOR;

   clrscr();
   DoTracking();
   gotoxy(x+7,y);    printf("Tracking");
   gotoxy(x-2,y+8);  printf("1 - Slew to new coordinates");
   gotoxy(x-2,y+9);  printf("2 - Stop tracking");
   gotoxy(x-2,y+11); printf("Enter your selection: ");
   xx=wherex(); yy=wherey(); firstxx=xx;
   do
   {
      do
      {
         TrackCalc();
         if ((++i)>DELAYFACTOR) 
         {
            i=0;
            DisplayCoordinates(y+2);  // only re-display every so often.
         }
         gotoxy(xx,yy);
      }  while ( !kbhit() );
      letter = getche();
      switch(letter)
      {
         case '\r':      break;
         case BACKSPACE: if (xx > firstxx) 
                            { gotoxy(--xx,yy); printf(" "); } 
                         break;
         default: xx++;
                  if (letter=='1' || letter=='2')
                  {
                     *menu = letter + CHAROFFSET;
                     menuset = 1;
                  }
      }
   }  while ( !(letter==Return && menuset) );
   switch (*menu)
   {
      case 1: *menu=SlewMenu; break;
      case 2: *menu=MainMenu; break;
   }
}

void DoSlewMenu( int *menu )
{
   int  x=28, y=8, xx, yy, yt, okay=0, escape_flag=0, sign=1, letter;
   double  remainder;

   while( !escape_flag && !okay )
   {
      clrscr();
      gotoxy(x+7,y); printf("Tracking");
      DisplayCoordinates(y+2);
      yt=y+7;
      gotoxy(x,++yt); printf("Enter Slew Coordinates... ");
      gotoxy(x,++yt); printf("(hit return to escape)");
      yt++;
      gotoxy(x,++yt); printf("    RA: ");
      RA.hour=(double)(-1); 
      RA.minute=(double)(-1); 
      RA.second=(double)(-1);
      GetCoordinate( &RA );
      if ( RA.hour<0 && RA.minute<0 && RA.second<0 ) escape_flag=1;
      else
      {
         escape_flag=0;
         if (RA.hour<0)   RA.hour=(double)0;
         if (RA.minute<0) RA.minute=(double)0;
         if (RA.second<0) RA.second=(double)0;
         do
         {
            okay=0;
            gotoxy(21,yt+1); 
            printf("You entered %2.0f:%02.0f:%02.0f for the new RA.", RA.hour, RA.minute, RA.second);
            gotoxy(25,yt+2); printf("Is this what you want? (y/n): ");
            xx=wherex(); yy=wherey();
            letter=getchar();
            if (letter=='\n') gotoxy(xx,yy);
            else ClearKeyboard();
            if (letter=='N') letter='n';
            if (letter=='Y') letter='y';
            if (letter=='y') okay=1;
         } while (letter!='y' && letter!='n');
      } //else
   } //while
   if (!escape_flag)
   {
      clrscr();
      gotoxy(x+7,y); printf("Tracking");
      okay = 0;
      while ( !okay && !escape_flag )
      {
         clrscr();
         gotoxy(x+7,y); printf("Tracking");
         DisplayCoordinates(y+2);
         yt=y+7;
         gotoxy(x,++yt); printf("Enter Slew Coordinates... ");
         gotoxy(x,++yt); printf("(hit return to escape)");
         yt++;
         gotoxy(x,++yt); printf("    RA:  %2.0f:%02.0f:%02.0f", RA.hour, RA.minute, RA.second);
         gotoxy(x,++yt); printf("    Dec: ");
         Dec.hour = (double)(-1);
         Dec.minute=(double)(-1); 
         Dec.second=(double)(-1);
         if ( GetSign( &sign ) ) escape_flag=1;
         else  GetCoordinate( &Dec );
         if (Dec.hour<0 && Dec.minute<0 && Dec.second<0) escape_flag=1;
         else
         {
            if (Dec.hour<0)   Dec.hour = (double)0;
            if (Dec.minute<0) Dec.minute=(double)0;
            if (Dec.second<0) Dec.second=(double)0;
            Dec.hour = Dec.hour*sign;
            do
            {
               okay=0;
               gotoxy(21,yt+1); 
               printf("You entered %2.0f:%02.0f:%02.0f for the new Dec.", Dec.hour, Dec.minute, Dec.second);
               gotoxy(25,yt+2); printf("Is this what you want? (y/n): ");
               xx=wherex(); yy=wherey();
               letter=getchar();
               if (letter=='\n') gotoxy(xx,yy);
               else ClearKeyboard();
               if (letter=='N') letter='n';
               if (letter=='Y') letter='y';
               if (letter=='y') okay=1;
            } while (letter!='y' && letter!='n');
         } //else
      } //while
   } //if
   if (escape_flag) *menu=TrackingMenu;
   else *menu=SlewingMenu;
}

void DoSlewingMenu( int *menu )
{
   int x=28, y=8, xx, yy, done=0, hit, letter;
   double RA1, Dec1;

   clrscr();
   gotoxy(x+7,y); printf("Slewing...");
   gotoxy(22,y+8); printf("Slewing to coordinates RA:%3.0f:%02.0f:%02.0f",
      RA.hour, RA.minute, RA.second);
   gotoxy(22,y+9); printf("                      Dec:%+3.0f:%02.0f:%02.0f",
      Dec.hour, Dec.minute, Dec.second);
   gotoxy(16,y+11); printf("Press Q to halt slewing and return to tracking: ");
   RA1 = RA.hour + ( RA.minute + (RA.second/60.0) ) / 60.0 ;
   Dec1 = Dec.hour + ( Dec.minute + (Dec.second/60.0) ) / 60.0 ;
   DoSlew(RA1, Dec1);
   xx=wherex(); yy=wherey();
   do
   {
      do
      {
         DisplayCoordinates(y+2);
         gotoxy(xx,yy);
         delay(100);
         hit = kbhit();
         done = ( Status==TRACKING ? 1 : 0 );
      }  while ( !hit && !done );
      if (hit) 
      {
         letter = getche();
         if (letter=='Q') letter='q';
      }
      else letter='0';
   }  while (letter!='q' && !done);
   Status=TRACKING;
   *menu = TrackingMenu;
}

void DoSorryMenu( int *menu )
{
   clrscr();
   gotoxy(20,20); printf("Sorry, that feature is not implemented.");
   gotoxy(20,21); printf("(hit <return> to continue) ");
   while ( getchar() != '\n' ) ;
   *menu = MainMenu;
}

void DoExitProgram( int *menu )
{
   switch(Status)
   {
      case SLEWING:
      {
         x_index = max( x_index, last_index-x_index );
         y_index = max( y_index, last_index-y_index );
         break;
      }
      case TRACKING:
      {
         setvect(8,stop_tracking_ticks);
         delay(10);
      }
   }
   clrscr();
   gotoxy(10,12);
   printf("Thank you for using the Dunham telescope PC user interface.");
   gotoxy(1,24);
   *menu=-1;
}

void Initialize( void )
{
   Status=STOPPED;
   gX=0L;
   gY=0L;
   outp( IOCNTLPORT, A_OUTPUT + B_OUTPUT + C_INPUT ); 
      // initialize the I/O board
   outp( PADDLEPORT, PADDLEWORD );
      // initialize the Paddle;
   old_int8 = getvect(8);
      // this is so we can reset the original clock-tick routine 
      // when we're done with this program. 
   SlewSetUp();  // sets up global variables for use when slewing.
}

void GetSelection( int *menu, int max )
{
   int x=wherex(), y=wherey(), letter;

   do 
   {
      gotoxy(x,y);   delline(); 
      gotoxy(x,y+1); delline();
      gotoxy(x,y); printf("Enter your selection: ");
      letter=getchar();
   } while ( (letter<'1') || (letter>(max-CHAROFFSET)) );
   *menu = letter+CHAROFFSET;
   ClearKeyboard();
}

void DisplayCoordinates( int y )
{
   double RA, Dec, Alt, Az, HA;

      // GetRADec( &gX, &gY, &RA, &Dec );
   XYtoAltAz( &gX, &gY, &Alt, &Az );
   ConvertCoordinates( &Az, &Alt, &HA, &Dec );
   HA = HA/(double)15;
   RA = RightAscension( &HA );
   gotoxy(32,y+0); printf("RA: %4d:%02d:%02d", _HOURS(RA), _MINUTES(RA), _SECONDS(RA));
   gotoxy(32,y+1); printf("Dec:%+4d:%02d:%02d", _SIGN(Dec)*_HOURS(Dec), _MINUTES(Dec), _SECONDS(Dec));
   gotoxy(32,y+2); printf("X:   %+9ld", gX);
   gotoxy(32,y+3); printf("Y:   %+9ld", gY);
}

void GetCoordinate( Coordinate *C )
{
   double remainder;
   int letter, err;

   err = GetNextNumber( &(C->hour) );
   if (!err) err = GetNextNumber( &(C->minute) );
   if (!err) err = GetNextNumber( &(C->second) );
   if (err==MoreToCome) ClearKeyboard();
   if (C->minute > 60.0) C->minute=(double)0;
   if (C->second > 60.0) C->second=(double)0;
   C->second = (double)((int)(C->second));
   if ((remainder=(C->minute-(int)(C->minute))) > 0.0)
   {
      C->minute = (double)((int)(C->minute));
      C->second += (remainder*60.0);
      while (C->second>60.0)
      {
         (C->second) -= 60.0;
         (C->minute)++;
      }
      while (C->minute>60.0)
      {
         (C->minute) -= 60.0;
         (C->hour)++;
      }
      if (C->hour>90.0) C->hour = 90.0;
   }
}

int GetSign( int *sign )
{
   int letter, err=0;

   *sign=1;
   do letter = getchar();
   while ( (letter<'0' || letter>'9') && letter!='\n' && letter!='-' && letter!='+');
   if (letter=='-') *sign=-1;
   if (letter=='\n') err=2;
   if (letter>='0' && letter<='9') ungetc(letter,stdin);
   return( err );
}

int GetNextNumber( double* Num )
// Function that reads the user's input (keyboard) and finds the next 
// number (integer or floating-point) with any # of sig-figs.         
// A status indicator is returned.                                    
{
   int letter, i=0;
   double number=(double)0;
   double decimal=(double)0;

   GoToNextNumber( &letter );
   if (letter=='\n' || letter==NULL) return( NoNumber );
   do  // read until the next non-numeric character 
   {                               
      number = 10*number + (double)(letter+CHAROFFSET);
      letter = getchar();
   } while ( letter>='0' && letter<='9' ); 
   *Num = number;
   if (letter=='\n' || letter==NULL) return( NumberAtEndOfLine );
   else if (letter!='.')
   {
      return ( MoreToCome );
   }
   letter = getchar();
   if (letter=='\n') return( NumberAtEndOfLine );
   else if (letter<'0' || letter>'9') return( MoreToCome );
   do
   {
      decimal += (double)(letter+CHAROFFSET)/pow(10,++i);
      letter = getchar();
   } while ( letter>='0' && letter<='9' );
   *Num += decimal;
   if (letter=='\n' || letter==NULL) return( NumberAtEndOfLine );
   else return( MoreToCome );
}

void GoToNextNumber( int *letter )
{
   do  // skip to the first numeric character 
   {
      *letter = getchar();
   }  while ( (*letter<'0' || *letter>'9') && *letter != '\n' );
}

void ClearKeyboard( void )
// Empties the keyboard buffer. 
{
   while ( (getchar()) != '\n') ;
}

