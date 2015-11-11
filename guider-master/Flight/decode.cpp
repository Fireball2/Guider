//#define TESTMODE

#include "decode.h"
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include "parpulse.h"


#include <mcheck.h>
#define MEMCHECK

#define PULSELENGTH 10000
#define PULSEVALUE 255


unsigned short *pus;
short *ps;
float *pf;
double *pd;
unsigned char *puc;
unsigned long *pul;
double *pud;

// these two keep track of the last message ID received from 
// either the MPF Num or the TMTC link. 
unsigned short lastfromMPF;
unsigned short lastfromTMTC;

void ResetReceivedMessageCounters(){
  #ifdef MEMCHECK
mtrace();
#endif
  lastfromMPF=0;
  lastfromTMTC=0;
};


/******
Message ID 1
Guider Pointing Errors Data
string ----> 
EL/CE data
RA/DEC data
*****/
void G001_GuiderPointingErrorsData(
				    message &m,
				    float &el,
				    float &ce,
				    unsigned char &valELCE,
				    double &ra,
				    double &dec,
				    unsigned char &valRADEC
				    ){
#ifdef MEMCHECK
mtrace();
#endif
  pf = (float *) &m.body[6];
  el = *pf;  
  pf = (float *) &m.body[10];
  ce = *pf;
  puc = (unsigned char *) &m.body[14];
  valELCE = *puc;

  pud = (double *) &m.body[15];
  ra = *pud;

  pud = (double *) &m.body[23];
  dec  = *pud;

  puc = (unsigned char *) &m.body[31];
  valRADEC = *puc;

  return;
};


/******
Message ID 2
Autocolimation Request. 
Nothing much to do here
*******/
void G002_AutocolimationRequest(
				message &
				)
{
  return;
};

/******
Message ID 3
Switching Guider Request
Nothing much to do here
*******/
void G003_SwitchingGuiderRequest(
				 message &
				 )
{
  return;
};

/******
       Message ID 4
       Switching DTU Request
       Nothing much to do here
*******/
void G004_SwitchingDTURequest(
			      message &
			      )
{
#ifdef MEMCHECK
mtrace();
#endif
  return;
};




/******
       Message ID 5
       Tracking Request
       message --->
        RA, DEC
	config type.
*****/
void G005_TrackingRequest(
			  message &m,
			  double &ra,
			  double &dec,
			  unsigned char &configtype
			  ){
#ifdef MEMCHECK
mtrace();
#endif
  pud = (double *) &m.body[6];
  ra = *pud;

  pud = (double *) &m.body[14];
  dec = *pud;
  puc = (unsigned char *) &m.body[22];
  configtype = *puc;

  return;
};

/******
       Message ID 6
       Slewing Request
       message --->
       RA, DEC
*****/
void G006_SlewingRequest(
			 message &m,
			 double &ra,
			 double &dec
			 ){
  
#ifdef MEMCHECK
mtrace();
#endif
  pud = (double *) &m.body[6];
  ra = *pud;

  pud = (double *) &m.body[14];
  dec = *pud;

  return;
};


/******
       Message ID 7
       EL & CE Offsets Request
       message --->
       EL CE
*****/
void G007_ELCEOffsetsRequest(
			     message &m,
			     float &EL,
			     float &CE
			     )
{
  #ifdef MEMCHECK
mtrace();
#endif
  pf = (float *) &m.body[6];
  EL = *pf;
  pf = (float *) &m.body[10];
  CE = *pf;

  return;
};


/******
       Message ID 8
       Guider Status Data
       message --->
       Operational State
       Fine Pointing Contriller
*****/
void G008_GuiderStatusData(
			   message &m,
			   unsigned char &opstate,
			   unsigned char &fpc
			   )
{
#ifdef MEMCHECK
mtrace();
#endif
  opstate = m.body[6];
  fpc = m.body[7];
  
  return;
};



/******
Message ID 9
Center ROI Request
Nothing much to do here
*******/
void G009_CenterROIRequest(
			   message &
			   ){

  return;
};

/******
       Message ID 100
       Autocolimation TC
       Nothing much to do here
*******/
void G100_AutocolimationTC(
			   message &
			   ){

  return;
};


