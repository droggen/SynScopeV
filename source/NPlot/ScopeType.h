#ifndef SCOPETYPE_H
#define SCOPETYPE_H


/**
  Pseudo float operations using fraction notation
**/


/**
FRAC.magnitude is the magnitude. FRAC.gt1 indicates whether the number is greater than 1. E.g. magnitude=5 gt1=1 -> x=5. magnitude=5 gt1=0 -> x=1/5
**/
typedef struct
{
    int magnitude;
    int gt1;
} FRAC;


#endif // SCOPETYPE_H
