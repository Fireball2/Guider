#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


#include <QMutexLocker>
#include <QMutex> 
#include <QThread>
#include "writelog.h"
#include "gps_func.h"

#include <time.h>



FILE *logfile1, *logfile2;
#define MAXENTRIES 65535
unsigned int currentlogentries;
unsigned long totalentries; 
char logstarted;
unsigned int logfileindex;
timespec ts;
double startt, nowt; // start time and current time

bool filesaving;

QMutex logmutex;



char StartLog(){

  QMutexLocker locker(&logmutex);
  char filename[100];
  int foundfile;

  logfileindex = 0;
  foundfile = 0;
  logstarted = 0;

  do{
    sprintf(filename,"%07d_Guider.log",logfileindex);
    
    if(access(filename,F_OK)){
      
      logfile1 = fopen(filename,"w");
      if(logfile1 == NULL) return 1;
      currentlogentries = 0;
      totalentries = 0;
      
      fprintf(logfile1,"Log File Created at %lf on %2d/%2d/%4d\n",gps_utc_time,gps_day,gps_month,gps_year);
      //      fflush(logfile1);
      logstarted = 1;
    } else {
      logfileindex++;
    };
    
  } while (!logstarted);

  clock_gettime(CLOCK_REALTIME, &ts);
  startt = ts.tv_sec + ts.tv_nsec/1000000000.;

  filesaving = true;

  // this opens the binary frames logfile. 
  logfile2 = fopen("frames.log","a");

  
  return 0;
};


char CloseLog(){
  QMutexLocker locker(&logmutex);
  if(filesaving){
    if(logstarted != 1) return 2;
    
    fprintf(logfile1,"Log File Closed at %lf on %2d/%2d/%4d with %d entries\n",gps_utc_time, gps_day, gps_month, gps_year,currentlogentries);
  };
  //  fflush(logfile1);
  logstarted = 0;
  fclose(logfile1);
  fclose(logfile2);
  filesaving = false;
  return 0;
}


char wtl_filename[100];
char WriteToLog(char *logentry){
  QMutexLocker locker(&logmutex);
  char *filename;
  int a;

  filename = wtl_filename;

  if(logstarted !=1) return 2;

  clock_gettime(CLOCK_REALTIME, &ts);
  nowt = ts.tv_sec + ts.tv_nsec/1000000000.;

  if(filesaving){
    a = 0;
    a+=fprintf(logfile1,"%5d::%s:: ",currentlogentries,logentry);
    a+=fprintf(logfile1,"%lf, %lf\n",gps_utc_time,nowt-startt);
    
    fflush(logfile1);
  };
  //  printf("Got Here %d\t%d\n",currentlogentries,a);    
  if(a==0) return 1;

  totalentries++;
  



  //  printf("%d .. ",currentlogentries);
  if(currentlogentries == MAXENTRIES){
    
    ReopenLog();
    
  } else {
    currentlogentries++;    
  };

  //  fflush(logfile1);
  
  //  return 0;
};

void ReopenLog(){
  char filename[100];
    /* close the log */
  if (filesaving){
    fprintf(logfile1,"Log File Closed at %lf on %2d/%2d/%4d with %d entries\n",gps_utc_time, gps_day, gps_month, gps_year,currentlogentries);
    logstarted = 0;
  };
    fclose(logfile1);
    
    /* start a new one */
    
    sprintf(filename,"%07d_Guider.log",++logfileindex);
    logfile1 = fopen(filename,"w");
    if(logfile1 == NULL) return;
    
    fprintf(logfile1,"Log File Created at %lf on %2d/%2d/%4d\n",gps_utc_time,gps_day,gps_month,gps_year);
    logstarted = 1;
    currentlogentries=0;  

};


char WriteToLog(char *function, char *format, ...){
  QMutexLocker locker(&logmutex);
  
  
    fprintf(logfile1,"%5d::[%s]",currentlogentries,function);
    int j=0;
    va_list ap;
    va_start(ap,format);
    clock_gettime(CLOCK_REALTIME, &ts);
    nowt = ts.tv_sec + ts.tv_nsec/1000000000.;
    j = vfprintf(logfile1, format, ap);
    va_end(ap);
    j+= fprintf(logfile1,"::%lf::%lf\n",gps_utc_time,nowt-startt);
    
    totalentries++;
    
    
    if(currentlogentries == MAXENTRIES){
      ReopenLog();
    } else {
      currentlogentries++;
    };
    
    fflush(logfile1);
  
  return 0;
};

char FrameToLog(unsigned int framenumber, unsigned short width, unsigned short height, unsigned char *buffer){
  QMutexLocker locker(&logmutex);
  if (filesaving){
    FILE *ofile;
    char fname[255];
    int j;
    clock_gettime(CLOCK_REALTIME, &ts);
    sprintf(fname,"frames/AFrame%06d.frm",framenumber);
    ofile = fopen(fname,"w");
    nowt = ts.tv_sec + ts.tv_nsec/1000000000.;
    j=0;
    j=fprintf(logfile1,"%5d::[%d][%d][%d]\n",currentlogentries,framenumber,width,height);
    //  j+= fwrite(buffer,1, height*width, logfile1);
    //  j+= fprintf(logfile1, "\n::End::[%d]\n",framenumber);
    fwrite(buffer,1, height*width, ofile);
    fclose(ofile);
    totalentries++;
    if(currentlogentries == MAXENTRIES){
      ReopenLog();
    } else {
      currentlogentries++;
    };
  };
  return 0;
};