/******
       Message ID 101
       Slewing TC
       message --->
       RA, DEC
*****/
void G101_SlewingTC(
		    message &m,
		    double &ra,
		    double &dec
		    )
{

  pud = (double *) &m.body[6];
  ra = *pud;

  pud = (double *) &m.body[14];
  dec = *pud;
  return;
};


/******
       Message ID 102
       Slewing TC
       message --->
       RA, DEC
*****/
void G102_AlignmentTC(
		    message &m,
		    double &ra,
		    double &dec
		    )
{
#ifdef MEMCHECK
mtrace();
#endif
  pud = (double *) &m.body[6];
  ra = *pud;

  pud = (double *) &m.body[14];
  dec = *pud;
  return;
};


/******
       Message ID 103
       EL & CE Offsets TC
       message --->
       EL CE
*****/
void G103_ELCEOffsetsTC(
			message &m,
			float &EL,
			float &CE
			)
{
#ifdef MEMCHECK
mtrace();
#endif
  pf = (float *) &m.body[6];
  EL = *pf;
  pf = (float *) &m.body[10];
  CE = *pf;

  return;
};



/***************************
Second set decodes commands sent from MPF to guider
**************************** /

/ ****** 
	Message ID 1
	DTU Pointing Errors Data
	message -->
	EL, CE
	type of measurement
	valid data
******* /
void M001_DTUPointingErrorsData(
				message &m,
				float &EL,
				float &CE,
				unsigned char &meastype,
				unsigned char &valid
				)
{
#ifdef MEMCHECK
mtrace();
#endif
  pf = (float *) &m.body[6];
  EL = *pf;
  pf = (float *) &m.body[10];
  CE = *pf;
  meastype = m.body[14];
  valid = m.body[15];

#ifdef TESTMODE

printf("TESTMODE: DTU Pointing Errors Data\n");
printf("----------------------------------\n");
printf("EL Offset  \t\t: %f\n",EL);
printf("CE Offset  \t\t: %f\n",CE);
printf("Meas. Type \t\t: %d\n",meastype);
printf("Meas. Valid\t\t: %d\n",valid);
printf("----------------------------------\n\n");
  unsigned short *lngth;
  int i;
  lngth = (unsigned short *) &m.body[3];
  for(i=0;i<*lngth+7;i++) printf("%d ",m.body[i]);
printf("\n----------------------------------\n\n");
ParPulse(PULSEVALUE, PULSELENGTH);
#endif


  return;
};

/ *********
	  Message ID 2 
	  DTU Coordinate Data
	  message ---->
 	  DTU Attitude Data
******** /	  






//This stuff needs to be worked out.

typedef struct AscAttitudeTM{
  unsigned char &PIDVersionNo;
  unsigned char &PIDType;
  unsigned char &PIDDataFieldHeaderFlag;
  unsigned short &PIDApplicationProcessID;
  unsigned char &SCSegmentationFlag;
  unsigned short &SCSourceSequenceControl;
  unsigned short &Length;
  unsigned char &DFHspare1;
  unsigned char &DFHPUSVersion;
  unsigned char &DFHspare2;
  unsigned char &DFHType;
  unsigned char &DFHSubtype;
  unsigned long &DFHTimestamp;
  unsigned char &DFHSpare3;
};

  
void M002_DTUCoordinateData(message &m,
			    AscAttitude &acs){
  //  FILE *ofile;
  unsigned char *intc;
  unsigned char *shortc;
  unsigned char tmp[4];


  //  ofile = fopen("ACSCoord.dat","w");
  //  fprintf(ofile,"test");

  unsigned short packid;

  puc = (unsigned char *) &m.body[6];
  packid = puc[1]+puc[0]*256;
  acs.PIDVersion = (packid & 57344) >> 13;
  acs.PIDType = (packid & 4096) >> 12;
  acs.PIDDataFieldHeaderFlag = (packid & 2048) >> 11;
  acs.PIDApplicationProcessID = (packid & 2047);
  

  //  pus = (unsigned short *) &m.body[8];
  //  acs.SCSegmentationFlag = (*pus &3);
  //  acs.SCSourceSequenceControl = (*pus & 65532) >> 2;

  puc = (unsigned char *) &m.body[8];
  packid = puc[1]+256*puc[0];
  acs.SCSegmentationFlag = (packid & 49152) >> 14;
  acs.SCSourceSequenceControl = (packid & 16383);


  / *
  fprintf(ofile, "SC Segmentation Flag \t: %d\n", acs.SCSegmentationFlag);
  fprintf(ofile, "SC Source Sequence Count \t: %d\n", acs.SCSourceSequenceControl);
  * /
  puc = (unsigned char *) &m.body[10];
  acs.Length = 256*puc[0]+puc[1];
  //fprintf(ofile, "Message Length \t\t: %d\n", acs.Length);

  puc = (unsigned char *) &m.body[12];
  acs.DFHspare1 = ((*puc) & 128) >> 7;
  acs.DFHPUSVersion =  ( (*puc) & 112 )>>4;
  acs.DFHspare2 = ((*puc) & 15);



  acs.DFHType = m.body[13];
  acs.DFHSubtype = m.body[14];
  / *
  fprintf(ofile, "Data Field Header Version \t:%d\n",acs.DFHPUSVersion);
  fprintf(ofile, "Data Field Header Type \t: %d\n",acs.DFHType);
  fprintf(ofile, "Data Field Header SubType \t: %d\n",acs.DFHSubtype);
  * /
  puc = (unsigned char *) &m.body[15];
  intc = (unsigned char *) &acs.DFHTimestampSec;
  // flipping endianness
  intc[0] = puc[3]; //--
  intc[1] = puc[2];
  intc[2] = puc[1];
  intc[3] = puc[0];

  puc = (unsigned char *) &m.body[19];
  shortc = (unsigned char *) &acs.DFHTimestampSubSec;
  shortc[0] = puc[1];
  shortc[1] = puc[0];

  / *
  fprintf(ofile, "Data Field Header Timestamp \t%d sec\n",acs.DFHTimestampSec);
  fprintf(ofile, "Data Field Header Timestamp \t%d subsec\n",acs.DFHTimestampSubSec);
  * /
  shortc = (unsigned char *) &acs.ReportID;
  puc = (unsigned char *) &m.body[22];
  
  shortc[0] = puc[1];
  shortc[1] = puc[0];
  / *
  fprintf(ofile, "Report ID \t\t%x\n",acs.ReportID);
  * /
  intc = (unsigned char *) &acs.q1;
  puc = (unsigned char *)  &m.body[24];

  intc[0] = puc[3];
  intc[1] = puc[2];
  intc[2] = puc[1];
  intc[3] = puc[0];


  intc = (unsigned char *) &acs.q2;
  puc = (unsigned char *)  &m.body[28];
  intc[0] = puc[3];
  intc[1] = puc[2];
  intc[2] = puc[1];
  intc[3] = puc[0];

  intc = (unsigned char *) &acs.q3;
  puc = (unsigned char *)  &m.body[32];
  intc[0] = puc[3];
  intc[1] = puc[2];
  intc[2] = puc[1];
  intc[3] = puc[0];

  intc = (unsigned char *) &acs.q4;
  puc = (unsigned char *)  &m.body[36];
  intc[0] = puc[3];
  intc[1] = puc[2];
  intc[2] = puc[1];
  intc[3] = puc[0];
  / *
  fprintf(ofile,"Q1 \t\t %d\n",acs.q1);
  fprintf(ofile,"Q2 \t\t %d\n",acs.q2);
  fprintf(ofile,"Q3 \t\t %d\n",acs.q3);
  fprintf(ofile,"Q4 \t\t %d\n",acs.q4);
  * /
  acs.infoFlags  = m.body[40];
  acs.infoResidual = m.body[41];
  acs.infoLocks = m.body[42];
  acs.infoObjects = m.body[43];
  acs.infoStarsFail = m.body[44];
  acs.AGC[0] = m.body[45];
  acs.AGC[1] = m.body[46];
  acs.AGC[2] = m.body[47];



  / *
  fprintf(ofile,"Flags\n");
  fprintf(ofile,"\t Sequence %d\n", acs.infoFlags & 1);
  fprintf(ofile,"\t Correction %d\n", (acs.infoFlags & 2)/2);
  fprintf(ofile,"\t Camera ID %d\n", (acs.infoFlags & 4)/4);
  fprintf(ofile,"\t Sequence %d\n", (acs.infoFlags & 8)/8);
  fprintf(ofile,"\t Highrate %d\n", (acs.infoFlags & 16)/16);
  fprintf(ofile,"\t BBO %d\n", (acs.infoFlags & 32/32));
  fprintf(ofile,"\t Time Ref %d\n", (acs.infoFlags & 64)/64);
  fprintf(ofile,"\t Valid %d\n", (acs.infoFlags & 128)/128);
  
  fprintf(ofile,"info Residual \t: %d\n", acs.infoResidual);
  fprintf(ofile,"info Locks  \t: %d\n",acs.infoLocks);
  fprintf(ofile,"info Objects  \t: %d\n",acs.infoObjects);
  fprintf(ofile,"info Stars Fail  \t: %d\n",acs.infoStarsFail);
  * /  
  intc = (unsigned char *) &acs.timestampSec;
  puc = (unsigned char *) &m.body[48];
  intc[0] = puc[3];
  intc[1] = puc[2];
  intc[2] = puc[1];
  intc[3] = puc[0];

  puc = (unsigned char *) &m.body[52];
  shortc = (unsigned char *) &acs.timestampSubSec;
  
  shortc[0] = puc[0];
  shortc[1] = puc[1];
  / *
  fprintf(ofile, "Timestamp \t%d sec\n",acs.timestampSec);
  fprintf(ofile, "Timestamp \t%d subsec\n",acs.timestampSubSec);

  
  fclose(ofile);
  * /
#ifdef TESTMODE 
  printf("Testmode. Parsing Message #2\n");
  printf("----------------------------\n");
  printf("PID Version No\t\t: %d\n",acs.PIDVersion);
  printf("PID Type      \t\t: %d\n",acs.PIDType);
  printf("PID Data FHF  \t\t: %d\n",acs.PIDDataFieldHeaderFlag);
  printf("PID App PID  \t\t: %d\n",  acs.PIDApplicationProcessID);
  printf("SC Seg Flag   \t\t: %d\n",acs.SCSegmentationFlag);
  printf("SC Source Sequence Control : %d\n",acs.SCSourceSequenceControl);
  printf("ACS Length    \t\t: %d\n", acs.Length);
  printf("DFH PUS Version\t\t: %d\n",acs.DFHPUSVersion);
  printf("DFH Type       \t\t: %d\n",acs.DFHType);
  printf("DFH SUbtype    \t\t: %d\n",acs.DFHSubtype);
  printf("DFH Timestamp  \t\t: %f\n",(acs.DFHTimestampSec + acs.DFHTimestampSubSec/65536.)/3600.);
  printf("Report ID      \t\t: %d\n",acs.ReportID);
  printf("Q1             \t\t: %8lf\n",acs.q1/2147483647.); 
  printf("Q2             \t\t: %8lf\n",acs.q2/2147483647.); 
  printf("Q3             \t\t: %8lf\n",acs.q3/2147483647.); 
  printf("Q4             \t\t: %8lf\n",acs.q4/2147483647.); 
  printf("info Flags     \t\t: %d\n",acs.infoFlags);
printf("info Residual  \t\t: %d\n",acs.infoResidual);
printf("info Locks     \t\t: %d\n",acs.infoLocks);
printf("info Objects   \t\t: %d\n",acs.infoObjects);
printf("info Stars Fail\t\t: %d\n",acs.infoStarsFail);
printf("Timestamp      \t\t: %f\n",(acs.timestampSec + acs.timestampSubSec/65536.)/3600.);
printf("--------------------------------\n\n");
  unsigned short *lngth;
  int i;
  lngth = (unsigned short *) &m.body[3];
  for(i=0;i<*lngth+7;i++) printf("%d ",m.body[i]);
printf("\n----------------------------------\n\n");
ParPulse(PULSEVALUE, PULSELENGTH);
#endif
  


};



/ ********* 
	   Message ID 3
	   EL and CE Position Data
	   message ---->
	   EL and CE axis position
********* /
void M003_ELCEPositionData(
			   message &m,
			   float &EL,
			   float &CE
			   )
{
#ifdef MEMCHECK
mtrace();
#endif
  pf = (float *) &m.body[6];
  EL = *pf;
  pf = (float *) &m.body[10];
  CE = *pf;

#ifdef TESTMODE
printf("TESTMODE: ELCE Position Data\n");
printf("----------------------------\n");
printf("EL Position\t\t : %f\n",EL);
printf("CE Position\t\t : %f\n",CE);
printf("----------------------------\n\n");
  unsigned short *lngth;
  int i;
  lngth = (unsigned short *) &m.body[3];
  for(i=0;i<*lngth+7;i++) printf("%d ",m.body[i]);
printf("\n----------------------------------\n\n");
ParPulse(PULSEVALUE, PULSELENGTH);
#endif



  return;
};

/ **********
	   MESSAGE ID 4
	   MPF Num Status Data
	   message -->
	   operational mode
	   fine pointing controller used.
 ********** /
void M004_MPFNumStatusData(
			    message &m,
			    unsigned char &opstate,
			    unsigned char &fpc
			    ){
#ifdef MEMCHECK
mtrace();
#endif
  opstate = m.body[6];
  fpc = m.body[7];

#ifdef TESTMODE
printf("TESTMODE: MPF Num Status Data\n");
printf("-----------------------------\n");
printf("MPF Opstate \t\t: %d\n",opstate);
printf("MPF Fine PS \t\t: %d\n",fpc);
printf("-----------------------------\n\n");
  unsigned short *lngth;
  int i;
  lngth = (unsigned short *) &m.body[3];
  for(i=0;i<*lngth+7;i++) printf("%d ",m.body[i]);
printf("\n----------------------------------\n\n");
ParPulse(PULSEVALUE, PULSELENGTH);
#endif

  return; 
};

  */

