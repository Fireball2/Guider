#include "QCam.h"
#include "QCamApi.h"
#include "mds.h"
#include "writelog.h"
#include <sys/io.h>
#include <math.h>
#include <unistd.h>
#include <time.h> // this is for sleeping. 
#include <exception>
#include "GuiderToTMTC.h"
#include "ChipAndSky.h"




#define IOCARDADDR 0x340

QCamera::QCamera(){
  int i;



  // first we initialize the camera.
  QCam_CamListItem list[10];
  unsigned long listLen;
  
  QCam_LoadDriver();
  QCam_ListCameras(list, &listLen);
  
  printf("Camera(s) found: %ld\n",listLen);

  if(!listLen){
    existsCamera = false;
  } else {
    existsCamera = true;
  };

  if(existsCamera){

  if((listLen >0) && (list[0].isOpen == false)){
    QCam_OpenCamera(list[0].cameraId, &camHandle);
    WriteToLog("Camera: QCAM Camera Open");
  } else {
    WriteToLog("QCam Couldn't Open Camera");
    WriteToLog("Panicking! AaaaaaaaaaaAAAA!!!!!!!");
    GtoT_CameraError(255);
    system("shutdown -h now");
  };

  for(i=0;i<FRAMES;i++){
    frame[i].pBuffer = new unsigned char [CHIPWIDTH * CHIPHEIGHT];
    frame[i].bufferSize = CHIPWIDTH*CHIPHEIGHT;
    buffer[i] = (unsigned char *) frame[i].pBuffer;
  };

  settings.size = sizeof(settings);
  QCam_ReadDefaultSettings(camHandle, &settings);

  

  // now let's initialize the IO card counters so that the appropriate 
  // trigger times are set up. 

  

  ioperm(IOCARDADDR,32,1);
  outb(0x74,IOCARDADDR+0x0F);
  outb(0xE8,IOCARDADDR+0x0D);
  outb(0x03,IOCARDADDR+0x0D);
  isStreaming = false;
  changeSettings();
  
  nextframe = 0;
  frameNumber =  0;

  }
  setCenterSpecX(0);
  setCenterSpecY(0);
  normgain = 1000000;
  offset = 0;
  frameCounter = 0;

  delay.tv_sec = 0;
  delay.tv_nsec = 1000000;

  UserELOffset = UserCEOffset = 0;
  RoiGainEL = RoiGainCE = 1.0;

  //  FCGainEL = FCGainCE = 0.50;
  // Values changed in Ft Sumner, May 10, 2009
  FCGainEL = FCGainCE = 0.33;



  // now we set up the frame-blob structures.
  printf("I am here!\n");

  for(i=0; i<FRAMES;i++){
    fblob[i].commonconstructor((MAPTYPE *) buffer[i], 1360, 1036, 8,this->platescale/3600.);
    fblob[i].set_gain(1);
    fblob[i].set_readout_offset(0);
    fblob[i].set_readout_noise(0);
    fblob[i].set_satval(255);
    fblob[i].set_threshold(5.0);
    fblob[i].set_disttol(900);
    fblob[i].set_grid(32);
    fblob[i].set_cenbox(17);
    fblob[i].set_apbox(5);
    fblob[i].set_maxblobs(100);
  };

  for(i=0; i<FRAMES;i++){
    smallblobs[i].commonconstructor((MAPTYPE *) buffer[i], 1360, 1036, 8,this->platescale/3600.);
    smallblobs[i].set_gain(1);
    smallblobs[i].set_readout_offset(0);
    smallblobs[i].set_readout_noise(0);
    smallblobs[i].set_satval(255);
    smallblobs[i].set_threshold(5.0);
    smallblobs[i].set_disttol(900);
    smallblobs[i].set_grid(32);
    smallblobs[i].set_cenbox(17);
    smallblobs[i].set_apbox(5);
    smallblobs[i].set_maxblobs(100);
  };
  
  WriteToLog("QCAMCONSTRUCTOR"," %lf",fblob[0].get_threshold());

  // setting the platescale to 1.000 so that everything is in terms of pixels.
  setPlateScale(1.000);
  setRotation(0.0);
  setTargetBlob(0);
  flatfield = (float *) malloc(CHIPWIDTH*CHIPHEIGHT*sizeof(float));
  // not using a flatfield. too slow.
  //  FILE *ifile;
  // ifile = fopen("flatfield.dat","r");
  // fread(flatfield,4,CHIPHEIGHT*CHIPWIDTH,ifile);
  // fclose(ifile);

  setActiveSensor(OTHERSENSOR);
  MoveStepSize = 100;
  StartFrameNumber();

  gondolaoffsetx = gondolaoffsety=0;
  feellucky = false;

  smallTrigT = 33;
  largeTrigT = 111;
  smallExpT = 30000;
  largeExpT = 100000;
};


