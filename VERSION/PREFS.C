// prefs.c

// All preferences are changed with these routines.

// Barbara Carter    October, 1994

#include <stdio.h>
#include <conio.h>
#include "prefs.h"
#include "globals.h"
#include "utils.h"
#include "motor.h"

void DoPreferencesMenu( int *menu )
// Allows the user to change various settings affecting the speed of the
// motors, what is displayed, etc.
{
   int  x=10, y=7, diff;

   if (!TimeToDarken) x+=5;
   else
   {
      if (SecsToDarken>=1000L) x--;
      if (SecsToDarken>=100000L) x--;
      if (SecsToDarken>=10000000L) x--;
   }
   clrscr();
   FrameScreen();
   gotoxy(35,y); printf("Preferences");
   gotoxy(x,y+3); printf("1 - Change Show-X-and-Y setting (");
   if (ShowXY) printf("show)"); else printf("don't show)");
   gotoxy(x,y+4); printf("2 - Change Slew Style (");
   if (FastSlew) printf("faster slew)"); else printf("slower slew)");
   gotoxy(x,y+5); 
   printf("3 - Change Fast paddle button speed (%1.1f steps/sec)", 
               (double)RATE/((double)TRACKTICK*(double)FastPaddleSpeed*2.0) );
   gotoxy(x,y+6); 
   printf("4 - Change Slow paddle button speed (%1.1f steps/sec)", 
               (double)RATE/((double)TRACKTICK*(double)SlowPaddleSpeed*2.0) );
   gotoxy(x,y+7); 
   printf("5 - Change slew acceleration curve parameters...");
   gotoxy(x,y+8); 
   printf("6 - Change Slew Accuracy setting (%ld arcsec)", 
         round( ((double)CloseEnough)*3600.0/((double)STEPSPERDEGREE) ) );
   gotoxy(x,y+9);
   printf("7 - Change Screen-Saver setting (");
   if (TimeToDarken) printf("darken screen after %ld seconds)", SecsToDarken);
   else              printf("never darken screen)");
   gotoxy(x,y+10);
   printf("8 - Return to Main Menu");
   gotoxy(29,y+12);
   GetSelection(menu,8);
   switch(*menu)
   {
      case 1: *menu=ShowXYMenu;       break;
      case 2: *menu=SlewSpeedMenu;    break;
      case 3: *menu=FastPaddleMenu;   break;
      case 4: *menu=SlowPaddleMenu;   break;
      case 5: *menu=AccelerationMenu; break;
      case 6: *menu=AccuracyMenu;     break;
      case 7: *menu=DarkTimeMenu;     break;
      case 8: *menu=MainMenu;         
              diff=0;
              if ( ShowXY!=oldShowXY || FastSlew!=oldFastSlew ) diff=1;
              if ( FastPaddleSpeed!=oldFastPaddleSpeed ) diff=1;
              if ( SlowPaddleSpeed!=oldSlowPaddleSpeed ) diff=1;
              if ( CloseEnough!=oldCloseEnough ) diff=1;
              if ( TimeToDarken!=oldTimeToDarken ) diff=1;
              if ( MaxDelay!=oldMaxDelay || MinDelay!=oldMinDelay ) diff=1;
              if ( MinSteps!=oldMinSteps || MaxSteps!=oldMaxSteps ) diff=1;
              if ( Levels!=oldLevels ) diff=1;
              if (diff)
              {
                 SavePrefsFile();
              }
              break;
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
   int letter, done=0, Up=72, Down=80, x=10;
   long int tickspersec;
   double speed;

   *menu = PreferencesMenu;
   speed = (double)RATE/((double)TRACKTICK*(double)FastPaddleSpeed*2.0);
   tickspersec = round( 18.2*(double)FACTOR/2.0 );
   clrscr();
   FrameScreen();
   gotoxy(28,8); printf("Fast Paddle Button Speed");
   gotoxy(x,10); 
   printf("This sets the speed for the paddle when the upper right-hand ");
   gotoxy(x,11);
   printf("red button is in (LED is on).  Only certain values are valid.");
   gotoxy(x,12);
   printf("Use the %c and %c arrow keys to select a faster or slower value.",
         24,25);
   gotoxy(x,14);
   printf("Current setting is: %1.1f", speed);
   do
   {
      EraseLine(16); EraseLine(17); EraseLine(18); EraseLine(19);
      gotoxy(x,16);
      printf("Use arrow keys.  Hit return when done, to save setting.");
      gotoxy(x,17);
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
   int letter, done=0, Up=72, Down=80, x=9;
   long int tickspersec;
   double speed;

   *menu = PreferencesMenu;
   speed = (double)RATE/((double)TRACKTICK*(double)SlowPaddleSpeed*2.0);
   tickspersec = round( 18.2*(double)FACTOR/2.0 );
   clrscr();
   FrameScreen();
   gotoxy(28,8); printf("Slow Paddle Button Speed");
   gotoxy(x,10); 
   printf("This sets the speed for the paddle when the upper right-hand ");
   gotoxy(x,11);
   printf("red button is out (LED is off).  Only certain values are valid.");
   gotoxy(x,12);
   printf("Use the %c and %c arrow keys to select a faster or slower value.",
         24,25);
   gotoxy(x,14);
   printf("Current setting is: %1.1f", speed);
   do
   {
      EraseLine(16); EraseLine(17); EraseLine(18); EraseLine(19);
      gotoxy(x,16);
      printf("Use arrow keys.  Hit return when done, to save setting.");
      gotoxy(x,17);
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
   int x=27;

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
   int x=9, y=8, err;
   long int arcsec;
   double number;

   *menu=PreferencesMenu;
   arcsec = round( ((double)CloseEnough)*3600.0/((double)STEPSPERDEGREE) );
   clrscr();
   FrameScreen();
   gotoxy(31,y); printf("Slew Accuracy Setting");
   gotoxy(x,y+2); 
   printf("This determines how close the slew needs to get to the given");
   gotoxy(x,y+3);
   printf("coordinates before returning to tracking.  A smaller value");
   gotoxy(x,y+4);
   printf("(closer) will force the slew routine to do more iterations."); 
   gotoxy(x,y+5);
   printf("Default setting is 300 arcseconds.");  
   gotoxy(x,y+7);
   printf("Current setting: %ld arcseconds", arcsec);
   gotoxy(x,y+9);
   printf("Enter the slew accuracy in arcseconds"); 
   gotoxy(x,y+10);
   printf("(Hit Return to keep the current value): ");
   number=ZERO;
   err = GetNextNumber( &number );
   if (err != NoNumber)  // got valid number
   {
      if (number>0)
      {
         CloseEnough = round( number*(double)STEPSPERDEGREE/3600.0 );
      }
      while (err==MoreToCome) err=GetNextNumber(&number);
   }
}

void DoMaxDelayMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80, x=11, y=7;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,y); printf("Change MaxDelay");
   gotoxy(x,y+2); 
   printf("\"Delay\" is the number of times the computer goes through a ");
   gotoxy(x,y+3);
   printf("busy wait loop before sending the next pulse to the motor.");
   gotoxy(x,y+4);
   printf("The MaxDelay value is the number of delay loops to perform");
   gotoxy(x,y+5);
   printf("when on the lowest velocity level-- hence, the biggest delay");
   gotoxy(x,y+6);
   printf("which creates the slowest speed (\"base speed\" of the motors).");
   gotoxy(x,y+7);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(x,y+8);
   printf("(Remember: the bigger the delay, the lower the velocity.)");
   gotoxy(x,y+10);
   printf("Current setting is: %ld", MaxDelay);
   do
   {
      EraseLine(y+12); EraseLine(y+13); EraseLine(y+14);
      gotoxy(x,y+12);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(x,y+13);
      printf("MaxDelay: %ld", MaxDelay);
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
         }
         if ( letter==Down && MaxDelay>1L ) 
         {
            MaxDelay--;
         }
      }
   } while (!done);
}

