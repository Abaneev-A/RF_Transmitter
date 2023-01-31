#ifndef Rotary_h
#define Rotary_h

#include <avr/io.h>

// Enable this to emit codes twice per step.
// #define HALF_STEP
#define DIR_NONE 0x0
#define DIR_CW 0x10
#define DIR_CCW 0x20

class Rotary
{
  public:
    uint8_t process();
  private:
    uint8_t state = 0;
};

#endif
 
