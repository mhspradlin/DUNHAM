// globals.h

// ************************ precompiler constants **************************

    // the 8253 timer chip 
#define CNTLPORT 0x43   // the 8253's control word register 
#define CH0 0x00
#define CH1 0x40
#define CH2 0x80
#define LATCH 0x00
#define WRITEHILO 0x30
#define WRITEHI   0x10
#define WRITELO   0x20
#define MODE0 0x00
#define MODE1 0x02
#define MODE2 0x04
#define MODE3 0x06
#define MODE4 0x08
#define MODE5 0x0A
#define CHANNEL0 0x40   // timer channel 0 (the clock-tick timer) 
#define CHANNEL2 0x42   // timer channel 2 (for timing the pulse width) 
#define CNTLWORD CH0+MODE2+WRITEHILO // Channel 0, mode 2, load hiword then loword 

#define RATE        1193180L  // the clock speed on the 8253 chip (1.19 MHz) 
#define SLOWTICK    65535L  // count value for normal (slow) clock-ticks 
#define FACTOR      51  // how many times faster the tracking ticks are.
#define TRACKTICK   SLOWTICK/(long)FACTOR  // count value for tracking ticks
#define SLOWHIWORD  0xFF
#define SLOWLOWORD  0xFF  // hi,low words of the normal (slow) count value 
#define TRACKHIWORD SLOWHIWORD/FACTOR
#define TRACKLOWORD SLOWLOWORD/FACTOR
#define TRACKTKTIME (double)TRACKTICK/(double)RATE //seconds per tracking-tick 
#define SLOWTKTIME  (double)SLOWTICK/(double)RATE  //seconds per normal tick

    // I/O board  
#define IOBASE         0x300
#define IOPORTA       IOBASE
#define IOPORTB     IOBASE+1
#define IOPORTC     IOBASE+2
#define IOCNTLPORT  IOBASE+3
#define PULSEPORT    IOPORTA
#define PADDLEPORT   IOPORTB
#define SLOWFASTPORT IOPORTC
#define PADDLEWORD      0x0F
#define A_OUTPUT        0x00
#define B_OUTPUT        0x00
#define C_OUTPUT        0x00
#define A_INPUT         0x10
#define B_INPUT         0x02
#define C_UPPER_INPUT   0x08
#define C_LOWER_INPUT   0x01
#define C_INPUT         0x09

#define XPLUS   1            // stepper motor controls  
#define XMINUS  2
#define YPLUS   4
#define YMINUS  8
#define XBITS   XPLUS+XMINUS
#define YBITS   YPLUS+YMINUS
#define NOPULSE XBITS+YBITS

    // telescope Status settings 
#define TRACKING 1
#define SLEWING  2
#define STOPPED  0

    // latitude and longitude, in degrees.
#define LATITUDE  41.79111111
#define LONGITUDE 87.60166666

    // slew values 
#define LEVELS 24L
#define DT           (double)0.1000          // time increment, in seconds 
//#define susie        (long)(V0*(double)LEVELS*(double)DT)
//#define last_index   susie*2L + 1L
//#define cruise_index susie

    // miscellaneous
#define STEPSPERDEGREE 16000L  // 16000 steps = 1 degree
#define MAXZENITHANGLE 75L //degrees
#define MAX 1000
#define SLOWSTEPSPERSEC  11.1111
#define FASTSTEPSPERSEC 266.6667
#define ZERO (double)0
#define ONE  (double)1
#define FIFTEEN (double)15
#define SIXTY (double)60
#define CHAROFFSET -48
#define BACKSPACE    8
#define CLOSE_ENOUGH 1500L

#define STEPSPERLEVEL 150L

#define ExitProgram       0
#define MainMenu          1
#define ZenithMenu        6 
#define TrackingMenu      7
#define ChangeJogMenu     8
#define RA_DecSlewMenu    9
#define SlewingMenu      13
#define MotorMenu        14
#define MotorTestMenu    15
#define OutOfBoundsMenu  18
#define PreferencesMenu  19
#define ShowXYMenu       20
#define SlewSpeedMenu    21
#define FastPaddleMenu   22
#define SlowPaddleMenu   23
#define AccelerationMenu 24
#define AccuracyMenu     25
#define MaxDelayMenu     26
#define MinDelayMenu     27
#define MinStepsMenu     28
#define MaxStepsMenu     29
#define LevelsMenu       30
#define SorryMenu        40

#define NoNumber          2 
#define NumberAtEndOfLine 1
#define MoreToCome        0

#define DELAYFACTOR 20 // number of times to run the loop in DoTrackingMenu
// before calling DisplayCoordinates, because printf takes too long to do
// every single time.

#define RETURN '\r'

#define TRACKINGDISPLAY 5   // which screen line to display "Tracking" title.

//********************************* macros **********************************

#define _HIWORD(LongInt) ( (LongInt) & 0xFF00 )
#define _LOWORD(LongInt) ( (LongInt) & 0x00FF )

#define _HOURS(num)   (int)fabs(num)
#define _MINUTES(num) (int)((fabs(num)-_HOURS(num))*60)
#define _SECONDS(num) (int)((fabs(num)-_HOURS(num))*3600-_MINUTES(num)*60)
#define _SIGN(num)    ((num<0?-1:1)*(num==0?0:1))

//******************************* Structures ******************************

typedef struct
{
   double hour;
   double minute;
   double second;
} Coordinate;

// *************************** global variables **************************** 

extern long int   gX, gY;
extern long int   dX, dY;
extern long int   x_max_index, y_max_index;
extern long int   x_cruise_distance, y_cruise_distance;
extern long int   x_sign, y_sign;
extern double     RA_Target;
extern double     DecTarget;
extern long int   TOD;
extern double     TimeSinceLastClock, StartingUT, T0;
extern int        Status;
extern int        busy; 
extern int        PrevStatus;
extern long int   x_index, y_index; 
extern long int   cumulative[LEVELS+1];
extern int        DaylightSavings;
extern Coordinate RA, Dec;
extern int        abort_slew_flag;
extern int        keyboard_buffer;
extern int        Xpaddle, Ypaddle;
extern int        ticks, CoordinatesInitialized;  
extern long int   Xoffset, Yoffset;
extern double     RA_Orig, DecOrig;
extern int        UpArrow, DnArrow, LfArrow, RtArrow;
extern long int   Jog_Angle;
extern int        ShowXY, FastSlew;
extern long int   MaxDelay, MinDelay;
extern long int   MaxSteps, MinSteps;
extern long int   FastPaddleSpeed, SlowPaddleSpeed;
extern long int   Levels;

// ************************* function prototypes **************************

void DoMenu( int *menu );
void DoMainMenu( int *menu );
void DoOutOfBoundsMenu( int *menu );  
void DoSorryMenu( int *menu );
void DoExitProgram( int *menu );

void Initialize( void );
void DisplayCoordinates( int y, int show_RADec, int show_XY, 
                         int show_offsets );
void GetSelection( int *menu, int max );
void ClearKeyboard( void );
void FrameScreen( void );
void EraseLine( int linenumber );





