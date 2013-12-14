#ifndef _BUZZER_H
#define _BUZZER_H

#include "stdint.h"

typedef enum {
  BUZZ_RED=0,  
  BUZZ_YELLOW=1,
  BUZZ_GREEN=2,
  BUZZ_ORANGE=3,
  BUZZ_BLUE=4
} buzz_button;

// translates a button number to the buzzer handheld
uint8_t buzz_identify_buzzer(uint8_t givenButton);

// translates a button number to the buzzer color
buzz_button buzz_identify_button(uint8_t givenButton);

#endif // _BUZZER_H
