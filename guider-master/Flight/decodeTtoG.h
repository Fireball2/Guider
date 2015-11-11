#ifndef DECODETTOG_H_
#define DECODEGTOT_H_

//#include "decode.h"
#include "messages.h"


// the functions declared here decode the messages sent from the ground to the guider.
// for a description of the formats and meanings see the guider to ground documentation


void T015_OvermaxCnt(message &m,
		     float &f);

void T016_OvermaxThr(message &m,
		     float &f);

void T017_ToggleAlgorithm(message &m);

void T018_SpotRMSThr(message &m,
		     float &f);

void T019_BoxSize(message &m,
		     char &c);

void T200_InitTC(message &m);

void T201_AutoCollimationTC(message &m);

char T202_AlignmentTC(message &m,
		      double &ra,
		      double &dec);

char T203_SlewingTC(message &m,
		      double &ra,
		    double &dec);

 char T204_ELCEOffsetTC(message &m, float &el, float &ce);

char  T205_AlignTrackTC(message &m,
		      unsigned char &roiflag,
		      unsigned short &roicx,
		      unsigned short &roicy,
		      unsigned short &roid,
		      unsigned char &trigt,
		       unsigned int &expt);

void T206_AutoCollimationXYOffsets(message &m,
				   float &xoffset,
				   float &yoffset
				   );

void T207_AutocollimationZoomXY(message &m,
				unsigned short &roix,
				unsigned short &roiy
				);


void T207_GPSLatLon(message &m,
		    float &lat,
		    float &lon
		    );

void T208_GPSUTime(message &m,
		   double &ut
		   );

void T209_GPSToggle(message &m);



char T210_SetROITC(message &m,
		   unsigned char &mode,
		   unsigned short &roix,
		   unsigned short &roiy,
		   unsigned short &roidx,
		   unsigned short &roidy
		   );

char T211_TriggerPeriodTC(message &m,
			  unsigned char &mode,
			  unsigned char &trigt,
			  unsigned int &expt
			  );


char T212_SetNormalizedGainTC(message &m,
			      unsigned int &gain
			      );


char T213_SetAbsoluteOffsetTC(message &m,
			      short &offset
			      );

void T215_SetDisplayControlsTC(message &m,
			       unsigned char &brightness,
			       unsigned char &contrast,
			       unsigned char &gamma);

void T216_FrameUpdatePeriodTC(message &m,
			      unsigned char &mode,
			      unsigned char &period
			      );

void T217_DisplayFormatTC(message &m,
			  unsigned char &mode,
			  unsigned char &bin,
			  unsigned short &cornerx,
			  unsigned short &cornery,
			  unsigned char &ticks,
			  unsigned char &centroid,
			  unsigned char &unused1,
			  unsigned char &unused2
			  );

void T218_ImageStackTC(message &m,		       
		       unsigned char &Ndisplay,
		       unsigned char &Ndisk
		       );

void T219_NudgeTracking(message &m,
			char &elnudge,
			char &cenudge);

void T220_OffsetsAndGains(message &m,
			  unsigned char &id,
			  float &value
			  );

void T221_FBGain(message &m,
		 double &fbgain
		 );

void T222_FBSatVal(message &m, 
		   unsigned char &sv
		   );

void T223_FBThreshold(message &m,
		      double &thr
		      );

void T224_FBDistTol(message &m,
		    int &dt
		    );

void T225_FBGrid(message &m,
		 unsigned int &grd
		 );

void T226_FBCenBox(message &m, 
		   unsigned int &cb
		   );

void T227_FBApBox(message &m,
		  unsigned int &ab
		  );

void T228_FBReadoutOffset(message &m,
			  double &ro);

void T229_FBReadoutNoise(message &m,
			double &rg);

void T230_FocusTC(message &m,
			  short &stagemove
		  );

void T231_AutoFocusTC(message &m
		      );

void T232_TurnOnLEDsTC(message &m
		       );


void T233_TurnOffLEDsTC(message &m
			);

void T234_TagLog(message &m,
	      unsigned short &nb
	      );

char T235_SetCameraPlateScaleTC(message &m,
				double &platescale
				);

void T236_ResetCameraPlateScaleTC(message &m
				  );



char T237_SetRotationTC(message &m,
			double &rot
			);

void T238_DACOutput(message &m,
		    unsigned char &daco
		    );

void T239_TrackBlob(message &m,
		    unsigned char &blb
		    );

void T240_InfoBlob(message &m,
		   unsigned char &blb
		   );

void T242_SetTrackPoint(message &m,
			unsigned short &tpx,
			unsigned short &tpy
);

void T243_SetMoveStepSize(message &m,
			  float &stepsize);

#pragma pack(push)
#pragma pack(1)

typedef struct{
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned short wx;
  unsigned short wy;
  unsigned short wd;
  unsigned short tpx;
  unsigned short tpy;
  unsigned int expt;
  unsigned char trigt;
  unsigned char crc;
  unsigned char syncend;
} LockedFC;

typedef struct{
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned short rx;
  unsigned short ry;
  unsigned short rd;
  unsigned int expt;
  unsigned char trigt;
  unsigned char crc;
  unsigned char syncend;
} LockedROI;

typedef struct{
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned short wx;
  unsigned short wy;
  unsigned short wd;
  float ra;
  float dec;
  unsigned int expt;
  unsigned char trigt;
  unsigned char crc;
  unsigned char syncend;
} GuidingFC;

typedef struct{
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned short rx;
  unsigned short ry;
  unsigned short rd;
  float ra;
  float dec;
  unsigned int expt;
  unsigned char trigt;
  unsigned char crc;
  unsigned char syncend;
} GuidingROI;


typedef struct{
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned short wx;
  unsigned short wy;
  unsigned short wd;
  unsigned short tx;
  unsigned short ty;
  unsigned char crc;
  unsigned char syncend;
}WindowTrack;

typedef struct{
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned char trigt;
  unsigned int expt;
  unsigned char crc;
  unsigned char syncend;
}CamTime;

#pragma pack(pop)

void T245_LockedFC(message &m,  LockedFC &lfc);

void T246_LockedROI(message &m, LockedROI &lroi);
  void T247_GuidingFC(message &m, GuidingFC &gfc);
  void T248_GuidingROI(message &m, GuidingROI &groi);

void T164_WindowTrack(message &m, WindowTrack &wt);

void T165_SmallTime(message &m, CamTime &ct);
void T166_LargeTime(message &m, CamTime &ct);

void T012_DitherPattern(message &m, unsigned char &uc);

#endif
