// globals.c

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
long int   cumulative[LEVELS+1];
int        DaylightSavings=1;
Coordinate RA, Dec;
int        abort_slew_flag=0;
int        keyboard_buffer=0;
int        Xpaddle=0, Ypaddle=0;
int        ticks=0, CoordinatesInitialized=0;  
long int   Xoffset=0L, Yoffset=0L;
double     RA_Orig, DecOrig;
int        UpArrow=0, DnArrow=0, LfArrow=0, RtArrow=0;
long int   Jog_Angle=15;
int        ShowXY=0, FastSlew=1;
long int   MaxDelay=100, MinDelay=24; // determines velocity at each level.
long int   MaxSteps=250L, MinSteps=6L;  // determines duration of each level.
long int   FastPaddleSpeed=4L, SlowPaddleSpeed=50L;
long int   Levels=LEVELS;
                                     
// **************************** isr prototypes *****************************

void interrupt (*oldclk)(void);
void interrupt (*oldkb)(void);