/***********************************************
Third set decodes data sent do the ground by the guider 
and by the guider back to the ground. 
************************************************/



/**********
	   MESSAGES 20 and 120
	   Autocollimation TC
***********/	   

void T020_AutocollimationTC(
			    message &
			    )
{
  return;
};


void G120_AutocollimationTCReply(
				 message &m,
				 unsigned char &success
				 )
{
#ifdef MEMCHECK
mtrace();
#endif
  success = m.body[6];
  return;
};


/**********
	   MESSAGES 21 and 121
	   Slewing TC
***********/	   
void T021_SlewingTC(
		     message &m,
		     double &ra,
		     double &dec
		     )
{
#ifdef MEMCHECK
mtrace();
#endif
  pud = (double *) &m.body[6];
  ra = *pud;

  pud = (double *) &m.body[14];
  dec = *pud;

  return;

};

void G121_SlewingTCReply(
			 message &m,
			 unsigned char &success
			 )
{
#ifdef MEMCHECK
mtrace();
#endif
  success = m.body[6];
  return;
};


/**********
	   MESSAGES 22 and 122
	   Slewing TC
***********/	   
void T022_AlignmentTC(
		     message &m,
		     double &ra,
		     double &dec
		    )
{
#ifdef MEMCHECK
mtrace();
#endif
  pud = (double *) &m.body[6];
  ra = *pud;
  
  pud = (double *) &m.body[14];
  dec = *pud;
  return;

};


