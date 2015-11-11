#ifndef DECODE_H_
#define DECODE_H_


#include "messages.h"



typedef struct AscAttitude{
  unsigned char PIDVersion;
  unsigned char PIDType;
  unsigned char PIDDataFieldHeaderFlag;
  unsigned short PIDApplicationProcessID;
  unsigned char SCSegmentationFlag;
  unsigned short SCSourceSequenceControl;
  unsigned short Length;
  unsigned char DFHspare1;
  unsigned char DFHPUSVersion;
  unsigned char DFHspare2;
  unsigned char DFHType;
  unsigned char DFHSubtype;
  unsigned int DFHTimestampSec;
  unsigned short DFHTimestampSubSec;
  
  unsigned char DFHspare3;
  
  unsigned short ReportID;
  
  int q1, q2, q3, q4;
  
  unsigned char infoFlags;
  unsigned char infoResidual;
  unsigned char infoLocks;
  unsigned char infoObjects;
  unsigned char infoStarsFail;
  unsigned char AGC[3];
  unsigned short AGCFloor;
  unsigned short AGCCeil;

  unsigned long info;

  unsigned int timestampSec;
  unsigned short timestampSubSec;
  
};


void ResetReceivedMessageCounters();


/*****************************
First set decodes things sent to the MPF Num by the Guider 
******************************/

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
				    );


/******
Message ID 2
Autocolimation Request. 
Nothing much to do here
*******/
void G002_AutocolimationRequest(
				message &m
				);

/******
Message ID 3
Switching Guider Request
Nothing much to do here
*******/
void G003_SwitchingGuiderRequest(
				 message &m
				 );

/******
       Message ID 4
       Switching DTU Request
       Nothing much to do here
*******/
void D004_SwitchingDTURequest(
			      message &m
			      );


/******
       Message ID 5
       Tracking Request
       message --->
        RA, DEC
	config type.
*****/
void D005_TrackingRequest(
			  message &m,
			  double &ra,
			  double &dec,
			  unsigned char &configtype
			  );

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
			 );


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
			     );


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
			   );



/******
Message ID 9
Center ROI Request
Nothing much to do here
*******/
void G009_CenterROIRequest(
			   message &m
			   );

/******
       Message ID 100
       Autocolimation TC
       Nothing much to do here
*******/
void G100_AutocolimationTC(
			   message &m
			   );


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
		    );


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
		    );


/******
       Message ID 103
       EL & CE Offsets TC
       message --->
       EL CE
*****/
void G007_ELCEOffsetsTC(
			message &m,
			float &EL,
			float &CE
			);




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
******* /
void M001_DTUPointingErrorsData(
				message &m,
				float &EL,
				float &CE,
				unsigned char &meastype,
				unsigned char &valid
				);

/ *********
	  Message ID 2 
	  DTU Coordinate Data
	  message ---->
	  DTU Attitude Data
******** /	  
void M002_DTUCoordinateData(
			    message &m,
			    AscAttitude &asc
			    );

/ ********* 
	   Message ID 3
	   EL and CE Position Data
	   message ---->
	   EL and CE axis position
*********/
void M003_ELCEPositionData(
			   message &m,
			   float &EL,
			   float &CE
			   );

/**********
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
			    );

/ ***********************************************
Third set decodes data sent do the ground by the guider 
and by the guider back to the ground. 
************************************************/



/**********
	   MESSAGES 20 and 120
	   Autocollimation TC
***********/	   

void T020_AutocollimationTC(
			    message &m
			    );

void G120_AutocollimationTCReply(
				 message &m,
				 unsigned char &success
				 );


/**********
	   MESSAGES 21 and 121
	   Slewing TC
***********/	   
void T021_SlewingTC(
		     message &m,
		     double &ra,
		     double &dec
		    );

void G121_SlewingTCReply(
			 message &m,
			 unsigned char &success
			 );


/**********
	   MESSAGES 22 and 122
	   Slewing TC
***********/	   
void T022_AlignmentTC(
		     message &m,
		     double &ra,
		     double &dec
		    );

void G122_AlignmentTCReply(
			   message &m,
			   unsigned char &success
			   );


/**********
	   MESSAGES 23 and 123
	   EL+CE TC
***********/	   
void T023_ELCETC(
		 message &m,
		 float &EL,
		 float &CE
		 );

void G123_ELCETCReply(
		      message &m,
		      unsigned char &success
		      );



/*********
	  MESSAGE 25
	  Focus TC
********/
void T025_FocusTC(message &m, short &foc);


/*********
	  MESSAGE 26
	  AutoFocus TC
	  For Completeness
**********/
void T026_AutoFocusTC();

/*********
	  MESSAGE 27
	  Adjust Plate Scale TC
********/
void T027_AdjustPlateScaleTC(message &m, double &newps);

/*********
	  MESSAGE 28
	  Record Alignment TC
	  For Completeness
********/

void T028_RecordAlignment();





/**********
	   MESSAGES 30 and 130
	   Camera Status TC 
***********/	   
void T030_CameraStatusTC(
			 message &m
			 );

void G130_CameraStatusTCReply(
			      message &m,
			      unsigned short &ROIX,
			      unsigned short &ROIY,
			      unsigned short &ROIdX,
			      unsigned short &ROIdY,
			      unsigned char &binning,
			      unsigned short &expt
			      );


/**********
	   MESSAGES 31 and 131
	   Video Status TC
***********/	   
void T031_VideoStatusTC(
			message &m
			);

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
			     );
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
			  unsigned char &mode
			  );

void G132_CameraROIChangeTCReply(
				 message &m,
				 unsigned char &success
				 );
			  
/**********
	   MESSAGES 33 and 133
	   Camera Exposure Time TC
***********/	   
void T033_CameraExposureTimeTC(
			       message &m,
			       unsigned short &expt,
			       unsigned char &mode
			       );

void G133_CameraExposureTimeTCReply(
				    message &m,
				    unsigned char &success
				    );

/**********
	   MESSAGES 34 and 134
	   video frame period change
***********/	   
void T034_VideoFramePeriodChangeTC(
				   message &m,
				   unsigned char &freq
				   );

void G134_VideoFramePeriodChangeTCReply(
					message &m,
					unsigned char &success
					);


/**********
	   MESSAGES 35 and 135
	   Video Display Change TC
***********/	   
void T035_VideoDisplayChangeTC(
			       message &m,
			       unsigned char &param,
			       unsigned char &value
			       );

void G135_VideoDisplayChangeTCReply(
				    message &m,
				    unsigned char &success
				    );

/**********
	   MESSAGE 36
	   Force Frame Update
***********/	   
void T036_ForceFrameUpdateTC(
			     message &m
			     );



/**********
	   MESSAGES 37 and 137
	   Get Frame
***********/	   
void T037_GetFrameTC(
		     message &m
		     );

void G137_GetFrameTCReply(
			  message &m,
			  unsigned short &ROIdX,
			  unsigned short &ROIdY
			  );


/**********
	   MESSAGES 40 and 140
	   Status Update TC
***********/	   
void T040_UpdatePeriodTC(
			 message &m,
			 unsigned short &delay
			 );

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
);


void T050_SystemCommandTC(message m, char *cmd, unsigned char &lngth);

#endif 



