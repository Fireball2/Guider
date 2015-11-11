#pragma once


// we want to write to the log from this header file.
//#include "writelog.h"

#include <QCamApi.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

// must be able to play with modes
#include "mds.h"

//must be able to putz around with the dac
#include "dac.h"

// want to put in sturdy delays
#include "time.h"

//we want to be able to do stuff with the frameblobs...
#include "frameblob.h"



#define CHIPWIDTH 1360
#define CHIPHEIGHT 1036
#define FRAMES 20 
#define MINET 40
#define MINTT 33
#define MAXTT 255
#define PIXSIZE 0.00645 //pixel size in mm


typedef enum{
  GUIDERSENSOR = 0,
  OTHERSENSOR = 1
} OpticalSensor;


class QCamera{
  QCam_Handle camHandle;
  QCam_Settings settings;
  QCam_Frame frame[FRAMES];
  
  unsigned char nextframe;

  timespec delay;

  FILE *framenumberfile;

 public:
  unsigned char *buffer[FRAMES];
  unsigned long framenumber[FRAMES];
  float distxbuffer[FRAMES];
  float distybuffer[FRAMES];
  frameblob fblob[FRAMES];
  frameblob smallblobs[FRAMES];
  unsigned short targetBlob;
  int verifycounter;
  float MoveStepSize;
  float pointingTrackPointX, pointingTrackPointY;
  bool existsCamera;
  float spectroCenterPointX, spectroCenterPointY;
  Mode nowMode;
  Mode nextMode;
  float gondolaoffsetx, gondolaoffsety;
  bool feellucky;
  unsigned char smallTrigT, largeTrigT;
  unsigned int smallExpT, largeExpT;
 private:
  unsigned long frameNumber;
  unsigned long frameCounter;



  bool nextModeReady;

  unsigned long normgain;
  int offset;

  bool isStreaming;


  double platescale;
  double rotation;
  OpticalSensor activesensor;

  // right ascension and declination of center of the star tracking onx
  double RAroi;
  double DECroi;

  // right ascension and declination of the spectrograph center alignment.
  double RAspec, DECspec;

  // right ascension and declination of the target.
  double targetRA, targetDEC;

  // x and y chip coordinates of the target point.
  double targetX, targetY;
  
  // the coordinates of the tracking window. this is used in full chip
  // mode to track on a star within a specific region. winX and winY
  // are the centers of the box and winDX, winDY are the sizes
  //  unsigned short winX, winY, winDX, winDY;
  
  // spectrograph offset from the center of the chip.
  double SpecCenterX;
  double SpecCenterY;

  // values to output to the DAC
  float UserELOffset, UserCEOffset;
  float RoiGainEL, RoiGainCE;
  float FCGainEL, FCGainCE;





  //  float flatField[CHIPWIDTH*CHIPHEIGHT];
  float *flatfield;
  bool badPixMap[CHIPWIDTH*CHIPHEIGHT];
 public:
  QCamera();
  ~QCamera();
  bool startStreaming();
  bool stopStreaming();
  bool SlewingSwitchToPointing;

  float getUserELOffset();
  void setUserELOffset(float el);
  
  float getUserCEOffset();
  void setUserCEOffset(float ce);

  float getRoiGainEL();
  void setRoiGainEL(float gel);

  float getRoiGainCE();
  void setRoiGainCE(float gce);

  float getFCGainEL();
  void setFCGainEL(float gel);

  float getFCGainCE();
  void setFCGainCE(float gce);


  float userRoiEL(float el);

  float userRoiCE(float ce);

  float userFCEL(float el);

  float userFCCE(float ce);


  
  char prepSettings(Mode &mset, double targetra, double targetdec);
  bool changeSettings();
  
  char prepSameSettings();

  unsigned char queueFrame();
  void setTriggerTime();

  double getRotation();
  void setRotation(double rot);
  double cosangle, sinangle;
  
  double getPlateScale();
  void setPlateScale(double ps);

  OpticalSensor getActiveSensor();
  void setActiveSensor(OpticalSensor as);
  