void G122_AlignmentTCReply(
			   message &m,
			   unsigned char &success
			   )
{
#ifdef MEMCHECK
mtrace();
#endif
  success = m.body[6];
  return;
};


/**********
	   MESSAGES 23 and 123
	   EL+CE TC
***********/	   
void T023_ELCETC(
		 message &m,
		 float &EL,
		 float &CE
		 )
{
#ifdef MEMCHECK
mtrace();
#endif
  pf = (float *) &m.body[6];
  EL = *pf;
  pf = (float *) &m.body[10];
  CE = *pf;
  return;
};



void G123_ELCETCReply(
		      message &m,
		      unsigned char &success
		      )
{
#ifdef MEMCHECK
mtrace();
#endif
  success = m.body[6];
  return;
};

/**********
	   MESSAGE 124
	   Text Error Sent to Ground
***********/	   
void G124_TextError(
		    message &
		    )
{
  return;
};


/*********
	  MESSAGE 25
	  Focus TC
********/
void T025_FocusTC(message &m, short &foc){
  ps = (short *) &m.body[6];
  foc = *ps;
  return;
};

/*********
	  MESSAGE 26
	  AutoFocus TC
	  For Completeness
**********/
void T026_AutoFocusTC(){
  return;
};


/*********
	  MESSAGE 27
	  Adjust Plate Scale TC
********/
void T027_AdjustPlateScaleTC(message &m, double &newps){
  pd = (double *) &m.body[6];
  newps = *pd;
  return;
};


