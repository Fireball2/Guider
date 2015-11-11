#include <QMutexLocker>
#include <QMutex>
#include "messages.h"
#include "GuiderToTMTC.h"
#include "writelog.h"
#include "comsetup.h"
#include <stdio.h>
#include <string.h>
#include "mpfmessages.h"
#include "QCam.h"
#include "dac.h"
#include "frameblob.h"
#include "bloblist.h"
#include "gps_func.h"
#include "ChipAndSky.h"
#include "LEDs.h"

#include <mcheck.h>
#define MEMCHECK

unsigned short GtoT_Count();
char GtoT_StartCount();
unsigned short GtoT_CountVar;


DTUPointingErrorsData DTUPED;
ELCEPositionData ELCEPD;
MPFStatus MPFS;
DTUQuaternions DTUQ;

message tmp;
unsigned short *_pus;
unsigned char *_puc;
unsigned int * _pui;
short *_ps;
float *_pf;
double *_pd;
unsigned long *_pul;
unsigned short _cnt;

QMutex GtoT_mutex;


#define GTOTMAXCOUNT 65536

void GtoT_FrameBlobs(frameblob *fb){
  bloblist *pblob;
  unsigned short x[10],y[10];
  unsigned int flux[10];
  double cx, cy;
  unsigned char blobs;
  unsigned char currentblob;
  static message m;

  currentblob = 0;
  blobs = 0;

  pblob = fb->getblobs();
  m.length = 89;
  m.body[0] = 0x55;
  _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 82;  
  
  m.body[5] = 250;
  
  static int i;
  for(i=0;i<10;i++){
    x[i] = 0;
    y[i] = 0;
  };

  while(pblob!=NULL && currentblob<10){
    x[currentblob]=(unsigned short) floor(pblob->getx()+0.5);
    y[currentblob]=(unsigned short) floor(pblob->gety()+0.5);
    flux[currentblob] = (unsigned int) pblob->getflux();
    pblob = pblob->getnextblob();
    currentblob++;
  };

  blobs = currentblob;
  m.body[6] = blobs;
  for(i=0;i<10;i++){
    _pus= (unsigned short *) &m.body[7+8*i];
    *_pus = x[i];
    _pus= (unsigned short *) &m.body[9+8*i];
    *_pus = y[i];
    _pui = (unsigned int *) &m.body[11+8*i];
    *_pui = flux[i];
  };
  m.body[87]=m.FindCRC();
  m.body[88] = 0xAA;

  SendToTMTC(m);
  

};


void GtoT_AutocollimationOffsets(float x, float y, float r){//251
  static message m;
  m.length = 20;
  m.body[0] = 0x55;
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 13;  
  
  m.body[5] = 251;
  
  _pf = (float *) &m.body[6];
  *_pf = x;
  
  _pf = (float *) &m.body[10];
  *_pf = y;

  _pf=(float*) &m.body[14];
  *_pf = r;
  
  m.body[18]=m.FindCRC();
  m.body[19] = 0xAA;
  
  SendToTMTC(m);
  //  WriteToLog("Sent out pointint errors!\0");  
  return;
};

