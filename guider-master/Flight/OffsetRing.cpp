#include "OffsetRing.h"
#include <math.h>


OffsetRing::OffsetRing(unsigned int n){
  unsigned int sz;
  if(n<16537){
    sz = n;
  } else {
    sz = 16536;
  };
  
  size = sz;
  val = new float[sz];
  sum = 0;
  sumsq = 0;
  use = 0;
  start = 0;
  end = 0;
};

OffsetRing::OffsetRing(){
  size = 100;
  val = new float[100];
  sum = 0;
  sumsq = 0;
  use = 0;
  start = 0;
  end = 0;
};

OffsetRing::~OffsetRing(){
  delete [] val;
};

void OffsetRing::push(float newval){
  if(use < size){
    // set the next value
    val[end] = newval;
    // update the sums
    sum += newval;
    sumsq += newval*newval;
    // change the end index
    end++;
    end = end % size;
    // update length
    use++;
  } else {
    // we need to move the start and the end indeces
    // first we remove the contribution of the first
    // item on the list
    sum -= val[start];
    sumsq -= val[start]*val[start];
    // now we translate the indeces.
    end = start;
    start++;
    start = start % size;
    // now we add the new value
    val[end] = newval;
    sum += newval;
    sumsq += newval;
  };

};



void OffsetRing::clear(){
  int i;
  for(i=0;i<size;i++) val[i]=0;
  sum = sumsq = 0;
  start = end = 0;
  use = 0;
};


float OffsetRing::mean(){
  if(use>0){
    return sum/use;
  } else {
    return 0;
  }
};


float OffsetRing::rms(){
  float mn,sig;
  if(use>0){
    mn = mean();
    sig = sumsq/use-mn*mn;
    if(sig<0){
      return -1;
    } else {
      return sqrt(sig);
    };
  } else {
    return 0;
  };
};


float OffsetRing::lastValue(){
  if(use>0){
  return val[end];
  };
}
unsigned int OffsetRing::used(){
  return use;
};