/*********
	  MESSAGE 28
	  Record Alignment TC
	  For Completeness
********/

void T028_RecordAlignment(){
};




/**********
	   MESSAGES 30 and 130
	   Camera Status TC 
***********/	   
void T030_CameraStatusTC(
			 message &
			 )
{
#ifdef MEMCHECK
mtrace();
#endif
  return;
};

void G130_CameraStatusTCReply(
			      message &m,
			      unsigned short &ROIX,
			      unsigned short &ROIY,
			      unsigned short &ROIdX,
			      unsigned short &ROIdY,
			      unsigned char &binning,
			      unsigned short &expt
			      ){
#ifdef MEMCHECK
mtrace();
#endif
  pus = (unsigned short*) &m.body[6];
  ROIX = *pus;
  pus = (unsigned short*) &m.body[8];
  ROIY = *pus;
  pus = (unsigned short*) &m.body[10];
  ROIdX = *pus;
  pus = (unsigned short*) &m.body[12];
  ROIdY = *pus;
  binning = m.body[14];
  pus = (unsigned short*) &m.body[13];
  expt = *pus;
  return;
};


/**********
	   MESSAGES 31 and 131
	   Video Status TC
***********/	   
void T031_VideoStatusTC(
			message &
			)
{
  return;
}

void G131_VideoStatusTCReply(
			     message &m,
			     unsigned char &freq,
			     unsigned char &cross,
			     unsigned char &GPSt,
			     unsigned char &GPScoord,
			     unsigned char &RADEC,
			     unsigned char &contrast,
			     unsigned char &brightness,
			     unsigned char &gamma
			     )
{
#ifdef MEMCHECK
mtrace();
#endif
  freq = m.body[6];
  cross = m.body[7];
  GPSt = m.body[8];
  GPScoord = m.body[9];
  RADEC = m.body[10];
  contrast = m.body[11];
  brightness   = m.body[12];
  gamma  = m.body[13];
  return ;
};



