#include "mds.h"
#include "QCam.h"
#include "writelog.h"


char *modeName[7] = {"ZERO","INIT","ACOL","ALGN","SLEW","PNTG","ADTN"};
char *longModeName[7] = {"NULL Mode","Initialization Mode", "Autocollimation Mode", "Alignment Mode", "Slewing Mode", "Pointing Mode", "Fault Mode"};


Mode::Mode(){
  ID = (GuiderMode) 1;
  subID = INITMODE;
  roiX = 0;
  roiY = 0;
  roiDX = CHIPWIDTH;
  roiDY = CHIPHEIGHT;
  expt = MINET;
  trigt = 111;
  CallbackFunction = NULL;
  isValid = true;
};

Mode::Mode(GuiderMode id, unsigned char subid, unsigned short rX, unsigned short rY, unsigned short rDX, unsigned short rDY, unsigned int et, unsigned char tt){
  isValid = false;
  ID = id;
  subID = subid;
  roiX = rX;
  roiY = rY;
  roiDX = rDX;
  roiDY = rDY;
  expt = et;
  trigt = tt;
  CallbackFunction = NULL;
  if(valid()) isValid = true;
  //  WriteToLog("%s Started",longModeName[id]);
};

Mode::~Mode(){
};


bool Mode::setModeParams(unsigned short rX, unsigned short rY, unsigned short rDX, unsigned short rDY, unsigned int et, unsigned char tt){
  isValid = false;
  
  roiX = rX;
  roiY = rY;
  roiDX = rDX;
  roiDY = rDY;
  expt = et;
  trigt = tt;
  //CallbackFunction = NULL;
  if(valid()) isValid = true;
  return true;
};

bool Mode::setMode(GuiderMode id, unsigned char subid){
  isValid = false;
  ID = id;
  subID = subid;
  if(valid()) isValid = true;
  return true;
};

bool Mode::setCallback(void QCAMAPI (*cbf)(void *, unsigned long, QCam_Err, unsigned long)){
  CallbackFunction = cbf;
  //  cbpointer = (void *) CallbackFunction;
  return true;
};

bool Mode::valid(){
  int err;
  err = 0;
  if(ID < MINMODE || ID > MAXMODE) return false;
  if(subID < MINSUBMODE || subID > MAXSUBMODE) return false;
  if(roiX + roiDX > CHIPWIDTH) return false;
  if(roiY + roiDY > CHIPHEIGHT) return false;
  if(expt < MINET) return false;
  if(trigt < MINTT || trigt > MAXTT) return false;
  
  return true;
};

bool Mode::setEqual(Mode source){
  setMode(source.ID, source.subID);
  setModeParams(source.roiX, source.roiY, source.roiDX, source.roiDY, source.expt, source.trigt);
  winX = source.winX;
  winY = source.winY;
  winDX = source.winDX;
  winDY = source.winDY;

  CallbackFunction = source.CallbackFunction;
  
  return true;
};