  double getRA();
  void setRA(double ra);
  
  double getDEC();
  void setDEC(double dec);


  double getTargetRA();
  void setTargetRA(double ra);
  
  double getTargetDEC();
  void setTargetDEC(double dec);

  double getCenterSpecX();
  void setCenterSpecX(double setCSX);
  
  double getCenterSpecY();
  void setCenterSpecY(double setCSY);

  unsigned short getRoiX();
  unsigned short getRoiY();
  unsigned short getRoiDX();
  unsigned short getRoiDY();
  unsigned int getExpT();
  unsigned char getTrigT();

  void setRoi(unsigned short x, unsigned short y, unsigned short dx, unsigned short dy, unsigned int et, unsigned char tt);
  
  void setRestoreMode(GuiderMode id, unsigned char subid);
  
  


  double outputELGain;
  double outputCEGain;
  double outputELOffset;
  double outputCEOffset;

  void setOutputELGain(double oel);
  void setOutputCEGain(double oce);
  void setOutputELOffset(double oelo);
  void setOutputCEOffset(double oceo);
  
  // boolean used to designate whether the chip is in range of the
  // coordinates  
  bool inRange;
  

  void setNormalizedGain(unsigned int gain);
  unsigned int onlyGetFrame();

  


  unsigned int getFrame();

  GuiderMode getModeID();
  unsigned char getSubModeID();
  void setModeID(GuiderMode md, unsigned char sm);
  void setSubModeID(unsigned char smid);
  void setNextSubModeID(unsigned char smid);


  // functions to handle the frameblob objects.
  void setFrameBlobXpix(unsigned int xpix_in);
  void setFrameBlobYpix(unsigned int ypix_in);

  void setSmallBlobXpix(unsigned int xpix_in);
  void setSmallBlobYpix(unsigned int ypix_in);

  void setFrameBlobGain(double gain_in);
  void setFrameBlobReadoutOffset(double in_readout_offset);
  void setFrameBlobReadoutNoise(double in_readout_noise);
  void setFrameBlobSatVal(MAPTYPE in_satval);
  void setFrameBlobGrid(unsigned int in_grid);
  void setFrameBlobCenBox(unsigned int in_cenbox);
  void setFrameBlobApBox(unsigned int in_apbox);
  void setFrameBlobThreshold(double in_threshold);
  void setFrameBlobDistTol(int in_disttol);

  MAPTYPE getFrameBlobSatVal();
  unsigned int getFrameBlobXpix();
  unsigned int getFrameBlobYpix();
  double getFrameBlobGain();
  double getFrameBlobReadoutOffset();
  double getFrameBlobReadoutNoise();
  unsigned int getFrameBlobGrid();
  unsigned int getFrameBlobCenBox();
  unsigned int getFrameBlobApBox();
  double getFrameBlobThreshold();
  int getFrameBlobDistTol();

  void setTargetBlob(unsigned short tb);
  unsigned short getTargetBlob();

  unsigned short getWinX();
  unsigned short getWinY();
  unsigned short getWinDX();
  unsigned short getWinDY();

  void setNextWinX(unsigned short x);
  void setNextWinY(unsigned short y);
  void setNextWinDX(unsigned short dx);
  void setNextWinDY(unsigned short dy);

  void setNowWinX(unsigned short x);
  void setNowWinY(unsigned short y);
  void setNowWinDX(unsigned short dx);
  void setNowWinDY(unsigned short dy);


  void setNextWin(unsigned short wx, unsigned short wy, unsigned short wdx, unsigned short wdy);

  void setNowWin(unsigned short wx, unsigned short wy, unsigned short wdx, unsigned short wdy);
    
    void FlattenChip(unsigned char qid);

    // qcam state is saved every time we attempt to change modes
    bool SaveQCamState();
    
    // this is called every time the program starts up. returns 0 if there was nothing to load. otherwise restores the previous state of the camera
    char LoadQCamState();

    void setMoveStepSize(float stepsize);

    void StartFrameNumber();


    void WriteFrameNumber();


};