QCamera::~QCamera(){
  if(existsCamera){
    stopStreaming();
    QCam_CloseCamera(camHandle);
    QCam_ReleaseDriver();
    free(flatfield);
  };
};


void QCamera::StartFrameNumber(){
  FILE *ifile;
  ifile = fopen("framenumber.dat","r");
  if(ifile){
    unsigned long fmnb;
    fscanf(ifile,"%ld\n",&fmnb);
    fclose(ifile);
    WriteToLog("SFN","Read fmnb value %ld",fmnb);
    fmnb = (fmnb/1000+1)*1000;
    frameCounter = fmnb;
  } else {
    ifile = fopen("framenumber.dat","w");
    fprintf(ifile,"0\n");
    fclose(ifile);
  };
};


void QCamera::WriteFrameNumber(){
  FILE *ifile;
  ifile = fopen("framenumber.dat","w");
  if(ifile){
    fprintf(ifile,"%ld\n",frameCounter);
    fclose(ifile);
  } else {
    // send some sort of error to the TM
  };
};

unsigned int QCamera::getFrame(){
      if(!(frameCounter%1000)){
      WriteFrameNumber();
      WriteToLog("FrameCounter","1000 Tick : %ld",frameCounter);
    };
   
    return frameCounter++;
}

void QCamera::setTriggerTime(){
  
  unsigned char msb, lsb;
  unsigned short tt;
  unsigned char *ptr;
  if(nowMode.isValid){
    tt = nowMode.gettrigt();
    ptr = (unsigned char *) &tt;
    
    msb = ptr[0];
  lsb = ptr[1];
  //  printf("Outputting trigger time %d\n",msb);
  ioperm(IOCARDADDR,32,1);
  outb(0x34,IOCARDADDR+0x0F);
  outb(msb,IOCARDADDR+0x0C);
  outb(lsb,IOCARDADDR+0x0C);
  };
    return;
  
};


  unsigned short QCamera::getRoiX(){return nowMode.getroiX();};
  unsigned short QCamera::getRoiY(){return nowMode.getroiY();};
  unsigned short QCamera::getRoiDX(){return nowMode.getroiDX();};
  unsigned short QCamera::getRoiDY(){return nowMode.getroiDY();};
  unsigned int QCamera::getExpT(){ return nowMode.getExpT();};
  unsigned char QCamera::getTrigT(){ return nowMode.getTrigT();};

  void QCamera::setRoi(unsigned short x, unsigned short y, unsigned short dx, unsigned short dy, unsigned int et, unsigned char tt){
    nextMode.setModeParams(x,y,dx,dy,et,tt);
  };
  
  void QCamera::setRestoreMode(GuiderMode id, unsigned char subid){
    nextMode.setMode(id, subid);
  };
  

  void QCamera::setOutputELGain(double oel){
    outputELGain = oel;
  };
  
  void QCamera::setOutputCEGain(double oce){
    outputCEGain = oce;
  };
  
  void QCamera::setOutputELOffset(double oelo){
    outputELOffset = oelo;
  };

  void QCamera::setOutputCEOffset(double oceo){
    outputCEOffset = oceo;
  }

  void QCamera::setNormalizedGain(unsigned int gain){
    normgain = gain;
    return;
  };

  unsigned int QCamera::onlyGetFrame(){
    return frameCounter;
  };


  GuiderMode QCamera::getModeID(){
    return nowMode.getID();
  };

  unsigned char QCamera::getSubModeID(){
    return nowMode.getsubID();
  };

  void QCamera::setModeID(GuiderMode md, unsigned char sm){
    nowMode.setMode(md,sm);
  };

  void QCamera::setSubModeID(unsigned char smid){
    nowMode.setsubID(smid);
  };

  void QCamera::setNextSubModeID(unsigned char smid){
    nextMode.setsubID(smid);
  };

