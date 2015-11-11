#pragma once
#include "OffsetRing.h"



typedef struct{
  float EL;
  float CE;
  OffsetRing ELR();
  unsigned char MeasType;
  unsigned char Valid;
}DTUPointingErrorsData;

class DTUQuaternions{
 private:
  long int q1, q2, q3, q4;
  double Q1, Q2, Q3, Q4;
  double ex, ey, ez;
  double phiovertwo;
  double sinphiovertwo;
  double ra,dec;
 public:
  DTUQuaternions(){};
  ~DTUQuaternions(){};
  void set(long int _q1, long int _q2, long int _q3, long int _q4){
    double phi;
    q1 = _q1;
    q2 = _q2;
    q3 = _q3;
    q4 = _q4;
    Q1 = q1/2147483647.0;
    Q2 = q2/2147483647.0;
    Q3 = q3/2147483647.0;
    Q4 = q4/2147483647.0;
    if(Q4==1){
      dec = 90;
      ra = 0;
    } else {
      phiovertwo = acos(Q4);
      sinphiovertwo = sin(Q4);

      ex = Q1/sinphiovertwo;
      ey = Q2/sinphiovertwo;
      ez = Q3/sinphiovertwo;
      phi = acos(ez);
      dec = M_PI/2.0-phi;
      ra = atan2(ey,ex);  
    };
  };
  
  double getRA(){ return ra;};
  double getDEC(){return dec;};
};


typedef struct{
  float EL;
  float CE;
}ELCEPositionData;

typedef struct{
  unsigned char OpState;
  unsigned char PointControl;
  float ELal, CEal;
  float ELac, CEac;
}MPFStatus;
