#include "decodeTtoG.h"
#include "decode.h"
#include "messages.h"
#include "ChipAndSky.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "writelog.h"
#include "GuiderToTMTC.h"
#include "QCam.h"


// variables defined in "decode.h" that allow us to parse the messages 
// quickly. 
extern unsigned short *pus;
extern short *ps;
extern float *pf;
extern double *pd;
extern unsigned char *puc;
extern unsigned char *puc;
extern unsigned long *pul;
unsigned int *pui;

unsigned short TCount;

/**** 
      This functions in this file decode the messages sent from the ground to the guider. 
***/

/*****
      Message ID 200
      Init Telecommand (TC);
      
      This is here for completeness mainly and to adjust the counter count.
****/

void T200_InitTC(message &){
};

/****
    Message ID 201
    Autocollimation TC

    This is here for completeness and to adjust the counter count.
****/

void T201_AutoCollimationTC(message &){

};

/**** 
      Message ID 202
      Alignment TC
      takes RA & DEC in J2000 coordinates.
      
      returns 0 if the values are valid and 0 otherwise.
***/



char T202_AlignmentTC(message &m,
		      double &ra,
		      double &dec){



  //  WriteToLog("1");
  pd = (double*) &m.body[6];
  ra = *pd;
  //    WriteToLog("2");
  pd = (double*) &m.body[14];
  dec = *pd;
  //  WriteToLog("3");

  //  WriteToLog("4");
  if(ra<0) return 1;
  if(ra>M_PI) return 1;
  if(dec<-M_PI/4) return 1;
  if(dec>M_PI/4) return 1;
  
  if( (ra<0) || (ra>M_PI) || (dec<-M_PI/4.) || (dec > M_PI/4)) return 1;

  return 0;
};




/**** 
      Message ID 203
      Slewing TC
      takes RA & DEC in J2000 coordinates.
      
      returns 0 if the values are valid and 0 otherwise.
***/

char T203_SlewingTC(message &m,
		      double &ra,
		      double &dec){




  pd = (double*) &m.body[6];
  ra = *pd;
  
  pd = (double*) &m.body[14];
  dec = *pd;



  if(ra<0) return 1;
  if(ra>M_PI) return 1;
  if(dec<-M_PI/4) return 1;
  if(dec>M_PI/4) return 1;

  if( (ra<0) || (ra>M_PI) || (dec<-M_PI/4.) || (dec > M_PI/4)) return 1;

  return 0;
};


/***
    Message ID 204
    EL+CE Offsets TC
    takes EL + CE offsets (floats);
    
    returns 0 if the EL CE offsets are smaller than the chip size and 1 otherwise.
***/

char T204_ELCEOffsetTC(message &m, float &el, float &ce){



  pf = (float*) &m.body[6];
  el = *pf;
  pf = (float*) &m.body[10];
  ce = *pf;



  return 0;
};



/***
    Message ID 205
    Alignment Track  TC
    forces the guider to take control of the pointing 
    
    returns 1 if any parts of the ROI go outside the chip 0 otherwise.
***/

char  T205_AlignTrackTC(message &m,
		      unsigned char &roiflag,
		      unsigned short &roicx,
		      unsigned short &roicy,
		      unsigned short &roid,
		      unsigned char &trigt,
		      unsigned int &expt){

  roiflag = m.body[6];
  
  pus = (unsigned short *) &m.body[7];
  roicx = *pus;

  pus = (unsigned short *) &m.body[9];
  roicy = *pus;

  pus = (unsigned short *) &m.body[11];
  roid = *pus;
  
  trigt = m.body[13];


  pui = (unsigned int *) &m.body[14];
  expt = *pui;
  //  diff = m.body[14];


  if(roicx+roid > CHIPWIDTH) roid = CHIPWIDTH-roicx;
  if(roicx-roid < 0) roicx = roid;
  if(roicy+roid > CHIPHEIGHT) roid = CHIPWIDTH-roicy;
  if(roicy-roid <0 ) roicy = roid;

  if( (roicx+roid> CHIPWIDTH) || (roicx-roid < 0) || (roicy + roid > CHIPHEIGHT) || (roicy - roid < 0))  return 1;

  return 0;
};


