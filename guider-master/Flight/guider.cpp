/*****************************
Guider FLIGHT program. 
******/

//I want to be somewhat careful in what I include over here to make
//sure that only those files that are necessary are put in. no extra
//fluff. we're on a diet!

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//error checking include
#include <assert.h>

//timing include 
#include <time.h>


//include functions to process information from the ground and from
//the MPF num

#include "decodeMtoG.h"
#include "decodeTtoG.h"



// include the camera class
#include "QCam.h"

// include the modes class
#include "mds.h"

// include mode specific display values and the display 
#include "displaymodes.h"
#include "display.h"

// we have to be able to write to the log
#include "writelog.h"

// must be able to identify commands
#include "commandIDs.h"

// must be able to send messages to the ground and the mpf
#include "GuiderToMPF.h"
#include "GuiderToTMTC.h"
#include "GuiderToDet.h"


//must be able to loook at messages and access the message stack
#include "messages.h"
#include "messagestack.h"


//have to be able to set up ports and such
#include "comsetup.h"

// have to be able to look at blobs
#include "frameblob.h"

//some constants and functions used for centroiding
#include "centroiding.h"

//have to be able to access some of the dac variables
#include "dac.h"

//centroiding
#include "centroiding.h"


//chip and sky
#include "ChipAndSky.h"
#include "skytime.h"

//light control
#include "LEDs.h"


// we might want to dither at some point
#include "dither.h"

//small class to do some simple mean and rms calculations
#include "statistic.h"





InitMD initMD;
AutocollimationMD autocollimationMD;
AlignmentMD alignmentMD;
SlewingMD slewingMD;
PointingMD pointingMD;
DisplayMD displayMD;
 
Dither HelloDither;

FrameInfo FInfo;





// now function declarations for the callbacks

void QCAMAPI InitModeCallback(void *frame, unsigned long info, QCam_Err err, unsigned long ul2);

void QCAMAPI AutocollimationModeCallback(void *frame, unsigned long info, QCam_Err err, unsigned long ul2);

void QCAMAPI AlignmentModeCallback(void *frame, unsigned long info, QCam_Err err, unsigned long ul2);

void QCAMAPI SlewingModeCallback(void *frame, unsigned long info, QCam_Err err, unsigned long ul2);

void QCAMAPI PointingModeCallback(void *frame, unsigned long info, QCam_Err err, unsigned long ul2);

void QCAMAPI SlewPointModeCallback(void *frame, unsigned long info, QCam_Err err, unsigned long ul2);


Mode InitMode(INITMODE,0,0,0,1360, 1036, 100000, 111);
Mode AutocollimationMode(AUTOCOLLIMATIONMODE,0,0,0,1360,1036,100000, 111);
Mode AlignmentMode(ALIGNMENTMODE,0,0,0,1360,1036,100000,111);
Mode AlignmentSubMode(ALIGNMENTMODE,0,580,418,200,200,30000,33);
Mode SlewingMode(SLEWINGMODE,0,0,0,1360,1036,40,111);
Mode PointingMode(POINTINGMODE,0,0,0,1360,1036,100,111);
Mode PointingModeRoi(POINTINGMODE,0,518,480,200,200,100,111);

// some data from the MPF that most functions need access to.
#include "mpfmessages.h"

DTUPointingErrorsData DTUPE;