void GtoT_GuiderStatus(QCamera *cam){//252
  static message m;
  GuiderStatus *gs;
  

  gs = (GuiderStatus *) &m.body[0];
  (*gs).mid = 252;
  m.length = 58;
    
  (*gs).startb = 0x55;

  _cnt = GtoT_Count();
  (*gs).count = _cnt;
  (*gs).length = m.length - 7;
  // ---------------------------------------
  (*gs).guidermode = (char) cam->getModeID();
  (*gs).guidersubmode = (char) cam->getSubModeID();
  (*gs).activesensor = (char) cam->getActiveSensor();

  (*gs).frame = (unsigned int) cam->onlyGetFrame();


  (*gs).roix = cam->getRoiX();
  (*gs).roiy = cam->getRoiY();
  (*gs).roidx = cam->getRoiDX();
  (*gs).roidy = cam->getRoiDY();

  (*gs).tt = cam->getTrigT();
  (*gs).et = cam->getExpT();

  (*gs).wx = cam->getWinX();
  (*gs).wy = cam->getWinY();
  (*gs).wdx = cam->getWinDX();
  (*gs).wdy = cam->getWinDY();
  (*gs).tx = trackpointCE;
  (*gs).ty = trackpointEL;

  (*gs).lvdt = leakylvdt;

  if(getOutput()){
    (*gs).dacstatus= 0;
  } else {
    (*gs).dacstatus= 1;
  };

  if(ledstatus){
    (*gs).lightstatus = 1; 
  } else { 
    (*gs).lightstatus = 0;
  };

  (*gs).gpslat = gps_latitude;
  (*gs).gpslon = gps_longitude;
  (*gs).gpsalt = gps_altitude;

  (*gs).crc = m.FindCRC();
  (*gs).endb = 0xAA;

  SendToTMTC(m);

  return;
};


void GtoT_CameraError(unsigned char err){ //253
  static message m;

  m.length = 9;
  m.body[0] = 0x55;
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 2;  

  m.body[5] = 253;
  m.body[6] = err;
  m.body[7] = m.FindCRC();
  m.body[8] = 0xAA;

  SendToTMTC(m);
  return;
};

void GtoT_PointingErrors(float el, float ce){
  static message m;

  
  m.length = 16;
  m.body[0] = 0x55;
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 9;  
  
  m.body[5] = 255;
  
  _pf = (float *) &m.body[6];
  *_pf = el;
  
  _pf = (float *) &m.body[10];
  *_pf = ce;
  
  m.body[14]=m.FindCRC();
  m.body[15] = 0xAA;
  
  SendToTMTC(m);
  //  WriteToLog("Sent out pointint errors!\0");  
  return;
};


void GtoT_MessageRcvd(unsigned char msg){
  static message m;
  m.length = 9;
  
  m.body[0]=0x55;

    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    m.body[5] = 254;
    
    m.body[6] = msg;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    //    sprintf(logentry,"GtoT,AutoCollimationTCReply,%d",success);
    //    WriteToLog(logentry);
}; 




void GtoT_AutoCollimationTCReply(unsigned char success)
{
  static char logentry[128];
  static char err;
  static message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    m.body[5] = 120;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,AutoCollimationTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
}

void GtoT_SlewingTCReply(unsigned char success)
{
  static char logentry[128];
  static char err;
  static message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 121;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,SlewingTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
}

void GtoT_AlignmentTCReply(unsigned char success)
{
static   char logentry[128];
static   char err;
static  message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 122;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,AlignmentTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
}

void GtoT_ELCETCReply(unsigned char success)
{
static  char logentry[128];
static   char err;
static   message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 123;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,ELCETCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
};

void GtoT_TextError(char *str){
static   char logentry[255];
static  int i;
static   unsigned short msglng;
static   message m;

  msglng = strlen(str);

  //  sprintf(logentry,"STRINGLENGTH %d",msglng);
  //  WriteToLog(logentry);

  m.length= msglng + 8;


  m.body[0]=0x55;
  
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;


  _pus = (unsigned short *) &m.body[3];
  *_pus = msglng+1;

  _puc = (unsigned char *) &m.body[5];
  *_puc = 124;


  sprintf(logentry,"GtoT:");


  for(i=0;i<msglng;i++){
    m.body[6+i] = str[i];
    logentry[13+i]= str[i];
  };
  logentry[13+msglng]=0;
  m.body[m.length-2] = m.FindCRC();
  m.body[m.length-1] = 0xAA;
  

  SendToTMTC(m);
  //  sprintf(logentry,"GTT:233");
  //  WriteToLog(logentry);  

  return;
};