/*** 
     Message ID 206
     X + Y offsets in autocollimation mode
     sends the appropriate EL and CE offset commands to the 
     MPFN, adjusted for pixel size and the platescale;
***/
void T206_AutoCollimationXYOffsets(message &m,
				   float &xoffset,
				   float &yoffset
				   ){
  pf = (float *) &m.body[6];
  xoffset = *pf;
  pf = (float *) &m.body[10];
  yoffset = *pf;
  return;
};


/***
    Message ID 207
    Sets the coordinates of the autocollimation zoom
***/
void T207_AutocollimationZoomXY(message &m,
				unsigned short &roix,
				unsigned short &roiy
				){



  pus = (unsigned short *) &m.body[6];
  roix = *pus;
  pus = (unsigned short *) &m.body[8];
  roiy = *pus;



  if (roix >= CHIPWIDTH || roix<0) roix = CHIPWIDTH/2;
  if (roiy >= CHIPHEIGHT || roiy<0) roiy = CHIPHEIGHT/2;

  return;
  
				     
};





/***
    Message ID 210
    Set Region of Interest TC
    for a specified mode
    sets the ROIx, ROIy, ROIdx and ROIdy variables

    returns 0 if the region of interest can be fit on the screen, 1 if not.
***/

char T210_SetROITC(message &m,
		   unsigned char &mode,
		   unsigned short &roix,
		   unsigned short &roiy,
		   unsigned short &roidx,
		   unsigned short &roidy
		   ){
  
  mode = m.body[6];
  
  pus = (unsigned short *) m.body[7];
  roix = *pus;
  pus = (unsigned short *) m.body[9];
  roiy = *pus;
  pus = (unsigned short *) m.body[10];
  roidx = *pus;
  pus = (unsigned short *) m.body[13];
  roidy = *pus;
  

  //  WriteToLog("210","%d %d %d",roix, roiy, roidx);


  if( (roix+roidx> CHIPWIDTH) || (roiy+roidy > CHIPHEIGHT)) return 1;

  return 0;
};


/***
    Message ID 211
    Set Trigger Period  TC
    for a specified mode
    sets the trigger time and the difference between the trigger time and the exposure length

    returns 0 if the region of interest can be fit on the screen, 1 if not.
***/

char T211_TriggerPeriodTC(message &m,
			  unsigned char &mode,
			  unsigned char &trigt,
			  unsigned int &expt
			  ){

  mode = m.body[6];
  trigt = m.body[7];
  pui = (unsigned int *) &m.body[8];
  expt = *pui;
  


  //  if((diff>5) || (trigt < 25) || (trigt > 500)) return 1;
  return 0;
};


/***
    Message ID 212
    Set Camera Normalized Gain  TC

***/

char T212_SetNormalizedGainTC(message &m,
							unsigned int &gain
							  ){

static unsigned int *pui;

pui  = (unsigned int *) &m.body[6];
gain = *pui;
  


  return 0;
};


/***
    Message ID 213
    Set AbsoluteOffsetTC

***/

char T213_SetAbsoluteOffsetTC(message &m,
			      short &offset
			      ){

ps = (short *) &m.body[6];
offset = *ps;
  


  return 0;
};



/***
    Message ID 215
    Set Display Controls  TC
    for a specified mode
    sets the brightness, contrast and gamma (on a specific scale!)
***/


void T215_SetDisplayControlsTC(message &m,
			       unsigned char &brightness,
			       unsigned char &contrast,
			       unsigned char &gamma){


  brightness = m.body[6];
  contrast = m.body[7];
  gamma = m.body[8];
  




  return;
};


/***
    Message ID 216
    Frame Update Period  TC
    for a specified mode
    sets the period (in frames) for fresh updates
***/

void T216_FrameUpdatePeriodTC(message &m,
			      unsigned char &mode,
			      unsigned char &period
			      ){

  mode = m.body[6];
  period = m.body[7];



  return;
};


/****
     Message ID 217
     Display Format TC
     formats what the display looks like for a particular mode. 
     binning, location of the top left corner of the chip, etc. 
****/

void T217_DisplayFormatTC(message &m,
			  unsigned char &mode,
			  unsigned char &bin,
			  unsigned short &cornerx,
			  unsigned short &cornery,
			  unsigned char &ticks,
			  unsigned char &centroid,
			  unsigned char &unused1,
			  unsigned char &unused2
			  ){


  mode = m.body[6];
  bin = m.body[7];

  pus = (unsigned short *) m.body[8];
  cornerx = *pus;

  pus = (unsigned short *) m.body[10];
  cornery = *pus;

  ticks = m.body[11];
  centroid = m.body[12];
  unused1 = m.body[13];
  unused2 = m.body[14];


  return;
};