int main(){ // the argument list is not used
  // but there is provision for it.

  printf("Welcome to the FIREBALL Guider. Please wait while we configure\n");

    fflush(NULL);

  StartLog();
  GtoM_StartCount(); 
  GtoT_StartCount();
  ResetReceivedMessageCounters();

  trackpointEL = CHIPHEIGHT/2;
  trackpointCE = CHIPWIDTH/2;

  QCamera camera; // open the camera.

  //  UniversalTime();
  //  printf("The Local Sidereal Time is: %lf\n",LST(31.761, -95.63));

  /*
  double ra1, ra2;
  double dec1, dec2;
  double alt1, alt2;
  double az1, az2;
  double del, dce;
  double lat, lon;
  */ 

  GtoT_CameraError(0);

  SetLatLon(31.761,-95.64);

  /*  lat = 31.761;
  lon = -95.64;
  ra1 = 0.50;
  ra2 = 0.51;
  dec1 = 0.41;
  dec2 = 0.41;
  RADEC_to_ALTAZ(ra1,dec1, lat,lon, &alt1,&az1);
  printf("the first alt-az I have found: %lf %lf\n",alt1,az1);
  RADEC_to_ALTAZ(ra2,dec2, lat,lon, &alt2,&az2);
  printf("the second alt-az I have found: %lf %lf\n",alt2,az2);
  ALTAZ_to_ELCE(alt1,az1,alt2,az2,&del,&dce); 
  printf("The difference I have found is: %lf %lf\n",del,dce);
  */
  InitDAC(); // prepare the dac.

  InitializeLEDs(); // turn off the Lamp.

  if(!ComSetup()) GtoT_TextError("COM working");


  //  RunSillyProg();

  StartDisplay(); // begin the display.

  turnOnGps();
  

  // starts some of the statistic quantities. Defaulting to 100 frames -- 10s in full chip mode, 3s in ROI mode. 





  InitMode.setCallback(InitModeCallback);
  AutocollimationMode.setCallback(AutocollimationModeCallback);
  AlignmentMode.setCallback(AlignmentModeCallback);
  AlignmentSubMode.setCallback(AlignmentModeCallback);
  SlewingMode.setCallback(SlewPointModeCallback);
  PointingMode.setCallback(SlewPointModeCallback);
  PointingModeRoi.setCallback(SlewPointModeCallback);
 
  camera.prepSettings(InitMode,0,0);
  if(camera.LoadQCamState()){
    switch(camera.nextMode.getModeID()){
    case INITMODE:{
      camera.nextMode.setCallback(InitModeCallback);
      break;
    };
    case ALIGNMENTMODE:{
      camera.nextMode.setCallback(AlignmentModeCallback);
      alignmentMD.winx = camera.getWinX();
      alignmentMD.winy = camera.getWinY();
      alignmentMD.wind = camera.getWinDX();
      WriteToLog("Stuff","%d %d %d",alignmentMD.winx,alignmentMD.winy,alignmentMD.wind);
      break;
    };
    case SLEWINGMODE:{
      camera.nextMode.setCallback(SlewPointModeCallback);
      break;
    };
    case POINTINGMODE:{
      camera.nextMode.setCallback(SlewPointModeCallback);
      break;
    };
    case AUTOCOLLIMATIONMODE:{
      camera.nextMode.setCallback(AutocollimationModeCallback);
      break;
    };  
    default:
      break;
    };
  };
  //  camera.prepSettings(SlewingMode,0,0);
  //  camera.prepSettings(PointingMode,0,0);
  //  camera.nowMode.setEqual(AlignmentMode);
 
  //    HomeCamera();
  
    //MoveStage(30000);

  // periodicmessages.h/c contains the variables and calls needed to 
  // set up periodic messages that have to be sent to the MPF and the 
  // ground in various modes. 
#include "periodicmessages.h"
  
  // processmessages.h/c contains the variables and calls needed to 
  // process the messages from the MPF and the ground.
#include "processmessages.h"


  //for every change of mode there will be a goto mainloop command
  //to exit a program there will be a goto loopexit command. 

  FillRSquared();
  //  if(camera.getActiveSensor() == GUIDERSENSOR){
  //    GtoM_SwitchingGuiderRequest();
  //  };
  //  HelloDither.DitherPattern1(10);
 MainLoop:
  
  camera.stopStreaming();  // stop camera streaming


  camera.changeSettings(); // adjust camera parameters, including 
                           // the trigger time.

  modeID = camera.getModeID();

  camera.startStreaming(); // restart camera streaming. 


  //  WriteToLog("Got Here! B ");
  loopcounter = 0;

  if(camera.getModeID() == POINTINGMODE || (camera.getModeID()==ALIGNMENTMODE && camera.getSubModeID()==ALIGNTRACKROI)){
    ClearScreen();
    FontOverwrite();
  } else {
    FontMask();
  };
  
  //  WriteToLog("Got Here A !");

  if(camera.getModeID()==ALIGNMENTMODE && (camera.getSubModeID() == ALIGNTRACKROI || camera.getSubModeID() == ALIGNTRACKFULLCHIP || camera.getSubModeID() == ALIGNDRAGFULLCHIP)){
    //    GtoM_SwitchingGuiderRequest();
    //    camera.setActiveSensor(GUIDERSENSOR);
  } else {
    //    GtoM_SwitchingDTURequest();
    //    camera.setActiveSensor(OTHERSENSOR);
  }

  //    WriteToLog("Got Here!");
  //    WriteToLog("FLOOD","%d %d %d" ,camera.getRoiX(), camera.getRoiY(),camera.getRoiDX());
  do{

    #include "periodicmessages.cpp"
    #include "processmessages.cpp"

    camera.queueFrame();
    


  } while(1);
  

  camera.~QCamera();
  CloseLog();

  return 0;
};



// and now the definitions of the callbacks.


#include "initSubCode.cpp"
#include "autocollimationSubCode.cpp"
#include "alignmentSubCode.cpp"