void DoMinDelayMenu( int *menu )
{
   int letter, done=0, Up=72, Down=80, x=11, y=7;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,y); printf("Change MinDelay");
   gotoxy(x,y+2); 
   printf("\"Delay\" is the number of times the computer goes through a ");
   gotoxy(x,y+3);
   printf("busy wait loop before sending the next pulse to the motor.");
   gotoxy(x,y+4);
   printf("The MinDelay value is the number of delay loops to perform");
   gotoxy(x,y+5);
   printf("when on the highest velocity level-- hence, the smallest delay");
   gotoxy(x,y+6);
   printf("which creates the fastest speed-- the slewing speed.");
   gotoxy(x,y+7);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(x,y+8);
   printf("(Remember: the bigger the delay, the lower the velocity.)");
   gotoxy(x,y+10);
   printf("Current setting is: %ld", MinDelay);
   do
   {
      EraseLine(y+12); EraseLine(y+13); EraseLine(y+14);
      gotoxy(x,y+12);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(x,y+13);
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
   int letter, done=0, Up=72, Down=80, x=11, y=7;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,y); printf("Change MinSteps");
   gotoxy(x,y+2); 
   printf("\"Steps\" is the number of pulses that are sent to the motor ");
   gotoxy(x,y+3);
   printf("at a given velocity level during acceleration or deceleration.");
   gotoxy(x,y+4);
   printf("The MinSteps value is the number of steps sent at the lowest");
   gotoxy(x,y+5);
   printf("velocity level, the \"base speed,\" before going to the next");
   gotoxy(x,y+6);
   printf("velocity level.");
   gotoxy(x,y+7);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(x,y+8);
   printf("(Remember: More steps means a slower acceleration.)");
   gotoxy(x,y+10);
   printf("Current setting is: %ld", MinSteps);
   do
   {
      EraseLine(y+12); EraseLine(y+13); EraseLine(y+14);
      gotoxy(x,y+12);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(x,y+13);
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
   int letter, done=0, Up=72, Down=80, x=11, y=7;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(33,y); printf("Change MaxSteps");
   gotoxy(x,y+2); 
   printf("\"Steps\" is the number of pulses that are sent to the motor ");
   gotoxy(x,y+3);
   printf("at a given velocity level during acceleration or deceleration.");
   gotoxy(x,y+4);
   printf("The MaxSteps value is the number of steps sent at the almost-");
   gotoxy(x,y+5);
   printf("highest velocity level, at the end of the acceleration.");
   gotoxy(x,y+6);
   printf("This value is important to keep the motors from choking.");
   gotoxy(x,y+7);
   printf("Use the %c and %c arrow keys to select a higher or lower value.",
         24,25);
   gotoxy(x,y+8);
   printf("(Remember: More steps means a slower acceleration.)");
   gotoxy(x,y+10);
   printf("Current setting is: %ld", MaxSteps);
   do
   {
      EraseLine(y+12); EraseLine(y+13); EraseLine(y+14);
      gotoxy(x,y+12);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(x,y+13);
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
   int letter, done=0, Up=72, Down=80, x=8, y=7;

   *menu = AccelerationMenu;
   clrscr();
   FrameScreen();
   gotoxy(24,y); printf("Change Number of Velocity Levels");
   gotoxy(x,y+2); 
   printf("The slew acceleration consists of several discrete \"levels,\"");
   gotoxy(x,y+3);
   printf("each level sending pulses slightly faster than the previous,");
   gotoxy(x,y+4);
   printf("until the fastest pulse rate is achieved.  This is also known");
   gotoxy(x,y+5);
   printf("as \"ramping up\" the velocity, and is called the \"acceleration");
   gotoxy(x,y+6);
   printf("curve.\"  The number of levels determines how smooth this is.");
   gotoxy(x,y+7);
   printf("Use the %c and %c arrow keys to select more or fewer levels.",
         24,25);
   gotoxy(x,y+8);
   printf("(Remember: More levels means a slower, more gradual acceleration.");
   gotoxy(x,y+10);
   printf("Current setting is: %ld", Levels);
   do
   {
      EraseLine(y+12); EraseLine(y+13); EraseLine(y+14);
      gotoxy(x,y+12);
      printf("Use arrow keys.  Hit return when done.");
      gotoxy(x,y+13);
      printf("Levels: %ld", Levels);
      while (!kbhit()) ;
      letter = getch();
      if (letter=='\n' || letter=='\r') 
      {
         done = 1;
      }
      else 
      {
         if ( letter==Up && Levels<MAXLEVELS) 
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

void DoDarkTimeMenu( int *menu )
{
   int err, x=9;
   double number;

   *menu = PreferencesMenu;
   clrscr();
   FrameScreen();
   gotoxy(26,8); printf("Tracking Screen-Saver Setting");
   gotoxy(x,10); 
   printf("This sets whether the screen will go dark after a set amount ");
   gotoxy(x,11);
   printf("of time, and how long it will wait to go dark.  This should");
   gotoxy(x,12);
   printf("help prevent screen \"burn in\" while tracking, if the telescope"); 
   gotoxy(x,13);
   printf("is used for long periods of time.  Default is wait 4 minutes.");  
   gotoxy(x,15);
   printf("Current setting: ");
   if (TimeToDarken) printf("darken screen after %ld seconds.", SecsToDarken);
   else printf("never darken screen.");
   gotoxy(x,17);
   printf("Enter the number of seconds to wait "); 
   printf("until darkening the screen");
   gotoxy(x,18);
   printf("(Return to keep same setting; 0 to turn off screen-saver): ");
   number=ZERO;
   err = GetNextNumber( &number );
   if (err != NoNumber)  // got valid number
   {
      if (number>0)
      {
         SecsToDarken = round(number);
         TimeToDarken = SecsToDarken*FACTOR*18L;
      }
      else
      {
         SecsToDarken = 0L;
         TimeToDarken = 0L;
      }
      while (err==MoreToCome) err=GetNextNumber(&number);
   }
}

void SavePrefsFile( void )
{
   FILE *fileptr;

   fileptr = fopen("DUNHAM.DAT","w");
   if (fileptr)
   {
      fprintf( fileptr, "ShowXY %d\n", ShowXY );
      fprintf( fileptr, "FastSlew %d\n", FastSlew );
      fprintf( fileptr, "FastPaddleSpeed %ld\n", FastPaddleSpeed );
      fprintf( fileptr, "SlowPaddleSpeed %ld\n", SlowPaddleSpeed );
      fprintf( fileptr, "CloseEnough %ld\n", CloseEnough );
      fprintf( fileptr, "TimeToDarken %ld\n", TimeToDarken );
      fprintf( fileptr, "MaxDelay %ld\n", MaxDelay );
      fprintf( fileptr, "MinDelay %ld\n", MinDelay );
      fprintf( fileptr, "MinSteps %ld\n", MinSteps );
      fprintf( fileptr, "MaxSteps %ld\n", MaxSteps );
      fprintf( fileptr, "Levels %ld\n", Levels );
      fclose(fileptr);
   }
}
