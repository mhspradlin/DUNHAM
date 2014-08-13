#include <stdio.h>
#include <conio.h>

void main(void)
{
   clrscr();
   gotoxy(1,1); printf("%c%c%c%c%c%c", 0xC9, 0xCD, 0xBB, 0xC8, 0xCD, 0xBC);
}
