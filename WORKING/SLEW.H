// slew.h

// Dunham slew functions other code segments need to access.

// Barbara Carter    September 6, 1994

void Slew( int y_display );
void DoRA_DecSlewMenu( int *menu );
void DoSlewingMenu( int *menu );

void GetSignFromString( int *s, int *sign );
int NextNumberFromString( double *Num, int **s );
