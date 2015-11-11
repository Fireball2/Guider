#pragma once

class statistic{
 public:

  statistic();
  statistic(unsigned short n);
  ~statistic();
  
  unsigned short N; // number of entries.

  unsigned short getCount();   // number of entries query.

  float *buffer;   // buffer for the values

  float mean; // the mean value of the buffer

  float rsq; // sum of squares
  float sum; // sum of values

  float rms; // the rms value of the buffer


  char insert(float value, unsigned short index);
 

  float getRMS();
  float getMean();
};