// --------

  unsigned short QCamera::getWinX(){ return nowMode.winX; };
  unsigned short QCamera::getWinY(){ return nowMode.winY; };
  unsigned short QCamera::getWinDX(){ return nowMode.winDX; };
  unsigned short QCamera::getWinDY(){ return nowMode.winDY; };

  void QCamera::setNextWinX(unsigned short x){ nextMode.winX = x; };
  void QCamera::setNextWinY(unsigned short y){ nextMode.winY = y; };
  void QCamera::setNextWinDX(unsigned short dx){ nextMode.winDX = dx; };
  void QCamera::setNextWinDY(unsigned short dy){ nextMode.winDY = dy; };

  void QCamera::setNowWinX(unsigned short x){ nowMode.winX = x; };
  void QCamera::setNowWinY(unsigned short y){ nowMode.winY = y; };
  void QCamera::setNowWinDX(unsigned short dx){ nowMode.winDX = dx; };
  void QCamera::setNowWinDY(unsigned short dy){ nowMode.winDY = dy; };


  void QCamera::setNextWin(unsigned short wx, unsigned short wy, unsigned short wdx, unsigned short wdy){
    nextMode.winX = wx;
    nextMode.winY = wy;
    nextMode.winDX = wdx;
    nextMode.winDY = wdy;
    
    if(nextMode.winDX>100) nextMode.winDX = 100;
    if(nextMode.winX<nextMode.winDX) nextMode.winX = nextMode.winDX;
    if(nextMode.winX+nextMode.winDX> CHIPWIDTH) nextMode.winX = CHIPWIDTH-nextMode.winDX-1;

    if(nextMode.winDY>100) nextMode.winDY = 100;
    if(nextMode.winY<nextMode.winDY) nextMode.winY = nextMode.winDY;
    if(nextMode.winY+nextMode.winDY> CHIPHEIGHT) nextMode.winY = CHIPHEIGHT-nextMode.winDY-1;
};

  void QCamera::setNowWin(unsigned short wx, unsigned short wy, unsigned short wdx, unsigned short wdy){
    nowMode.winX = wx;
    nowMode.winY = wy;
    nowMode.winDX = wdx;
    nowMode.winDY = wdy;
    
    if(nowMode.winDX>100) nowMode.winDX = 100;
    if(nowMode.winX<nowMode.winDX) nowMode.winX = nowMode.winDX;
    if(nowMode.winX+nowMode.winDX> CHIPWIDTH) nowMode.winX = CHIPWIDTH-nowMode.winDX-1;

    if(nowMode.winDY>100) nowMode.winDY = 100;
    if(nowMode.winY<nowMode.winDY) nowMode.winY = nowMode.winDY;
    if(nowMode.winY+nowMode.winDY> CHIPHEIGHT) nowMode.winY = CHIPHEIGHT-nowMode.winDY-1;
};

//--------
  float QCamera::getUserELOffset(){return UserELOffset;};
    void QCamera::setUserELOffset(float el){UserELOffset = el;};
  
  float QCamera::getUserCEOffset(){  return UserCEOffset;};
  void QCamera::setUserCEOffset(float ce){UserCEOffset = ce;};

  float QCamera::getRoiGainEL(){return RoiGainEL;};
  void QCamera::setRoiGainEL(float gel){ RoiGainEL = gel;};

  float QCamera::getRoiGainCE(){return RoiGainCE;};
  void QCamera::setRoiGainCE(float gce) { RoiGainCE = gce;};

  float QCamera::getFCGainEL(){ return FCGainEL;};
  void QCamera::setFCGainEL(float gel){ FCGainEL = gel;};

  float QCamera::getFCGainCE(){ return FCGainCE;}
  void QCamera::setFCGainCE(float gce){ FCGainCE = gce; };


  float QCamera::userRoiEL(float el){
    return (el ) * RoiGainEL;
  };

  float QCamera::userRoiCE(float ce){
    return (ce) * RoiGainCE;
  };

  float QCamera::userFCEL(float el){
    return (el ) * FCGainEL;
  };

  float QCamera::userFCCE(float ce){
    return (ce ) * FCGainCE;
  };



