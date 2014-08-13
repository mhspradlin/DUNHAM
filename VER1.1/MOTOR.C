// motor.c

// Dunham routines for testing the motors and slewing functions.  (Part of
// the Dunham PC user interface program, not a stand-alone program.)

// Barbara Carter    September 6, 1994

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "globals.h"
#include "motor.h"
#include "slew.h"

//*********************** local function prototypes ***********************

int GetXYSlewValues( int x, int y ); 
int GetSign( int *sign );
void GoToNextNumber( int *letter );

//************************** motor test functions *************************

void DoMotorMenu( int *menu )
// Shows the user where the telescope is (it's stopped), and finds out if
// the user wishes to move the telescope ("test the motors").
{
   clrscr();
   FrameScreen();
   gotoxy(35,8);  printf("Motor Test");
   gotoxy(32,10); printf("  X: %+9ld", gX);
   gotoxy(32,11); printf("  Y: %+9ld", gY);
   gotoxy(29,14); printf("1 - Run the Motors");
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
// The user has indicated they want to test the motors.  This routine gets
// the distance the user wants the telescope to be moved, and then calls the
// routine to slew the telescope by that amount.  It does not allow the user
// to request the telescope move outside its allowed bounds.
{
   int escape=0, acceptable=0, i, prevShowXY;

   *menu = MotorMenu;
   do
   {
      clrscr();
      FrameScreen();
      gotoxy(35,8);  printf("Motor Test");
      gotoxy(32,10); printf("  X: %+9ld", gX);
      gotoxy(32,11); printf("  Y: %+9ld", gY);
      escape = GetXYSlewValues(25,15); // puts slew values in globals dX, dY
      if (!escape)
      {
         if ((pow((gX+dX),2)+pow((gY+dY),2)) > 
                                 pow(MAXZENITHANGLE*STEPSPERDEGREE,2) )
         {
            dX = 0; dY = 0;
            gotoxy(1,15);
            for (i=wherey();i<24;i++) EraseLine(i);
            FrameScreen();
            gotoxy(4,15);
            printf("The values you typed will put the telescope ");
            printf("outside its allowed bounds.");
            gotoxy(15,16); printf("It is not");
            printf(" allowed lower than a %ld degree", MAXZENITHANGLE);
            printf(" zenith angle.");                         
            gotoxy(28,17);
            printf("(press any key to continue)");
            while (!kbhit()) ;  // waits for user to type a key...
            getch();            // ...then swallows it.
         }
         else acceptable=1;
      }
   } while (!acceptable && !escape);

   if (!escape) 
   {
      PrevStatus = Status;
      Status = SLEWING;
      prevShowXY = ShowXY;
      ShowXY = 1;
      gotoxy(46,10); printf(" (slewing)");
      gotoxy(46,11); printf(" (slewing)");
      gotoxy(31,13); printf("Hit q to stop slewing.");
      Slew(37,10);   // moves the telescope the distance dX in X, and dY in Y.
      ShowXY = prevShowXY;
      PrevStatus = Status;
      Status = STOPPED;
   }
}

int GetXYSlewValues( int x, int y ) 
// Gets the distance to slew in X and Y (in "steps" of the motors).  Allows
// the user to escape by entering nothing.
{
   int    okay=0, err, escape=0, sign=1;        
   double number;

   while (!escape && !okay)
   {
      gotoxy(x,y); printf("Enter distance to go in X and Y...");
      gotoxy(x,y+1); printf("(return to escape)");
      gotoxy(x,y+2); printf("Enter distance to go in X: ");
      number=ZERO;
      if ( !(err = GetSign( &sign )) ) 
      {
         err = GetNextNumber( &number );
         if (err!=NoNumber) 
         {
            okay=1;
            dX = (long)(number * (long)sign);
         }
      }
      if (err==NoNumber) escape=1;
   }
   okay=0;
   while (!escape && !okay)
   {
      gotoxy(x,y); printf("Enter distance to go in X and Y...");
      gotoxy(x,y+1); printf("(return to escape)");
      gotoxy(x,y+2); printf("      Distance to go in X: %ld", dX);
      gotoxy(x,y+3); printf("Enter distance to go in Y: ");
      number=ZERO;
      if ( !(err = GetSign( &sign )) ) 
      {
         err = GetNextNumber( &number );
         if (err!=NoNumber) 
         {
            okay=1;
            dY = (long)(number * (long)sign);
         }
      }
      if (err==NoNumber) escape=1;
   }
   EraseLine(y);
   EraseLine(y+1);
   gotoxy(x,y+2); printf("      Distance to go in X: %ld", dX);
   gotoxy(x,y+3); printf("      Distance to go in Y: %ld", dY);
   return (escape);
}

int GetSign( int *sign )
// Looks for a "+" or "-" sign in front of the number the user entered.  If
// no sign is found, it is assumed to be "+".  Returns a status indicator,
// which tells whether there is truly a number there or not.
{
   int letter, err=0;

   *sign=1;
   do letter = getchar();
   while ( (letter<'0' || letter>'9') && letter!='\n' && 
                                          letter!='-' && letter!='+');
   if (letter=='-') *sign=-1;
   if (letter=='\n') err=NoNumber;
   if (letter>='0' && letter<='9') ungetc(letter,stdin);
   return( err );
}

int GetNextNumber( double* Num )
// Reads the user's input (keyboard) and finds the next number (integer or 
// floating-point) with any # of sig-figs.  A status indicator is returned.                                    
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
// Skips spaces, or other non-numeric characters, in the user's input, to
// get to the first numeric character in the keyboard buffer.
{
   do  // skip to the first numeric character 
   {
      *letter = getchar();
   }  while ( (*letter<'0' || *letter>'9') && *letter != '\n' );
}

