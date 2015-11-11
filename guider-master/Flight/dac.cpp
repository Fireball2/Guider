
#include "dac.h"
#include "ChipAndSky.h"
#include <sys/io.h>
#include "writelog.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define BASEADDR 0x340

//#define SENDZERO

float CardXOffset;
float CardYOffset;
float TrackerXOffset;
float TrackerYOffset;


unsigned short adcValue;
float leakylvdt;


bool outputzero;



//#define ELZEROOFFSET -90
//#define CEZEROOFFSET -200

//old zero
//#define CEZEROOFFSET -25
//#define ELZEROOFFSET -14


//new zero before ft sumber 4/7/2009
//#define ELZEROOFFSET -14
//#define CEZEROOFFSET -5

//ft sumner 4/7/2009
#define ELZEROOFFSET -16
#define CEZEROOFFSET -6

// newest zero
//#define CEZEROOFFSET -520
//#define ELZEROOFFSET -97



float port0405[4096];
float port0A0B[4096];

float p0405[42];
float p0A0B[42];
short int i0405[42];
short int i0A0B[42];


bool getOutput(){
  return outputzero;
};

void InitDAC(){
  ioperm(BASEADDR, 32, 1); // allow us to use port addresses
  outb(1,BASEADDR+0x18);   // turn on the DACs  
  TrackerXOffset = -0.0176; // in Volts
  TrackerYOffset = -0.0422; // in Volts
  TrackerXOffset = 0.00; // in Volts
  TrackerYOffset = 0.00; // in Volts
  CardXOffset = CEZEROOFFSET;
  CardYOffset = ELZEROOFFSET;
  outputzero = false;
  leakylvdt =  0;
  //  GainX = 1;
  //  GainY = 1;

  outb(0x89,BASEADDR+0x13); // configure port C for input, ports A and B for output.
};

void SendOutEL(float el){
  float outvalue;
  int index;
  unsigned char lsb, msb;
  
  outvalue = -1.0*el*0.01+TrackerYOffset;
  ioperm(BASEADDR, 32, 1); // allow us to use port addresses

  index = (int) (outvalue/0.002441);

  index += (int) (2048 + CardYOffset);

  if(outputzero)  index = (int) (2048 + CardYOffset);

  if(index<0) index = 0;
  if(index>4095) index = 4095;

  lsb = index & 255;
  msb = index >> 8;
  outb(lsb, BASEADDR+0x0A);
  outb(msb, BASEADDR+0x0B);
  //  WriteToLog("EL","Sending out %d", index-2048);
  return;

};

void SendOutCE(float ce){
  float outvalue;
  int index;
  unsigned char lsb, msb;
  
  outvalue = -1.0*ce*0.01+TrackerXOffset;
  ioperm(BASEADDR, 32, 1); // allow us to use port addresses

  index = (int) (outvalue/0.002441);

  index += (int) (2048 + CardXOffset);

  if(outputzero) index = (int) (2048+ CardXOffset);


  if(index<0) index = 0;
  if(index>4095) index = 4095;

  lsb = index & 255;
  msb = index >> 8;
  outb(lsb, BASEADDR+0x04);
  outb(msb, BASEADDR+0x05);
  //  WriteToLog("CE","Sending out %d", index-2048);
  return;

};


void setOutputZero(){
  outputzero = true;
};

void setOutputTrue(){
  outputzero = false;
};



void outPortB(unsigned char b){
  ioperm(BASEADDR, 32, 1); // allow us to use port addresses
  outb(b,BASEADDR+0x11);
};

void outPortA(unsigned char b){
  ioperm(BASEADDR, 32, 1); // allow us to use port addresses
  outb(b,BASEADDR+0x10);
};



float getAdcValue(){
  static unsigned short readVal;
  ioperm(BASEADDR, 32, 1); // allow us to use port addresses
  outb(16, BASEADDR+0x02); // read it 
    static int i;
  for(i=0;i<5000;i++);
  adcValue = inw(BASEADDR+0x02);
  //  fprintf(stderr,"Hello\n");
  outb(16+32, BASEADDR+0x02); // see the DIO manual for explanation. 
                           // this configures the lowest (0) port
                           // for readout. 



  leakylvdt = 0.97*leakylvdt + 0.03*(adcValue-5.531);
  return leakylvdt;
};
