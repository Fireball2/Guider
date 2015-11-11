#ifndef GUIDERTOTMTC_H_
#define GUIDERTOTMTC_H_


#include <QMutexLocker>
#include <QMutex>


#include "QCam.h"
#include "messages.h"
#include "mpfmessages.h"
#include "frameblob.h"

unsigned short GtoT_Count();
char GtoT_StartCount();


#define GTOTMAXCOUNT 65536

extern DTUPointingErrorsData DTUPED;
extern ELCEPositionData ELCEPD;
extern MPFStatus MPFS;
extern DTUQuaternions DTUQ;

// x and y in mm

// sends information on up to 10 frameblobs to the ground;
void GtoT_FrameBlobs(frameblob *fb);  //250

void GtoT_AutocollimationOffsets(float x, float y, float r); //251

void GtoT_GuiderStatus(QCamera *cam); //252

void GtoT_CameraError(unsigned char err); //253

void GtoT_MessageRcvd(unsigned char msg); //254

// el and ce in arcsec
void GtoT_PointingErrors(float el, float ce); //255

void GtoT_AutoCollimationTCReply(unsigned char success);

void GtoT_SlewingTCReply(unsigned char success);

void GtoT_AlignmentTCReply(unsigned char success);

void GtoT_ELCETCReply(unsigned char success);

void GtoT_TextError(message m);
void GtoT_TextError(char *str);

void GtoT_CameraStatusTCReply(
			      unsigned short ROIX,
			      unsigned short ROIY,
			      unsigned short ROIdX,
			      unsigned short ROUdY,
			      unsigned char binning,
			      unsigned short expt
			      );

void GtoT_VideoStatusTCReply(
			     unsigned char freq,
			     unsigned char cross,
			     unsigned char GPSt,
			     unsigned char GPScoord,
			     unsigned char RADEC,
			     unsigned char contrast,
			     unsigned char brightness,
			     unsigned char gamma
			     );

void GtoT_CameraROIChangeTCReply(
				 unsigned char success
				 );

void GtoT_CameraExposureTimeTCReply(
				    unsigned char success
				    );


void GtoT_VideoFramePeriodChangeTCReply(
				    unsigned char success
				    );

void GtoT_VideoDisplayChangeTCReply(
				    unsigned char success
				    );

// THIS NEEDS TO BE THOUGHT OUT!!!!
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
		       double Grdec,
		       unsigned char GRvalid,
		       unsigned char Gmode,
		       unsigned char MPFmode,
		       unsigned char GSensor,
		       unsigned char MPFsensor,
		       unsigned short expt
		       );


#pragma pack(push)
#pragma pack(1)

typedef struct GuiderStatus{
  unsigned char startb;
  unsigned short count;
  unsigned short length;
  unsigned char mid;
  unsigned char guidermode;
  unsigned char guidersubmode;
  unsigned char activesensor;
  unsigned int frame;
  unsigned short roix;
  unsigned short roiy;
  unsigned short roidx;
  unsigned short roidy;
  unsigned char tt;
  unsigned int et;
  unsigned char dacstatus;
  unsigned char lightstatus;
  float gpslat;
  float gpslon;
  float gpsalt;
  unsigned short wx;
  unsigned short wy;
  unsigned short wdx;
  unsigned short wdy;
  unsigned short tx;
  unsigned short ty;
  float lvdt;
  unsigned char crc;
  unsigned char endb;
};

#pragma pack(pop)

#endif
