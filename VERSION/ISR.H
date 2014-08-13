// isr.h

// Header file for "isr.c"

// Barbara Carter    September 6, 1994

extern void interrupt (*oldclk)(void);
extern void interrupt (*oldkb)(void);
void interrupt newclk(void);
void interrupt newkb(void);
void interrupt onbreak(void);
void interrupt tracking_ticks(void);
void interrupt stop_tracking_ticks( void );