void QCAMAPI SlewPointModeCallback(void *cam, unsigned long info, QCam_Err err, unsigned long ){
  int qID;
  qID = info;
 
  unsigned char *buffer;
  static QCamera *camera;
  camera = (QCamera *) cam;
  
  

  //  WriteToLog("Hello");

  static unsigned char subMode;
  static SlewingSubMode slewingsubmode;
  static PointingSubMode pointingsubmode;
  static frameblob blob;
  static GuiderMode guiderMode;
  //  static unsigned char guiderSubMode;
  static bool switchtopointing;
  static bool lostPointing,guidtrk;
  static float lastblobx, lastbloby=0;
  static int lastcount;
  unsigned short x,y,dx,dy;
  unsigned short wx, wy, wdx, wdy;
  static bloblist *pblob;
  static unsigned int frmnumber;
  double el,ce;

  guidtrk = false;
  frmnumber = camera->onlyGetFrame();
  if(MPFS.PointControl == 0) guidtrk = true;
  StatusToDetector(guidtrk, err);
  FrameNumberToDetector(frmnumber);

  
  subMode = camera->getSubModeID();

  if(guiderMode == POINTINGMODE){
    pointingsubmode = (PointingSubMode) subMode;
  } else if(guiderMode == SLEWINGMODE){
    slewingsubmode = (SlewingSubMode) subMode;
  };

  if(err){
    GtoT_CameraError(err);
    WriteToLog("PNTG","Error(%d)",err);
  };


  x = camera->getRoiX();
  y = camera->getRoiY();
  dx = camera->getRoiDX();
  dy = camera->getRoiDY();
  buffer = camera->buffer[qID];

  guiderMode = camera->getModeID();
  subMode = camera->getSubModeID();

  wx = camera->getWinX();
  wy = camera->getWinY();
  wdx = camera->getWinDX();
  wdy = camera->getWinDY();


  switchtopointing = false;
  lostPointing = false;


  if(err){
    GtoT_CameraError(err);
  };

  //  WriteToLog("hello","%d",(guiderMode == POINTINGMODE && (pointingsubmode == LOCKEDROI || pointingsubmode == TRACKINGROI)));

  


  if(!( guiderMode == POINTINGMODE && (pointingsubmode == LOCKEDROI || pointingsubmode == TRACKINGROI))){
    camera->setFrameBlobXpix(CHIPWIDTH);
    camera->setFrameBlobYpix(CHIPHEIGHT);
    camera->fblob[qID].clearblobs();
    camera->fblob[qID].calc_mapstat();
    camera->fblob[qID].calc_mapstat();
    camera->fblob[qID].calc_searchgrid();
    camera->fblob[qID].calc_centroid();
    camera->fblob[qID].calc_flux();
    camera->fblob[qID].fix_multiple();
    camera->fblob[qID].sortblobs();
    pblob = camera->fblob[qID].getblobs();
  };


  
  WriteToLog("SLPT","I think the mode is: %d\n",(char) guiderMode);

  // do this if in slewing mode
  if(guiderMode==SLEWINGMODE){
    slewingsubmode = (SlewingSubMode) subMode;
    switch(slewingsubmode){
    case SEARCHINGDTU:{
      static double bx,by;
      WriteToLog("SLEW","SearchingDTU");

      if(fabs(DTUPE.EL)<500 && fabs(DTUPE.CE)<500){
	
	//oooh... we found ourselves within the range of the chip!,
	//assuming the alignment between the guider and the DTU is
	//calculated correctly. Soooooo... we will 
	if(pblob!=NULL){
	  
	  //okay! we actually found a blob!
	  while(pblob!=NULL){
	    
	      bx = pblob->getx();
	      by = pblob->gety();
	      
	    if(fabs(bx-CHIPWIDTH/2)<500 && fabs(by-CHIPHEIGHT/2)<400){
	      
	      //alright! we found a blob within our range!
	      lastblobx = pblob->getx();
	      lastbloby = pblob->gety();
	      lastcount = 0;
	      camera->setModeID(SLEWINGMODE,(unsigned char) WAITINGTOSETTLE);
	      

	      return;
	    }
	    //	    WriteToLog("hmmmm...");
	    
	    pblob = pblob->getnextblob();

	  };
	  
	};

      };
      
      PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0 ,err);	
      // otherwise we repeat the exercise!
      return;
      break;
    };

    case WAITINGTOSETTLE:{
      WriteToLog("SLEW","Waiting to settle");
      if(fabs(DTUPE.EL)<500 && fabs(DTUPE.CE)<500){
	// super! we are still within range!
	if(pblob!=NULL){
	  //okay! we actually still have a blob.
	  while(pblob!=NULL){
	    if(fabs(pblob->getx()-lastblobx)<100 && fabs(pblob->gety()-lastbloby)<100){
	      //alright! we found a blob within range!
	      lastblobx = pblob->getx();
	      lastbloby = pblob->gety();
	      lastcount++;
	      if(lastcount == 10){
		camera->setModeID(POINTINGMODE,LOCKED);
		camera->setNowWin((unsigned short) floor(lastblobx+0.5),(unsigned short) floor(lastbloby+0.5),100,100);
		camera->pointingTrackPointX = lastblobx;
		camera->pointingTrackPointY = lastbloby;
		switchtopointing = true;
		//		goto donemodespecific;
		PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0 ,err);	
		return;
	      };
	    };
	    //	    WriteToLog("Blah");
	    pblob = pblob->getnextblob();
	  };

	} else {
	      camera->setModeID(SLEWINGMODE,(unsigned char)SEARCHINGDTU);
	};   
	} else {
	      camera->setModeID(SLEWINGMODE,(unsigned char)SEARCHINGDTU);

	};
      PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0 ,err);	
      //  if(!qerr && 
      break;
      return;
    };

    case SEARCHINGHUMAN:{
      if(camera->SlewingSwitchToPointing){
	if(pblob!=NULL){
	  //okay! we actually found a blob!
	  while(pblob!=NULL){
	    if(fabs(pblob->getx()-CHIPWIDTH/2)<500 && fabs(pblob->gety()-CHIPHEIGHT/2)<400){
	      //alright! we found a blob within our range!
	      lastblobx = pblob->getx();
	      lastbloby = pblob->gety();
	      camera->setModeID(POINTINGMODE,LOCKED);
	      //	      camera->setModeID(POINTINGMODE);
	      camera->setNowWin((unsigned short) floor(lastblobx+0.5),(unsigned short) floor(lastbloby+0.5),100,100);
	      camera->pointingTrackPointX = lastblobx;
	      camera->pointingTrackPointY = lastbloby;
	      switchtopointing = true;
	      goto donemodespecific;
	    };
	  };

	};	

      };
      break;
    };


    };

    // do this if in pointing mode.
  } else if(guiderMode==POINTINGMODE){
    static double targetX, targetY;
    static double targetRA, targetDEC;
    static double scienceX, scienceY;
    static double scienceRA, scienceDEC;
    static bool isblob;
    static double bx,by;
    static double distx, disty, dist;
    static bool movewin;
    
    //    WriteToLog("Entered POINTINGMODE conditional");
    //    WriteToLog("PNTG","Pointing submode recognized = %d",pointingsubmode);
    switch(pointingsubmode){
      
    case LOCKED:{
      if(!err){
      // in this mode we are using the full chip. A box is selected
      // and if there is a star inside the box, that star is then
      // tracked on the chosen fixed point.

      // step 1
      // select target
      targetX = camera->pointingTrackPointX;
      targetY = camera->pointingTrackPointY;
      
      // step 2
      // search for blobs

      /* but this was already done!
      camera->fblob[qID].clearblobs();
      camera->fblob[qID].calc_mapstat();
      camera->fblob[qID].calc_mapstat();
      camera->fblob[qID].calc_searchgrid();
      camera->fblob[qID].calc_centroid();
      camera->fblob[qID].calc_flux();
      camera->fblob[qID].fix_multiple();
      camera->fblob[qID].sortblobs();
      pblob = camera->fblob[qID].getblobs();
      */
      // step 3
      // look for blob in window
      isblob = false;
      if(pblob!=NULL){
	
	while(pblob!=NULL){
	  
	  bx = pblob->getx();
	  by = pblob->gety();
	  isblob = false;
	  movewin = false;
	  if(bx>wx-wdx && bx<wx+wdx && by>wy-wdy && by<wy+wdy){
	    // so the blob is in the box. 
	    // now check how far from the tracking point the star is.
	    distx = bx - targetX;
	    disty = by - targetY;
	    ce = camera->cosangle * distx + camera->sinangle*disty;
	    el = -camera->sinangle * distx + camera->cosangle*disty;
	    if(distx > 50 || disty >50) movewin = true;
	    if(distx*distx+disty*disty>0){
	      dist = sqrt(distx*distx + disty*disty);
	      if(dist>camera->MoveStepSize){	     
		distx = distx/dist * camera->MoveStepSize;
		disty = disty/dist * camera->MoveStepSize;
	      }
	    }else {
	      distx = disty = 0;
	    };
	    
	    
	    el *= camera->getPlateScale();
	    ce *= camera->getPlateScale();
	    
	    SendOutEL(-el);
	    SendOutCE(-ce);
	    WriteToLog("PNTG","(%ld)EL=%lf,CE=%lf,trkX=%lf,trkY=%lf",frmnumber,el,ce,targetX,targetY);
	    GtoT_PointingErrors(el,ce);
	    FInfo.send(frmnumber, leakylvdt, el,ce,camera->getExpT(),err);
	    
	    if(movewin) {
	      camera->setNowWin((unsigned short)bx,(unsigned short) by, 100,100);
	    };
	    isblob = true;
	    goto done;
	  };
	  
	  pblob = pblob->getnextblob();
	};
	//	PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0 ,err);  

      } else {
	// there were no blobs on the screen. this is a problem. how
	// do we handle it? 
	distx = 0;
	disty = 0;
	FInfo.send(frmnumber,leakylvdt, 3000.0,3000.0,camera->getExpT(),255);
	GtoT_PointingErrors(0,0);
	isblob = false;
	WriteToLog("PNTG","(%ld)NoBlob",frmnumber);
      };
      };
    done:
      PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0 ,err);
      if(!err){
	if(!(frmnumber % 10)){
	  GtoT_FrameBlobs(&(camera->fblob[qID]));
	  FrameToLog(frmnumber,dx,dy,camera->buffer[qID]);
	}
      };
      return;
      break;
    }; //LOCKED
      
    case LOCKEDROI:{
      static int loststarcounter;
      // just like the locked mode, except that we use a region of
      // interest instead of a subwindow of the main window.
      targetX = dx/2;
      targetY = dy/2;
      // now find the blobs in the small region of interest.
      camera->setSmallBlobXpix(2*dx);
      camera->setSmallBlobYpix(2*dy);

      camera->smallblobs[qID].clearblobs();
      camera->smallblobs[qID].calc_mapstat();
      camera->smallblobs[qID].calc_mapstat();
      camera->smallblobs[qID].calc_searchgrid();
      camera->smallblobs[qID].calc_centroid();
      camera->smallblobs[qID].calc_flux();
      camera->smallblobs[qID].fix_multiple();
      camera->smallblobs[qID].sortblobs();
      pblob = camera->smallblobs[qID].getblobs();
      
      //okay. now let's see if we have a blob in the image.
      if(pblob!=NULL){
	// okay, there is a star. 
	//	loststarcounter = 0;
	bx = pblob->getx();
	by = pblob->gety();
	camera->setNowWinX((unsigned short)bx);
	camera->setNowWinY((unsigned short)by);
	distx = bx - targetX;
	disty = by - targetY;
	
	distx *= camera->getPlateScale();
	disty *= camera->getPlateScale();
	
	ce = camera->cosangle * distx + camera->sinangle*disty;
	el = -camera->sinangle * distx + camera->cosangle*disty;
	
	SendOutEL(-el);
	SendOutCE(-ce);
	WriteToLog("PNTGROI","(%ld) EL=%lf, CE=%lf, trkX=%lf, trky=%lf",frmnumber,el,ce,x+dx/2.,y+dy/2.);
	GtoT_PointingErrors(el,ce);
	FInfo.send(frmnumber, leakylvdt, el,ce,camera->getExpT(),err);
      

      PlaceSmallBox(camera, camera->buffer[qID], NULL, err, qID);


      return;
	
    
      } else {
	//	loststarcounter++;
	ce = 0;
	el = 0;
	
	SendOutEL(-el);
	SendOutCE(-ce);
	WriteToLog("PNTG-LOCKROI","(%ld)NoBlob",frmnumber);
	GtoT_PointingErrors(el,ce);
	FInfo.send(frmnumber, leakylvdt, 3000.0,3000.0,camera->getExpT(),255);

	PlaceSmallBox(camera, camera->buffer[qID], NULL, err, qID);
	//no star. how many times has this happened now?
	if(loststarcounter==FRAMES){
	  // no star for a while. we need to switch to a different mode. hmmm..
	  // what mode? Slewing mode!
	};
	
	WriteToLog("hey there");
	return;
      };

      return;
      break;


    };

    case TRACKINGFC:{
      // in this mode we are using the full chip. the RA and DEC of
      // the tracking star is known, as are the RA and DEC of the
      // desired center of the field. The EL and CE offsets between
      // the two are calculated and the tracking point is placed at
      // the appropriate offset. 
      scienceRA = camera->getTargetRA();
      scienceDEC = camera->getTargetDEC();
      targetRA = camera->getRA();
      targetDEC = camera->getDEC();
      static double jd;
      static double alt1, az1, alt2, az2;
      static double lat,lon;
      static double el,ce;


      jd = JulianDay();
      //      getPos(lat,lon);
      eq2hor(scienceRA, scienceDEC,lat,lon, jd, alt1,az1);
      eq2hor(targetRA, targetDEC,lat,lon, jd, alt2,az2);

      ALTAZ_to_ELCE(alt1,az1,alt2,az2,el,ce);
      // WARNING -- ignoring CCD rotation.
      camera->pointingTrackPointX = camera->spectroCenterPointX - ce/camera->getPlateScale();
      camera->pointingTrackPointY = camera->spectroCenterPointY = el/camera->getPlateScale();
	     
      // in this mode we are using the full chip. A box is selected
      // and if there is a star inside the box, that star is then
      // tracked on the chosen fixed point.
      
      // step 1
      // select target
      targetX = camera->pointingTrackPointX;
      targetY = camera->pointingTrackPointY;
      
      // step 2
      // search for blobs
      
      /* but this was already done!
	 camera->fblob[qID].clearblobs();
	 camera->fblob[qID].calc_mapstat();
	 camera->fblob[qID].calc_mapstat();
	 camera->fblob[qID].calc_searchgrid();
	 camera->fblob[qID].calc_centroid();
	 camera->fblob[qID].calc_flux();
	 camera->fblob[qID].fix_multiple();
	 camera->fblob[qID].sortblobs();
	 pblob = camera->fblob[qID].getblobs();
      */
      // step 3
      // look for blob in window
      isblob = false;
      if(pblob!=NULL){
	
	while(pblob!=NULL){
	  
	  bx = pblob->getx();
	  by = pblob->gety();
	  isblob = false;
	  movewin = false;
	  if(bx>wx-wdx && bx<wx+wdx && by>wy-wdy && by<wy+wdy){
	    // so the blob is in the box. 
	    // now check how far from the tracking point the star is.
	    distx = bx - targetX;
	    disty = by - targetY;
	    if(distx > 150 || disty >150) movewin = true;
	    dist = sqrt(distx*distx + disty*disty);
	    if(dist>0)
	      if(dist/camera->MoveStepSize){
		distx = distx/dist * camera->MoveStepSize;
		disty = disty/dist * camera->MoveStepSize;
	      };
	    
	    distx *= camera->getPlateScale();
	    disty *= camera->getPlateScale();
	    
	    ce = camera->cosangle * distx + camera->sinangle*disty;
	    el = -camera->sinangle * distx + camera->cosangle*disty;
	    
	    SendOutEL(-el);
	    SendOutCE(-ce);
	    WriteToLog("PNTG","(%ld) EL=%lf, CE=%lf",frmnumber,el,ce);

	    GtoT_PointingErrors(el,ce);
	    FInfo.send(frmnumber, leakylvdt, el,ce,camera->getExpT(),err);
	    
	    if(movewin) {
	      camera->setNowWin((unsigned short)bx,(unsigned short)by, 100,100);
	    };
	    goto done2;
	  };
	  
	  pblob = pblob->getnextblob();
	};

	if(!(frmnumber % 10))GtoT_FrameBlobs(&(camera->fblob[qID]));
      } else {
	// there were no blobs on the screen. this is a problem. how
	// do we handle it? 
	distx = 0;
	disty = 0;
	FInfo.send(frmnumber, leakylvdt, 3000.0,3000.0,camera->getExpT(),255);
	GtoT_PointingErrors(0,0);

	WriteToLog("PNTG-Track","(%ld)NoBlob",frmnumber);
      };

    done2:
      	PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0 ,err);
      return;


      
      
      
      break;
    };
      
    case TRACKINGROI:{
      // this mode is similar to the last, with the exception that the
      // box used is a ROI instead of a window. 
      scienceRA = camera->getRA();
      scienceDEC = camera->getDEC();
      targetRA = camera->getTargetRA();
      targetDEC = camera->getTargetDEC();
            static double jd;
      static double alt1, az1, alt2, az2;
      static double lat,lon;
      static double el,ce;
      bool nearedge;

      jd = JulianDay();
      //      getPos(lat,lon);
      eq2hor(scienceRA, scienceDEC,lat,lon, jd, alt1,az1);
      eq2hor(targetRA, targetDEC,lat,lon, jd, alt2,az2);

      ALTAZ_to_ELCE(alt1,az1,alt2,az2,el,ce);
      // WARNING -- ignoring CCD rotation.
      camera->pointingTrackPointX = camera->spectroCenterPointX - ce/camera->getPlateScale();
      camera->pointingTrackPointY = camera->spectroCenterPointY = el/camera->getPlateScale();
	     
      // in this mode we are using the full chip. A box is selected
      // and if there is a star inside the box, that star is then
      // tracked on the chosen fixed point.
      
      // step 1
      // select target
      targetX = camera->pointingTrackPointX-x;
      targetY = camera->pointingTrackPointY-y;
      nearedge = false;
      

      
      camera->setSmallBlobXpix(2*dx);
      camera->setSmallBlobYpix(2*dy);
      
      camera->smallblobs[qID].clearblobs();
      camera->smallblobs[qID].calc_mapstat();
      camera->smallblobs[qID].calc_mapstat();
      camera->smallblobs[qID].calc_searchgrid();
      camera->smallblobs[qID].calc_centroid();
      camera->smallblobs[qID].calc_flux();
      camera->smallblobs[qID].fix_multiple();
      camera->smallblobs[qID].sortblobs();
      pblob = camera->smallblobs[qID].getblobs();
      
      //okay. now let's see if we have a blob in the image.
      if(pblob!=NULL){
	// okay, there is a star. 
	//	loststarcounter = 0;
	bx = pblob->getx();
	by = pblob->gety();
	camera->setNowWinX(bx);
	camera->setNowWinY(by);
	distx = bx - targetX;
	disty = by - targetY;
	
	distx *= camera->getPlateScale();
	disty *= camera->getPlateScale();
	
	ce = camera->cosangle * distx + camera->sinangle*disty;
	el = -camera->sinangle * distx + camera->cosangle*disty;
	
	SendOutEL(-el);
	SendOutCE(-ce);
	WriteToLog("PNTG","(%ld) EL=%lf, CE=%lf",el,ce,frmnumber);
	GtoT_PointingErrors(el,ce);
	FInfo.send(frmnumber, leakylvdt, el,ce ,camera->getExpT(),err);
	
	PlaceSmallBox(camera, camera->buffer[qID], NULL, err, qID);


	//	if(targetX<25 || targetY <25 || targetX > dx-25 || targetY >dy-25){
	//	};

	return;
	

      } else {
	//	loststarcounter++;
	ce = 0;
	el = 0;
	
	SendOutEL(-el);
	SendOutCE(-ce);
	WriteToLog("ALGN","ROI: %ld, %f, %f, 0",frmnumber, targetX, targetY);
	GtoT_PointingErrors(el,ce);
	PlaceSmallBox(camera, camera->buffer[qID], NULL, err, qID);
	FInfo.send(frmnumber, leakylvdt, 3000.0,3000.0,camera->getExpT(),255);

	//no star. how many times has this happened now?
	//	if(loststarcounter==FRAMES){
	if(1){
	  // no star for a while. we need to switch to a different mode. hmmm..
	  // what mode? Slewing mode!
	};
	return;
      };



      return;
      break;
    }
      
    };


  };

 donemodespecific:
  return;

};