char ExtraFrameToLog(unsigned int framenumber, unsigned short x, unsigned short y,unsigned short width, unsigned short height, float bx, float by,unsigned int expt, unsigned char *buffer){
  QMutexLocker locker(&logmutex);
  if (filesaving) { 
    FILE *ofile;
    char fname[255];
    int j;
    clock_gettime(CLOCK_REALTIME, &ts);
    sprintf(fname,"frames/AFrame%06d.frm",framenumber);
    ofile = fopen(fname,"w");
    nowt = ts.tv_sec + ts.tv_nsec/1000000000.;
    j=0;
    j=fprintf(logfile1,"%5d::[%d][%d][%d][%d][%d][%d][%f][%f][%f]\n",currentlogentries,framenumber,x,y,width,height,expt,bx,by,nowt-startt);
    //  j+= fwrite(buffer,1, height*width, logfile1);
    //  j+= fprintf(logfile1, "\n::End::[%d]\n",framenumber);
    fwrite(buffer,1, height*width, ofile);
    fclose(ofile);
    totalentries++;
    if(currentlogentries == MAXENTRIES){
      ReopenLog();
    } else {
      currentlogentries++;
    };
  }
  return 0;
};



void InfoToLog(QCamera *qcam, frameblob *fb, unsigned long fn, unsigned short tx, unsigned short ty, float cx, float cy){
  static imageInfo imageinfo;
  static bloblist *blob;
  static int i;


    QMutexLocker locker(&logmutex);
    if(filesaving){
      // let's get the current clock time
      clock_gettime(CLOCK_REALTIME, &ts);
      imageinfo.cpuTime = ts.tv_sec + ts.tv_nsec/1000000000.;

      // let's get the gps time
      imageinfo.gpsTime = gps_utc_time;

      // what is the target point?
      imageinfo.targetX = tx;
      imageinfo.targetY = ty;

      // what is the framenumber?
      imageinfo.framenumber = fn;

      // other frame parameters?
      imageinfo.expT = qcam->getExpT();
      imageinfo.trigT = qcam->getTrigT();
      imageinfo.wx = qcam->getRoiX();
      imageinfo.wy = qcam->getRoiY();
      imageinfo.wdx = qcam->getRoiDX();
      imageinfo.wdy = qcam->getRoiDY();

      // do we have blobs or do we not have blobs. if cx=0 and cy=-1 then we
      // were looking at a small image and there is no blob.
      // if cx = -1 and cy = -1, then we are looking at a full frame,
      // and need to populate with blobs instead. 

      if (cx == -1 and cy == -1) {
	// full chip mode, look for blobs.
	i = 0;
	blob = fb->getblobs();

	while (blob != NULL & i < 10){
	  imageinfo.bx[i] = blob->getx();
	  imageinfo.by[i] = blob->gety();
	  i++;
	  blob = blob->getnextblob();
	};

	imageinfo.blobs = i;
      

      } else if (cx ==0 and cy == -1) {
	imageinfo.blobs = 0;



      } else if (cx >= 0 and cy >= 0){ 
	imageinfo.blobs = 1;
	imageinfo.bx[0] = cx;
	imageinfo.by[0] = cy;

      };


      fwrite((char *) &imageinfo, sizeof(imageinfo),1,logfile2);
      
      
    };


};


char FrameInfoToLog(unsigned int framenumber, unsigned short x, unsigned short y, unsigned short dx, unsigned short dy, unsigned short tx, unsigned short ty, unsigned char tt, unsigned int et, frameblob *fb){
  static char longstr[500];
  static float blobx[10],bloby[10], blobf[10];
  static char currentblob;
  unsigned char blobs;
  bloblist *pblob;
  int i;

  currentblob = 0; 
  for(i=0;i<10;i++){
    blobx[i] = 0;
    bloby[i] = 0;
    blobf[i] = 0;
  };

  blobs = 0;
  pblob = fb->getblobs();
  
  while(pblob!=NULL && currentblob<10){
    blobx[currentblob]=  floor(pblob->getx()+0.5);
    bloby[currentblob]=  floor(pblob->gety()+0.5);
    blobf[currentblob] = pblob->getflux();
    pblob = pblob->getnextblob();
    currentblob++;
  };



  /*WriteToLog("FRAME","%ld, %d, %d, %d, %d, %d, %d, %d, %d, %d, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f",framenumber, x,y,dx,dy, tx, ty, tt, ty, currentblob, \
	     blobx[0],bloby[0],bloby[0],\
	     blobx[1],bloby[1],bloby[1],\
	     blobx[2],bloby[2],bloby[2],\
	     blobx[3],bloby[3],bloby[3],\
	     blobx[4],bloby[4],bloby[4],\
	     blobx[5],bloby[5],bloby[5],\
	     blobx[6],bloby[6],bloby[6],\
	     blobx[7],bloby[7],bloby[7],\
	     blobx[8],bloby[8],bloby[8],\
	     blobx[9],bloby[9],bloby[9]);
  */
  return 0;

};



void FlushLog(){
  fflush(logfile1);
  fflush(logfile2);
};