void GtoT_TextError(message msg){
static  char logentry[128];
static   message m;
static   int i;
static   unsigned short msglng;

  msglng = msg.length;

  m.length = msglng+8;

  m.body[0]=0x55;
  
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;
  
  _pus = (unsigned short *) &m.body[3];
  *_pus = msglng+1;
  
  _pus = (unsigned short *) &m.body[5];
  *_puc = 124;

  sprintf(logentry,"GtoM,Message:");

  for(i=0;i<msglng;i++){
    m.body[6+i] = msg.body[i];
    logentry[13+i]=msg.body[i];
  };
  logentry[13+msglng]=0;
  m.body[m.length-2] = m.FindCRC();
  m.body[m.length-1] = 0xAA;
  
  
  SendToTMTC(m);
  
  WriteToLog(logentry);

  
  return;
};

void GtoT_CameraStatusTCReply(
			      unsigned short ROIX,
			      unsigned short ROIY,
			      unsigned short ROIdX,
			      unsigned short ROIdY,
			      unsigned char binning,
			      unsigned short expt
			      ){

static  char logentry[128];
static   message m;





  m.length = 19;

  m.body[0]=0x55;
  
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;
  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 12;
  
  _pus = (unsigned short *) &m.body[5];
  *_puc = 130;

  _pus = (unsigned short *) &m.body[6];
  *_pus = ROIX;

  _pus = (unsigned short *) &m.body[8];
  *_pus = ROIY;

  _pus = (unsigned short *) &m.body[10];
  *_pus = ROIdX;

  _pus = (unsigned short *) &m.body[12];
  *_pus = ROIdY;

  m.body[14] = binning;

  _pus = (unsigned short *) &m.body[15];
  *_pus = expt;

  m.body[17] = m.FindCRC();
  m.body[18] = 0xAA;  
  
  SendToTMTC(m);

  sprintf(logentry,"GtoT,CameraStatusTCReply,%d,%d,%d,%d,%d,%d",ROIX, ROIY,ROIdX, ROIdY, binning, expt);
  
  WriteToLog(logentry);

  

  return;
}

void GtoT_VideoStatusTCReply(
			     unsigned char freq,
			     unsigned char cross,
			     unsigned char GPSt,
			     unsigned char GPScoord,
			     unsigned char RADEC,
			     unsigned char contrast,
			     unsigned char brightness,
			     unsigned char gamma
			     ){

static   char logentry[128];
static   message m;



  m.length = 16;

  m.body[0]=0x55;
  
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;
  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 9;
  
  _pus = (unsigned short *) &m.body[5];
  *_puc = 131;

  m.body[6] = freq;
  m.body[7] = cross;
  m.body[8] = GPSt;
  m.body[9] = GPScoord;
  m.body[10] = RADEC;
  m.body[11] = contrast;
  m.body[12] = brightness;
  m.body[13] = gamma;

  m.body[14] = m.FindCRC();
  m.body[15] = 0xAA;
  
  SendToTMTC(m);
  
  sprintf(logentry,"GtoT,VideoStatusTCReply,%d,%d,%d,%d,%d,%d,%d,%d",freq,cross,GPSt,GPScoord,RADEC,contrast,brightness,gamma);
  
  return;
};

void GtoT_CameraROIChangeTCReply(
				 unsigned char success
				 ){
static  char logentry[128];
static   char err;
static  message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 132;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,CameraROIChangeTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
}

void GtoT_CameraExposureTimeTCReply(
				    unsigned char success
				    ){


static   char logentry[128];
static   char err;
static   message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 133;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,CameraExposureTimeTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
};


void GtoT_VideoFramePeriodChangeTCReply(
				    unsigned char success
				    ){

static   char logentry[128];
static  char err;
static   message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 134;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,VideoFramePeriodChangeTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
};


