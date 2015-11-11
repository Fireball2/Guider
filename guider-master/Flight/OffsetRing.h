#pragma once


class OffsetRing{
  float *val;
  unsigned int size;
  unsigned int use;
  float sum, sumsq;
  unsigned int start, end;

 public:
  OffsetRing();
  OffsetRing(unsigned int n);
  ~OffsetRing();
  
  void push(float);
  float mean();
  float rms();
  unsigned int used();
  void clear();
  float lastValue();


};
