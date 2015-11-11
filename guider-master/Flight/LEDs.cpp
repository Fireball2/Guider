#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "writelog.h"
#include "LEDs.h"

#define PARPORTADDR 0x378

unsigned char ledstatus;

void InitializeLEDs(){
  // basically, make sure they are off.
  ioperm(PARPORTADDR,24,1);
  outb(0,PARPORTADDR);
  WriteToLog("LED","Initialized");
  ledstatus = 0;
  return;
};

void TurnLEDsOn(){
  ioperm(PARPORTADDR,24,1);
  outb(0xFF,PARPORTADDR);
  ledstatus=1;
  return;
};

void TurnLEDsOff(){
  ioperm(PARPORTADDR,24,1);
  outb(0x00, PARPORTADDR);
  ledstatus=0;
  return;
};