/***
    Message ID 218
    Frame Image Stack  TC
    number of images to stack for display and disk writing purposes

***/

void T218_ImageStackTC(message &m,		       
		       unsigned char &Ndisplay,
		       unsigned char &Ndisk
		       ){

  Ndisplay = m.body[6];
  Ndisk = m.body[7];
  


  return;
};

void T219_NudgeTracking(message &m,
			char &elnudge,
			char &cenudge){
  elnudge = m.body[6];
  cenudge = m.body[7];
  return;
};






/****
     Message ID 230
     Focus TC
     how much to move the stage focus by in stage units. 
****/

void T230_FocusTC(message &m,
			  short &stagemove
			  ){

  ps = (short *) &m.body[6];
  stagemove = *ps;



  return;
};


/****
     Message ID 231
     Auto Focus TC
     Tries to autofocus when tracking on a star.
****/

void T231_AutoFocusTC(message &
		      ){
  

  return;
};


/****
     Message ID 232
     Turn ON LEDs TC
     Turns on the focal plane LEDs. 
****/

void T232_TurnOnLEDsTC(message &
		      ){
  

  return;
};


/****
     Message ID 233
     Turn Off LEDs TC
     Turns on the focal plane LEDs. 
****/

void T233_TurnOffLEDsTC(message & 
			){
  

  return;
};


/****
     Message ID 235
     Set Camera Plate Scale
     Sets the plate scale for the camera
     retuns 0 if platescale is within the interval (1,1.2) and 1 otherwise
****/
char T235_SetCameraPlateScaleTC(message &m,
				double &platescale
				){

  pd = (double *) &m.body[6];
  platescale = *pd;
  


  if(platescale<1 || platescale >1.2) return 1;

  return 0;
};

void T234_TagLog(message &m,
	      unsigned short &nb
	      ){

  pus = (unsigned short *) &m.body[6];
  nb = *pus;
  
  return;
};


/****
     Message ID 236
     Reset Camera Plate Scale
     Resets the plate scale for the camera to the default value

****/
void T236_ResetCameraPlateScaleTC(message &m
				){




  return;
};

/****
     Message ID 237
     Set Rotational Correction between CCD and gondola
     returns 0 if the angle is within 0 to 2 Pi, 1 otherwise

****/
char T237_SetRotationTC(message &m,
			double &rot
			){
  
  pd = (double *) &m.body[6];
  rot = *pd;
  

  
  
  if( (rot <0) || (rot > 2*M_PI)) return 1;
  
  return 0;
};



void T220_OffsetsAndGains(message &m, 
			  unsigned char &id,
			  float &value
			  ){
  id = m.body[6];
  pf = (float *) &m.body[7];
  value = *pf;
  return;
};
  

void T221_FBGain(message &m, 
		 double &fbgain
		 ){
  pd = (double *) &m.body[6];
  fbgain = *pd;
  return;
};


void T222_FBSatVal(message &m,
		   unsigned char &sv
		   ){
  sv = m.body[6];
  return;
};

void T223_FBThreshold(message &m, 
		 double &thr
		 ){
  pd = (double *) &m.body[6];
  thr = *pd;
  return;
};

void T224_FBDistTol(message &m,
		    int &dt
		    ){
  int *pi;
  pi = (int *) &m.body[6];
  dt = *pi;
  return;
};

void T225_FBGrid(message &m,
		 unsigned int &grd){
  pui = (unsigned int *) &m.body[6];
  grd = *pui;
  return;
};

void T226_FBCenBox(message &m,
		   unsigned int &cb){
  pui = (unsigned int *) &m.body[6];
  cb = *pui;
  return;
};

void T227_FBApBox(message &m,
		  unsigned int &ab){
  pui = (unsigned int *) &m.body[6];
  ab = *pui;
  return;
};

void T228_FBReadoutOffset(message &m,
			  double &ro
			  ){
  pd = (double *) &m.body[6];
  ro = *pd;
  return;
};

void T229_FBReadoutNoise(message &m,
			double &rg
			){
  pd = (double *) &m.body[6];
  rg = *pd;
  return;
};

void T238_DACOutput(message &m,
		    unsigned char &daco
		    ){
  daco = m.body[6];
  return;
};

