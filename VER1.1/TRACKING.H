// tracking.h

// All functions in "tracking.c" are available for use by other Dunham
// routines.

// Barbara Carter    September 6, 1994

void DoZenithMenu( int *menu );
void DoTrackingMenu( int *menu );
void DoChangeJogMenu( int *menu );
void SaveCoords( void );
void StartTracking( void );
void StopTracking( void );
void TrackingKeyboard( int y_display, int xx, int yy, int *letter);
void TrackCalc( void );
void FastPaddle( void );
long int PaddleSpeed( void );
int Bits( int number, int* bit3, int* bit2, int* bit1, int* bit0 );
