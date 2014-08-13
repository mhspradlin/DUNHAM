// prefs.c

// All preferences are changed with these routines.

// Barbara Carter    October, 1994

#include <stdio.h>
#include <conio.h>
#include "prefs.h"
#include "globals.h"
#include "utils.h"

void DoPreferencesMenu( int *menu )
// Allows the user to change various settings affecting the speed of the
// motors, what is displayed, etc.
{
   int x=18;

   clrscr();
   FrameScreen();
   gotoxy(35,8); printf("Preferences");
   gotoxy(x,11); printf("1 - Change Show-X-and-Y setting (");
   if (ShowXY) printf("show)"); else printf("don't show)");
   gotoxy(x,12); printf("2 - Change Slew Style (");
   if (FastSlew) printf("faster slew)"); else printf("slower slew)");
   gotoxy(x,13); 
   printf("3 - Change Fast paddle button speed (%1.1f steps/sec)", 
               (double)RATE/((double)TRACKTICK*(double)FastPaddleSpeed*2.0) );
   gotoxy(x,14); 
   printf("4 - Change Slow paddle button speed (%1.1f steps/sec)", 
               (double)RATE/((double)TRACKTICK*(double)SlowPaddleSpeed*2.0) );
   gotoxy(x,15); 
   printf("5 - Change slew acceleration curve parameters");
   gotoxy(x,16); 
   printf("6 - Change Slew Accuracy setting (%ld arcsec)", CLOSE_ENOUGH);
   gotoxy(x,17);
   printf("7 - Return to Main Menu");
   gotoxy(29,19);
   GetSelection(menu,7);
   switch(*menu)
   {
      case 1: *menu=ShowXYMenu;       break;
      case 2: *menu=SlewSpeedMenu;    break;
      case 3: *menu=FastPaddleMenu;   break;
      case 4: *menu=SlowPaddleMenu;   break;
      case 5: *menu=AccelerationMenu; break;
      case 6: *menu=AccuracyMenu;     break;
      case 7: *menu=MainMenu;         break;
      default: *menu=SorryMenu;       break;
   }
}

void DoShowXYMenu( int *menu )
{  
   int letter, x=18;

   *menu = PreferencesMenu;
   clrscr();
   FrameScreen();
   gotoxy(28,8); printf("Show/Don't Show X and Y");
   gotoxy(x,10); 
   printf("This sets whether X and Y, the actual positions");
   gotoxy(x,11);
   printf("of the telescope motors, are displayed.");
   gotoxy(x,12);
   printf("(Units are \"steps,\" where 1 step = 0.225 arcsec).");  
   gotoxy(x,13);
   printf("This does not affect the RA and Dec display.");
   gotoxy(x,14);
   printf("Default is:  Don't Show X and Y");
   gotoxy(x,16);
   printf("Current setting is: ");
   if (ShowXY) printf("Show X and Y"); else printf("Don't Show X and Y");
   gotoxy(x,18);
   printf("Show X and Y?  (y/n)");
   gotoxy(x,19);
   printf("(Hit return to leave setting unchanged): ");
   letter=getchar();
   if (letter=='y' || letter=='Y') {ShowXY=1; ClearKeyboard();}
   else
   {
      if (letter=='n' || letter=='N') {ShowXY=0; ClearKeyboard();}
      else if (letter!='\n') ClearKeyboard();
   }
}

void DoSlewSpeedMenu( int *menu )
{
   int letter, x=8;

   *menu = PreferencesMenu;
   clrscr();
   FrameScreen();
   gotoxy(34,8); printf("Set Slew Style");
   gotoxy(x,10); 
   printf("This sets whether the telescope slews faster, without displaying");
   gotoxy(x,11);
   printf("X and Y coordinates, or slower, with X and Y updated in real time.");
   gotoxy(x,12);
   printf("Default is Faster, however Slower may be better if the motors are");
   gotoxy(x,13);
   printf("sticking due to cold weather.");
   gotoxy(x,15);
   printf("Current setting is: ");
   if (FastSlew) printf("Faster"); else printf("Slower");
   gotoxy(x,17);
   printf("Slew Slower or Faster? (s/f)");
   gotoxy(x,18);
   printf("(Hit return to keep same setting): ");
   letter=getchar();
   if (letter=='f' || letter=='F') {FastSlew=1; ClearKeyboard();}
   else
   {
      if (letter=='s' || letter=='S') {FastSlew=0; ClearKeyboard();}
      else if (letter!='\n') ClearKeyboard();
   }
}

void DoFastPaddleMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;
   long int tickspersec;
   double speed;

   *menu = PreferencesMenu;
   speed = (double)RATE/((double)TRACKTICK*(double)FastPaddleSpeed*2.0);
   tickspersec = round( 18.2*(double)FACTOR/2.0 );
   clrscr();
   FrameScreen();
   gotoxy(28,8); printf("Fast Paddle Button Speed");
   gotoxy(20,10); 
   printf("This sets the speed for the paddle when the upper right-hand ");
   gotoxy(20,11);
   printf("red button is in (LED is on).  Only certain values are valid.");
   gotoxy(20,12);
   printf("Use the %c and %c arrow keys to select a faster or slower value.",
         24,25);
   gotoxy(20,14);
   printf("Current setting is: %1.1f", speed);
   do
   {
      EraseLine(16); EraseLine(17); EraseLine(18); EraseLine(19);
      gotoxy(20,16);
      printf("Use arrow keys.  Hit return when done, to save setting.");
      gotoxy(20,17);
      printf("Fast Paddle Speed: %1.1f ticks per second", speed);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up && FastPaddleSpeed>4L ) 
         {
            FastPaddleSpeed--;
            printf("%c",24);
            speed = (double)RATE/((double)TRACKTICK*(double)FastPaddleSpeed*2.0);
         }
         if ( letter==Down && FastPaddleSpeed<tickspersec ) 
         {
            FastPaddleSpeed++;
            printf("%c",25);
            speed = (double)RATE/((double)TRACKTICK*(double)FastPaddleSpeed*2.0);
         }
      }
   } while (!done);
}

void DoSlowPaddleMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;
   long int tickspersec;
   double speed;

   *menu = PreferencesMenu;
   speed = (double)RATE/((double)TRACKTICK*(double)SlowPaddleSpeed*2.0);
   tickspersec = round( 18.2*(double)FACTOR/2.0 );
   clrscr();
   FrameScreen();
   gotoxy(28,8); printf("Slow Paddle Button Speed");
   gotoxy(20,10); 
   printf("This sets the speed for the paddle when the upper right-hand ");
   gotoxy(20,11);
   printf("red button is out (LED is off).  Only certain values are valid.");
   gotoxy(20,12);
   printf("Use the %c and %c arrow keys to select a faster or slower value.",
         24,25);
   gotoxy(20,14);
   printf("Current setting is: %1.1f", speed);
   do
   {
      EraseLine(16); EraseLine(17); EraseLine(18); EraseLine(19);
      gotoxy(20,16);
      printf("Use arrow keys.  Hit return when done, to save setting.");
      gotoxy(20,17);
      printf("Slow Paddle Speed: %1.1f ticks per second", speed);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up && SlowPaddleSpeed>4L ) 
         {
            SlowPaddleSpeed--;
            printf("%c",24);
            speed = (double)RATE/((double)TRACKTICK*(double)SlowPaddleSpeed*2.0);
         }
         if ( letter==Down && SlowPaddleSpeed<tickspersec ) 
         {
            SlowPaddleSpeed++;
            printf("%c",25);
            speed = (double)RATE/((double)TRACKTICK*(double)SlowPaddleSpeed*2.0);
         }
      }
   } while (!done);
}

void DoAccelerationMenu( int *menu )
{
   int x=18;

   clrscr();
   FrameScreen();
   gotoxy(27,8); printf("Slew Acceleration Parameters");
   gotoxy(x,11); printf("1 - Change MaxDelay (%ld)", MaxDelay);
   gotoxy(x,12); printf("2 - Change MinDelay (%ld)", MinDelay);
   gotoxy(x,13); printf("3 - Change MinSteps (%ld)", MinSteps);
   gotoxy(x,14); printf("4 - Change MaxSteps (%ld)", MaxSteps);
   gotoxy(x,15); printf("5 - Change Levels (%ld)", Levels);
   gotoxy(x,16); printf("6 - Return to Preferences Menu");
   gotoxy(29,18);
   GetSelection(menu,6);
   switch(*menu)
   {
      case 1: *menu=MaxDelayMenu;    break;
      case 2: *menu=MinDelayMenu;    break;
      case 3: *menu=MinStepsMenu;    break;
      case 4: *menu=MaxStepsMenu;    break;
      case 5: *menu=LevelsMenu;      break;
      case 6: *menu=PreferencesMenu; break;
      default: *menu=SorryMenu;      break;
   }
}

void DoAccuracyMenu( int *menu )
{
   *menu=SorryMenu;
}

void DoMaxDelayMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,8); printf("Change MaxDelay");
   gotoxy(20,10); 
   printf("\"Delay\" is the number of times the computer goes through a ");
   gotoxy(20,11);
   printf("busy wait loop before sending the next pulse to the motor.");
   gotoxy(20,12);
   printf("The MaxDelay value is the number of delay loops to perform");
   gotoxy(20,13);
   printf("when on the lowest velocity level-- hence, the biggest delay");
   gotoxy(20,14);
   printf("which creates the slowest speed (\"base speed\" of the motors).");
   gotoxy(20,15);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(20,16);
   printf("(Remember: the bigger the delay, the lower the velocity.)");
   gotoxy(20,18);
   printf("Current setting is: %ld", MaxDelay);
   do
   {
      EraseLine(20); EraseLine(21); EraseLine(22);
      gotoxy(20,20);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(20,21);
      printf("MaxDelay: ");
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up ) 
         {
            MaxDelay++;
            printf("%ld",MaxDelay);
         }
         if ( letter==Down && MaxDelay>1L ) 
         {
            MaxDelay--;
            printf("%ld",MaxDelay);
         }
      }
   } while (!done);
}

void DoMinDelayMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,8); printf("Change MinDelay");
   gotoxy(20,10); 
   printf("\"Delay\" is the number of times the computer goes through a ");
   gotoxy(20,11);
   printf("busy wait loop before sending the next pulse to the motor.");
   gotoxy(20,12);
   printf("The MinDelay value is the number of delay loops to perform");
   gotoxy(20,13);
   printf("when on the highest velocity level-- hence, the smallest delay");
   gotoxy(20,14);
   printf("which creates the fastest speed-- the slewing speed.");
   gotoxy(20,15);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(20,16);
   printf("(Remember: the bigger the delay, the lower the velocity.)");
   gotoxy(20,18);
   printf("Current setting is: %ld", MinDelay);
   do
   {
      EraseLine(20); EraseLine(21); EraseLine(22);
      gotoxy(20,20);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(20,21);
      printf("MinDelay: %ld", MinDelay);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up ) 
         {
            MinDelay++;
         }
         if ( letter==Down && MinDelay>1L ) 
         {
            MinDelay--;
         }
      }
   } while (!done);
}

void DoMinStepsMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,8); printf("Change MinSteps");
   gotoxy(20,10); 
   printf("\"Steps\" is the number of pulses that are sent to the motor ");
   gotoxy(20,11);
   printf("at a given velocity level during acceleration or deceleration.");
   gotoxy(20,12);
   printf("The MinSteps value is the number of steps sent at the lowest");
   gotoxy(20,13);
   printf("velocity level, the \"base speed,\" before going to the next");
   gotoxy(20,14);
   printf("velocity level.");
   gotoxy(20,15);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(20,16);
   printf("(Remember: More steps at \"base speed\" means a ");
   printf("slower acceleration.)");
   gotoxy(20,18);
   printf("Current setting is: %ld", MinSteps);
   do
   {
      EraseLine(20); EraseLine(21); EraseLine(22);
      gotoxy(20,20);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(20,21);
      printf("MinSteps: %ld", MinSteps);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up ) 
         {
            MinSteps++;
         }
         if ( letter==Down && MinSteps>1L ) 
         {
            MinSteps--;
         }
      }
   } while (!done);
}

void DoMaxStepsMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,8); printf("Change MaxSteps");
   gotoxy(20,10); 
   printf("\"Steps\" is the number of pulses that are sent to the motor ");
   gotoxy(20,11);
   printf("at a given velocity level during acceleration or deceleration.");
   gotoxy(20,12);
   printf("The MaxSteps value is the number of steps sent at the almost-");
   gotoxy(20,13);
   printf("highest velocity level, at the end of the acceleration.");
   gotoxy(20,14);
   printf("This value is important to keep the motors from choking.");
   gotoxy(20,15);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(20,16);
   printf("(Remember: More steps near the end of the acceleration means a ");
   printf("slower acceleration.)");
   gotoxy(20,18);
   printf("Current setting is: %ld", MaxSteps);
   do
   {
      EraseLine(20); EraseLine(21); EraseLine(22);
      gotoxy(20,20);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(20,21);
      printf("MaxSteps: %ld", MaxSteps);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up ) 
         {
            MaxSteps++;
         }
         if ( letter==Down && MaxSteps>1L ) 
         {
            MaxSteps--;
         }
      }
   } while (!done);
}

void DoLevelsMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(24,8); printf("Change Number of Velocity Levels");
   gotoxy(20,10); 
   printf("The slew acceleration consists of several discrete \"levels\"");
   gotoxy(20,11);
   printf("each of which sends pulses at a constant rate, and each suc-");
   gotoxy(20,12);
   printf("ceeding level sends pulses at a slightly higher rate.");
   gotoxy(20,13);
   printf("The velocity of any given level is set by MaxDelay, MinDelay.");
   gotoxy(20,14);
   printf("Number of pulses sent at each level is set by MaxSteps, MinSteps");
   gotoxy(20,15);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(20,16);
   printf("(Remember: More levels means a slower, more gradual acceleration.");
   gotoxy(20,18);
   printf("Current setting is: %ld", Levels);
   do
   {
      EraseLine(20); EraseLine(21); EraseLine(22);
      gotoxy(20,20);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(20,21);
      printf("Levels: %ld", Levels);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up ) 
         {
            Levels++;
         }
         if ( letter==Down && Levels>1L ) 
         {
            Levels--;
         }
      }
   } while (!done);
}