/**********
	   MESSAGES 32 and 132
	   Camera ROI Change TC
***********/	   
void T032_CameraROIChangeTC(
			  message &m,
			  unsigned short &ROIX,
			  unsigned short &ROIY,
			  unsigned short &ROIdX,
			  unsigned short &ROIdY,
			  unsigned char &binning,
			  unsigned char &gm
			  )
{
#ifdef MEMCHECK
mtrace();
#endif
  pus = (unsigned short*) &m.body[6];
  ROIX = *pus;
  pus = (unsigned short*) &m.body[8];
  ROIY = *pus;
  pus = (unsigned short*) &m.body[10];
  ROIdX = *pus;
  pus = (unsigned short*) &m.body[12];
  ROIdY = *pus;
  binning = m.body[14];
  gm = m.body[15];
  return;
};

void G132_CameraROIChangeTCReply(
				 message &m,
				 unsigned char &success
				 )
{
  success = m.body[6];
  
  return;
};
			  
/**********
	   MESSAGES 33 and 133
	   Camera Exposure Time TC
***********/	   
void T033_CameraExposureTimeTC(
			       message &m,
			       unsigned short &expt,
			       unsigned char &gm
			       )
{
#ifdef MEMCHECK
mtrace();
#endif

  pus = (unsigned short *) &m.body[6];

  expt = *pus;

  gm = m.body[8];

  return;
};

void G133_CameraExposureTimeTCReply(
				    message &m,
				    unsigned char &success
				    )
{
  success = m.body[6];
  return;
};


/**********
	   MESSAGES 34 and 134
	   video frame period change
***********/	   
void T034_VideoFramePeriodChangeTC(
				   message &m,
				   unsigned char &freq
				   )
{
#ifdef MEMCHECK
mtrace();
#endif
  freq = m.body[6];
  return;
};