/*void QCAMAPI SlewingModeCallback(void *cam, unsigned long info, QCam_Err err, unsigned long){
  
  static QCamera *camera;
  camera = (QCamera *) cam;
  
  int qID;
  qID = info;
  
  unsigned short x, y, dx, dy;
  unsigned char *buffer;
  bool OKtoLock;
  bool tooClose;
  
  double bx, by, bx2, by2;
  unsigned short mx,my, mdx, mdy;
  float maxx, maxy, flux, sumx, sumy;
  int maxval, indx, indy;
  int i,j;
  float totval, val, sum, sumsq;
  float apsum, apmean;
  float sigma, mean;
  static unsigned char subMode;
  static GuiderMode guiderMode;
  static unsigned short wx, wy, wdx, wdy;

  x = camera->getRoiX();
  y = camera->getRoiY();
  dx = camera->getRoiDX();
  dy = camera->getRoiDY();
  buffer = camera->buffer[qID];

  guiderMode = camera->getModeID();
  subMode = camera->getSubModeID();

  wx = camera->getWinX();
  wy = camera->getWinY();
  wdx = camera->getWinDX();
  wdy = camera->getWinDY();


  
  static SlewingSubMode submodeID;

  bloblist *pblob, *pblob2;
  int blobcounter, blobcounter2;

  static frameblob blob;
  if(err){
    GtoT_CameraError(err);
  };

    camera->setFrameBlobXpix(CHIPWIDTH);
    camera->setFrameBlobYpix(CHIPHEIGHT);
    camera->fblob[qID].clearblobs();
    camera->fblob[qID].calc_mapstat();
    camera->fblob[qID].calc_mapstat();
    camera->fblob[qID].calc_searchgrid();
    camera->fblob[qID].calc_centroid();
    camera->fblob[qID].calc_flux();
    camera->fblob[qID].fix_multiple();
    camera->fblob[qID].sortblobs();
    pblob = camera->fblob[qID].getblobs();

    submodeID = (SlewingSubMode) camera->getSubModeID();
    
  switch(submodeID){
    // in this mode the MPF is slewing the pointing around and the
    // guider just sits there waiting for good things to happen. 
  case SLEWING:{
    if(1){ // if within range.
      camera->setSubModeID((unsigned char) SEARCHINGDTU);
    };
    break;
  };
    // in this submode the guider is looking for a star in the field
    // of view that it can lock onto to freeze the position. 
  case SEARCHING:{
    blobcounter = blobcounter2 = 0;
    while(pblob!=NULL){
      OKtoLock = false;
      tooClose = false;
      bx = pblob->getx();
      by = pblob->gety();
      // if we happen to find a star in the central region that 
      if(bx>200 && by>200 && bx<CHIPWIDTH-200 && by<CHIPHEIGHT-200){
	pblob2 = camera->fblob[qID].getblobs();
	while(pblob2!=NULL){
	  pblob2 = pblob->getnextblob();
	  blobcounter2++;
	  bx2 = pblob2->getx();
	  by2 = pblob2->gety();
	  if(fabs(bx2-bx)<100 || fabs(by2-by)<100) tooClose = true;
	};
	
	if(!tooClose){
	  camera->setSubModeID((unsigned char) VERIFYING);
	  camera->setNowWin((unsigned short) bx, (unsigned short) by, 100,100);
	  camera->verifycounter = 0;
	  goto donesearching;
	  
	};

      };

    }
  donesearching:
    break; 
  }

    // in this submode the guider verifies that the locked star stays
    // within a bounding box centered around the original position. if
    // it does, great. if it does not... we go back to the searching
    // submode. the verification itself lasts only a few frames.  Once
    // this is verified, the guider sends a pointing request and
    // enters the pointing mode. see the submodes there.
  case VERIFYING:{
    mx = camera->getWinX();
    my = camera->getWinY();
    mdx = camera->getWinDX();
    mdy = camera->getWinDY();
    sum = sumsq = 0;
    indx = indy = 0;
    maxval = -1;
    for(i=mx-mdx;i<mx+mdx;i++){
      for(j=my-mdy; j<my+mdy;j++){
	val = buffer[i+CHIPWIDTH*j];
	sum+=val;
	sumsq+=val*val;
	sumx += val*(i-mx);
	sumy += val*(j-my);
	if(val>maxval){
	  maxval = val;
	  indx = i;
	  indy = j;
	};
      };
    };

    apsum = 0;
    for(i=indx-2;i<indx+3;i++){
      for(j=indy-2;j<indy+3;j++){
	val = buffer[i+CHIPWIDTH*j];
	sum-=val;
	sumsq-=val*val;
	if(i!=0 && j!=0) apsum += val;
      };
    };
    
    apsum /= 24;
    mean = sum/(mdy*mdx);
    sigma = sqrt(sumsq/(mdy*mdx) - mean*mean);
    if(sigma==0) sigma = 1;


    if((apsum-mean)/sigma > 3) camera->verifycounter++;
    
    
    break;
  };
  };
    


  camera->prepSettings(PointingMode, camera->getRA(), camera->getDEC());
  
  PlaceFullChip(camera, camera->buffer[qID], blob, 0 ,err);
  
};*/

