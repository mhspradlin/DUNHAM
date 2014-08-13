#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
// #include <sys\timeb.h>

void main(void)
{
   time_t timer, *dummy, t, truetime;
   struct tm *t_;
   int hour=19, min=30, sec=0;

   timer = time(NULL);
   truetime = timer;
   t_ = localtime( &timer );
   printf("Current time is %2d:%2d:%2d\n",t_->tm_hour,t_->tm_min,t_->tm_sec);
   t_->tm_hour = hour;
   t_->tm_min = min;
   t_->tm_sec = sec;
   t = mktime( t_ );  // Converts time to calendar format.
   stime( &t );         // Sets system date and time.
   timer = time(NULL);
   t_ = localtime( &timer );
   printf("Current time is %2d:%2d:%2d\n",t_->tm_hour,t_->tm_min,t_->tm_sec);
   stime( &truetime );
   timer = time(NULL);
   t_ = localtime( &timer );
   printf("Current time is %2d:%2d:%2d\n",t_->tm_hour,t_->tm_min,t_->tm_sec);


//   char *s;
//   time_t td;
//   struct timeb t;

//   putenv("TZ=CST6CDT");
//   tzset();

//   s = getenv("TZ");
//   printf("TZ = %s\n", s);

//   time(&td);
//   printf("Current time: %s\n", asctime(localtime(&td)));

//   ftime(&t);
//   printf("Difference between local time and GMT: %d\n", t.timezone);
//   printf("Daylight savings in effect (1) not (0): %d\n", t.dstflag);
}
