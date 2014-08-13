#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "globals.h"
#include "string.h"
#include "utils.h"

void main(void)   
{
   FILE *fileptr;
   int result, err, invalid_ra, invalid_xy, *p, s[255], num, letter;
   int too_many=200, i=0;
   long int gX, gY, dummy1, dummy2;
   double dummy3, dummy4, RA, Dec;
   char tempstr[10], ch, string1[201], string2[201];
   char *str="0";
   Coordinate C;
   
   printf("Demonstrating reading individual characters from a file.\n");
   printf("File: DUNHAM.SAV\n\n");
   fileptr = fopen("DUNHAM.SAV", "r");
   if (fileptr)
   {
      do
      {
         ch = fgetc(fileptr);
         if (ch!='\n') putch(ch);
         else printf("\n");
      } while (ch != EOF);
      printf("\n\n");
      fseek(fileptr, 0, 0); // Reposition file pointer to beginning of file.

      result = fscanf(fileptr,"%*s %ld %*s %ld",&dummy1,&dummy2);
      ch = fgetc(fileptr);   // just to get to the next line...
      printf("Read X and Y: X=%ld, Y=%ld.\n", dummy1, dummy2);

      fgets(string1,200,fileptr);
      printf("line 1: %s",string1);

      for (i=0;i<200;i++) s[i] = string1[i];
      if (GetCoordinateFromString(s,&C)) 
            printf("error in GetCoordinateFromString\n");
      RA = C.hour + C.minute/SIXTY + C.second/(SIXTY*SIXTY);
      printf("Got RA = %18.12f\n",RA);

      fgets(string2,200,fileptr);
      printf("line 2: %s\n",string2);
      for (i=0;i<200;i++) s[i] = string2[i];
      if (GetCoordinateFromString(s,&C)) 
            printf("error in GetCoordinateFromString\n");
      Dec = C.hour + C.minute/SIXTY + C.second/(SIXTY*SIXTY);
      printf("Got Dec = %18.12f\n",Dec);

      printf("\n");
//      for (i=0;i<20;i++)
//      {
//         letter=string1[i];
//         printf("string1[%1d]=%c\n", i, letter);
//      }

      if (0) {

      i=0;
      p = s;
      do    // read in RA...
      {
//         ch = fgetc(fileptr);
         letter = fgetc(fileptr);
         ch=letter;
         printf("%c", ch);
//         s[i]=(int)ch;        // might need to subtract 48.
         *p++ = ch;      
      } while (ch!='\n' && ch!=EOF && i<too_many);

      } //if(0)

      fclose( fileptr );
   }
}