void GtoT_VideoDisplayChangeTCReply(
				    unsigned char success
				    ){

static   char logentry[128];
static   char err;
static  message m;

  err = 0;

  if(success == 0 || success == 1){
    
    m.length = 9;
    m.body[0]=0x55;
    
    _cnt = GtoT_Count();
    _pus = (unsigned short *) &m.body[1];
    *_pus = _cnt;

    _pus = (unsigned short *) &m.body[3];
    *_pus = 2;

    _pus = (unsigned short *) &m.body[5];
    *_puc = 135;
    
    m.body[6] = success;
    
    m.body[7]=m.FindCRC();
    m.body[8] = 0xAA;
    
    SendToTMTC(m);

    sprintf(logentry,"GtoT,VideoDisplayChangeTCReply,%d",success);
    WriteToLog(logentry);
  }; 
  return;
};

// THIS NEEDS TO BE THOUGHT OUT!!!
void GtoT_FrameTCReply(
		       unsigned short ROIdX,
		       unsigned short ROIdY
		       // there should be more here!!!!
		       );

void GtoT_StatusUpdate(
		       unsigned long gpst,
		       unsigned short gpslond,
		       unsigned short gpslonm,
		       float gpslons,
		       unsigned char gpslonc,
		       unsigned short gpslatd,
		       unsigned short gpslatm,
		       float gpslats,
		       unsigned char gpslatc,
		       double GSra,
		       double GSdec,
		       double GRra,
		       double GRdec,
		       unsigned char GRvalid,
		       unsigned char Gmode,
		       unsigned char MPFmode,
		       unsigned char GSensor,
		       unsigned char MPFsensor,
		       unsigned short expt
		       ){
  //  char logentry[128];
static  message m;
  //  int i;
  //  unsigned short msglng;



  m.length = 73;

  m.body[0]=0x55;
  
  _cnt = GtoT_Count();
  _pus = (unsigned short *) &m.body[1];
  *_pus = _cnt;
  
  _pus = (unsigned short *) &m.body[3];
  *_pus = 66;
  
  _pus = (unsigned short *) &m.body[5];
  *_puc = 140;

  
  _pul = (unsigned long *) &m.body[6];
  *_pul = gpst;

  _pus = (unsigned short *) &m.body[14];
  *_pus = gpslond;
  
  _pus = (unsigned short *) &m.body[16];
  *_pus = gpslonm
;
  _pf = (float *) &m.body[18];
  *_pf = gpslons;

  _puc = (unsigned char *) &m.body[22];
  *_puc = gpslonc;

  _pus = (unsigned short *) &m.body[23];
  *_pus = gpslatd;

  _pus = (unsigned short *) &m.body[25];
  *_pus = gpslatm;
  _pf = (float *) &m.body[27];
  *_pf = gpslats;
  _puc = (unsigned char *) &m.body[31];
  *_puc = gpslatc;

  _pd = (double *) &m.body[32];
  *_pd = GSra;

  _pd = (double *) &m.body[40];
  *_pd = GSdec;

  _pd = (double *) &m.body[48];
  *_pd = GRra;

  _pd = (double *) &m.body[56];
  *_pd = GRdec;

  m.body[64] = GRvalid;
  m.body[65] = Gmode;
  m.body[66] = MPFmode;
  m.body[67] = GSensor;
  m.body[68] = MPFsensor;

  _pus = (unsigned short *) &m.body[69];
  *_pus = expt;

  m.body[71] = m.FindCRC();
  m.body[72] = 0xAA;
  
};



char GtoT_StartCount(){

  QMutexLocker locker(&GtoT_mutex);
  GtoT_CountVar = 0;
  WriteToLog("GtoT Message Count Started.");
  
  return 0;
};

unsigned short GtoT_Count(){
static   unsigned short ret;
static   int err;

  QMutexLocker locker(&GtoT_mutex);

  err = 0;

  ret = GtoT_CountVar;

  if(GtoT_CountVar != GTOTMAXCOUNT-1){
 GtoT_CountVar++;
  } else {
    GtoT_CountVar = 0;
    WriteToLog("GtoT Message Count Reset to 0.");
  };
  

  
  return ret;
};