void T239_TrackBlob(message &m,
		    unsigned char &blob
		    ){
  blob = m.body[6];
};

void T240_InfoBlob(message &m,
		   unsigned char &blob
		   ){
  blob = m.body[6];
};



void T242_SetTrackPoint(message &m,
			unsigned short &tpx,
			unsigned short &tpy
			){
  pus = (unsigned short *) &m.body[6];
  tpx = *pus;
  pus = (unsigned short *) &m.body[8];
  tpy = *pus;
};

void T243_SetMoveStepSize(message &m,
			  float &stepsize){
  pf = (float *) &m.body[6];
  stepsize = *pf;
};


// This sets the received message counter //
void TCountSet(){
  TCount = 0;
};


LockedFC *lockedFC;
LockedROI *lockedROI;
GuidingFC *guidingFC;
GuidingROI *guidingROI;


void T245_LockedFC(message &m,
		   LockedFC &lfc){
  
  lockedFC = (LockedFC *) &m.body[0];
  lfc.wx = (*lockedFC).wx;
  lfc.wy = (*lockedFC).wy;
  lfc.wd = (*lockedFC).wd;
  lfc.tpx = (*lockedFC).tpx;
  lfc.tpy = (*lockedFC).tpy;
  lfc.expt = (*lockedFC).expt;
  lfc.trigt = (*lockedFC).trigt;
  return;
};


void T246_LockedROI(message &m, LockedROI &lroi){
  lockedROI = (LockedROI*) &m.body[0];
  lroi.rx = (*lockedROI).rx;
  lroi.ry = (*lockedROI).ry;
  lroi.rd = (*lockedROI).rd;
  lroi.expt = (*lockedROI).expt;
  lroi.trigt = (*lockedROI).trigt;
  return;
};

void T247_GuidingFC(message &m, GuidingFC &gfc){
  guidingFC = (GuidingFC*) &m.body[0];
  gfc.wx = (*guidingFC).wx;
  gfc.wy = (*guidingFC).wy;
  gfc.wd = (*guidingFC).wd;
  gfc.ra = (*guidingFC).ra;
  gfc.dec = (*guidingFC).dec;
  gfc.expt = (*guidingFC).expt;
  gfc.trigt = (*guidingFC).trigt;
  return;
};

void T248_GuidingROI(message &m, GuidingROI &groi){
  guidingROI = (GuidingROI*) &m.body[0];
  groi.rx = (*guidingROI).rx;
  groi.ry = (*guidingROI).ry;
  groi.rd = (*guidingROI).rd;
  groi.ra = (*guidingFC).ra;
  groi.dec = (*guidingFC).dec;
  groi.expt = (*guidingROI).expt;
  groi.trigt = (*guidingROI).trigt;
  return;
};


void T164_WindowTrack(message &m, WindowTrack &wt){
  WindowTrack *WT;
  WT = (WindowTrack *) &m.body[0];
  wt.wx  = (*WT).wx;
  wt.wy  = (*WT).wy;
  wt.wd  = (*WT).wd;
  wt.tx  = (*WT).tx;
  wt.ty  = (*WT).ty;
  return;
};

void T165_SmallTime(message &m, CamTime &ct){
  CamTime *st;
  st = (CamTime *) &m.body[0];
  ct.trigt = (*st).trigt;
  ct.expt = (*st).expt;

};

void T166_LargeTime(message &m, CamTime &ct){
  CamTime *st;
  st = (CamTime *) &m.body[0];
  ct.trigt = (*st).trigt;
  ct.expt = (*st).expt;
};


void T012_DitherPattern(message &m, unsigned char &uc){
  uc = m.body[6];
};


void T015_OvermaxCnt(message &m,
		     float &f){
  float *pf;
  pf = (float *) &m.body[6];
  f = *pf;
};

void T016_OvermaxThr(message &m,
		     float &f){
  float *pf;
  pf = (float *) &m.body[6];
  f = *pf;
}

void T017_ToggleAlgorithm(message &m){
};


// change the RMS spot criterion. 
void T018_SpotRMSThr(message &m,
		     float &f){
  float *pf;
  pf = (float *) &m.body[6];
  f = *pf;
};


// divisor step
void T019_BoxSize(message &m,
		  char &c){
  c = m.body[6];
  if( c<5 ) c=5;
  if( c>40) c=40;
};


