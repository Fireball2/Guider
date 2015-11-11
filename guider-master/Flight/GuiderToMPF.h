#ifndef GUIDERTOMPF_H_
#define GUIDERTOMPF_H_



/**************************************************************************/
/* The following functions construct messages that are sent to MPF Num ****/
/**************************************************************************/
/* For complete details see the MPF-Guider communication document      ****/
/* ftp://hassif.....                                                   ****/
/**************************************************************************/

#include <QMutexLocker>
#include <QMutex>


unsigned short GtoM_Count();
char GtoM_StartCount();


#define GTOMMAXCOUNT 65536

 char GtoM_GuiderPointingErrorsData(
				    float EL,
				    float CE,
				    unsigned char validELCE,
				    double ra,
				    double dec,
				    unsigned char validRADEC
				    );
 
 
 char GtoM_AutocollimationRequest(
				  );

 char GtoM_SwitchingGuiderRequest(
				  );
 
 char GtoM_SwitchingDTURequest(
			       );

 char GtoM_TrackingRequest(
			   double ra,
			   double dec,
			   unsigned char config
			   );

 char GtoM_SlewingRequest(
			  double ra,
			  double dec
			  );

 char GtoM_ELCEOffsetsRequest(
			      float EL,
			      float CE
			      );

 char GtoM_GuiderStatusData(
			    unsigned char OpState,
			    unsigned char FPController
			    );

 char GtoM_CenterROIRequest(
			   );

 char GtoM_AutoCollimationTC(
                            );
 
 char GtoM_SlewingTC(
		     double ra,
		     double dec
		     );
 
 char GtoM_AlignmentTC(
		       double ra,
		       double dec
		       );
		     
 char GtoM_ELCEOffsetsTC(
			 float EL,
			 float CE
			 );
 
 
 
#endif 

