#include <QMutex>
#include <QMutexLocker>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "GuiderToMPF.h"
#include "messages.h"
#include "writelog.h"
#include "comsetup.h"
//#include "globals.h"

#include <mcheck.h>
#define MEMCHECK


/******************** to speed up the functions, here are some pointers to use */


QMutex GtoM_mutex;
unsigned short  GtoM_CountVar;

 char GtoM_GuiderPointingErrorsData(
				    float EL,
				    float CE,
				    unsigned char validELCE,
				    double ra,
				    double dec,
				    unsigned char validRADEC
				    ){
static  unsigned short *pUS;
static  float *pF;
 
static  double *pd; 
 
static  int err;
 


   
static  char logentry[128];
static unsigned short int cnt;   
static  message m;

   err = 0;
   
   
    m.length = 34; 

    
   if (err > 0) return 1;
   
   m.body[0] = 0x55;

   // needs a lock here
   
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt;
   
   
   pUS = (unsigned short *) &m.body[3];
   *pUS = 27;

   m.body[5] = 1;

   pF = (float *) &m.body[6];
   *pF = EL;
   pF = (float *) &m.body[10];
   *pF = CE;

   m.body[14] = validELCE;

   pd = (double *) &m.body[15];
   *pd = ra;

   pd = (double *) &m.body[23];
   *pd = dec;

   m.body[31] = validRADEC;


   m.body[32] = m.FindCRC();
   m.body[33]=0xAA;
   
   // 
  
   if(SendToMPFNum(m)) return 2;
   
   
   sprintf(logentry,"GuiderPED:%d,%.2f,%.2f,%1d,%.5lf,%.5lf,%d",
	   m.body[1],
	   EL,CE,validELCE,
	   ra,dec,validRADEC);

   WriteToLog(logentry); 
   

   return 0;
};



 char GtoM_AutocollimationRequest(
				  ){


static unsigned short *pUS;

static int err;

   
static   message m;
static   char logentry[128];
static unsigned short int cnt;

   err = 0;
   m.length = 8;
   m.body[0]=0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 1;
   
   m.body[5] = 2;
   
   m.body[6] = m.FindCRC();
   m.body[7] = 0xAA;
   
   if(SendToMPFNum(m)) return 2;

   sprintf(logentry,"GtoM,S,2,AutoCollimationRequest");
   
   WriteToLog(logentry);
   
   
   return 0;
 };


 char GtoM_SwitchingGuiderRequest(
				  ){

static unsigned short *pUS;

static int err;


static   message m;
static   char logentry[128];
static unsigned short  int cnt;
   err = 0;
   m.length = 8;
   m.body[0]=0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 1;
   
   m.body[5] = 3;
   
   m.body[6] = m.FindCRC();
   m.body[7] = 0xAA;
   
   if(SendToMPFNum(m)) return 2;

   sprintf(logentry,"GtoM,S,3,SwitchingGuiderRequest");
   
   WriteToLog(logentry);
   
   
   return 0;
 };


 char GtoM_SwitchingDTURequest(
				  ){
static unsigned short *pUS;
static int err;
static unsigned short    int cnt;   
static   message m;
static  char logentry[128];
   err = 0 ;

   m.length = 8;
   m.body[0]=0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 1;
   
   m.body[5] = 4;
   
   m.body[6] = m.FindCRC();
   m.body[7] = 0xAA;
   
   if(SendToMPFNum(m)) return 2;

   sprintf(logentry,"GtoM,S,4,SwitchingDTURequest");
   
   WriteToLog(logentry);
   
   
   return 0;
 };


 char GtoM_TrackingRequest(
			   double ra,
			   double dec,
			   unsigned char config
			   ){
static unsigned short *pUS;
static  double *pd;
static int err;
static  unsigned short int cnt;   
static   message m;
static    char logentry[128];
   err = 0 ;

   if(config > 2) err++;
   
   if(err >0) return 1;

   m.length = 25; 

   m.body[0] = 0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 18;
   
   m.body[5] = 5;
   
   pd = (double *) &m.body[6];
   *pd = ra;
   
   pd = (double *) &m.body[14];
   *pd = dec;

   m.body[22] = config;
   
   m.body[23] = m.FindCRC();
   

   m.body[24] = 0xAA;
   

   if(SendToMPFNum(m)) return 2;

   sprintf(logentry,"GtoM:TrackingRequest:%.5f,%.5f,%d",ra,dec,config);
   
   WriteToLog(logentry);

   
   return 0;
 };



 char GtoM_SlewingRequest(
			  double ra,
			  double dec
			   ){
static  unsigned short *pUS;
static  double *pd;
 
static int err;
static unsigned short cnt;   
static    message m;
static   char logentry[128];
   err = 0;

   
   if(err >0) return 1;

   m.length = 24; 

   m.body[0] = 0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 17;
   
   m.body[5] = 6;
   
   pd = (double *) &m.body[6];
   *pd = ra;
 

   pd = (double *) &m.body[14];
   *pd = dec;


   m.body[22] = m.FindCRC();
   
   m.body[23] = 0xAA;
   

   if(SendToMPFNum(m)) return 2;
   
   sprintf(logentry,"GtoM:SlewingRequest:%.5lf,%.5lf",ra,dec);
   
   WriteToLog(logentry);
   
   return 0;
 };




