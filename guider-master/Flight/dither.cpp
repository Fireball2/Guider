// dither.cpp
// implementation of dither

#include <time.h>
#include "writelog.h"
#include "dither.h"

namespace dither {
bool isDithering; // are we currently dithering?
}
//available patterns

#define DITHERPATTERNS 4
char *filename[DITHERPATTERNS] = {"dither-example.dat","snake_dither.dat","spiral_dither.dat","vertical_snake_dither.dat"};


// timing mechanism. 
timespec tsd; // time constant used to read the time
double currentT; // currently read time
double startT; // the start time
double deltaT; // the time difference


// pattern parameters
unsigned short repeat; // how many times do I repeat? if 0, ad nauseaum
unsigned short steps; // how many steps in the pattern?
unsigned short currentStep; // the current step index
unsigned short currentRep; // the current repeat cycle;
double timeStep; // the size of the timestep that will need to be taken
float dx[MAXDITHERSTEPS]; // the x and y offset arrays for the pattern. 
float dy[MAXDITHERSTEPS];
bool isRelative; // is the pattern relative or absolute?
unsigned short dither::patternID; // the id of the pattern that is being used


void dither::config(){
  dither::isDithering = false; // when we start the program, we don't want 
  // there to be any dithering. 
}

void dither::start(){
  clock_gettime(CLOCK_REALTIME, &tsd);
  startT = tsd.tv_sec + tsd.tv_nsec/1000000000.;
  dither::isDithering = true;
};

void dither::stop(){
  dither::isDithering = false; 
};

void dither::add(unsigned short &tx, unsigned short &ty){
  clock_gettime(CLOCK_REALTIME, &tsd);
  currentT = tsd.tv_sec + tsd.tv_nsec/1000000000.;
  // absolute step number:
  currentStep = (currentT - startT)/timeStep;
  // this puts is in which repetition? 
  currentRep = currentStep / steps;
 
  if ( currentRep < repeat || repeat == 0 ) { 
    // so which step are we actually in?
    currentStep = currentStep % steps;
   
    if (isRelative) { 
      tx = tx + dx[currentStep];
      ty = ty + dy[currentStep];
    } else {
      tx = dx[currentStep];
      ty = dy[currentStep];
    };

  } else { 
    dither::isDithering = false;
  } 
  
};


void dither::load(unsigned char pattern){
  FILE *ifile;
  int i;
  if (pattern > DITHERPATTERNS) pattern = 0;

  if (pattern == 0){
    dither::isDithering = false;
  } else {
    if (ifile = fopen(filename[pattern-1],"r") ) { 

      fscanf(ifile, "TIMESTEP %lf\n",&timeStep);
      fscanf(ifile, "STEPS %d\n",&steps);
      fscanf(ifile, "RELATIVE %d\n",&i);
      if ( i == 0) {
	isRelative = false;
      } else { 
	isRelative = true;
      };
      fscanf(ifile, "REPEAT %d\n",&repeat);
      for(i=0;i<steps;i++){
	fscanf(ifile,"%f %f\n",&dx[i], &dy[i]);
      };
      fclose(ifile);
    };
  }

};
