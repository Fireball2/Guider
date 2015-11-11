
/************************************************************************/
// first we sleep for 10ms or so. 
nanosleep(&tim, NULL);
// now we trigger whichever periods have just elapsed.
if(!(loopcounter % SHORTDELAY))  shortdelay = true;
if(!(loopcounter % MEDIUMDELAY)) mediumdelay = true;
if(!(loopcounter % LONGDELAY)) longdelay = true;
if(!(loopcounter % QUARTERSEC)) quartersec = true;
if(!(loopcounter % HALFSEC)) halfsec = true;
if(!(loopcounter % SEC)) sec = true;
if(!(loopcounter % FIVESEC)) fivesec = true;
/************************************************************************/
/************************************************************************/

// This is where all the action takes place.
// first we do the things that have to be done in every mode, no matter what

// we have to read the MPF and TMTC ports
if(shortdelay){

  ReadMPF();
  ReadTMTC();
};




// we have to read the GPS port. 
if(mediumdelay){
  GPSRead();

};


if(longdelay){
    getAdcValue();
};


if(fivesec){
  GtoT_GuiderStatus(&camera);
  WriteToLog("LVDT","%05f",getAdcValue());
  WriteToLog("GPS");
};

// below we send specific messages at specific times for specific modes.
// each mode has its own if clause 

/************************************************************************/
if(modeID == INITMODE){
  
};



/************************************************************************/
if(modeID == AUTOCOLLIMATIONMODE){

};

/************************************************************************/
if(modeID == ALIGNMENTMODE){


};


/************************************************************************/
if(modeID == SLEWINGMODE){


};

/************************************************************************/
if(modeID == POINTINGMODE){


};




/************************************************************************/
/************************************************************************/
//now we reset all of the booleans
shortdelay = mediumdelay = longdelay = false;
quartersec = halfsec = sec = fivesec = false;
// and we increment the loop counter.
loopcounter++;
if(loopcounter == 1000000) loopcounter = 0;
//we don't have to roll it over, but it costs nothing to do so. 
/************************************************************************/
