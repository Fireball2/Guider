#pragma once

#include "QCam.h"

#define BOXHALFSIZE 7

extern short BOXES;
extern char DIVISOR;


extern double RSQUARED[2*BOXHALFSIZE+1][2*BOXHALFSIZE+1];
extern double RMASK[2*BOXHALFSIZE+1][2*BOXHALFSIZE+1];
extern double RDIST[2*BOXHALFSIZE+1][2*BOXHALFSIZE+1];
extern double BGDMASK[2*BOXHALFSIZE+1][2*BOXHALFSIZE+1];

extern double BOXISUM;
extern double BOXJSUM;
extern double BOXI2SUMl;
extern double BOXJSUM2;

void FillRSquared();


extern float ctrovermaxthr;
extern float ctrovermaxcnt;
extern float ctrspotrms;
extern char ctralgorithm;



// calculates the centroid for the star in the specified window. also calculates the mean and the rms noise for the window. Note that this is to be used for full chip exposures and a subwindow.
bool centroidWindow(QCamera *cam, unsigned char *buf,double &x, double &y, double &mean, double &sigma, double &rmsr);

