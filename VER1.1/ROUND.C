long round( double number )
// Returns the rounded (long) value of the number.  
// Handles negative numbers as well as positive.
{
   int remainder = (number - (long)number) * 10;

   return ( number + (remainder>4) - (remainder<(-4)) );
}