char GtoM_ELCEOffsetsRequest(
			    float EL,
			    float CE
			    ){
#ifdef MEMCHECK
mtrace();
#endif
static unsigned short *pUS;
static float *pF;
static   char elceor_lgntr[128];
static   message elceor_m;
static   char *logentry;
static unsigned short int cnt;
  logentry = elceor_lgntr;
  
  elceor_m.length=16;
  
  elceor_m.body[0] = 0x55;

  cnt = GtoM_Count();
  pUS = (unsigned short *) &elceor_m.body[1];
  *pUS = cnt;   
  
  pUS = (unsigned short *) &elceor_m.body[3];
  *pUS = 9;
  
  elceor_m.body[5] = 7;
  
  pF = (float *) &elceor_m.body[6];
  *pF = EL;

  pF = (float *) &elceor_m.body[10];
  *pF = CE;
  
  elceor_m.body[14] = elceor_m.FindCRC();
  elceor_m.body[15] = 0xAA;
  
  if(SendToMPFNum(elceor_m)) return 2;
  
    sprintf(logentry,"GtoM,S,7,%.2f,%.2f",EL,CE);
    WriteToLog(logentry);
  
  return 0; 
};

char logentr[128];

char GtoM_GuiderStatusData(
			   unsigned char OpState,
			   unsigned char FPController
			   ){
static unsigned short *pUS;
static int err;
static unsigned short int cnt;
static message m_gsd; 

  err=0;

  m_gsd.length = 10;
  
  m_gsd.body[0] = 0x55;
  
  cnt = GtoM_Count();
  pUS = (unsigned short *) &m_gsd.body[1];
  *pUS = cnt;   
  
  pUS = (unsigned short *) &m_gsd.body[3];
  *pUS = 3;
  
  m_gsd.body[5] = 8;
  
  m_gsd.body[6] = OpState;
  m_gsd.body[7] = FPController;
  
  m_gsd.body[8] = m_gsd.FindCRC();
  m_gsd.body[9] = 0xAA;
  
  if(SendToMPFNum(m_gsd)) return 2;
  
  sprintf(logentr,"GtoM,S,8,%d,%d",OpState, FPController);
  WriteToLog(logentr);
  return 0;
};


