// are there any messages in the message queue?
if(MessageQueueCount()){

  modeID = camera.getModeID();

  // if so, grab one.
  GetFromMessageQueue(m);

  // and get its ID;
  mID = m.body[5]; // this is a little format specific, but it 
                   // should do for now.

  //  if(mID==4)WriteToLog("processmessages.cpp","Got Message %d",mID);

  //now comes a big switch statement. message processing is both
  //message (surprise!) and mode specific. 

  switch(mID){

    /************************************************************************/
    //Message ID 1 from MPF
    //DTU Pointing Errors Data 
  case 1:{
    // load in the values passed by the message.
    M001_DTUPointingErrorsData(m, tmpEL, tmpCE, tmpMeasType, tmpValid1);
    // register the message in the log. 
    WriteToLog(modeName[modeID],"MPF001 DTU PED,EL=%.2f, CE=%.2f,%1d,%1d",tmpEL,tmpCE,tmpMeasType,tmpValid1);
    DTUPED.EL = tmpEL;
    DTUPED.CE = tmpCE;
    DTUPED.MeasType = tmpMeasType;
    DTUPED.Valid = tmpValid1;
#ifndef DONOTPROCESSMPF    
    if(modeID == INITMODE){      //This message has no effect in Init mode. 



    } else if(modeID == AUTOCOLLIMATIONMODE){
      // this message should not be received in autocollimation mode.
      WriteToLog(modeName[modeID],"Illegal MPF001 message");
    } else if(modeID == ALIGNMENTMODE){
      alignmentMD.DTUpeEL=tmpEL;
      alignmentMD.DTUpeCE=tmpCE;
      // in the principal alignment mode we don't necesserily need 
      // to know whether we are in range or not. it can be useful, but
      // we can get a full screen lock and identify stars from there
      // using EL CE offset TCs or Request to shift the FOV accordingly.
      if(fabs(tmpEL) >  CHIPHEIGHT/2. * camera.getPlateScale()  || fabs(tmpCE > CHIPWIDTH/2. * camera.getPlateScale())){
	camera.inRange = false;
	alignmentMD.inRange = false;
      } else {
	camera.inRange = true;
	alignmentMD.inRange = true;
      };

    } else if(modeID == SLEWINGMODE){
      // in slewing mode it is only useful to know whether we are in
      // range for star-identifying purposes. 
      slewingMD.DTUpeEL=tmpEL;
      slewingMD.DTUpeCE=tmpCE;
      if(fabs(tmpEL) >  CHIPHEIGHT/2. * camera.getPlateScale() || fabs(tmpCE > CHIPWIDTH/2. * camera.getPlateScale())){
	camera.inRange = false;
	slewingMD.inRange = false;
      } else {
	camera.inRange = true;
	slewingMD.inRange = false;
      };
	 

    } else if(modeID == POINTINGMODE){
      // if the pointing error is larger than the size of our box,
      // then we are clearly in the wrong spot and need to fix
      pointingMD.DTUpeEL = tmpEL;
      pointingMD.DTUpeCE = tmpCE;
      if(fabs(tmpEL) > camera.getRoiDY()/2 * camera.getPlateScale() || fabs(tmpCE)> camera.getRoiDX()/2. * camera.getPlateScale()){
	//	camera.prepSettings(SlewingMode, camera.getTargetRA(), camera.getTargetDEC());
	// send a slewing request to the MPF, as we are in the wrong place!
	//	GtoM_SlewingRequest(camera.getTargetRA(), camera.getTargetDEC());
	//	goto MainLoop;
      }; 

    };
#endif // DONOTPROCESSMPF
      break;
  } // CASE 1
    /************************************************************************/
    // MPF 002
    // DTU Coordinate Data
    //******
    // This is a large structure that is just passed on from the DTU
    // via the MPF Num. It contains things that may be of interest to
    // us, including the bore-sight. it is sent over in big-endian
    // notation, so before it is used it needs to be decoded. 
    //***********************************************************************/

  case 2:{
    M002_DTUCoordinateData(m,asc);
    DTUQ.set(asc.q1, asc.q2, asc.q3, asc.q4);
    WriteToLog(modeName[modeID],"MPF002, RA=%.6lf, DEC=%.6lf",DTUQ.getRA(), DTUQ.getDEC());
#ifndef DONOTPROCESSMPF
    if(modeID == INITMODE){

    } else if(modeID == AUTOCOLLIMATIONMODE){

    } else if(modeID == ALIGNMENTMODE){

    } else if(modeID == SLEWINGMODE){

    } else if(modeID == POINTINGMODE){

    };
#endif //DONOTPROCESSMPF 
    break;
  }; //case 2
    
    
    
    /*************************************************************************/
    // MPF 003
    // DTU EL + CE Position Data 
    //**********
    // Sent in Autocollimation mode gives the EL and CE position of
    // the table
    
  case 3:{
    M003_ELCEPositionData(m, tmpEL, tmpCE);
    WriteToLog(modeName[modeID],"MPF003:%f,%f.",tmpEL,tmpCE);
    ELCEPD.EL = tmpEL;
    ELCEPD.CE = tmpCE;
#ifndef DONOTPROCESSMPF
    if(modeID == INITMODE){
      // should not be received in INIT mode.
    } else if(modeID == AUTOCOLLIMATIONMODE){
      autocollimationMD.DTUposEL = tmpEL;
      autocollimationMD.DTUposCE = tmpCE;
    } else if(modeID == ALIGNMENTMODE){
      // should not be received in ALIGNMENT mode
    } else if(modeID == SLEWINGMODE){
      // should not be received in SLEWING mode
    } else if(modeID == POINTINGMODE){
      // should not be received in POINTING mode
    };
#endif //DONOTPROCESSMPF
    break;
  }; //case 3


    /*************************************************************************/
    //*** MPF 004
    //DTU status
    //this is sent every five seconds or so to the guider to relay the
    //operational state of the MPF and DTU.
    
  case 4:{
    M004_MPFNumStatusData(m,tmpOpState, tmpPointControl, tmpEL, tmpCE, tmpEL1, tmpCE1);

    WriteToLog(modeName[modeID],"MPF004,%1d,%1d,%.2f,%.2f,%.2f,%.2f",tmpOpState,tmpPointControl, tmpEL, tmpCE, tmpEL1, tmpCE1);
    MPFS.OpState = tmpOpState;
    MPFS.PointControl = tmpPointControl;
    MPFS.ELal = tmpEL;
    MPFS.CEal = tmpCE;
    MPFS.ELac = tmpEL1;
    MPFS.CEac = tmpCE1;
#ifndef DONOTPROCESSMPF
    // flag some mode mismatches.
    if(tmpOpState != modeID){
      displayMD.modeMismatch = true; 
	} else {
	  displayMD.modeMismatch = false;
	};
    if(tmpPointControl != camera.getActiveSensor()){
      displayMD.sensorMismatch = true;
    } else {
      displayMD.sensorMismatch = false;
    };
    

    if(modeID == INITMODE){
      // nothing to be done in INIT mode

    } else if(modeID == AUTOCOLLIMATIONMODE){
      autocollimationMD.offsetEL = tmpEL;
      autocollimationMD.offsetCE = tmpCE;

    } else if(modeID == ALIGNMENTMODE){
      alignmentMD.offsetEL = tmpEL1;
      alignmentMD.offsetCE = tmpCE1;

    } else if(modeID == SLEWINGMODE){
      //nothing to be done in slewing

    } else if(modeID == POINTINGMODE){
      //nothing to be done in pointing
    };
#endif //DONOTPROCESSMPF
    break;
  }; //case 4

    
    /************************************************************************/
    //*** GND140
    // Reboots the guider. Use with caution!
    
  case GG_REBOOTGUIDER:{
    GtoT_MessageRcvd(GG_REBOOTGUIDER);
    camera.stopStreaming();
    WriteToLog("Reboot Requested");
    CloseLog();
    system("reboot");
    break;
  }; //case GG_REBOOTGUIDER
  
    /***********************************************************************/
    //*** GND141
    // Reset Display.
    // sometimes the display becomes screwy for no good reason that i
    // can find. this usually restores it.

  case GG_RESTOREDISPLAY:{
    GtoT_MessageRcvd(GG_RESTOREDISPLAY);
    WriteToLog("Display Restore Requested.");
    system("/home/fireball/bin/restore");
    RestoreDisplay();

    break;
  }; //case GG_RESTOREDISPLAY;
    
    /************************************************************************/
    //*** GND142
    // Shutdown guider
    // shuts down the guider. for full shutdown the power will need to
    // be cut
  
  case GG_SHUTDOWNGUIDER:{
    GtoT_MessageRcvd(GG_SHUTDOWNGUIDER);
    camera.stopStreaming();
    WriteToLog("Shutdown Requested");
    //    FlushLog();
    CloseLog();
    system("shutdown -h now");
    break;
  }; // case GG_SHUTDOWNGUIDER


    /************************************************************************/
    //*** GND200
    //*** Init Mode TC
    // puts the guider into the initialization mode. This can be sent
    // from any other mode. In Init Mode we just look and do nothing
    // else.
    
  case GG_INITMODETC:{
    GtoT_MessageRcvd(GG_INITMODETC);
    camera.prepSettings(InitMode,0,0);
    WriteToLog(modeName[modeID],"%03d",GG_INITMODETC);
    goto MainLoop;
    break;
  };

    /*************************************************************************/
    //*** GND201
    //puts the telescope in autocollimation mode.
  case GG_AUTOCOLLIMATIONTC:{
    GtoT_MessageRcvd(GG_AUTOCOLLIMATIONTC);
    camera.prepSettings(AutocollimationMode,0,0);
    GtoM_AutoCollimationTC();
    WriteToLog(modeName[modeID],"%03d",GG_AUTOCOLLIMATIONTC);
    goto MainLoop;
    break;
  };


    /************************************************************************/
    //*** GND202
    // puts the guider in alignment mode. passes the command on to the
    // MPF. the command includes the RA and DEC of the target. 
  case GG_ALIGNMENTTC:{
    GtoT_MessageRcvd(GG_ALIGNMENTTC);
    T202_AlignmentTC(m,tmpRA, tmpDEC);
    WriteToLog(modeName[modeID],"%03d,%.5lf,%.5lf",GG_ALIGNMENTTC, tmpRA, tmpDEC);
    
    if(modeID == INITMODE){
      camera.prepSettings(AlignmentMode,tmpRA, tmpDEC);
      //      GtoM_AlignmentTC(tmpRA, tmpDEC);
      goto MainLoop;
    } else if(modeID == AUTOCOLLIMATIONMODE){
      camera.prepSettings(AlignmentMode, tmpRA, tmpDEC);
      //      GtoM_AlignmentTC(tmpRA, tmpDEC);
      goto MainLoop;
    } else if(modeID == ALIGNMENTMODE){
      camera.prepSettings(AlignmentMode, tmpRA, tmpDEC);
      //      GtoM_AlignmentTC(tmpRA, tmpDEC);
      goto MainLoop;
    } else if(modeID == SLEWINGMODE){
      WriteToLog(modeName[modeID],"Can't switch to Align in this mode");
    } else if(modeID == POINTINGMODE){
      WriteToLog(modeName[modeID],"Can't switch to Align in this mode");
    };


    break;
  };

    /************************************************************************/
    //*** GND203
    // Slewing TC
    //**************
    // Puts the guider into slewing mode to go to a particular set of
    // stellar coordinates  

  case GG_SLEWINGTC:{
    T203_SlewingTC(m, tmpRA, tmpDEC);
    GtoT_MessageRcvd(GG_SLEWINGTC);
    
    camera.setTargetRA (tmpRA);
    camera.setTargetDEC(tmpDEC);
    WriteToLog(modeName[modeID],"%03d,%.5lf,%.5lf",GG_SLEWINGTC,tmpRA, tmpDEC);

    if(modeID == INITMODE){
      camera.prepSettings(SlewingMode, tmpRA,tmpDEC);
      GtoM_SlewingTC(tmpRA, tmpDEC);
      goto MainLoop;
    } else if(modeID == AUTOCOLLIMATIONMODE){
      camera.prepSettings(SlewingMode, tmpRA,tmpDEC);
      GtoM_SlewingTC(tmpRA, tmpDEC);
      goto MainLoop;
    } else if(modeID == ALIGNMENTMODE){
      camera.prepSettings(SlewingMode, tmpRA,tmpDEC);
      GtoM_SlewingTC(tmpRA, tmpDEC);
      goto MainLoop;
    } else if(modeID == SLEWINGMODE){
      WriteToLog(modeName[modeID],"Can't switch to Slewing");
    } else if(modeID == POINTINGMODE){
      camera.prepSettings(SlewingMode, tmpRA,tmpDEC);
      GtoM_SlewingTC(tmpRA, tmpDEC);
      goto MainLoop;
    };
    break;
  }; // case GG_SLEWINGTC


    /*********************************************************************/
    //*** GND204
    // EL CE Offset TC
    // asks for EL and CE offsets to be added to the position of the
    // siderostat mirror table
    // we will relay the message in ALL modes and have the MPF sort it
    // out as to whether to use it or not. that should be confusing
    // enough :)


  case GG_ELCEOFFSETTC:{
    GtoT_MessageRcvd(GG_ELCEOFFSETTC);
    T204_ELCEOffsetTC(m, tmpEL, tmpCE);
    WriteToLog(modeName[modeID],"%03d,%.2f,%.2f",GG_ELCEOFFSETTC,tmpEL, tmpCE);
    
    if(modeID == INITMODE){
         GtoM_ELCEOffsetsTC(tmpEL, tmpCE);
    } else if(modeID == AUTOCOLLIMATIONMODE){
         GtoM_ELCEOffsetsTC(tmpEL, tmpCE);
    } else if(modeID == ALIGNMENTMODE){
         GtoM_ELCEOffsetsTC(tmpEL, tmpCE);
    } else if(modeID == SLEWINGMODE){
      WriteToLog(modeName[modeID],"no ELCEOffsets in this mode");
    } else if(modeID == POINTINGMODE){
      WriteToLog(modeName[modeID],"no ELCEOffsets in this mode");
    };
    
    break;
  };// case GG_ELCEOFFSETSTC


    /************************************************************************/
    //*** GND205
    // Alignment Track TC
    // selects which submode of the alignment mode to be in

  case GG_ALIGNTRACKTC:{
    GtoT_MessageRcvd(GG_ALIGNTRACKTC);

    T205_AlignTrackTC(m,tmpRoiFlag, tmpRoiCX, tmpRoiCY, tmpRoiD, tmpTrigT, tmpExpT);

    WriteToLog(modeName[modeID],"%03d,%d,%d,%d,%d,%d,%d",GG_ALIGNTRACKTC,tmpRoiFlag, tmpRoiCX, tmpRoiCY, tmpRoiD, tmpTrigT, tmpExpT);
    


    if(modeID == INITMODE){
      WriteToLog(modeName[modeID],"%03d not allowed in this mode.");

    } else if(modeID == AUTOCOLLIMATIONMODE){
      WriteToLog(modeName[modeID],"%03d not allowed in this mode.");

    } else if(modeID == ALIGNMENTMODE){
      if(tmpRoiFlag==ALIGNTRACKROI){
	tX = (int) tmpRoiCX - (int)tmpRoiD;
	tY = (int) tmpRoiCY - (int)tmpRoiD;
	if(tX<0) tX = 0;
	if(tY<0) tY = 0;
	tDX = 2* (int) tmpRoiD;
	tDY = 2* (int) tmpRoiD;
	if(tX + tDX > CHIPWIDTH) tDX = CHIPWIDTH-tX;
	if(tY + tDY > CHIPHEIGHT) tDY = CHIPHEIGHT-tY;
	//	printf("X=%d\t Y=%d\t dX =%d\n",tmpRoiCX, tmpRoiCY, tmpRoiD);
	//	printf("X=%d\t Y=%d\t dX =%d\t dy=%d\n",tX, tY, tDX, tDY);
	AlignmentSubMode.setModeParams((unsigned short)(tX), (unsigned short)(tY), (unsigned short) (tDX), (unsigned short)(tDY), tmpExpT, tmpTrigT);
	//	  AlignmentSubMode.setModeParams(300, 400, 200, 200, tmpExpT, tmpTrigT);
	AlignmentSubMode.setMode(ALIGNMENTMODE, tmpRoiFlag);
	if((tmpExpT/1000.+2.999) < 1.0 * tmpTrigT){
	  camera.prepSettings(AlignmentSubMode, camera.getTargetRA(), camera.getTargetDEC());
	  
	  goto MainLoop;
	} else {
	  //  camera.SaveQCamState();
	};
      } else {
	camera.setNowWin(tmpRoiCX, tmpRoiCY, tmpRoiD, tmpRoiD);
	  camera.setNextWin(tmpRoiCX, tmpRoiCY, tmpRoiD, tmpRoiD);
      };
      if((tmpExpT/1000.+2.999) < 1.0 * tmpTrigT){
	
	alignmentMD.winx = tmpRoiCX;
	alignmentMD.winy = tmpRoiCY;
	alignmentMD.wind = tmpRoiD;
	if(tmpExpT != camera.getExpT() || tmpTrigT != camera.getTrigT() || camera.getSubModeID()==ALIGNTRACKROI){	  	
	  AlignmentSubMode.setModeParams(0,0,CHIPWIDTH,CHIPHEIGHT,tmpExpT, tmpTrigT);
	  AlignmentSubMode.setMode(ALIGNMENTMODE, tmpRoiFlag);
      	  camera.prepSettings(AlignmentSubMode, camera.getTargetRA(), camera.getTargetDEC());
	  camera.setNowWin(tmpRoiCX, tmpRoiCY, tmpRoiD, tmpRoiD);
	  camera.setNextWin(tmpRoiCX, tmpRoiCY, tmpRoiD, tmpRoiD);
	  //	  trackpointEL = tmpRoiCY;
	  //	  trackpointCE = tmpRoiCX;

	  goto MainLoop;
	} else {
	  camera.SaveQCamState();
	  // camera.setSubModeID(tmpRoiFlag);
	};
      };      

    } else if(modeID == SLEWINGMODE){
      WriteToLog(modeName[modeID],"%03d not allowed in this mode.",GG_ALIGNTRACKTC);
      //          printf("GG6\n");
    } else if(modeID == POINTINGMODE){
      WriteToLog(modeName[modeID],"%03d not allowed in this mode.",GG_ALIGNTRACKTC);
      //          printf("GG7\n");
    };
    break;
  }; // case GG_ALIGNTRACTC


    /***********************************************************************/
    //*** GND 206
    // get the X and Y offsets in Autocollimation mode

  case GG_AUTOCOLLIMATIONXYOFFSETS:{
    GtoT_MessageRcvd(GG_AUTOCOLLIMATIONXYOFFSETS);
    T206_AutoCollimationXYOffsets(m, tmpCE, tmpEL);
    WriteToLog(modeName[modeID],"%03d,%lf,%lf",GG_AUTOCOLLIMATIONXYOFFSETS,tmpCE,tmpEL);
    
    if(modeID == AUTOCOLLIMATIONMODE){
      WriteToLog(modeName[modeID],"%03d,%lf,%lf",GG_AUTOCOLLIMATIONXYOFFSETS,tmpCE, tmpEL);
      tmpCE = tmpCE / PIXSIZE * camera.getPlateScale();
      tmpEL = tmpEL / PIXSIZE * camera.getPlateScale();
      GtoM_ELCEOffsetsTC(tmpEL, tmpCE);
      
    } else {
      WriteToLog(modeName[modeID],"%03d not allowed in this mode.",GG_AUTOCOLLIMATIONXYOFFSETS);
    };
    
    break;
  };



    /***********************************************************************/
    //*** GND207
    //* Sets the zoom location for the autocollimation mode.
    
  case GG_AUTOCOLLIMATIONZOOMXY:{
    GtoT_MessageRcvd(GG_AUTOCOLLIMATIONZOOMXY);
    T207_AutocollimationZoomXY(m,tmpRoiX, tmpRoiY);
    WriteToLog(modeName[modeID],"%03d, %d, %d",GG_AUTOCOLLIMATIONZOOMXY, tmpRoiX,tmpRoiY);
    autocollimationMD.xpos = tmpRoiX;
    autocollimationMD.ypos = tmpRoiY;
    break;
  }// GG_AUTOCOLLIMATIONZOOMXY



    /***********************************************************************/
    //*** GND210
    // SetROI TC
    // Set ROI TC can be sent in any mode. Note that if it is altered
    // for the current mode, it will restart the mode. 

  case GG_SETROITC:{
    GtoT_MessageRcvd(GG_SETROITC);
    T210_SetROITC(m, tmpMode, tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY);
    WriteToLog(modeName[modeID],"%03d,%d,%d,%d,%d,%d",GG_SETROITC,tmpMode, tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY);


    if(tmpMode == modeID) sameModeChange = true;
    
    if(tmpMode   == INITMODE){
      InitMode.setModeParams(tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY, InitMode.getexpt(), InitMode.gettrigt());
      if(sameModeChange){
	camera.prepSettings(InitMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };

    } else if(tmpMode == AUTOCOLLIMATIONMODE){
      AutocollimationMode.setModeParams(tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY, AutocollimationMode.getexpt(), AutocollimationMode.gettrigt());
      if(sameModeChange){
	camera.prepSettings(AutocollimationMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };

    } else if(tmpMode == ALIGNMENTMODE){
      AlignmentMode.setModeParams(tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY, AlignmentMode.getexpt(), AlignmentMode.gettrigt());
      if(sameModeChange){
	camera.prepSettings(AlignmentMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };

    } else if(tmpMode == SLEWINGMODE){
      SlewingMode.setModeParams(tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY, SlewingMode.getexpt(), SlewingMode.gettrigt());
      if(sameModeChange){
	camera.prepSettings(SlewingMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };
    } else if(tmpMode == POINTINGMODE){
      PointingMode.setModeParams(tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY, PointingMode.getexpt(), PointingMode.gettrigt());
            if(sameModeChange){
	camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };
    };
    
    break;
  } // case GG_SETROITC;

    /************************************************************************/
    //*** GND211
    // Set Trigger and Exposure
    //********************8
    // sets the trigger period and the exposure length 

  case GG_SETTRIGGERPERIODTC:{
    GtoT_MessageRcvd(211);
    T211_TriggerPeriodTC(m,tmpMode, tmpTrigT, tmpExpT);
    WriteToLog(modeName[modeID],"%3d,%d,%d,%d",GG_SETTRIGGERPERIODTC,tmpMode,tmpTrigT, tmpExpT);

    if(tmpMode == modeID) sameModeChange = true;
    
    if(tmpMode   == INITMODE){
      InitMode.setModeParams(InitMode.getroiX(), InitMode.getroiY(),InitMode.getroiDX(), InitMode.getroiDY(), tmpExpT, tmpTrigT);
      if(sameModeChange){
	camera.prepSettings(InitMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };

    } else if(tmpMode == AUTOCOLLIMATIONMODE){
      AutocollimationMode.setModeParams(AutocollimationMode.getroiX(), AutocollimationMode.getroiY(), AutocollimationMode.getroiDX(), AutocollimationMode.getroiDY(), tmpExpT, tmpTrigT);
      if(sameModeChange){
	camera.prepSettings(AutocollimationMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };

    } else if(tmpMode == ALIGNMENTMODE){
      AlignmentMode.setModeParams(AlignmentMode.getroiX(), AlignmentMode.getroiY(),AlignmentMode.getroiDX(), AlignmentMode.getroiDY(), tmpExpT, tmpTrigT);
      if(sameModeChange){
	camera.prepSettings(AlignmentMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };

    } else if(tmpMode == SLEWINGMODE){
      SlewingMode.setModeParams(SlewingMode.getroiX(), SlewingMode.getroiY(),SlewingMode.getroiDX(), SlewingMode.getroiDY(), tmpExpT, tmpTrigT);
      if(sameModeChange){
	camera.prepSettings(SlewingMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };
    } else if(tmpMode == POINTINGMODE){
      PointingMode.setModeParams(PointingMode.getroiX(), PointingMode.getroiY(),PointingMode.getroiDX(), PointingMode.getroiDY(), tmpExpT, tmpTrigT);
            if(sameModeChange){
	camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	sameModeChange = false;
	goto MainLoop;
      };
    };

    break;
  }; //GG_SETTRIGGERPERIODTC


    /*************************************************************************/
    //**** GND212
    // Normalized Gain TC
    // changes the normalized gain on the camera. This setting will
  case GG_CAMNORMGAINTC:{
    T212_SetNormalizedGainTC(m, tmpGain);
    GtoT_MessageRcvd(GG_CAMNORMGAINTC);
    WriteToLog(modeName[modeID],"%03d,%f",GG_CAMNORMGAINTC, tmpGain);
    camera.prepSameSettings();
    goto MainLoop;
    break;
  }; // GG_CAMNORMGAINTC


    /*************************************************************************/
    //**** GND213
    // Absolute Offset TC
    // changes the absolute offset on the camera
  case GG_CAMABSOLUTEOFFSETTC:{
    T213_SetAbsoluteOffsetTC(m, tmpOffset);
    GtoT_MessageRcvd(GG_CAMABSOLUTEOFFSETTC);
    WriteToLog(modeName[modeID],"%03d,%f",GG_CAMABSOLUTEOFFSETTC, tmpOffset);
    camera.prepSameSettings();
    goto MainLoop;
    break;
  }; // GG_CAMABSOLUTEOFFSETTC;
    

    /************************************************************************/
    //*** GND214
    // Toggles the zoom feature on the autocollimation mode
    
  case GG_TOGGLEACZOOM:{
    GtoT_MessageRcvd(GG_TOGGLEACZOOM);
    WriteToLog(modeName[modeID],"%03d",GG_TOGGLEACZOOM);
    if(autocollimationMD.zoom){
      autocollimationMD.zoom = false;
    } else {
      autocollimationMD.zoom = true;
    };
    break;
  }; //GG_TOGGLEZOOM;
    
    /************************************************************************/
    //*** GND215
    //*********
    //*** graphics control TC
    // Sets the brightness, contrast and gamma for the video.
    
  case GG_SETDISPLAYCONTROLSTC:{
    T215_SetDisplayControlsTC(m, tmpBr, tmpCt, tmpGa);
    WriteToLog(modeName[modeID],"%03d,%d,%d,%d",GG_SETDISPLAYCONTROLSTC, tmpBr, tmpCt, tmpGa);
    GtoT_MessageRcvd(GG_SETDISPLAYCONTROLSTC);
    SetVPalette(tmpBr, tmpCt, tmpGa);
    
    break;
  }; //case GG_SETDISPLAYCONTROLSTC

  case GG_NUDGETRACKING:{
    GtoT_MessageRcvd(GG_NUDGETRACKING);
    T219_NudgeTracking(m,tgEL, tgCE);
    WriteToLog(modeName[modeID],"%03d, %d, %d",GG_NUDGETRACKING, tgEL, tgCE);
    if(tgEL==0 && tgCE == 0){
      trackpointEL = 676;
      trackpointCE = 550;
    } else {
      trackpointEL += tgEL;
      trackpointCE += tgCE;
    };
    break;
  };

    

    /***********************************************************************/
    //** GND230
    //*********
    //*** Focus TC
    // sends a move command to the stage to change the focus by a
    // specified number of user units. 
    
  case GG_FOCUSTC:{
    T230_FocusTC(m, tmpMove);
    GtoT_MessageRcvd(GG_FOCUSTC);
    WriteToLog(modeName[modeID],"GG_FOCUSTC,%d,%d",GG_FOCUSTC, tmpMove);
    MoveStage(tmpMove);
    break;
  };

    /**********************************************************************/
    //*** GND234
    //*** Used for tagging the log stream. 
  case GG_TAGLOG:{
    GtoT_MessageRcvd(GG_TAGLOG);
    T234_TagLog(m,tmpRoiX); // reuse a unsigned short
    WriteToLog(modeName[modeID],"TAG Rcvd: %d",tmpRoiX);
    break;
  }
    
    /**********************************************************************/
    //** GND235
    //***************
    //*** Set Platescale TC
    // adjusts the platescale of the chip (arcsec/pixel);
  case GG_SETPLATESCALETC:{
    GtoT_MessageRcvd(GG_SETPLATESCALETC);
    T235_SetCameraPlateScaleTC(m,tmpPS);
    camera.setPlateScale(tmpPS);
    WriteToLog(modeName[modeID],"Platescale changed to: %lf arcsec/pix",tmpPS);

    break;
  };

    /**********************************************************************/
    //**** GND237
    //********************
    //*** Set Rotation TC
    // adjusts the relative rotation between the chip and the EL + CE
    // axes

  case GG_SETROTATIONTC:{
    GtoT_MessageRcvd(GG_SETROTATIONTC);
    T237_SetRotationTC(m, tmpRot);
    WriteToLog(modeName[modeID],"Relative rotation changed to %lf rad", tmpRot);
    break;
  };// case GG_SETROTATIONTC

    /******************************************************************/

  case GG_OFFSETSANDGAINS:{
    GtoT_MessageRcvd(GG_OFFSETSANDGAINS);
 
    T220_OffsetsAndGains(m, tmpMode, tmpValue);

    if(tmpMode == 0){
      CardXOffset = tmpValue;
    } else if(tmpMode == 1){
      CardYOffset = tmpValue;
    } else if(tmpMode == 2){
      TrackerXOffset = tmpValue;
    } else if(tmpMode == 3){
      TrackerYOffset = tmpValue;
    } else if(tmpMode == 4){
      camera.setUserELOffset(tmpValue);
    } else if(tmpMode == 5){
      camera.setUserCEOffset(tmpValue);
    } else if(tmpMode == 6){
      camera.setRoiGainEL(tmpValue);
    } else if(tmpMode == 7){
      camera.setRoiGainCE(tmpValue);
    } else if(tmpMode == 8){
      camera.setFCGainEL(tmpValue);
    } else if(tmpMode == 9){
      camera.setFCGainCE(tmpValue);
    };
    break;
  };
  
    // --- Turn On LEDs 
    case GG_TURNONLEDTC:{
      GtoT_MessageRcvd(GG_TURNONLEDTC);
      WriteToLog(modeName[modeID],"LED ON RCVD");
      //      if(modeID == INITMODE || modeID == AUTOCOLLIMATIONMODE){
	WriteToLog(modeName[modeID],"Focal Plane Lights ON");
      TurnLEDsOn();
      //      } else { 
      //	WriteToLog(modeName[modeID],"Cannot turn ON Focal plane lights in this mode");
	//      };
      break;
    };

    case GG_TURNOFFLEDTC:{
      GtoT_MessageRcvd(GG_TURNOFFLEDTC);
      WriteToLog(modeName[modeID],"LED OFF RCVD");
      WriteToLog(modeName[modeID],"Focal Plane Lights OFF");
      TurnLEDsOff();

      break;
    };

    case GG_FBGAIN:{
      T221_FBGain(m, tmpFBGain);
      GtoT_MessageRcvd(GG_FBGAIN);
      camera.setFrameBlobGain(tmpFBGain);
      WriteToLog(modeName[modeID],"FrameBlob gain %lf",tmpFBGain);
      break;
    };

  case GG_FBSATVAL:{
    T222_FBSatVal(m, tmpSatVal);
    GtoT_MessageRcvd(GG_FBSATVAL);
    camera.setFrameBlobSatVal(tmpSatVal);
    WriteToLog(modeName[modeID],"FrameBlob Saturation Value %d",tmpSatVal);
    break;
  };

  case GG_FBTHRESHOLD:{
    T223_FBThreshold(m, tmpThreshold);
    GtoT_MessageRcvd(GG_FBTHRESHOLD);
    camera.setFrameBlobThreshold(tmpThreshold);
    WriteToLog(modeName[modeID],"FrameBlob Threshold %lf",tmpThreshold);
    break;
  };

  case GG_FBDISTTOL:{
    T224_FBDistTol(m, tmpDistTol);
    GtoT_MessageRcvd(GG_FBDISTTOL);
    camera.setFrameBlobDistTol(tmpDistTol);
    WriteToLog(modeName[modeID],"FrameBlob Distance Tolerance %d",tmpDistTol);
    break;
  };

  case GG_FBGRID:{
    T225_FBGrid(m, tmpGrid);
    GtoT_MessageRcvd(GG_FBGRID);
    camera.setFrameBlobGrid(tmpGrid);
    WriteToLog(modeName[modeID],"FrameBlob Grid %d",tmpGrid);
    break;
  };

  case GG_FBCENTBOX:{
    T226_FBCenBox(m, tmpCenBox);
    GtoT_MessageRcvd(GG_FBCENTBOX);
    camera.setFrameBlobCenBox(tmpCenBox);     
    WriteToLog(modeName[modeID],"FrameBlob Centroid Box %d",tmpCenBox);
    break;
  };

  case GG_FBAPBOX:{
    T227_FBApBox(m, tmpApBox);
    GtoT_MessageRcvd(GG_FBAPBOX);
    camera.setFrameBlobApBox(tmpApBox);
    WriteToLog(modeName[modeID],"FrameBlob ApertureBox %d",tmpApBox);
    break;
  };

  case GG_FBREADOUTOFFSET:{
    T228_FBReadoutOffset(m, tmpReadoutOffset);
    GtoT_MessageRcvd(GG_FBREADOUTOFFSET);
    camera.setFrameBlobReadoutOffset(tmpReadoutOffset);
    WriteToLog(modeName[modeID],"FrameBlob Readout Offset %lf",tmpReadoutOffset);
    break;
  };

  case GG_FBREADOUTNOISE:{
    T229_FBReadoutNoise(m, tmpReadoutNoise);
    GtoT_MessageRcvd(GG_FBREADOUTNOISE);
    camera.setFrameBlobReadoutNoise(tmpReadoutNoise);
    WriteToLog(modeName[modeID],"FrameBlob Readout Noise %lf",tmpReadoutNoise);
    break;
  };

  case GG_HOMECAMERA:{
    GtoT_MessageRcvd(GG_HOMECAMERA);
    HomeCamera();
    WriteToLog(modeName[modeID],"Home Camera RCVD");
    break;
  };

  case GG_DACOUTPUT:{
    T238_DACOutput(m, tmpDACoutput);
    GtoT_MessageRcvd(GG_DACOUTPUT);
    
    if(tmpDACoutput){
      setOutputTrue();
      WriteToLog(modeName[modeID],"DAC Output command received.ON");
    } else {
      setOutputZero();
      WriteToLog(modeName[modeID],"DAC Output command received. OFF");
    };
    break;
  };
    
  case GG_TRACKBLOB:{
    T239_TrackBlob(m, tmpBlob);
    GtoT_MessageRcvd(GG_TRACKBLOB);
    camera.setTargetBlob((unsigned short) tmpBlob);
    WriteToLog(modeName[modeID],"TargetBlob changed to %d",tmpBlob);
    break;
  };

  case GG_INFOBLOB:{
    T240_InfoBlob(m, tmpBlob);
    GtoT_MessageRcvd(GG_INFOBLOB);
    //camera.infoblob(tmpBlob);
    WriteToLog(modeName[modeID],"InfoBlob changed to %d",tmpBlob);
    break;
  };
    
  case GG_NEXTBLOB:{
    GtoT_MessageRcvd(GG_INFOBLOB);
    camera.setTargetBlob(camera.getTargetBlob()+1);
    WriteToLog(modeName[modeID],"Next Blob requested");
    break;
  };

  case GG_TRACKINGPOINT:{
    GtoT_MessageRcvd(GG_TRACKINGPOINT);
    T242_SetTrackPoint(m,tmptpx, tmptpy);
    WriteToLog(modeName[modeID],"%03d: %d, %d",GG_TRACKINGPOINT,tmptpx, tmptpy);
    if(tmptpx<1360) trackpointCE = tmptpx;
    if(tmptpy<1036) trackpointEL = tmptpy;
    break;
  };

  case GG_MOVESTEPSIZE:{
    GtoT_MessageRcvd(GG_MOVESTEPSIZE);
    T243_SetMoveStepSize(m, tmpstp);
    WriteToLog(modeName[modeID],"%03d: %d",GG_MOVESTEPSIZE, tmpstp);
    camera.setMoveStepSize(tmpstp);
    break;
  };

    case GG_SWITCHINGGUIDER:{
      GtoT_MessageRcvd(GG_SWITCHINGGUIDER);
      WriteToLog(modeName[modeID],"%03d: Switching to Guider",GG_SWITCHINGGUIDER);
      GtoM_SwitchingGuiderRequest();
      camera.setActiveSensor(GUIDERSENSOR);
      break;
    };

  case GG_SWITCHINGDTU:{
      GtoT_MessageRcvd(GG_SWITCHINGDTU);
      WriteToLog(modeName[modeID],"%03d: Switching to DTU",GG_SWITCHINGDTU);
      GtoM_SwitchingDTURequest();
      camera.setActiveSensor(OTHERSENSOR);
      break;
  };

  case GG_LOCKEDFC:{
    static char psubmode;
    static bool changemode;
    GtoT_MessageRcvd(GG_LOCKEDFC);
    if(modeID==POINTINGMODE){

      T245_LockedFC(m,lFC);
      changemode = false;
      if(camera.getExpT() != lFC.expt || camera.getTrigT()!= lFC.trigt) changemode = true;
      psubmode = (PointingSubMode)camera.getSubModeID();
      
      
      if(psubmode == LOCKED){
	if(!changemode){
	  camera.setNowWin(lFC.wx,lFC.wy,lFC.wd,lFC.wd);
	  camera.pointingTrackPointX = lFC.tpx;
	  camera.pointingTrackPointX = lFC.tpy;
	  camera.setSubModeID(LOCKED);
	} else {
	  camera.setNowWin(lFC.wx,lFC.wy,lFC.wd,lFC.wd);
	  PointingMode.setModeParams(0, 0, CHIPWIDTH, CHIPHEIGHT, lFC.expt, lFC.trigt);
	  PointingMode.setMode(POINTINGMODE,LOCKED);
	  camera.pointingTrackPointX = lFC.tpx;
	  camera.pointingTrackPointX = lFC.tpy;	
	  camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	};
      
	
      } else if(psubmode == LOCKEDROI || psubmode ==TRACKINGROI){
	changemode = true;
	PointingMode.setMode(POINTINGMODE,(char) LOCKED);
	PointingMode.setModeParams(0, 0, CHIPWIDTH, CHIPHEIGHT, lFC.expt, lFC.trigt);
	camera.setNowWin(lFC.wx,lFC.wy,lFC.wd,lFC.wd);
	PointingMode.setMode(POINTINGMODE,(char) LOCKED);

	camera.pointingTrackPointX = lFC.tpx;
	camera.pointingTrackPointX = lFC.tpy;	
	camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	goto MainLoop;
	

      } else if(psubmode == TRACKINGFC){
	if(!changemode){
	  camera.setNowWin(lFC.wx,lFC.wy,lFC.wd,lFC.wd);
	  camera.setSubModeID(TRACKINGFC);
	  camera.pointingTrackPointX = lFC.tpx;
	  camera.pointingTrackPointX = lFC.tpy;
	} else {

	  PointingMode.setModeParams(0, 0, CHIPWIDTH, CHIPHEIGHT, lFC.expt, lFC.trigt);
	  camera.setNowWin(lFC.wx,lFC.wy,lFC.wd,lFC.wd);
	  PointingMode.setMode(POINTINGMODE,(char) LOCKED);
	  camera.pointingTrackPointX = lFC.tpx;
	  camera.pointingTrackPointX = lFC.tpy;	
	  camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	  goto MainLoop;
	};
      }
    } else {
      WriteToLog(modeName[modeID],"Can't switch TRACKING in this mode",tmpBlob);
    };
    
    
    break;
  }; // case GG_LOCKED
    
  case GG_LOCKEDROI:{
    // here it doesn't matter what mode we are in, we just switch...
   
    T246_LockedROI(m,lROI);
    WriteToLog(modeName[modeID],"%03d: %d, %d, %d, %d, %d",GG_LOCKEDROI,lROI.rx, lROI.ry, lROI.rd, lROI.expt, lROI.trigt);
    GtoT_MessageRcvd(GG_LOCKEDROI);
    if(modeID==POINTINGMODE){
      PointingMode.setModeParams(lROI.rx-lROI.rd,lROI.ry-lROI.rd,2*lROI.rd,2*lROI.rd,lROI.expt,lROI.trigt);
      PointingMode.setMode(POINTINGMODE,LOCKEDROI);
      camera.setNextWin(lROI.rx,lROI.ry,lROI.rd,lROI.rd);
      camera.prepSettings(PointingMode,camera.getTargetRA(),camera.getTargetDEC());    
      
      goto MainLoop;
    };
      break;
    };//GG_LOCKEDROI
    
  case GG_GUIDINGFC:{
    // this one is a little complicated. first we check if we have to perform a full mode switch. 

    T247_GuidingFC(m,gFC);
    GtoT_MessageRcvd(GG_GUIDINGFC);
    WriteToLog(modeName[modeID],"%03d: %d, %d, %d, %lf, %lf, %d, %d",GG_GUIDINGFC, gFC.wx, gFC.wy, gFC.wd, gFC.ra, gFC.dec, gFC.expt, gFC.trigt);
    if(modeID == POINTINGMODE){
      static unsigned char psubmode;
      static bool modechange;
      if(camera.getExpT() != gFC.expt || camera.getTrigT() != gFC.trigt) modechange = true;
      psubmode = (unsigned) camera.getSubModeID();
      
      if(psubmode == LOCKED){
	
	if(!modechange){
	  camera.setNowWin(gFC.wx,gFC.wy,gFC.wd,gFC.wd);
	  camera.setSubModeID(TRACKINGFC);
	  camera.setRA(gFC.ra);
	  camera.setDEC(gFC.dec);
	} else {
	  camera.setNextWin(gFC.wx,gFC.wy,gFC.wd,gFC.wd);
	  PointingMode.setModeParams(0, 0, CHIPWIDTH, CHIPHEIGHT, gFC.expt, gFC.trigt);
	  PointingMode.setMode(POINTINGMODE,TRACKINGFC);
	  camera.setRA(gFC.ra);
	  camera.setDEC(gFC.dec);
	  camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	  
	  goto MainLoop;
	}
      } else if(psubmode == LOCKEDROI || psubmode == TRACKINGROI){
	//	changemode = true;
	PointingMode.setMode(POINTINGMODE,(char) TRACKINGFC);
	PointingMode.setModeParams(0, 0, CHIPWIDTH, CHIPHEIGHT, gFC.expt, gFC.trigt);
	camera.setNowWin(gFC.wx,gFC.wy,gFC.wd,gFC.wd);
	PointingMode.setMode(POINTINGMODE,(char) TRACKINGFC);
	
	//	camera.pointingTrackPointX = lFC.tpx;
	//	camera.pointingTrackPointX = lFC.tpy;	
	camera.prepSettings(PointingMode, camera.getTargetRA(), camera.getTargetDEC());
	camera.setRA(gFC.ra);
	camera.setDEC(gFC.dec);
	goto MainLoop;
      } else if(psubmode == TRACKINGFC){
	camera.setNowWin(gFC.wx,gFC.wy,gFC.wd, gFC.wd);
	camera.setRA(gFC.ra);
	camera.setDEC(gFC.dec);
      };
    };
    break;
  }; // GG_GUIDINGFC;

 
    

  case GG_GUIDINGROI:{
        // here it doesn't matter what mode we are in, we just switch...
    T248_GuidingROI(m,gROI);
    GtoT_MessageRcvd(GG_GUIDINGROI);

    WriteToLog(modeName[modeID],"%03: %d, %d, %d, %lf, %lf, %d, %d",GG_GUIDINGROI, gROI.rx, gROI.ry, gROI.rd, gROI.ra, gROI.dec, gROI.expt, gROI.trigt);

    if(modeID == POINTINGMODE){
      PointingMode.setModeParams(gROI.rx-gROI.rd,gROI.ry-gROI.rd,2*gROI.rd,2*gROI.rd,gROI.expt,gROI.trigt);
      camera.setNextWin(gROI.rx,gROI.ry,gROI.rd,gROI.rd);
      PointingMode.setMode(POINTINGMODE,TRACKINGROI);
      camera.prepSettings(PointingMode,camera.getTargetRA(), camera.getTargetDEC());
      camera.setRA((double) gROI.ra);
      camera.setDEC((double) gROI.dec);
      goto MainLoop;
    }
    break;
  }; //GG_GUIDINGROI;

  case GG_RESETPROGRAM:{
    GtoT_MessageRcvd(GG_RESETPROGRAM);
    WriteToLog(modeName[modeID],"%03d: Reset Program Requested",GG_RESETPROGRAM);
    unlink("lastmode.mds");
  };

  case GG_RESTARTPROGRAM:{
    GtoT_MessageRcvd(GG_RESTARTPROGRAM); 
    WriteToLog(modeName[modeID],"%03d: Program Restart Demanded.",GG_RESTARTPROGRAM); 
    // this needs to be fixed XXX
    exit(255);
    break;
  };

  case GG_TURNOFFMPF:{
    GtoT_MessageRcvd(GG_TURNOFFMPF);
    WriteToLog(modeName[modeID],"%03d: Disabling MPF Writes",GG_TURNOFFMPF);
    datatompf = false;
    break;
  };

  case GG_TURNONMPF:{
    GtoT_MessageRcvd(GG_TURNONMPF);
    WriteToLog(modeName[modeID],"%03d: Enabling MPF Writes",GG_TURNONMPF);
    datatompf = true;
    break;
  };

  case GG_ADJUSTOFFSET:{
    GtoT_MessageRcvd(GG_ADJUSTOFFSET);
    WriteToLog(modeName[modeID],"%03d: Adjust Offset Requested",GG_ADJUSTOFFSET);
    int ii;
    float ddx,ddy;
    ddx=ddy=0;
    for(ii=0;ii<FRAMES;ii++){
      ddx+=camera.distxbuffer[ii];
      ddy+=camera.distybuffer[ii];
    };
    ddx /= (float) FRAMES;
    ddy /= (float) FRAMES;
    camera.gondolaoffsetx = -ddx;
    camera.gondolaoffsety = -ddy;
    camera.SaveQCamState();
    WriteToLog(modeName[modeID],"%03d: Offset Calculated: %f, %f",GG_ADJUSTOFFSET,ddx,ddy);
    break;
  };


  case GG_FEELLUCKY:{
    WriteToLog(modeName[modeID],"%03d: Feel Lucky Received",GG_FEELLUCKY);
    GtoT_MessageRcvd(GG_FEELLUCKY); 
    if(modeID == ALIGNMENTMODE && camera.getRoiDX()==1360){
      camera.feellucky=true; 
      WriteToLog(modeName[modeID],"%03d: Success",GG_FEELLUCKY);
    } else {
      WriteToLog(modeName[modeID],"%03d: Failure",GG_FEELLUCKY);
    };
    break;
  };

  case GG_DITHERPATTERN:{
    GtoT_MessageRcvd(GG_DITHERPATTERN);
    T012_DitherPattern(m,tmpValid1);
    //     WriteToLog("HELLO!!!!!");
    
    
    if (tmpValid1) {
      WriteToLog(modeName[modeID],"Dither START(%d) Rcvd.",tmpValid1);
      dither::load(tmpValid1);
      dither::start();
    } else {
      WriteToLog(modeName[modeID],"Dither STOP Rcvd.");
      dither::stop();
    };
    break;
  };
    

  case GG_SWITCHZOOMTYPE: {    
    GtoT_MessageRcvd(GG_SWITCHZOOMTYPE);
    if (zoom_type) {
      WriteToLog(modeName[modeID],"ZOOM TYPE 0 RCVD");
      zoom_type = 0;
    } else {
      zoom_type = 1;
      WriteToLog(modeName[modeID],"ZOOM TYPE 1 RCVD");
    };

    break;
  };
    
    
  case GG_SWITCHTOLARGE:{ 
    GtoT_MessageRcvd(GG_SWITCHTOLARGE);
    WriteToLog(modeName[modeID],"%03d: Switch to Large window received",GG_SWITCHTOLARGE);
    if(modeID == ALIGNMENTMODE){

      WriteToLog("ALGN","Switching to Large window requested"); //XXX
	  unsigned short x,y,dx,dy;
	  x = camera.getRoiX();
	  y = camera.getRoiY();
	  dx = camera.getRoiDX()/2;
	  dy = camera.getRoiDY()/2;
	  x = x+dx;
	  y = y+dy;
	  /*	  AlignmentMode.winX=x;
	  AlignmentMode.winY=y;
	  AlignmentMode.winDX=dx;
	  AlignmentMode.winDY=dy;
	  */
	  AlignmentMode.setModeParams(0,0,CHIPWIDTH,CHIPHEIGHT, camera.largeExpT, camera.largeTrigT);
	  AlignmentMode.setMode(ALIGNMENTMODE,3);

	  //	  camera.setRA(gFC.ra);
	  //	  camera.setDEC(gFC.dec);
	  camera.prepSettings(AlignmentMode, camera.getTargetRA(), camera.getTargetDEC());
	  camera.setNextWin(x,y, dx,dy);
	  camera.setNowWin(x,y,dx,dy);
	  WriteToLog(modeName[modeID],"%03d: Success.",GG_SWITCHTOLARGE);
	  goto MainLoop;
    } else {
          WriteToLog(modeName[modeID],"%03d: Failure. Wrong Mode.",GG_SWITCHTOLARGE);
    };
    break;
  };

  case GG_SWITCHTOSMALL:{
    GtoT_MessageRcvd(GG_SWITCHTOSMALL);
        WriteToLog(modeName[modeID],"%03d: Switch to Small window received",GG_SWITCHTOSMALL);
	if(modeID == ALIGNMENTMODE){
	  WriteToLog("ALGN","Switching to small window requested");
	  unsigned short x,y,dx,dy;
	  x = camera.getWinX();
	  y = camera.getWinY();
	  dx = camera.getWinDX();
	  dy = camera.getWinDY();
	  x=x-dx;
	  y=y-dy;
	  WriteToLog("SWSM","%d %d %d %d",x,y,dx,dy);
	  /*
	  AlignmentMode.winX=x;
	  AlignmentMode.winY=y;
	  AlignmentMode.winDX=dx;
	  AlignmentMode.winDY=dy;	  
	  */
	  AlignmentMode.setModeParams(x,y,2*dx,2*dy, camera.smallExpT, camera.smallTrigT);
	  AlignmentMode.setMode(ALIGNMENTMODE,2);
	  //	  camera.setNowWin(x,y,dx,dy);
	  camera.prepSettings(AlignmentMode, camera.getTargetRA(), camera.getTargetDEC());
	  camera.setNextWin(x+dx,y+dy,dx,dy);
	  camera.setNowWin(x+dx,y+dy,dx,dy);
	  
	  WriteToLog(modeName[modeID],"%03d: Success.",GG_SWITCHTOLARGE);
	  //	  WriteToLog("WINSEEN","%d %d %d %d",camera.getWinX(), camera.getWinY(), camera.getWinDX(), camera.getWinDY());
	  goto MainLoop;
	} else {
	  WriteToLog(modeName[modeID],"%03d: Failure. Wrong Mode",GG_SWITCHTOLARGE);
	};
	break;
  };
    
  case GG_WINDOWTRACK:{
    GtoT_MessageRcvd(GG_WINDOWTRACK);
    	T164_WindowTrack(m,wt);
    WriteToLog(modeName[modeID],"%03d: %d, %d, %d, %d, %d",GG_WINDOWTRACK,wt.wx, wt.wy,wt.wd,wt.tx,wt.ty);
    if(modeID==ALIGNMENTMODE){
      if(camera.getSubModeID()!=ALIGNTRACKROI){
	//    tmpExpT = camera.largeExpT;
	//    tmpTrigT = camera.largeTrigT;
	camera.setNowWin(wt.wx, wt.wy, wt.wd, wt.wd);
	camera.setNextWin(wt.wx, wt.wy, wt.wd, wt.wd);
	trackpointEL = wt.ty;
	trackpointCE = wt.tx;
	AlignmentSubMode.setMode(ALIGNMENTMODE, tmpRoiFlag);
	camera.nowMode.setMode(ALIGNMENTMODE,ALIGNDRAGFULLCHIP);
	camera.SaveQCamState();
	WriteToLog(modeName[modeID],"%03d: Success.",GG_WINDOWTRACK);
      } else {
	WriteToLog(modeName[modeID],"%03d: Failure. Wrong submode.",GG_WINDOWTRACK);
      };
    } else {
      WriteToLog(modeName[modeID],"$03d: Failure. Wrong mode.",GG_WINDOWTRACK);
    };
    break;
  };

  case GG_SMALLTIME:{
    GtoT_MessageRcvd(GG_SMALLTIME);
    T165_SmallTime(m,ct);
    WriteToLog(modeName[modeID],"%03d: %d, %d, Rcv'd",GG_SMALLTIME,ct.expt, ct.trigt);
    if(1000.*ct.trigt>1.0*ct.expt+2.99){
      camera.smallTrigT = ct.trigt;
      camera.smallExpT = ct.expt;
      WriteToLog(modeName[modeID],"%03d: %d, %d, Success.",GG_SMALLTIME, ct.expt, ct.trigt);

      static unsigned short x,y,dx,dy;
      x = camera.getRoiX();
      y = camera.getRoiY();
      dx = camera.getRoiDX();
      dy = camera.getRoiDY();
      if(dx<201 && dy<201){
	AlignmentMode.setModeParams(x,y,dx,dy, camera.smallExpT, camera.smallTrigT);
	AlignmentMode.setMode(ALIGNMENTMODE,2);
	//	  camera.setNowWin(x,y,dx,dy);
	camera.prepSettings(AlignmentMode, camera.getTargetRA(), camera.getTargetDEC());
	camera.setNextWin(x+dx,y+dy,dx,dy);
	camera.setNowWin(x+dx,y+dy,dx,dy);
	goto MainLoop;
      };
    } else {
      WriteToLog(modeName[modeID],"%03d: %d, %d, Failure.",GG_SMALLTIME,ct.expt, ct.trigt);
    };
        camera.SaveQCamState();
    break;
  };

  case GG_LARGETIME:{
    GtoT_MessageRcvd(GG_LARGETIME);
    T166_LargeTime(m,ct);
    WriteToLog(modeName[modeID],"%03d: %d, %d, Rcv'd",GG_LARGETIME,ct.expt, ct.trigt);
    if(1000.*ct.trigt>1.0*ct.expt+2.99){
      camera.largeTrigT = ct.trigt;
      camera.largeExpT = ct.expt;
      WriteToLog(modeName[modeID],"%03d: %d, %d, Success",GG_LARGETIME,ct.expt, ct.trigt);
    } else {
      WriteToLog(modeName[modeID],"%03d: %d, %d, Failure",GG_LARGETIME,ct.expt, ct.trigt);
    };
    static unsigned short x,y,dx,dy;
      x = camera.getRoiX();
      y = camera.getRoiY();
      dx = camera.getRoiDX();
      dy = camera.getRoiDY();




    if(dx==1360 && dy==1036){    
      static unsigned short wx,wy,wdx,wdy;

      wx = camera.getWinX();
      wy = camera.getWinY();
      wdx = camera.getWinDX();
      wdy = camera.getWinDY();
      
      AlignmentMode.setModeParams(x,y,dx,dy, camera.largeExpT, camera.largeTrigT);
      AlignmentMode.setMode(ALIGNMENTMODE,3);
      //	  camera.setNowWin(x,y,dx,dy);
      camera.prepSettings(AlignmentMode, camera.getTargetRA(), camera.getTargetDEC());
      camera.setNextWin(wx,wy,wdx,wdy);
      camera.setNowWin(wx,wy,wdx,wdy);
      goto MainLoop;
    };
    camera.SaveQCamState();
    break;
  };



  case GG_OVERMAXCNT:{
      GtoT_MessageRcvd(GG_OVERMAXCNT);
    T015_OvermaxCnt(m,ctrovermaxcnt);
    WriteToLog(modeName[modeID],"%03d: Overmax Count Rcvd %f",GG_OVERMAXCNT,ctrovermaxcnt);
    break;
    break;
  };

  case GG_OVERMAXTHR:{
    GtoT_MessageRcvd(GG_OVERMAXTHR);
    T016_OvermaxThr(m,ctrovermaxthr);
    WriteToLog(modeName[modeID],"%03d: Overmax Threshold Rcvd %f",GG_OVERMAXTHR,ctrovermaxthr);
    break;
  };

  case GG_TOGGLEALGORITHM:{
    GtoT_MessageRcvd(GG_TOGGLEALGORITHM);   
    if(ctralgorithm){
      ctralgorithm = 0;
    } else {
      ctralgorithm = 1;
    };
    WriteToLog(modeName[modeID],"%03d: Toggle Display Rcvd (->%d)",GG_OVERMAXTHR,ctralgorithm);
    break;
  };
    
  case GG_SPOTRMS:{
    GtoT_MessageRcvd(GG_SPOTRMS);
    T018_SpotRMSThr(m,ctrspotrms);
    WriteToLog(modeName[modeID],"%03d: Spot RMS Rcvd %f",GG_OVERMAXCNT,ctrspotrms);
    break;

  };
    
  case GG_BOXSIZE:{
    GtoT_MessageRcvd(GG_BOXSIZE);
    T019_BoxSize(m, DIVISOR);
    WriteToLog(modeName[modeID],"%03d: BoxSize Rcvd %d",GG_BOXSIZE,DIVISOR);
    break;
  };
    

  case GG_RESETMODE:{
    GtoT_MessageRcvd(GG_RESETMODE);
    WriteToLog(modeName[modeID],"%03d: Reset Mode Recv'd",GG_RESETMODE);
    break;
  };


  case GG_STARTSAVING:{
    GtoT_MessageRcvd(GG_STARTSAVING);
    WriteToLog(modeName[modeID],"%03d: Start Savomg Recv'd",GG_STARTSAVING);
    filesaving = true;
    break;
  };

  case GG_STOPSAVING:{
    GtoT_MessageRcvd(GG_STOPSAVING);
    WriteToLog(modeName[modeID],"%03d: Stop Savomg Recv'd",GG_STOPSAVING);
    filesaving = false;
    break;
  };

    
  default:{
    GtoT_MessageRcvd(0);
    WriteToLog(modeName[modeID],"%03d: Unrecognized Command!", mID);
    break;
  };
  };
  
};







/*** temporary scratchpad below ***/
/*
    if(modeID == INITMODE){

    } else if(modeID == AUTOCOLLIMATIONMODE){

    } else if(modeID == ALIGNMENTMODE){

    } else if(modeID == SLEWINGMODE){

    } else if(modeID == POINTINGMODE){

    };
    

    if(modeID == INITMODE){

    } else if(modeID == AUTOCOLLIMATIONMODE){

    } else if(modeID == ALIGNMENTMODE){

    } else if(modeID == SLEWINGMODE){

    } else if(modeID == POINTINGMODE){

    };
    

    if(modeID == INITMODE){

    } else if(modeID == AUTOCOLLIMATIONMODE){

    } else if(modeID == ALIGNMENTMODE){

    } else if(modeID == SLEWINGMODE){

    } else if(modeID == POINTINGMODE){

    };
    

*/