///----




void QCamera::FlattenChip(unsigned char qid){

  int i,j;
  int idx;
  unsigned short dx,dy;
  float val;
  dy = CHIPHEIGHT;
  dx = CHIPWIDTH;
  for(j=0;j<dy;j++)
    for(i=0;i<dx;i++){
      idx = i+CHIPWIDTH*j;
      val = buffer[qid][idx]/flatfield[idx];
      if(val>=255){
	buffer[qid][idx]=255;
      } else {
	buffer[qid][idx] = (unsigned char) val;
    };

    }

}


bool QCamera::changeSettings(){

  static int qerr;
  static int trycount;
  if(!isStreaming){
    if(nextMode.isValid && nextModeReady){
      nowMode.setEqual(nextMode);
      nextModeReady = false;
      setTriggerTime();
      nanosleep(&delay,NULL);
      QCam_SetParam(&settings, qprmRoiX, nowMode.getroiX());
      QCam_SetParam(&settings, qprmRoiY, nowMode.getroiY());
      QCam_SetParam(&settings, qprmRoiWidth, nowMode.getroiDX());
      QCam_SetParam(&settings, qprmRoiHeight, nowMode.getroiDY());
      QCam_SetParam(&settings, qprmReadoutSpeed, qcReadout20M);
      QCam_SetParam(&settings, qprmNormalizedGain, normgain);
      QCam_SetParamS32(&settings, qprmS32AbsoluteOffset,offset);
      QCam_SetParam64(&settings, qprm64Exposure, nowMode.getexpt()*1000);
      QCam_SetParam(&settings, qprmTriggerType, qcTriggerEdgeLow);
      //      QCam_SetParam(&settings, qprmTriggerType, qcSoftware);
      WriteToLog("Change","%d, %d, %d, %d, %d, %d",nowMode.getroiX(), nowMode.getroiY(), nowMode.getroiDX(), nowMode.getroiDY(), nowMode.getexpt(), nowMode.gettrigt());
      nowMode.winX = nextMode.winX;
      nowMode.winY = nextMode.winY;
      nowMode.winDX = nextMode.winDX;
      nowMode.winDY = nextMode.winDY;
      trycount = 0;
    tryagain:
      try{
  if(existsCamera){
	qerr = QCam_SendSettingsToCam(camHandle, &settings);
	WriteToLog("QCamera","Result = %d",qerr);
  };
	nanosleep(&delay,NULL);
      }
      catch(std::bad_exception &e1){
	WriteToLog("QCam","Caught ChangeSettings Exception!");
	trycount++;
	if(trycount==5){
	  WriteToLog("QCam","That's 5 of them!. Giving Up!");
	  exit(-1);
	} else { 
	  goto tryagain;
	}
      };
      
      WriteToLog("QCam","Settings Transferred");
      //printf("Settings sent successfully\n");

      //      nanosleep(&delay,NULL);
      
      // and save the state for recovery.
      SaveQCamState();
      WriteToLog("sm","%d",getSubModeID());
      return true;
    };
  };
  
  return false;
};


char QCamera::prepSettings(Mode &mset, double ra, double dec){

  nextMode.setEqual(mset);

  targetRA = ra;
  targetDEC = dec;
  nextModeReady = true;

  return 1;
};

char QCamera::prepSameSettings(){

  nextMode.setEqual(nowMode);
  nextModeReady = true;

  return true;
};

bool QCamera::startStreaming(){

  static int qerr;
  if(!isStreaming){
    nanosleep(&delay, NULL);
      if(existsCamera){
    qerr=QCam_SetStreaming(camHandle, 1);
      };
    nanosleep(&delay, NULL);
    if(!qerr){
      isStreaming = true;
      WriteToLog("QCam","Started Streaming.");
      return true;
    } else {
      WriteToLog("QCam","Error starting streaming");
    }
  };

  return false;
};