void G134_VideoFramePeriodChangeTCReply(
					message &m,
					unsigned char &success
					)
{
  success = m.body[6];
  return;
};


/**********
	   MESSAGES 35 and 135
	   Video Display Change TC
***********/	   
void T035_VideoDisplayChangeTC(
			       message &m,
			       unsigned char &param,
			       unsigned char &value
			       )
{
#ifdef MEMCHECK
mtrace();
#endif
  param = m.body[6];
  value = m.body[7];
  return;
};

void G135_VideoDisplayChangeTCReply(
				    message &m,
				    unsigned char &success
				    )
{
#ifdef MEMCHECK
mtrace();
#endif
  success = m.body[6];
  return;
};

/**********
	   MESSAGE 36
	   Force Frame Update
***********/	   
void T036_ForceFrameUpdateTC(
			     message &
			     )
{
  return;
};



/**********
	   MESSAGES 37 and 137
	   Get Frame
***********/	   
void T037_GetFrameTC(
		     message &
		     )
{
  return;
};


void G137_GetFrameTCReply(
			  message &m,
			  unsigned short &ROIdX,
			  unsigned short &ROIdY
			  // there should be more here!!!!
			  )
{
#ifdef MEMCHECK
mtrace();
#endif
  pus = (unsigned short *) &m.body[6];
  ROIdX = *pus;
  pus = (unsigned short *) &m.body[8];
  ROIdY = *pus;
  return;
};


/**********
	   MESSAGES 40 and 140
	   Status Update TC
***********/	   
void T040_UpdatePeriodTC(
			 message &m,
			 unsigned short &delay
			 )
{
#ifdef MEMCHECK
mtrace();
#endif
  pus = (unsigned short *) &m.body[6];
  delay = *pus;
  return;
};

void G040_StatusUpdate(
		       message &m,
		       unsigned long &gpst,
		       unsigned short &gpslond,
		       unsigned short &gpslonm,
		       float &gpslons,
		       unsigned char &gpslonc,
		       unsigned short &gpslatd,
		       unsigned short &gpslatm,
		       float &gpslats,
		       unsigned char &gpslatc,
		       double &GSra,
		       double &GSdec,
		       double &GRra,
		       double &GRdec,
		       unsigned char &GRvalid,
		       unsigned char &Gmode,
		       unsigned char &MPFmode,
		       unsigned char &GSensor,
		       unsigned char &MPFsensor,
		       unsigned short &expt		       
		       )
{
#ifdef MEMCHECK
mtrace();
#endif
  pul = (unsigned long *) &m.body[6]; //gpst
  gpst = *pul;

  pus = (unsigned short *) &m.body[14]; //gps lon deg
  gpslond = *pus;
  
  pus = (unsigned short *) &m.body[16]; //gps lon min
  gpslonm = *pus;

  pf = (float *) &m.body[18];           //gps lon sec
  gpslons = *pf;

  gpslonc = m.body[22];                 //gps lon dir

  pus = (unsigned short *) &m.body[23]; // gps lat deg
  gpslatd = *pus;

  pus = (unsigned short *) &m.body[25]; //gps lat min
  gpslatm = *pus;

  pf = (float *) &m.body[27]; //gps lat sec
  gpslats = *pf;

  gpslatc = m.body[31]; // gps lat dir


  pud = (double *) &m.body[32];
  GSra = *pud;

  pud = (double *) &m.body[40];
  GSdec = *pud;

  pud = (double *) &m.body[48];
  GRra = *pud;

  pud = (double *) &m.body[56];
  GRdec = *pud;
  
  GRvalid = m.body[64];
  Gmode  = m.body[65];

  //WARNING

  MPFmode = m.body[66];
  GSensor = m.body[67];
  MPFsensor = m.body[68];
  
  pus = (unsigned short *) &m.body[69];  
  expt = *pus;

  return;
}

void T050_SystemCommandTC(message m, char *cmd, unsigned char &lngth){
  int _i;
  pus = (unsigned short *) &m.body[3];
  for(_i=0;_i<*pus;_i++){
    cmd[_i] = m.body[6+_i];
  };
  m.body[6+*pus]='\0';
  lngth = *pus;
};
