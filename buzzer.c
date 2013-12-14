#include "buzzer.h"

uint8_t buzz_identify_buzzer(uint8_t givenButton){
  return givenButton/5;
}

buzz_button buzz_identify_button(uint8_t givenButton){
  return (buzz_button)(givenButton%5);
}


