#include <stdio.h>
void main(void)
{
   int result=0, number=0;

   do
   {
      printf("Enter a letter followed by a number (99 to quit): ");
      result = scanf("%*s %d",&number);
      printf("Scanned %d fields, number=%d\n", result, number);
   } while (number != 99);

}
