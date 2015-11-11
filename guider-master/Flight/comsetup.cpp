//#define NOMPFMESSAGES 


#define NOINTERRUPT

#include <QMutexLocker>
#include <QMutex>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/io.h>
#include <time.h>
#include <QQueue>


#include "messages.h"
#include "messagestack.h"
#include "writelog.h"
#include "GuiderToTMTC.h"
#include "GuiderToMPF.h"
//#include "GuiderToDet.h"
#include "comsetup.h"
#include "gps_func.h"
//#include "globals.h"

#include "ringbuffer.h"
#include "portbuffer.h"



char rb;
bool datatompf;

  
// define the communication baud rates for the 
// various channels
#define GPSBAUDRATE B4800
#define MPFBAUDRATE B115200
#define TMTCBAUDRATE B9600
#define DETBAUDRATE B115200
#define LYNXBAUDRATE B9600

// define the files/ports that the various
// channels are on
#define MPFDEVICE "/dev/ttyS5"
#define TMTCDEVICE "/dev/ttyS0"
#define LYNXDEVICE "/dev/ttyS1"
#define DETDEVICE "/dev/ttyS2"
#define GPSDEVICE "/dev/ttyS6"

#define WRITEDELAY 100


#define BUFSIZE 255


// max function macro
#define MAX(a,b) (a>b ? a: b)

volatile int STOP=FALSE;

int maxfd;


unsigned short MessageLength[255];
unsigned short TMTCLength[255];


bool MPFFlag, TMTCFlag;
bool isMPFOpen;

int fd_mpf;
int fd_tmtc;
int fd_det;
int fd_stage;
int fd_gps;

// locks to make sure we don't try to write to each port
// from different places.
QMutex MPFNUM_mutex;
QMutex TMTC_mutex;
QMutex DET_mutex;
QMutex STAGE_mutex;

QMutex msgreceived_mutex;

unsigned char buf[255];

message msgtmp;

// procedure to be called when signal is received. 
void serial_signal_IO (int status);
//void serial_signal_IO_t (int status);
int wait_flag=TRUE;

// message receive data set. it registers on which 
// file the data was received.
fd_set readfs;

