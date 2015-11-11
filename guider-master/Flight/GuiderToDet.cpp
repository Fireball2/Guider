#include "dac.h"
#include "GuiderToDet.h"
#include "comsetup.h"
#include "writelog.h"




void FrameNumberToDetector(unsigned int fn){
  unsigned int fm;
  fm = fn;
  unsigned char c;
  unsigned char *cp;
  cp = (unsigned char *) &fm;
  c = *cp;
  GtoD_PortB(c);
  //  WriteToLog("Writing framenumber to detector");
  /*
  static unsigned int fm;
  unsigned char *fp;
  WriteToLog("Trying to send to detector");
  fm = fn;
  fp = (unsigned char *) fm;
  GtoD_PortB(*fp);
  WriteToLog("Succeded");
  */
  return;
};

void StatusToDetector(bool guidertrack, bool camerr){
  unsigned char status = 0;
  if(guidertrack) status = GUIDERTRACKING;
  if(camerr) status = status & CAMERAERROR;
  GtoD_PortA(status);
  //  WriteToLog("Writing status to detector");
  return;
};

void GtoD_PortB(unsigned char b){
  outPortB(b);
};

void GtoD_PortA(unsigned char a){
  outPortA(a);
};




unsigned short DetCount;
bool Detinitcount;

 DetectorHello::DetectorHello(){
  this->syncstart = 0x55;
  this->syncend = 0xAA;
  this->id = 1;
  if(!(Detinitcount)){
    DetCount = 0;
    Detinitcount = true;
  };
  this->length = 2;
  this->totlength = 9;
  this->ptr = (unsigned char *) this;
};


unsigned short getDetCount(){
  return DetCount++;
};

DetectorHello::~DetectorHello(){
};

void DetectorHello::send(unsigned char vl){
  val = vl;
  this->count = getDetCount();
  this->setCRC();
  SendToDet(this->ptr, this->totlength);
};

void DetectorHello::setCRC(){
  
  int i;
  crc = ptr[1];
  for(i=2;i<this->totlength-2;i++){
    crc ^= ptr[i];
  };
  return;
};


FrameInfo::FrameInfo(){
  this->syncstart = 0x55;
  this->syncend = 0xAA;
    this->id = 2;
  if(!(Detinitcount)){
    DetCount = 0;
    Detinitcount = true;
  };
  this->length = 18;
  this->totlength = 25;
  this->ptr = (unsigned char *) this;

};

FrameInfo::~FrameInfo(){
};


void FrameInfo::send(unsigned int fn, float lvdt, float el, float ce, unsigned int et, unsigned char valid){
  
  this->count = getDetCount();
  framenumber = fn;
  ELOffset = leakylvdt;;
  
  // the divisor is such so as to allow values of -2000, 2000, but not higher.  

  if ( el >= 0) {
    el = el + 0.031;
  } else if ( el == 0.0) { 
    el = el;
  } else {
    el = el - 0.031;
  }

  if ( ce >= 0) {
    ce = ce + 0.031;
  } else if( ce == 0.0) { 
    ce = ce;
  } else {
    ce = ce - 0.031;
  };

  sEL = (short) el/0.062;
  sCE = (short) ce/0.062;

  if (sEL < -2000) sEL = -2000;
  if (sEL > 2000) sEL = 2000;
  if (sCE < -2000) sCE = -2000;  
  if (sCE > 2000) sCE = 2000;


  
  
  psEL = (short *) &CEOffset;
  psCE = ((short *) &CEOffset)+1;
  *psEL = sEL;
  *psCE = sCE;
  
  ExpT = et;
  status = valid;
  this->setCRC();
  SendToDet(this->ptr, this->totlength);
};


void FrameInfo::setCRC(){
  
  int i;
  crc = ptr[1];
  for(i=2;i<this->totlength-2;i++){
    crc ^= ptr[i];
  };
  
};
