#pragma once

#include "QCamApi.h"



typedef enum {
  NOMODE = 0,
  INITMODE = 1,
  AUTOCOLLIMATIONMODE = 2,
  ALIGNMENTMODE = 3,
  SLEWINGMODE = 4,
  POINTINGMODE = 5,
  ERRORMOD = 6
} GuiderMode;

typedef enum {
  ALIGNFULLCHIP = 0,
    ALIGNTRACKFULLCHIP = 1,
    ALIGNTRACKROI = 2,
  ALIGNDRAGFULLCHIP = 3
} AlignSubMode;


typedef enum{
  // in this submode we wait for the dtu to tell us we are there
  SEARCHINGDTU = 0,
    // in this submode we wait for the DTU to settle. basically we
    // want the same star in the same box for 8 exposures. why 8?
    // because i said so... This can be overridden by a TC command
    WAITINGTOSETTLE = 1,
    // in this mode we wait for the human operator to say "we got to
    // the field, lock on something. 
    SEARCHINGHUMAN = 2
    } SlewingSubMode;

typedef enum {
  // this is a full screen lock on a star at a fixed position. if the position 
  // is outside of the box, we move the star with its bounding box to
  // that position
  LOCKED = 0,
  // this is a ROI lock on a star at a fixed position on the chip. if
  // the position is outside of the currently selected region, we
  // first must perform a full screen lock and bring the star to the
  // desired position
  LOCKEDROI = 1,
  // this is a full screen track. it requires a pair of RA and DEC
  // values to track.
  TRACKINGFC = 2,
  // this is a track in the region of interest, and also requires and
  // RA and DEC pair to track 
  TRACKINGROI = 3
} PointingSubMode;

#define MAXMODE 7
#define MINMODE 0
#define MAXSUBMODE 6
#define MINSUBMODE 0 


extern char *modeName[7];
extern char *longModeName[7];



class Mode{
 private:
  GuiderMode ID;
  unsigned char subID;
  unsigned short roiX, roiY, roiDX, roiDY;
  unsigned int expt; // in microseconds
  unsigned char trigt; // in milliseconds

 public:
  unsigned short winX, winY, winDX, winDY;
 private:
  double targetX, targetY;



  bool valid();  

 public:
  

  

  GuiderMode getModeID(){
    return ID;
  };
  
  void QCAMAPI (*CallbackFunction)(void *, long unsigned int, QCam_Err, long unsigned int);
  
  Mode();
  Mode(GuiderMode id, unsigned char subid, unsigned short rX, unsigned short rY, unsigned short rDX, unsigned short rDY, unsigned int et, unsigned char tt);
  ~Mode();

  bool setModeParams(unsigned short rX, unsigned short rY, unsigned short rDX, unsigned short rDY, unsigned int et, unsigned char  tt);
  
  bool setMode(GuiderMode id, unsigned char subid);
  bool setsubID(unsigned char subid){ subID = subid; return true;};

  bool setCallback(void QCAMAPI (*cbf)(void *,unsigned long int, QCam_Err, unsigned long int));  
  bool isValid;
  
  bool setEqual(Mode source);
  
  unsigned short getroiX(){ return roiX;};
  unsigned short getroiY(){ return roiY;};
  unsigned short getroiDX(){ return roiDX;};
  unsigned short getroiDY(){ return roiDY;};;
  GuiderMode getID(){ return ID;};
  unsigned char getsubID() { return subID;};
  unsigned short getTrigT(){ return trigt;};
  unsigned int getExpT(){return expt;};  
  unsigned short gettrigt(){ return trigt;};
  unsigned int getexpt(){return expt;};  

};


