#include "centroiding.h"
#include <math.h>
#include "QCam.h"

// variables used to determine which spot-finding algorithm
// and what parameters we are using. 
float ctrovermaxthr;
float ctrovermaxcnt;
float ctrspotrms;
char ctralgorithm;


double RSQUARED[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];
double RMASK[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];
double RDIST[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];
double BGDMASK[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];


int SMALLSIZEX, SMALLSIZEY;


double RSQUAREDS[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];
double RMASKS[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];
double RDISTS[BOXHALFSIZE*2+1][BOXHALFSIZE*2+1];


double BOXISUM;
double BOXJSUM;
double BOXI2SUM;
double BOXJ2SUM;


short BOXES;
char  DIVISOR;


void FillRSquared(){
  int i,j;
  for(i=-BOXHALFSIZE; i<BOXHALFSIZE+1; i++){  
    for(j=-BOXHALFSIZE; j<BOXHALFSIZE+1; j++){
      if(sqrt(1.0*(i*i+j*j)) < BOXHALFSIZE+1){
	RSQUARED[i+BOXHALFSIZE][j+BOXHALFSIZE]=i*i+j*j;
	RMASK[BOXHALFSIZE+i][BOXHALFSIZE+j]=1;
	RDIST[i+BOXHALFSIZE][j+BOXHALFSIZE]=sqrt(1.0*(i*i+j*j));
      } else {
	RSQUARED[i+BOXHALFSIZE][j+BOXHALFSIZE] = 0;
	RMASK[BOXHALFSIZE+i][BOXHALFSIZE+j]=0;
	RDIST[i+BOXHALFSIZE][j+BOXHALFSIZE]=0;
      };

      if(sqrt(1.0*(i*i+j*j)) < (BOXHALFSIZE+1.0)/1.5){
	BGDMASK[i+BOXHALFSIZE][j+BOXHALFSIZE] = 0;
      } else {
	BGDMASK[i+BOXHALFSIZE][j+BOXHALFSIZE] = 1;
      };
    };
  };

  double size;
  size = 2*BOXHALFSIZE+1;
  BOXI2SUM = size * (1/6.)*BOXHALFSIZE*(BOXHALFSIZE+1)*(2*BOXHALFSIZE+1);  
  BOXJ2SUM = BOXI2SUM;
  BOXISUM = 0;
  BOXJSUM = 0;

  //  BOXES = 361;
  DIVISOR = 20;
  BOXES = (DIVISOR-1)*(DIVISOR-1);

  ctrovermaxthr = 0.3; // 0.3 times the max pixel in a box.
  ctrovermaxcnt = 2.5; // more than 2.5 pixels have to be over the threshold
  ctrspotrms = 10.0; // things larger than 10 pixels in rms are not stars
  ctralgorithm = 1.0; // 1.0 is the new, 0.0 is the old



};


bool centroidWindow(QCamera *cam, unsigned char *buf,double &x, double &y, double &mean, double *sigma, double &rmsr){
  return true;
};