bool QCamera::stopStreaming(){

  static int qerr;
  if(isStreaming){
    WriteToLog("QCam","About to stop streaming");
  boo:
    try{
      nanosleep(&delay, NULL);
  if(existsCamera){
      qerr = QCam_SetStreaming(camHandle, 0);    
  };
    } 
    catch(...){
      WriteToLog("QCam","Caught stop streaming exceptions.");
      goto boo;
    };
    WriteToLog("QCam","Streaming stopped");
    nanosleep(&delay, NULL);
    nanosleep(&delay, NULL);
    WriteToLog("QCam","About to abort frames");
    qerr = QCam_Abort(camHandle);
    nanosleep(&delay, NULL);
    WriteToLog("QCam","Aborted frames.");
    isStreaming = false;
    return true;
  };
  isStreaming = false;

  return false;

};

unsigned char QCamera::queueFrame(){
  int qerr = 1;
  if(existsCamera){

  
  qerr = QCam_QueueFrame(camHandle, &frame[nextframe], nowMode.CallbackFunction, qcCallbackDone, this, nextframe);
  };
  if(!qerr){
    
    framenumber[nextframe] = frameNumber;
    nextframe++;
    frameNumber++;

    if(nextframe == FRAMES) nextframe = 0;
    
  };
 
  return qerr;
};

double QCamera::getRotation(){
  return rotation;
};

void QCamera::setRotation(double rot){
  if(rot>=0 && rot < 2*M_PI) rotation = rot;
  cosangle = cos(rotation);
  sinangle = sin(rotation);
  return;
};

double QCamera::getPlateScale(){
  return platescale;
};

void QCamera::setPlateScale(double ps){
  static int i;
  platescale = ps;
  for(i=0;i<FRAMES;i++){
    fblob[i].setPlatescale(ps/3600.);
  };
  return;
};

OpticalSensor QCamera::getActiveSensor(){
  return activesensor;
};

void QCamera::setActiveSensor(OpticalSensor as){
  activesensor = as;
  return;
};

double QCamera::getRA(){
  return RAroi;
};

void QCamera::setRA(double ra){
  RAroi = ra;
  return;
};

double QCamera::getDEC(){
  return DECroi;
};

void QCamera::setDEC(double dec){
  DECroi = dec;
  return;
};



void QCamera::setTargetDEC(double dec){
  targetDEC = dec;
  return;
};

double QCamera::getTargetDEC(){
  return targetDEC;
};

void QCamera::setTargetRA(double ra){
  targetRA = ra;
  return;
};

double QCamera::getTargetRA(){
  return targetRA;
};


void QCamera::setCenterSpecX(double x){
  SpecCenterX = x;
  return;
};

double QCamera::getCenterSpecX(){
  return SpecCenterX;
};

void QCamera::setCenterSpecY(double y){
  SpecCenterY = y;
  return;
};

double QCamera::getCenterSpecY(){
  return SpecCenterY;
};
  

// functions to handle the frameblob objects.

void QCamera::setFrameBlobXpix(unsigned int xpix_in){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].setXpix(xpix_in);
  };
};

void QCamera::setSmallBlobXpix(unsigned int xpix_in){
  int i;
  for(i=0;i<FRAMES;i++){
    smallblobs[i].setXpix(xpix_in);
  };
};


void QCamera::setFrameBlobYpix(unsigned int ypix_in){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].setYpix(ypix_in);
  };
};

void QCamera::setSmallBlobYpix(unsigned int ypix_in){
  int i;
  for(i=0;i<FRAMES;i++){
    smallblobs[i].setYpix(ypix_in);
  };
};


void QCamera::setFrameBlobGain(double gain_in){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_gain(gain_in);
    smallblobs[i].set_gain(gain_in);
  };
};

void QCamera::setFrameBlobReadoutOffset(double in_readout_offset){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_readout_offset(in_readout_offset);
    smallblobs[i].set_readout_offset(in_readout_offset);
  };
};

