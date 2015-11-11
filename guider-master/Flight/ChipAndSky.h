#ifndef CHIPANDSKY_H_
#define CHIPANDSKY_H_


#include <time.h>
#include <math.h>


extern unsigned short trackpointEL, trackpointCE;

void SetCameraPlateScale(double cps);
double GetCameraPlateScale();

void SetCameraRotation(double theta);
void GetCameraRotation(double &sintheta, double &costheta);

typedef struct RotMatrix{
  double pos[3][3];
};

typedef struct Vector{
  double ind[3];
};


typedef struct{
  double lat, lon;
} LatLon;

void SetLatLon(float lat, float lon);


bool isGpsActive();
void turnOnGps();
void turnOffGps();





double LST();
double HourAngle(double lat, double lon);
void RADEC_to_ALTAZ(double ra, double dec, double lat, double lon, double *alt, double *az);

void ALTAZ_to_ELCE(double alt1, double az1, double alt2, double az2, double &el, double &ce);
 
void getLatLon(double *lat, double *lon);


class Dither{
 private:
  float *x;
  float *y;


  timespec ts;
  double lastt;
  double nowt;
  bool isDithering;
  int Nsteps;
  int currentstep;
  int interval; // msec
 public:
  Dither(){
    int i;
    x = new float[8192];
    y = new float[8192];
    for(i=0;i<8192;i++){
      x[i] = y[i] = 0;
    };
    Nsteps = 5000;
    interval = 500;
  }

  ~Dither(){
    delete [] x;
    delete [] y;
  };
  
  void reset(){
    currentstep = 0;
  };

  void start(){
    isDithering = true;
    clock_gettime(CLOCK_REALTIME, &ts);
    lastt = ts.tv_sec + ts.tv_nsec/1000000000.;
		  
  };

  void stop(){
    isDithering = false;
  };


  int getStep(){
    return currentstep;
  };

  int getNStep(){
    return Nsteps;
  };

  void FillDither1(short timestep){
    Nsteps = 1000;
    interval = timestep;
    int i;
    for(i=0;i<Nsteps;i++){
      
    };
  };




  void checkstep(){
    if(isDithering){
      clock_gettime(CLOCK_REALTIME, &ts);
      nowt = ts.tv_sec + ts.tv_nsec/1000000000.;
      if((nowt-lastt)*1000. > interval){
	currentstep++;
	if(currentstep == Nsteps) stop();
	lastt = nowt;
      }; 
    };
  };
  
  float getx(){
    
    if(!isDithering){
      return 0;
    } else {
      checkstep();
      return x[currentstep%8192];
    };
  };
  
  float gety(){
    if(!isDithering){
      return 0;
    } else {
      checkstep();
      return y[currentstep%8192];
    };
  };
  
  bool isDither(){
    return isDithering;
  };
  
  void DitherPattern1(int msec){
    Nsteps = 8192;
    interval = msec;
    int i;
    for(i=0;i<8192;i++){
      x[i]=-100+100*cos(2.0*M_PI*i*1.0/Nsteps*8);
      y[i]= 100*sin(2.0*M_PI*i*1.0/Nsteps*8);
    };
    start();
  };


};


#endif