void QCAMAPI PointingModeCallback(void *cam, unsigned long info, QCam_Err err, unsigned long){
  /*
    static int qID;
  qID = info;
  static QCamera *camera;
  camera = (QCamera *) cam;
  static PointingSubMode submodeID;
  static frameblob blob;
  static float targetx, targety;
  static unsigned char *buffer;
  buffer = camera->buffer[qID];
  static float distx, disty;

  submodeID = (PointingSubMode) camera->getSubModeID();
  
  switch(submodeID){
    // in this mode the guider has chosen a single star in the FOV and
    // has locked onto it. It is guiding the pointing on that star at
    // where it first picked it up on the chip. This is where either
    // the software does star recognition or the observers try to
    // identify the field. 
    
  case LOCKED:{
    static unsigned short wx, wy, wdx, wdy;
    wx = camera->getWinX();
    wy = camera->getWinY();
    wdx = camera->getWinDX();
    wdy = camera->getWinDY();
    static int i,j, idx;
    static float maxx, maxy, max, sum, sumsq, val, sumx, sumy, sumtot, sumsurr;
    static float centx, centy;
    static bool extended;
    static float el, ce;
    

    sum += val;
    maxx = max = max = 0;
    sumsq += val*val;

    for(j=-wdy; j<wdy; j++){
      for(i=-wdx; i<wdx; i++){
	idx = (wx+j)+(wy+i)*CHIPWIDTH;
	val = buffer[idx];
	sum += val;
	sumsq += val*val;
	if(val>max){
	  maxx = wx+i; maxy = wy+j; max = val;
	};
      };
    };

    sum = sum/(4*wdy*wdx);
    sumsq = sqrt(sumsq/(4*wdx*wdy)-sum*sum);
    if(sumsq < 1) sumsq = 1;

    extended = true;
    sumx = sumy = sumtot = sumsurr = 0;
    for(j=-BOXHALFSIZE; j<BOXHALFSIZE+1; j++){
      for(i=-BOXHALFSIZE; i<BOXHALFSIZE+1; i++){
	val = buffer[(int)((maxx+i)+(maxy+j)*CHIPWIDTH)];
	sumx += (i+0.5)* val;
	sumy += (j+0.5)* val;
	sumtot += val;
	if(i!=0 && j!=0) sumsurr+=val;
      };
    };

    if(sumtot<1) sumtot = 1;

    sumsurr = sumsurr/((2*BOXHALFSIZE+1)*(2*BOXHALFSIZE+1));
    if(fabs(sumsurr-sum)<2*sumsq) extended = false;
    if(max < 3 * sumsq) extended = false; 

    if(extended){
      centx = sumx/sumtot + maxx;
      centy = sumy/sumtot + maxy;
      
      distx = (centx - wx)* camera->getPlateScale();
      disty = (centy - wy)* camera->getPlateScale();

      ce = camera->cosangle * distx - camera->sinangle*disty;
      el = -camera->sinangle * distx + camera->cosangle*disty;

      SendOutEL(-el);
      SendOutCE(-ce);

    } else {

    };


    PlaceFullChip(camera,camera->buffer[qID],camera->fblob[qID],0,err);


    break;
  }
    // field information is given to the computer (RA, DEC) of the
    // center of the field and (RA, DEC) of the star being moved. The
    // computer figures out where the star needs to be on the chip. it
    // then uses a window around it to slowly drag it to the desired position.
  case MOVING:{
    static unsigned short wx, wy, wdx, wdy;
    wx = camera->getWinX();
    wy = camera->getWinY();
    wdx = camera->getWinDX();
    wdy = camera->getWinDY();
    static int i,j, idx;
    static float maxx, maxy, max, sum, sumsq, val, sumx, sumy, sumtot, sumsurr;
    static float centx, centy;
    static bool extended;
    static float el, ce;
    

    sum += val;
    maxx = max = max = 0;
    sumsq += val*val;

    for(j=-wdy; j<wdy; j++){
      for(i=-wdx; i<wdx; i++){
	idx = (wx+j)+(wy+i)*CHIPWIDTH;
	val = buffer[idx];
	sum += val;
	sumsq += val*val;
	if(val>max){
	  maxx = wx+i; maxy = wy+j; max = val;
	};
      };
    };

    sum = sum/(4*wdy*wdx);
    sumsq = sqrt(sumsq/(4*wdx*wdy)-sum*sum);
    if(sumsq < 1) sumsq = 1;

    extended = true;
    sumx = sumy = sumtot = sumsurr = 0;
    for(j=-BOXHALFSIZE; j<BOXHALFSIZE+1; j++){
      for(i=-BOXHALFSIZE; i<BOXHALFSIZE+1; i++){
	val = buffer[(int)((maxx+i)+(maxy+j)*CHIPWIDTH)];
	sumx += (i+0.5)* val;
	sumy += (j+0.5)* val;
	sumtot += val;
	if(i!=0 && j!=0) sumsurr+=val;
      };
    };

    if(sumtot<1) sumtot = 1;

    sumsurr = sumsurr/((2*BOXHALFSIZE+1)*(2*BOXHALFSIZE+1));
    if(fabs(sumsurr-sum)<2*sumsq) extended = false;
    if(max < 3 * sumsq) extended = false; 

    if(extended){
      centx = sumx/sumtot + maxx;
      centy = sumy/sumtot + maxy;

      // now we have to figure out what point on the chip
      // we are trying to track on.
      
      static double tRA, tDEC, sRA, sDEC;
      static double talt, taz, salt, saz;
      static double del, dce;
      static double lat, lon;
      static double dist;
      
      getLatLon(&lat, &lon);


      tRA = camera->getTargetRA();
      tDEC = camera->getTargetDEC();
      sRA = camera->getRA();
      sDEC = camera->getTargetDEC();

      RADEC_to_ALTAZ(tRA,tDEC, lat, lon, &talt, &taz);
      RADEC_to_ALTAZ(sRA,sDEC, lat, lon, &salt, &saz);
      ALTAZ_to_ELCE(talt,taz, salt, saz, &del, &dce);
      
      // okay. so we know the offset from the center of the chip that 
      // our tracking point is at (in arcsec). 

      del = del/ camera->getPlateScale();
      dce = dce/ camera->getPlateScale();

      static double urdel, urdce;
      
      urdce = camera->cosangle * dce + camera->sinangle* del;
      urdel = -camera->sinangle *dce + camera->cosangle* del;

      distx = (centx - urdce);
      disty = (centy - urdel);
      
      
      dist = sqrt(distx*distx+disty*disty);

      if(dist>50){
	distx = distx/dist * 50.;
	disty = disty/dist * 50.;
      };


      camera->setNowWinX((unsigned short) (camera->getWinX()+distx));
      camera->setNowWinY((unsigned short) (camera->getWinY()+disty));
      
      distx = distx * camera->getPlateScale();
      disty = disty * camera->getPlateScale();


      ce = camera->cosangle * distx - camera->sinangle*disty;
      el = -camera->sinangle * distx + camera->cosangle*disty;

      SendOutEL(-el);
      SendOutCE(-ce);

    } else {

    };


    PlaceFullChip(camera,camera->buffer[qID],camera->fblob[qID],0,err);


    break;
  };
    // once the star is done moving the guider recalculates the
    // desired position and uses that as the chip position to track to.
  case TRACKINGFC:{
    break;
  };
    // this can be set to either go into manually or automatically
    // upon the conclusion of the moving stage. Here the guider
    // performs the fundamental task of tracking on a star, at a high
    // frame rate in a small region of interest. 
  case TRACKINGROI:{
    break;
  };
  default:{
    break;
  };
  };


  if(err){
    GtoT_CameraError(err);
  };
  PlaceSmallBox(camera, camera->buffer[qID], NULL, err, qID);
  //  PlaceFullChip(camera, camera->buffer[qID], blob, 0 ,err);
  */
};