void QCamera::setFrameBlobReadoutNoise(double in_readout_noise){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_readout_noise(in_readout_noise);
    smallblobs[i].set_readout_noise(in_readout_noise);
  };
    
};

void QCamera::setFrameBlobSatVal(MAPTYPE in_satval){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_satval(in_satval);
    smallblobs[i].set_satval(in_satval);
  };

};

void QCamera::setFrameBlobGrid(unsigned int in_grid){
  int i;
  for(i=0;i<FRAMES; i++){
    fblob[i].set_grid(in_grid);
    smallblobs[i].set_grid(in_grid);
  };
};

void QCamera::setFrameBlobCenBox(unsigned int in_cenbox){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_cenbox(in_cenbox);
    smallblobs[i].set_cenbox(in_cenbox);
  };
};

void QCamera::setFrameBlobApBox(unsigned int in_apbox){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_apbox(in_apbox);
    smallblobs[i].set_apbox(in_apbox);
  }
};

void QCamera::setFrameBlobThreshold(double in_threshold){
  int i;
  for(i=0;i<FRAMES;i++){
    fblob[i].set_threshold(in_threshold);
    smallblobs[i].set_threshold(in_threshold);
  };
};

void QCamera::setFrameBlobDistTol(int in_disttol){
  int i;
  for(i=0; i<FRAMES;i++){
    fblob[i].set_disttol(in_disttol);
    smallblobs[i].set_disttol(in_disttol);
  };
};

MAPTYPE QCamera::getFrameBlobSatVal(){
  return this->fblob[0].get_satval();
};
unsigned int QCamera::getFrameBlobXpix(){
  return fblob[0].getXpix();
};

unsigned int QCamera::getFrameBlobYpix(){
  return fblob[0].getYpix();
};

double QCamera::getFrameBlobGain(){
  return fblob[0].get_gain();
};

double QCamera::getFrameBlobReadoutOffset(){
  return fblob[0].get_readout_offset();
};

double QCamera::getFrameBlobReadoutNoise(){
  return fblob[0].get_readout_noise();
};

unsigned int QCamera::getFrameBlobGrid(){
  return fblob[0].get_grid();
};

unsigned int QCamera::getFrameBlobCenBox(){
  return fblob[0].get_cenbox();
};

unsigned int QCamera::getFrameBlobApBox(){
  return fblob[0].get_apbox();
};

double QCamera::getFrameBlobThreshold(){
  WriteToLog("uhm","The value i will report will be: %lf\n",fblob[0].get_threshold());
  return fblob[0].get_threshold();
};


int QCamera::getFrameBlobDistTol(){
  return fblob[0].get_disttol();
};


void QCamera::setTargetBlob(unsigned short tb){
  targetBlob = tb;
};

unsigned short QCamera::getTargetBlob(){
  return targetBlob;
};



bool QCamera::SaveQCamState(){
  // open the file for writing
  FILE *ofile;
  ofile = fopen("lastmode.mds","w");
  // save the mode
  fprintf(ofile,"%d\n",(char) getModeID());
  // save the submode
  fprintf(ofile,"%d\n", getSubModeID());
  // save roi info
  fprintf(ofile,"%d %d %d %d\n", getRoiX(), getRoiY(), getRoiDX(), getRoiDY());
  // save time info
  fprintf(ofile,"%d %d\n",getExpT(), getTrigT());
  // save window info
  fprintf(ofile,"%d %d %d %d\n",nowMode.winX, nowMode.winY, nowMode.winDX, nowMode.winDY);
  // save target RA and DEC
  fprintf(ofile,"%.10lf %.10lf\n",targetRA, targetDEC);
  // save the tracking RA and DEC info
  fprintf(ofile,"%.10lf %.10lf\n",RAroi, DECroi);
  // save the track point
  fprintf(ofile,"%d %d\n",trackpointCE, trackpointEL);
  //save the active sensor
  fprintf(ofile,"%d\n", getActiveSensor());
  // save the frameblob thresholds.
  fprintf(ofile,"%lf\n",fblob[0].get_threshold());
  // save the calculated offset.
  fprintf(ofile,"%lf %lf\n",gondolaoffsetx, gondolaoffsety);
  // save the short and long times;
  fprintf(ofile,"%d %d %d %d\n",smallTrigT, smallExpT, largeTrigT, largeExpT);
  //shortTrigT, shortExpT, longTrigT
  // close the file
  fclose(ofile);
  return true;
};

