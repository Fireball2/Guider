#ifndef DECODEMTOG_H_
#define DECODEMTOG_H_

#include "messages.h"
#include "decode.h"


// For the description of these messages please see the Guider<->MPF document.


void M001_DTUPointingErrorsData(
				message &m,
				float &EL,
				float &CE,
				unsigned char &meastype,
				unsigned char &valid
				);


void M002_DTUCoordinateData(message &m,
			    AscAttitude &acs);


void M003_ELCEPositionData(
			   message &m,
			   float &EL,
			   float &CE
			   );

void M004_MPFNumStatusData(
			    message &m,
			    unsigned char &opstate,
			    unsigned char &fpc,
			    float &acel,
			    float &acce,
			    float &alel,
			    float &alce
			    );


#endif
