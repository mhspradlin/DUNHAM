// calibr8.c

// Calibration routines for slewing the Dunham telescope.

// Barbara Carter    September 9, 1994

#include <bios.h>

void Calibrate( void )
{
   long int InfiniteDelay=1000000L;
   long int Loops=6000, looptime;

   oldPulse1Delay = Pulse1Delay;  // Make sure pulse is wide enough.
   oldPulse2Delay = Pulse2Delay;  // Make sure space between pulses is enough.
   oldMaxDelay = MaxDelay;   // Corresponds to base motor speed.
   oldMinDelay = MinDelay;   // Corresponds to maximum motor speed.
   oldJogAngle = Jog_Angle;  // User's choice for jog-angle (arrow-keys).

   if (can open "dunham.dat")
   {
      get MaxDelay, MinDelay, Jog_Angle, Pulse1Delay, Pulse2Delay;
      set flag if successful;
   }

   criticaltime = 18.2 * 0.1; // check this!!!  Use Loops to get the value...
   i=0; j=0;
   do                           // get maximum value for Pulse1Delay
   {
      looptime = TimeFirstHalf( InfiniteDelay, Loops, &i, &j );
      if (looptime < criticaltime) 
      {  
         temp = Pulse1Delay;                      
         Pulse1Delay *= (criticaltime/looptime);
         if (abs(temp-Pulse1Delay) < ONE)  ++Pulse1Delay;
      }
   } while (looptime < criticaltime);

   i=0; j=0;
   do               // then lower Pulse1Delay until it crosses the threshold
   {
      looptime = TimeFirstHalf( InfiniteDelay, Loops, &i, &j );
      if (looptime > criticaltime) --Pulse1Delay;
   } while ( (looptime > criticaltime) && (Pulse1Delay > 0L) );
   if (Pulse1Delay < 0L) Pulse1Delay = 0L;

   criticaltime = 18.2 * 0.1;
   do               // now work on Pulse2Delay, first getting the maximum
   {
      looptime = TimeSecondHalf( Loops );
      if (looptime < criticaltime)
      {  
         temp = Pulse2Delay;
         Pulse2Delay *= (criticaltime/looptime);
         if (abs(temp-Pulse2Delay) < ONE)  ++Pulse2Delay;
      }
   } while (looptime < criticaltime);

   do               // then lower Pulse2Delay until it crosses the threshold
   {
      looptime = TimeSecondHalf( Loops );
      if (looptime > criticaltime) --Pulse2Delay;
   } while ( (looptime > criticaltime) && (Pulse2Delay > 0L) );
   if (Pulse2Delay < 0L) Pulse2Delay = 0L;

   criticaltime = 18.2 * 0.1; // yeeeaaak!!!
   do               // MaxDelay: first get the maximum value
   {
      looptime = TimeBothHalves( MaxDelay, Loops );
      if (looptime < criticaltime)
      {  
         temp = MaxDelay;
         MaxDelay *= (criticaltime/looptime);
         if (abs(temp-MaxDelay) < ONE)  ++MaxDelay;
      }
   } while (looptime < criticaltime);

   do               // then lower MaxDelay until it crosses the threshold
   {
      looptime = TimeBothHalves( MaxDelay, Loops );
      if (looptime > criticaltime) --MaxDelay;
   } while ( (looptime > criticaltime) && (MaxDelay > 0L) );
   if (MaxDelay < 0L) MaxDelay = 0L;

   criticaltime = 18.2 * 0.1; // yeeeaaak!!!
   do               // MinDelay: first get the maximum value
   {
      looptime = TimeBothHalves( MinDelay, Loops );
      if (looptime < criticaltime)
      {  
         temp = MinDelay;
         MinDelay *= (criticaltime/looptime);
         if (abs(temp-MinDelay) < ONE)  ++MinDelay;
      }
   } while (looptime < criticaltime);

   do               // then lower MinDelay until it crosses the threshold
   {
      looptime = TimeBothHalves( MinDelay, Loops );
      if (looptime > criticaltime) --MinDelay;
   } while ( (looptime > criticaltime) && (MinDelay > 0L) );
   if (MinDelay < 0L) MinDelay = 0L;

   // now save the values into the file "Dunham.dat", overwriting the old
   // file or creating a new one.  If there's a write error, print an error
   // message, but permit the program to continue, because at least the
   // correct values have been found.
}

long int TimeFirstHalf( long int DelayAmount,long int maxloop,int *i,int *j )
{
   long int time1, time2, m;

   SetUp(DelayAmount);                    
   time1 = biostime(0,0L);                 
   for (m=0; m<maxloop; m++) FirstHalf(i,j);    
   time2 = biostime(0,0L);                 
   return(time2-time1);
}

long int TimeSecondHalf( long int maxloop )
{
   long int time1, time2, m;

   time1 = biostime(0,0L);
   for (m=0; m<maxloop; m++) SecondHalf();
   time2 = biostime(0,0L);
   return(time2-time1);
}

long int TimeBothHalves( long int DelayAmount, long int maxloop )
{
   long int time1, time2, m;
   int i=0, j=0;

   SetUp(DelayAmount);                    
   time1 = biostime(0,0L);
   for (m=0; m<maxloop; m++)
   {
      FirstHalf(&i,&j);
      SecondHalf();
   }
   time2 = biostime(0,0L);
   return(time2-time1);
}

void FirstHalf(int *i, int *j) 
{
   long int temp=0L;

   if (dX_)
   {
      if ((--x_skip_)==0)
      {
         X_ = x_sign_;
         dX_ -= X_;
         if ((--xsend_[*i])==0) ++(*i);
         x_skip_ = xskip_[*i];
      }
      else X_ = 0;
   }
   else X_ = 0;
   if (dY_)
   {
      if ((--y_skip_)==0)
      {
         Y_ = y_sign_;
         dY_ -= Y_;
         if ((--ysend_[*j])==0) ++(*j);
         y_skip_ = yskip_[*j];
      }
      else Y_ = 0;
   }
   else Y_ = 0;
   outword = NOPULSE + (int)(-X_*(2+6*X_)-Y_*(1+3*Y_)/2);
   if (Pulse1Delay) 
      for (temp=0;temp<Pulse1Delay;temp++) DoNothing();
}

void SecondHalf( void )
{
   long int ltemp;

   gX_ += X_;
   gY_ += Y_;
   if (loops_ > 2000) DoNothing();
   if (!(dX_ || dY_)) dX_++;
   if (abort_slew_flag) abort_slew_flag=0;
   if (Pulse2Delay) for (ltemp=0;ltemp<Pulse2Delay;ltemp++) DoNothing();
}

void SetUp( long int delayfactor )
{
   long int i;

   x_sign = _SIGN(dX_);
   y_sign = _SIGN(dY_);
   for (i=0; i<LEVELS*2; i++)
   {
      xsend[i] = STEPSPERLEVEL; 
      ysend[i] = STEPSPERLEVEL;
      xskip_[i] = delayfactor; 
      yskip_[i] = delayfactor; 
   }
   x_skip_ = xskip_[0];
   y_skip_ = yskip_[0];
   loops_ = 0;
   dX_ = 3000; dY_ = 3000;
}

void DoNothing( void )
{
}
