#include <sys/io.h>
#include "led.h"

#define PARALLELPORTADDRESS 0x378

unsigned char parallelportvalue;

unsigned char TurnOnLEDs(){
  ioperm(PARALLELPORTADDRESS,1,8);
  parallelportvalue = 0xFF;
  outb(parallelportvalue, PARALLELPORTADDRESS);
  return parallelportvalue;
};

unsigned char TurnOffLEDs(){
  ioperm(PARALLELPORTADDRESS,1,8);
  parallelportvalue = 0x00;
  outb(parallelportvalue, PARALLELPORTADDRESS);
  return parallelportvalue;
};
