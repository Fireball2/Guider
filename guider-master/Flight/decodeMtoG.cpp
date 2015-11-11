#include "decodeMtoG.h"
#include "decode.h"
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"


//#define TESTMODE


// variables defined in "decode.h" that allow us to parse the messages 
// quickly. 
extern unsigned short *pus;
extern short *ps;
extern float *pf;
extern double *pd;
extern unsigned char *puc;
extern unsigned char *puc;
extern unsigned long *pul;

/***************************
Second set decodes commands sent from MPF to guider
****************************/

/****** 
	Message ID 1
	DTU Pointing Errors Data
	message -->
	EL, CE
	type of measurement
	valid data
*******/
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
  //  ParPulse(PULSEVALUE, PULSELENGTH);
#endif
  
  
  return;
};

/*********
	  Message ID 2 
	  DTU Coordinate Data
	  message ---->
 	  DTU Attitude Data
********/	  






//This stuff needs to be worked out.

/*typedef struct AscAttitudeTM{
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
*/
  
void M002_DTUCoordinateData(message &m,
			    AscAttitude &acs){
  //  FILE *ofile;
  unsigned char *intc;
  unsigned char *shortc;
 


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


  /*
  fprintf(ofile, "SC Segmentation Flag \t: %d\n", acs.SCSegmentationFlag);
  fprintf(ofile, "SC Source Sequence Count \t: %d\n", acs.SCSourceSequenceControl);
  */
  puc = (unsigned char *) &m.body[10];
  acs.Length = 256*puc[0]+puc[1];
  //fprintf(ofile, "Message Length \t\t: %d\n", acs.Length);

  puc = (unsigned char *) &m.body[12];
  acs.DFHspare1 = ((*puc) & 128) >> 7;
  acs.DFHPUSVersion =  ( (*puc) & 112 )>>4;
  acs.DFHspare2 = ((*puc) & 15);



  acs.DFHType = m.body[13];
  acs.DFHSubtype = m.body[14];

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


  shortc = (unsigned char *) &acs.ReportID;
  puc = (unsigned char *) &m.body[22];
  
  shortc[0] = puc[1];
  shortc[1] = puc[0];

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

  acs.infoFlags  = m.body[40];
  acs.infoResidual = m.body[41];
  acs.infoLocks = m.body[42];
  acs.infoObjects = m.body[43];
  acs.infoStarsFail = m.body[44];
  acs.AGC[0] = m.body[45];
  acs.AGC[1] = m.body[46];
  acs.AGC[2] = m.body[47];

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
  //  ParPulse(PULSEVALUE, PULSELENGTH);
#endif
};



/********* 
	   Message ID 3
	   EL and CE Position Data
	   message ---->
	   EL and CE axis position
*********/
void M003_ELCEPositionData(
			   message &m,
			   float &EL,
			   float &CE
			   )
{
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
//ParPulse(PULSEVALUE, PULSELENGTH);
#endif



  return;
};

/**********
	   MESSAGE ID 4
	   MPF Num Status Data
	   message -->
	   operational mode
	   fine pointing controller used.
 **********/
void M004_MPFNumStatusData(
			    message &m,
			    unsigned char &opstate,
			    unsigned char &fpc,
			    float &acel,
			    float &acce,
			    float &alel,
			    float &alce
			    ){
  opstate = m.body[6];
  fpc = m.body[7];
  pf = (float *) &m.body[8];
  acel = *pf;
  pf = (float *) &m.body[12];
  acce = *pf;
  pf = (float *) &m.body[16];
  alel = *pf;
  pf = (float *) &m.body[20];
  alce = *pf;

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
//ParPulse(PULSEVALUE, PULSELENGTH);
#endif

  return; 
};
