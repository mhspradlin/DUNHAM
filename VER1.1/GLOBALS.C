// globals.c

// version 1.1

#include "globals.h"

// *************************** global variables **************************** 

long int   gX=0, gY=0;
long int   dX=0, dY=0;
double     RA_Target;
double     DecTarget;
long int   TOD=0L;
double     TimeSinceLastClock=ZERO, StartingUT, T0;
long int   x_max_index, y_max_index;
long int   x_cruise_distance, y_cruise_distance;
long int   x_sign, y_sign;
int        Status, PrevStatus, busy=0;
long int   x_index, y_index;
long int   cumulative[MAXLEVELS+1];
int        DaylightSavings=1;
Coordinate RA, Dec;
int        abort_slew_flag=0;
int        keyboard_buffer=0;
int        Xpaddle=0, Ypaddle=0;
int        ticks=0, HaveBeenTracking=0;  
long int   Xoffset=0L, Yoffset=0L;
double     RA_Orig, DecOrig;
int        UpArrow=0, DnArrow=0, LfArrow=0, RtArrow=0;
long int   Jog_Angle=15;
int        ShowXY=0, FastSlew=1;
long int   MaxDelay=100, MinDelay=24; // determines velocity at each level.
long int   MaxSteps=250L, MinSteps=6L;  // determines duration of each level.
long int   DisplayDelay=20L; // how often to re-display coord's in tracking  
long int   SlowPaddleSpeed=4L;
long int   Levels=24L, MaxRange=10;
int        ScreenDark;
long int   SecsToDarken=60L, TimeToDarken, TimeOfLastKey;
long int   CloseEnough=1333L;
int        oldShowXY, oldFastSlew;
long int   oldMaxRange, oldSlowPaddleSpeed;
long int   oldCloseEnough, oldTimeToDarken;
long int   oldMaxDelay, oldMinDelay, oldMinSteps, oldMaxSteps;
long int   oldLevels;
long int   WidenPulse=0L, WideEnough=0L;
int        E_W=-1, oldE_W=-1;
                                     
// **************************** isr prototypes *****************************

void interrupt (*oldclk)(void);
void interrupt (*oldkb)(void);