int ComSetup(){


// configuration structures for the MPF, TMTC, DET, LYNX and GPS. 
  struct termios oldtioMPF, newtioMPF;
  struct termios oldtioTMTC, newtioTMTC;
  struct termios oldtioDET, newtioDET;
  struct termios oldtioLYNX, newtioLYNX;
  struct termios oldtioGPS, newtioGPS;

  
  //  struct sigaction saio; /* defining signal action structure */
  
  // opening communication with the MPF
  fd_mpf = open(MPFDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
  if(fd_mpf < 0){
    WriteToLog("Couldn't Open MPF Serial Device.\0");
    exit(-1);
  };

	WriteToLog("MPF Port open.");

  // opening communication with the Ground
  //  fd_tmtc = open(TMTCDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
  fd_tmtc = open(TMTCDEVICE, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if(fd_tmtc < 0){
    WriteToLog("Couldn't Open TMTC Serial Device.\0");
    exit(-1);
  };

  WriteToLog("TMTC Port open.");

  // opening communication with the Detector
  fd_det = open(DETDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
  if(fd_det < 0){
	WriteToLog("Couldn't Open Deterctor Serial Device.\0");
	exit(-1);
  };

  WriteToLog("Detector Port open.");

  // opening communication with the motor controller
  fd_stage = open(LYNXDEVICE, O_RDWR | O_NONBLOCK | O_NDELAY);
  //  fd_stage = open(LYNXDEVICE, O_RDWR);
  if(fd_stage < 0){
	WriteToLog("Couldn't Open Stage Serial Device.\0");
	exit(-1);
  };

  WriteToLog("Motor Port open.");


  // Opening communication with the GPS port.
  fd_gps = open("/dev/ttyS6", O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);

  if(fd_gps<0){
    WriteToLog("GPS: Couldn't start serial port connection.\0");
    exit(-1);
  };
  
  WriteToLog("GPS: GPS Port Initialized.\0");

  /*
#ifndef NOINTERRUPT
  WriteToLog("INTERRUPT HANDLER INSTALLED");
  // install the signal handler before making the signal asynchronous 
  saio.sa_handler = serial_signal_IO;
  sigemptyset(&saio.sa_mask);
  saio.sa_flags = SA_NOMASK;
  saio.sa_restorer = NULL;
  sigaction(SIGIO,&saio,NULL);
  
#ifndef IGNOREMPF
  fcntl(fd_mpf, F_SETFL, FASYNC);	
  fcntl(fd_mpf, F_SETOWN, getpid());
  
#endif


  fcntl(fd_tmtc, F_SETFL, FASYNC);  
  fcntl(fd_tmtc, F_SETOWN, getpid());
  //   fcntl(fd_gps, F_SETOWN, getpid());
  
  
  //  fcntl(fd_gps, F_SETFL, FASYNC);
  
#endif //NOINTERRUPT
  */
  tcgetattr(fd_mpf, &oldtioMPF);
  tcgetattr(fd_tmtc, &oldtioTMTC);
  tcgetattr(fd_gps, &oldtioGPS);
  tcgetattr(fd_det, &oldtioDET);
  tcgetattr(fd_stage, &oldtioLYNX);

  //configure the MPF Port
  newtioMPF.c_cflag = MPFBAUDRATE  | CS8 | CLOCAL |  CREAD;
  newtioMPF.c_iflag = IGNPAR ;
  newtioMPF.c_oflag = 0;
  newtioMPF.c_lflag = 0;
  newtioMPF.c_cc[VMIN] = 0;
  newtioMPF.c_cc[VTIME] = 0;
  newtioMPF.c_cc[VTIME] = 0;
  tcflush(fd_mpf, TCIFLUSH);
  tcsetattr(fd_mpf, TCSANOW, &newtioMPF);

  // configure the TMTC port
  newtioTMTC.c_cflag = TMTCBAUDRATE | CS8 | CLOCAL | CREAD;
  newtioTMTC.c_iflag = IGNPAR;
  newtioTMTC.c_oflag = 0;
  newtioTMTC.c_lflag = 0;
  //  newtioTMTC.c_cc[VSTART]   = 0x55;     /* Ctrl-q */ 
  //  newtioTMTC.c_cc[VSTOP]   = 0xAA;     /* Ctrl-q */ 
  newtioTMTC.c_cc[VMIN] = 0;
  newtioTMTC.c_cc[VTIME] = 0;
  newtioTMTC.c_cc[VTIME] = 0;
  tcflush(fd_tmtc, TCIFLUSH);
  tcsetattr(fd_tmtc, TCSANOW, &newtioTMTC);


  
  // MIGHT WANT TO HAVE A GPS_STARTED VARIABLE HERE.
  
  // Configure GPS port. Read the Serial communication under Linux for 
  // a more detailed explanation.

  newtioGPS.c_cflag = GPSBAUDRATE | CS8 | CLOCAL | CREAD;
  //newtioGPS.c_iflag = IGNPAR | ICRNL;
  newtioGPS.c_iflag = IGNPAR;
  newtioGPS.c_oflag = 0;
  //  newtioGPS.c_lflag = ICANON;
  newtioGPS.c_lflag = 0;
  
  //  newtioGPS.c_cc[VMIN]= 0;
  //newtioGPS.c_cc[VTIME]= 5;
  // newtioGPS.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
  // newtioGPS.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
  //  newtioGPS.c_cc[VERASE]   = 0;     /* del */
  // newtioGPS.c_cc[VKILL]    = 0;     /* @ */
  // newtioGPS.c_cc[VEOF]     = 4;     /* Ctrl-d */
  // newtioGPS.c_cc[VTIME]    = 0;     /* inter-character timer unused */
  // newtioGPS.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
  // newtioGPS.c_cc[VSWTC]    = 0;     /* '\0' */
  // newtioGPS.c_cc[VSTART]   = '$';     /* Ctrl-q */ 
  // newtioGPS.c_cc[VSTOP]    = 0;     /* Ctrl-s */
  // newtioGPS.c_cc[VSUSP]    = 0;     /* Ctrl-z */
  // newtioGPS.c_cc[VEOL]     = 0;     /* '\0' */
  // newtioGPS.c_cc[VREPRINT] = 0;     /* Ctrl-r */
  // newtioGPS.c_cc[VDISCARD] = 0;     /* Ctrl-u */
  // newtioGPS.c_cc[VWERASE]  = 0;     /* Ctrl-w */
  // newtioGPS.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
  // newtioGPS.c_cc[VEOL2]    = 0;     /* '\0' */
   tcflush(fd_gps, TCIFLUSH);
  tcsetattr(fd_gps,TCSANOW,&newtioGPS);
  /*******************************/

  

  


// configure the motor port
  newtioLYNX.c_cflag = LYNXBAUDRATE | CS8 | CLOCAL | CREAD;
  newtioLYNX.c_iflag = IGNPAR ;
  newtioLYNX.c_oflag = 0;
  newtioLYNX.c_lflag = 0;
  newtioLYNX.c_cc[VMIN] = 0;
  newtioLYNX.c_cc[VTIME] = 0;
  tcflush(fd_stage, TCIFLUSH);
  tcsetattr(fd_stage, TCSANOW, &newtioLYNX);

// configure the detector port
  newtioDET.c_cflag = DETBAUDRATE | CS8 | CLOCAL | CREAD;
  newtioDET.c_iflag = IGNPAR;
  newtioDET.c_oflag = 0;
  newtioDET.c_lflag = 0;
  newtioDET.c_cc[VMIN] = 1;
  newtioDET.c_cc[VTIME] = 10;
  tcflush(fd_det, TCIFLUSH);
  tcsetattr(fd_det, TCSANOW, &newtioDET);



  
	// the TMTC and MPF ports will receive asynchronously
  // this configures that.
  maxfd = MAX(fd_mpf, fd_tmtc)+1;
  //  maxfd = MAX(fd_mpf, fd_gps)+1;
  FD_ZERO(&readfs);
  FD_SET(fd_tmtc, &readfs);
#ifndef IGNOREMPF
  FD_SET(fd_mpf, &readfs);
  isMPFOpen = true;
#endif //IGNOREMPF
  //  FD_SET(fd_gps, &readfs);
  
  //  datatompf = true;
  SetMessageLengths();
  return 0;
};



unsigned char mpf_strings;

unsigned char tmtc_start[100];
unsigned short tmtc_length[100];
unsigned short *tmtclength;
unsigned char tmtc_strings;

void serial_signal_IO(int){
// don't want the port read simultaneously from different places
//  QMutexLocker locker(&msgreceived_mutex);
  //int i,j;
  WriteToLog("CaughtInterrupt");
  
// figure out which port triggered the event.
  select(maxfd, &readfs, NULL, NULL, NULL);  

  // if message is from TMTC:
  if(FD_ISSET(fd_tmtc,&readfs)){
    TMTCFlag = true;
  };
  
#ifndef IGNOREMPF
  if(FD_ISSET(fd_mpf,&readfs)){
    MPFFlag = true;
  };
#endif //IGNOREMPF
  
//reset listening to the ports. 
    FD_SET(fd_tmtc, &readfs);
#ifndef IGNOREMPF
    FD_SET(fd_mpf, &readfs);
#endif //IGNOREMPF

};


  int gpslen;
  char gpsbuf[255];
  char gpsmsg[255];
  unsigned short gpstest;
  int gpsi;


QMutex gpslock;
void GPSReadOld(){

  QMutexLocker locker(&gpslock);
  gpslen = read(fd_gps,gpsbuf,255);
  if(gpslen>1 && gpsbuf[0]=='$'){
    
    gpstest = parse_nmea(gpsbuf);
    if(gpstest){
      sprintf(gpsmsg,"%s",gpsbuf);
      for(gpsi=0;gpsi<gpslen;gpsi++){
	gpsmsg[gpsi]=gpsbuf[gpsi];
      };
      gpsmsg[gpslen-1]='\0';
      WriteToLog(gpsmsg);
    } else {
      WriteToLog("GPS Parse Failed.\0");
    };
  };
  return;
  
};

void GPSRead(){
  static int readlen;
  static char gpsbuff[255];
  static PortBuffer gpsPB;
  static int result,i;
  static int start, end;
  static int gpstest;
  //  char buff[4096];
  gpstest = 0;
  
  readlen = read(fd_gps, gpsbuff, 255);
  if(readlen>0) result = gpsPB.ring.push(gpsbuff, readlen);
  


  //  int j;
  start = end = -1;
  if(!gpsPB.ring.isEmpty()){

    for(i=0;i<gpsPB.ring.used; i++){
      if(gpsPB.ring[i]=='$' && start==-1){
	start = i;
      };
      
      if(start>-1 && gpsPB.ring[i]=='\n' && end==-1){
	end=i;
      };
    };

    if(start>-1 && end>-1){ //i.e. if we found a message.
      gpsPB.ring.strip(start);
      end = end-start;
      start = 0;
      gpsPB.ring.pop(end,gpsbuff);
      gpstest = parse_nmea(gpsbuff);		     
      WriteToLog("GPS","(LT,LN,AL)=(%f, %f, %f), UTC=%f",gps_latitude, gps_longitude, gps_altitude,gps_utc_time);
    };
    /*    
    if(gpstest){
      gpslen = end;
      for(gpsi=0;gpsi<gpslen;gpsi++){
	gpsmsg[gpsi]=gpsbuff[gpsi];
      };
      gpsmsg[gpslen-1]='\0';
      WriteToLog(gpsmsg);
    } else {
      //  WriteToLog("GPS Parse Failed.");
    };
    */
  };
  return;
};  

int SendToMPFNum(message m){
	static unsigned int mpf_i;
	//	static int i;
	//	static int j;
  QMutexLocker locker(&MPFNUM_mutex);
  
  if(m.IsValid()){
#ifndef NOMPFMESSAGES
    mpf_i = write(fd_mpf, m.body, m.length);
#endif
  };
  //delay loop between successive writes.
  //  for(i=0;i<1000;i++)
  //	  for(j=0;j<WRITEDELAY;j++);
  return 0;

};

int SendToTMTC(message m){

  //  QMutexLocker locker(&TMTC_mutex);
  if(write(fd_tmtc, m.body, m.length) != m.length) return 1;

  return 0;
};

int SendToDet(message m){

  QMutexLocker locker(&DET_mutex);
  if(write(fd_det, m.body, m.length) != m.length) return 1;
  //delay loop between successive writes.
  return 0;
};

int SendToDet(unsigned char *ptr, unsigned int lngth){

  QMutexLocker lockar(&DET_mutex);
  if((unsigned int) write(fd_det, ptr, lngth) != lngth) return 1;
  return 0;
};


int SendToStage(const char *m, short lng){
  static int i,j;
	QMutexLocker locker(&STAGE_mutex);
	if(write(fd_stage, m, lng) != lng) return 1;
	  for(i=0;i<1000;i++)
		  for(j=0;j<WRITEDELAY;j++); 
	return 0;
};




void ComClose(){

  close(fd_tmtc);
#ifndef IGNOREMPF
  close(fd_mpf);
#endif
  close(fd_det);
  close(fd_gps);
  close(fd_stage);
};


void MPFClose(){
	if(isMPFOpen){
		close(fd_mpf);
	    isMPFOpen = false;
	     WriteToLog("MPFClosed");
	};
};

void MPFOpen(){
  return;
};


void ReadTMTC(){
  QMutexLocker locker(&TMTC_mutex);
  static int readlen;
  static PortBuffer tmtcPB;
  static char buff[255];
  static unsigned short mlen;
  static int i,j,k;
  message msg;
  
  readlen = read(fd_tmtc,buff,255);
  
  if(readlen > 0){
    // first we push the message into the ring buffer
    //    for (int ii=0;ii<readlen;ii++) fprintf(stderr,"%x ",buff[ii]);
    //    fprintf(stderr,"\n");
    tmtcPB.ring.push(buff, readlen);
    //    WriteToLog("comsetup","TMTC Message received. Ring used = %d %d %u %u %u %u %u %u %u %u %u",tmtcPB.ring.used,tmtcPB.ring.Start(), tmtcPB.ring.End(),tmtcPB.ring[0],  tmtcPB.ring[1],  tmtcPB.ring[2],  tmtcPB.ring[3],  tmtcPB.ring[4],  tmtcPB.ring[5],  tmtcPB.ring[6],  tmtcPB.ring[7],  tmtcPB.ring[8]);
    
  };


 restart:

  j=-1;k=-1;
  if(!tmtcPB.ring.isEmpty()){
    for(i=0;i<tmtcPB.ring.used;i++){
      if(tmtcPB.ring[i]==0x55){ // found possible starting point. 
	tmtcPB.ring.strip(i); i=0;
	if(tmtcPB.ring.used > 5){
	  mlen = (unsigned char) tmtcPB.ring[3]+(unsigned char)tmtcPB.ring[4]*256;
	  if(mlen!=TMTCLength[(unsigned char)tmtcPB.ring[5]]){
	    tmtcPB.ring.strip(1);
	    goto restart;
	  };
	  
	  if(mlen + 7 > tmtcPB.ring.used) goto exitloops; // if there is not enough room to
	  // reconstruct the message, exit. 
	  
	  //	WriteToLog("pffft","%d %d %d",mlen,TMTCLength[(unsigned char)tmtcPB.ring[5]],tmtcPB.ring[5]);

	 
	if(tmtcPB.ring[6+mlen]==-86){
	  if(IsValid(tmtcPB.ring,7+mlen)){
	    tmtcPB.ring.pop(7+mlen,(char *) msg.body);
	    AddToMessageQueue(msg);
	    //	    WriteToLog("ooops");
	    goto restart;
	  } else {
	    tmtcPB.ring.strip(1); goto restart;
	  };
	} else {
	  // obviously this 0x55 sync byte is not what we're after. 
	  tmtcPB.ring.strip(1); goto restart;
	}
	}
      }
    }
  }
 exitloops:
  return;
};






void ReadMPF(){
  QMutexLocker locker(&MPFNUM_mutex);
  static int readlen;
  static PortBuffer mpfPB;
  static char buff[255];
  static unsigned short mlen;
  static unsigned short buflen;
  static int i,j,k;
  message msg;
  static unsigned char messID;


  readlen = read(fd_mpf,buff,255);


  
  if(readlen > 0){
    // first we push the message into the ring buffer
    mpfPB.ring.push(buff, readlen);
    //    WriteToLog("comsetup","MPF Message received. Ring used = %d",mpfPB.ring.used);

  };
  
 tryagain:
  j=0; k=-1;
  if(!mpfPB.ring.isEmpty()){
    buflen = mpfPB.ring.used;
    for(i=0;i<buflen; i++){
      if(mpfPB.ring[i]==0x55){
	j=1;
	goto next;
      };
    };
  next:
    if(j){
      //      WriteToLog("comsetup","I even identified it..");
      mpfPB.ring.strip(i);
      buflen = mpfPB.ring.used;
      if(buflen > 7){
	mlen = (unsigned char) mpfPB.ring[3]+(unsigned char)mpfPB.ring[4]*256;
	messID = mpfPB.ring[5];
	//	WriteToLog("comsetup","mlen + 7 = %d\t ML[%d]=%d",mlen, messID, MessageLength[messID]);
	if(mlen+7 == MessageLength[messID]){
	  if(buflen > mlen+7)
	    if(mpfPB.ring[6+mlen] == -86){

	      mpfPB.ring.pop(7+mlen, (char*) msg.body);
	      if(msg.IsValid()==0){
		AddToMessageQueue(msg);

		goto tryagain;
	      } else {

	      };

	    } else {
	      mpfPB.ring.strip(7+mlen);
	      goto tryagain;
	    };

	} else { 
	  mpfPB.ring.strip(1);
	  goto tryagain;
	};
      };

    } else {
      mpfPB.ring.strip(buflen);
    };
    
    

  };


  return;
};
/*
  j=-1; k=-1;
 notempty:
  if(!mpfPB.ring.isEmpty()){
    buflen = mpfPB.ring.used;
    for(i=0;i<buflen; i++){
      if(mpfPB.ring[i] == 0x55){

	if(i+4<buflen){
	  mlen = (unsigned char) mpfPB.ring[i+3]+(unsigned char)mpfPB.ring[i+4]*256;
	  if(i+mlen + 7 > buflen) goto exitloops; // if there is not enough room to
	  if(mpfPB.ring[i+mlen+6]==-86){
	    // looks like we may have found a valid message.
	    // so let's first strip everything ahead of the 
	    WriteToLog("comsetup","found %d...%d",mpfPB.ring[i],mpfPB.ring[i+mlen+6]);
	    mpfPB.ring.strip(i);
	    mpfPB.ring.pop(7+mlen, (char*) msg.body);
	    if(!msg.IsValid()){
	      //	      WriteToLog("And it is valid!");
	      AddToMessageQueue(msg);
	    } else {
	    WriteToLog("not valid!");
	    };
	  goto notempty;
	  }    
	}
      }
    }
  }
 exitloops:
  return;
};
  

*/

void MoveStage(short mv){
  char msg[20];
  char msg2[20];
  sprintf(msg,"a=%6d\r\n",mv);
  write(fd_stage,msg,10);
  usleep(10000);
  //sprintf(msg2,"moving\r");
  write(fd_stage,"moving\r",7);
  //usleep(1000);
  
  return;
};

void RunSillyProg(){
  char msg[20];
  sprintf(msg,"\rCP 100\r\r");
  write(fd_stage,msg,9);
  sleep(1);
  sprintf(msg,"\rPGM 100\r\r");
 write(fd_stage,msg,10);
  sleep(1);
  sprintf(msg,"var test\r");
  write(fd_stage,msg,9);
  sleep(1);
  sprintf(msg,"input test\r");
  write(fd_stage,msg,11);
  sleep(1);
  sprintf(msg,"print test\r");
  write(fd_stage,msg,11);
  sleep(1);
  sprintf(msg,"end\r");
  write(fd_stage,msg,4);
  sleep(1);
  sprintf(msg,"pgm\r");
  write(fd_stage,msg,4);
  sleep(1);
  sprintf(msg,"run 100\v");
  write(fd_stage,msg,9);
  sleep(1);
  sprintf(msg,"100\r");
  write(fd_stage,msg,4);
  sleep(1);
};







/****************************************************************************/
void ReadMPFnu(){
  QMutexLocker locker(&MPFNUM_mutex);
  static int readlen;
  static PortBuffer mpfPB;
  static char buff[255];
  static unsigned short mlen;
  static int i,j,k;
  message msg;
  
  readlen = read(fd_mpf,buff,255);
  
  if(readlen > 0){
    // first we push the message into the ring buffer
    mpfPB.ring.push(buff, readlen);

  };
  
  j=-1;k=-1;
  if(!mpfPB.ring.isEmpty()){
    for(i=0;i<mpfPB.ring.used;i++){
      if(mpfPB.ring[i]==0x55){ // found possible starting point. 
	mpfPB.ring.strip(i); i=0;
	mlen = (unsigned char) mpfPB.ring[3]+(unsigned char)mpfPB.ring[4]*256;
	
	if(mlen>50) {
	  // obviously the message length is whack. 
	  
	};

	if(mlen + 7 > mpfPB.ring.used) goto exitloops; // if there is not enough room to
	                                                // reconstruct the message, exit. 
       
	 
	if(mpfPB.ring[6+mlen]==-86){
	  if(IsValid(mpfPB.ring,7+mlen)){
	    mpfPB.ring.pop(7+mlen,(char *) msg.body);
	    AddToMessageQueue(msg);
	    //	    WriteToLog("ooops");
	    i = 0;
	  }
	} else {
	  // obviously this 0x55 sync byte is not what we're after. 
	  mpfPB.ring.strip(1); i=0;
	};
      };
    };
  };
 exitloops:
  return;
};




void ReadMPFold(){
    QMutexLocker locker(&MPFNUM_mutex);
  static unsigned char io_lng;
  static unsigned char io_buf[255];
  static int io_i;
  static unsigned char io_valid;

  static unsigned short *cnt;
  static char error[100];
  // we do not want to read the port if the MPF port flag is not raised. 
  if(MPFFlag){
    message msgtmp;
 

    io_lng = read(fd_mpf,io_buf,255);
    // MPFFlag = false;
    // fflush(fd_mpf);
    msgtmp.length = io_lng;
    for(io_i=0;io_i<io_lng;io_i++){
     msgtmp.body[io_i] = io_buf[io_i];
      
    }
    io_valid = (int) msgtmp.IsValid();
    if(io_valid){
      AddToMessageQueue(msgtmp);
    } else {
      cnt = (unsigned short *) &io_buf[1];
      sprintf(error,"Message %d not valid",*cnt);
      WriteToLog(error);
    };
  };
};


void ReadTMTCold(){
  QMutexLocker locker(&TMTC_mutex);
  /*  static unsigned char io_lng;
  static unsigned char buf[255];
  static unsigned char mpf_start[100];
  static unsigned short *mpflength;
  static unsigned char mpf_length[100];
  static int i,j;*/

  static unsigned char io_lng;
  static unsigned char io_buf[255];
  static int io_i;
  static unsigned char io_valid;
  static char error[255];
  static unsigned short *cnt;

  message msgtmp;
  if(TMTCFlag){
    

    mpf_strings = 0;
    usleep(1500);
    io_lng = read(fd_tmtc,io_buf,255);

    TMTCFlag = false;



    msgtmp.length = io_lng;
    for(io_i=0;io_i<io_lng;io_i++){
      msgtmp.body[io_i] = io_buf[io_i];
      
    }

    io_valid = (int) msgtmp.IsValid();
    if(io_valid){
          printf("%d  \n",msgtmp.body[5]);
      AddToMessageQueue(msgtmp);
    } else {
      cnt = (unsigned short *) &io_buf[1];
      sprintf(error,"Message %d not valid with length %d",*cnt,io_lng);
      WriteToLog(error);
    };
  };



};



  /*
    j=-1;k=-1;
  if(!mpfPB.ring.isEmpty()){
    for(i=0;i<mpfPB.ring.used;i++){
      if(mpfPB.ring[i]==0x55){ // found possible starting point. 
	mpfPB.ring.strip(i); i=0;
	mlen = (unsigned char) mpfPB.ring[3]+(unsigned char)mpfPB.ring[4]*256;
	if(mlen + 7 > mpfPB.ring.used) goto exitloops; // if there is not enough room to
	                                                // reconstruct the message, exit. 
	if(mlen+7 > mpfPB.ring.used) goto exitloops;
	if(mpfPB.ring[6+mlen]==-86){
	  if(IsValid(mpfPB.ring,7+mlen)){
	    mpfPB.ring.pop(7+mlen,(char *) msg.body);
	    AddToMessageQueue(msg);
	    i = 0;
	  };
	} else {
	  mpfPB.ring.strip(6+mlen);
	  i=0;
	};
      };
    };
  };
  */


void SetMessageLengths(){
  MessageLength[1] = 18;
  MessageLength[2] = 57;
  MessageLength[3] = 16;
  MessageLength[4] = 26;
  
  TMTCLength[11] =  1;
  TMTCLength[12] =  2;
  TMTCLength[13] =  1;
  TMTCLength[14] =  1;
  TMTCLength[15] =  5;
  TMTCLength[16] =  5;
  TMTCLength[17] =  1;
  TMTCLength[18] =  5;
  TMTCLength[19] =  2;
  TMTCLength[140] =  1;
  TMTCLength[142] =  1;
  TMTCLength[202] = 17;
  TMTCLength[203] = 17;
  TMTCLength[205] = 13;
  TMTCLength[204] = 9;
  TMTCLength[206] = 9;
  TMTCLength[207] = 5;
  TMTCLength[210] = 10;
  TMTCLength[141] = 1;
  TMTCLength[201] = 1;
  TMTCLength[200] = 1;
  TMTCLength[220] = 6;
  TMTCLength[215] = 4;
  TMTCLength[232] = 1;
  TMTCLength[233] = 1;
  TMTCLength[234] = 3;
  TMTCLength[231] = 1;
  TMTCLength[238] = 2;
  TMTCLength[239] = 2;
  TMTCLength[240] = 2;
  TMTCLength[241] =1 ;
  TMTCLength[242] = 5;
  TMTCLength[243] = 5;
  TMTCLength[245] = 16;
  TMTCLength[246] = 12;
  TMTCLength[247] = 20;
  TMTCLength[248] = 20;
  TMTCLength[249] = 1;
  TMTCLength[250] = 1;
  TMTCLength[251] = 1;
  TMTCLength[255] = 1;
  TMTCLength[211] =7 ;
  TMTCLength[235] =9 ;
  TMTCLength[237] =9 ;
  TMTCLength[213] =5 ;
  TMTCLength[212] = 5;
  TMTCLength[230] = 3;
  TMTCLength[231] = 1;
  TMTCLength[221] =9 ;
  TMTCLength[222] =2 ;
  TMTCLength[223] =9 ;
  TMTCLength[224] = 5;
  TMTCLength[225] = 5;
  TMTCLength[226] = 5;
  TMTCLength[227] = 5;
  TMTCLength[228] = 9;
  TMTCLength[229] = 9 ;    
  TMTCLength[214] = 1;
  TMTCLength[219] = 3;
  TMTCLength[160] = 1;
  TMTCLength[161] = 1;
  TMTCLength[162] = 1;
  TMTCLength[163] = 1;
  TMTCLength[164] = 11;
  TMTCLength[165] = 6;
  TMTCLength[166] = 6;
  TMTCLength[167] = 1;
  TMTCLength[168] = 1;
  TMTCLength[169] = 1;

};



void HomeCamera(){
  char msg[255];
  sleep(1);
  sprintf(msg,"\rhome\r");
  write(fd_stage,msg,11);
  WriteToLog("Comsetup.cpp","Homing the stage");
  return;
};