char GtoM_CenterROIRequest(
){

static unsigned short *pUS;
static int err;
static   message m;
static   char logentry[128];
  err=0;
static unsigned short int cnt;
  
  m.length = 8;
  m.body[0]=0x55;
  cnt = GtoM_Count();
  pUS = (unsigned short *) &m.body[1];
  *pUS = cnt; 
  pUS = (unsigned short *) &m.body[3]; 
  *pUS = 1;
  
  m.body[5] = 9;
  
  m.body[6] = m.FindCRC();
  m.body[7] = 0xAA;
  
  if(SendToMPFNum(m)) return 2;
  
  sprintf(logentry,"GtoM,S,9,CenterROIRequest");
  
  WriteToLog(logentry);
  
  
  return 0;
  
};		  



 char GtoM_AutoCollimationTC(
				  ){
static unsigned short *pUS;

static int err;

   
static    message m;
static    char logentry[128];
   err=0;
static unsigned short int cnt;

   m.length = 8;
   m.body[0]=0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 1;
   
   m.body[5] = 100;
   
   m.body[6] = m.FindCRC();
   m.body[7] = 0xAA;
   
   if(SendToMPFNum(m)) return 2;

   sprintf(logentry,"GtoM,S,100,AutoCollimationTC");
   
   WriteToLog(logentry);
   
   
   return 0;
 };



 char GtoM_SlewingTC(
		     double ra,
		     double dec
		     ){
   
#ifdef MEMCHECK
   mtrace();
#endif
   unsigned short *pUS;
   double *pd;

int err;


   int cnt;   
   message m;
   char logentry[128];
   err = 0;
   
   
   if(err >0) return 1;

   m.length = 24; 

   m.body[0] = 0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 17;
   
   m.body[5] = 101;
   
   pd = (double *) &m.body[6];
   *pd = ra;

   pd = (double *) &m.body[14];
   *pd = dec;

   m.body[22] = m.FindCRC();
   
   m.body[23] = 0xAA;
   

   if(SendToMPFNum(m)) return 2;
   
   sprintf(logentry,"GtoM:SlewingTC,%.5lf,%.5lf",ra,dec);
   
   WriteToLog(logentry);
   
   return 0;
 };


 char GtoM_AlignmentTC(
		       double ra,
		       double dec
		       ){
#ifdef MEMCHECK
mtrace();
#endif
unsigned short *pUS;

 double *pd; 


int err;
int cnt;   
   message m;
   char logentry[128];
   err = 0 ;
   
   //   if(err >0) return 1;

   m.length = 24; 
   
   m.body[0] = 0x55;
   cnt = GtoM_Count();
   pUS = (unsigned short *) &m.body[1];
   *pUS = cnt; 
   pUS = (unsigned short *) &m.body[3]; 
   *pUS = 17;
   
   m.body[5] = 102;
   
   pd = (double *) &m.body[6];
   *pd = ra;

   pd = (double *) &m.body[14];
   *pd = dec;
   
   m.body[22] = m.FindCRC();
   
   m.body[23] = 0xAA;
   
   
   if(SendToMPFNum(m)) return 2;
   
   sprintf(logentry,"GtoM:AlignmentTC,%.5lf,%.5lf",ra,dec);
   
   WriteToLog(logentry);
   
   return 0;
 };




char GtoM_ELCEOffsetsTC(
			    float EL,
			    float CE
			    ){
#ifdef MEMCHECK
mtrace();
#endif
unsigned short *pUS;
float *pF;

int err;


  char logentry[128];
  message m;
  err=0;
  int cnt;
  
  m.length=16;

  m.body[0] = 0x55;
  cnt = GtoM_Count();
  pUS = (unsigned short *) &m.body[1];
  *pUS = cnt;   
  
  pUS = (unsigned short *) &m.body[3];
  *pUS = 9;
  
  m.body[5] = 103;
  
  pF = (float *) &m.body[6];
  *pF = EL;

  pF = (float *) &m.body[10];
  *pF = CE;
  
  m.body[14] = m.FindCRC();
  m.body[15] = 0xAA;
  
  if(SendToMPFNum(m)) return 2;
  sprintf(logentry,"GtoM,S,103,%.2f,%.2f",EL,CE);  
  WriteToLog(logentry);
  
  return 0;
};


char GtoM_StartCount(){
#ifdef MEMCHECK
mtrace();
#endif
  char err;
  err=0;
  QMutexLocker locker(&GtoM_mutex);
  GtoM_CountVar = 0;
  WriteToLog("GtoM Message Count Started.\0");


  return 0;
};


unsigned short cnt_ret;
unsigned short GtoM_Count(){
#ifdef MEMCHECK
mtrace();
#endif
  QMutexLocker locker(&GtoM_mutex);
  cnt_ret = GtoM_CountVar;
  
  if(GtoM_CountVar != GTOMMAXCOUNT-1){
    GtoM_CountVar++;
  } else {
    GtoM_CountVar = 0;
    WriteToLog("GtoM Message: Count Reset to 0.\0");
  };
  

  
  return cnt_ret;
};
