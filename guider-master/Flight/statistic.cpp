#include "statistic.h"
#include <math.h>

// default constructor. 
statistic::statistic(){
  int i;

  N = 100;
  buffer = new float(N);
  

  // reset the entire buffer
  for(i=0;i<N; i++) buffer[i]=0.0;


  // 
  mean = 0;
  rms = 0;
  
  

};



// constructor specifying the number of entries
statistic::statistic(unsigned short N_){
  int i;
  N=N_;
  buffer = new float[N];
  
  
  // reset the entire buffer
  for(i=0;i<N; i++) buffer[i]=0.0;
  
  
  // 
  mean = 0;
  rms = 0;
  
 
};

// a little bit of a cleanup
statistic::~statistic(){
  delete buffer;
};

// returns the number of entries in the buffer 
unsigned short statistic::getCount(){
  return N;
};



char statistic::insert(float value, unsigned short index){
  if (index >=N ) return -1;
  static float val;
  val = value;
  if (isnan(value)) val = 0;

  
  sum = sum +   val -  buffer[index];
  rsq = rsq + val*val - buffer[index]*buffer[index];
  
 

  mean = sum/(1.0*N);
  rms = sqrt(rsq/(1.0*N) - mean*mean);
  
  buffer[index] = val;

};


float statistic::getMean(){
  return mean;
};

float statistic::getRMS(){
  return rms;
};

  