char QCamera::LoadQCamState(){
  FILE *ifile;
  ifile = fopen("lastmode.mds","r");
  GuiderMode md;
  unsigned char smd;
  unsigned short rx, ry, rdx, rdy;
  unsigned int expt;
  unsigned char trigt;
  unsigned short wx, wy, wdx, wdy;
  double tra, tdec;
  double pra, pdec;
  double gox, goy;
  unsigned int a1, a2, a3,a4;
  float thr;
  
  if(ifile){
    WriteToLog("LoadState","There appears to  have been a crash. Loaded the following values from last time:");

    fscanf(ifile,"%d\n",&a1);
    md = (GuiderMode) a1;
    fscanf(ifile,"%d\n",&a1);
    smd = (unsigned char) a1;
    setRestoreMode(md,smd);


    WriteToLog("LoadState", "Mode = %d",(unsigned char)md);
    WriteToLog("LoadState", "Sub Mode = %d",smd);

    fscanf(ifile,"%d %d %d %d\n", &a1, &a2, &a3, &a4);
    rx = (unsigned short) a1;
    ry = (unsigned short) a2;
    rdx = (unsigned short) a3;
    rdy = (unsigned short) a4;
    
    fscanf(ifile,"%d %d", &a1, &a2);
    expt = a1;
    trigt = (unsigned char) a2;
    fscanf(ifile,"%d %d %d %d\n", &a1, &a2, &a3, &a4);
    wx = (unsigned short) a1;
    wy = (unsigned short) a2;
    wdx = (unsigned short) a3;
    wdy = (unsigned short) a4;
    nextMode.winX = wx;
    nextMode.winY = wy;
    nextMode.winDX = wdx;
    nextMode.winDY = wdy;
    nowMode.winX = wx;
    nowMode.winY = wy;
    nowMode.winDX = wdx;
    nowMode.winDY = wdy;
    fscanf(ifile,"%lf %lf\n",&tra, &tdec);
    fscanf(ifile,"%lf %lf\n",&pra, &pdec);
    fscanf(ifile,"%d %d\n",&a1, &a2);
    WriteToLog("The","values %d %d",a1,a2);
    trackpointCE = (unsigned short)a1;
    trackpointEL = (unsigned short)a2;

    fscanf(ifile,"%d\n", &a1);
    fscanf(ifile,"%f\n",&thr);
    fscanf(ifile,"%lf %lf\n",&gox, &goy);
    gondolaoffsetx = gox;
    gondolaoffsety = goy;

    smd = a1;
    setActiveSensor((OpticalSensor)smd);
    fscanf(ifile,"%d %d %d %d",&a1, &a2, &a3, &a4);

      smallTrigT = (unsigned char)a1;
      smallExpT = a2;
      largeTrigT = (unsigned char)a3;
      largeExpT = a4;


    WriteToLog("LoadState", "roi = %d %d %d %d",rx,ry,rdx, rdy);
    WriteToLog("LoadState","expt = %d us, trigt = %d ms",expt,trigt);
    WriteToLog("LoadState", "tracking window = %d %d %d %d",wx,wy,wdx, wdy);
    WriteToLog("LoadState","tracking point = %d %d",trackpointEL, trackpointCE);
    WriteToLog("LoadState","FrameBlob threshold %lf",thr);
    setFrameBlobThreshold(thr);
    setRoi(rx,ry,rdx,rdy,expt,trigt);
    targetRA = tra;
    targetDEC = tdec;
    RAroi = pra;
    DECroi = pdec;
    fclose(ifile);
  } else {

    return 0;
  };

  
  return 1;
}



void QCamera::setMoveStepSize(float stepsize){
  if(stepsize<75){
    MoveStepSize = stepsize;
  } else {
    MoveStepSize = 75;
  };
};
