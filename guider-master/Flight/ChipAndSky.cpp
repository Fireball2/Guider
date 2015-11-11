#include <stdio.h>
#include <math.h>
#include <time.h>



#include "ChipAndSky.h"
#include "writelog.h"
#include "GuiderToTMTC.h"
#include "coords.h"
#include "gps_func.h"


double CameraPlateScale;  // camera plate scale in arcseconds per pixel

double RotTheta;
double CosTheta, SinTheta;


bool GpsActive;

bool isGpsActive(){
  return GpsActive;
}

void turnOnGps(){
  GpsActive = true;
  WriteToLog("GPS","Turned ON");
};

void turnOffGps(){
  GpsActive = false;
  WriteToLog("GPS","Turned OFF");
}


unsigned short trackpointEL, trackpointCE;

void SetCameraPlateScale(double cps){
  if(cps>0 ){
    CameraPlateScale = cps; 
  } else {
    WriteToLog("Camera Plate Scale can not be negative\0");
    GtoT_TextError("Camera Plate Scale can not be negative\0");
  };
};

double GetCameraPlateScale(){
  return CameraPlateScale;
};


void SetCameraRotation(double theta){
  RotTheta = theta;
  CosTheta = cos(theta);
  SinTheta = sin(theta);
  return;
};

void GetCameraRotation(double &sintheta, double &costheta){
  sintheta = SinTheta;
  costheta = CosTheta;
};
  



LatLon FromGround;

void SetLatLon(float lat, float lon){
  FromGround.lat = lat;
  FromGround.lon = lon;
};




double LST(){
  static int year, month, day, hour, minute;
  float second;
  static time_t timenow;
  static float ut;
  static float lat, lon;
  static tm *tim;  
  static int latd, lond;
  
  if(isGpsActive()){
    year = gps_year;
    month = gps_month;
    day = gps_day;
    ut = gps_utc_time;
    lat = gps_latitude;
    lon = gps_longitude;
    hour = (int) ut/10000;;
    minute = (int) ((ut-10000*hour)/100);
    second = (ut-10000*hour - 100*minute);
    ut = (hour*1.0+minute/60. + second/3600.);
    latd = (int) (lat/100);
    lond = (int) (lon/100);
    lat = (lat-100*latd)/60.0 + latd;
    lon = (lon-100*lond)/60.0 + lond;
    if(gps_lat_reference=='S') lat*=-1;
    if(gps_long_reference=='W') lon*=-1;
  } else {
    
    
    time(&timenow);
    tim = gmtime(&timenow);
    
    
    year = tim->tm_year + 1900;
    month = tim->tm_mon+1;
    day = tim->tm_mday;
    hour = tim->tm_hour %24;
    minute = tim->tm_min;
    second = (float) tim->tm_sec;
    
    
  // ust can vary by an hour from gmt when
  // britain is in daylight savings..
    /*
    hour++;
    if(hour==24){
      day++;
      hour=0;
      };*/
    lat = FromGround.lat;
    lon = FromGround.lon;
    ut = (hour*1.0+minute/60. + second/3600.);
  };
  
  //  WriteToLog("UT","%f",ut);
  static int d;
  
  d = 367*year - (7*(year + (month+9)/12))/4;
  d+= (275*month)/9 + day -730530 + (int) ((ut)/25);
  


  float lst;

  //  WriteToLog("LL","%f %f",lat,lon);
  
  lst = 98.9818 + 0.985647352*d + ut*15 + lon;

  static int lstmod;
  lstmod = (int) lst/360;
  lst = lst - 360*lstmod;
  // now lst is in degrees;

  //  printf("the LST = %lf\n",lst/15.0);
  

  //  lst = lst/15;
  // now it is in hours.

    lst = lst/180*M_PI;
  // now it is in radians

  return lst;
  // 


};

/*
double LST(double lat, double lon){
time_t timenow;
  tm *tim;
  int year, month, day, hour, minute, second;
  double ut;

  time(&timenow);
  tim = gmtime(&timenow);
  

  year = tim->tm_year + 1900;
  month = tim->tm_mon+1;
  day = tim->tm_mday;
  hour = tim->tm_hour %24;
  minute = tim->tm_min;
  second = tim->tm_sec;
  

  // ust can vary by an hour from gmt when
  // britain is in daylight savings..
  hour++;
  if(hour>23){
    day++;
  };
  // and if day goes over the month... too bad for now. fix later.
  //

  printf("The GMT time that i think it is:\n");
  printf("Year: %d\n",year);
  printf("Month :  %d\n",month);
  printf("Day : %d\n",day);
  printf("Hour: %d\n",hour);
  printf("Minute: %d\n",minute);
  printf("Second: %d\n",second);

  ut = (hour*1.0+minute/60. + second/3600.);
  printf("universal time is: %lf\n",ut);




  int d;

  d = 367*year - (7*(year + (month+9)/12))/4;
  d+= (275*month)/9 + day -730530 + (int) ((ut)/25);

  printf("the 'day' = %d\n",d);



  

  double lst;

  
  lst = 98.9818 + 0.985647352*d + ut*15 + lon;


  int a;

  a = lst/360;
  lst = lst - 360*a;

  printf("the LST = %lf\n",lst/15.0);
  
  lst = lst/180*M_PI;

  return lst*M_PI/15.;

};
*/

// both lst and ra should be in radians. 
double HourAngle(double lst, double ra){
  double _ra;
  _ra = ra;
  if(ra<0.) _ra = M_PI-ra;

  return lst-ra;
};


void RADEC_to_ALTAZ(double ra, double dec, double lat, double lon, double *alt, double *az)
{
  double ha;
  
  ha = HourAngle(LST(),ra);
  *alt = asin(sin(lat)*sin(dec)+cos(lat)*cos(dec)*cos(ha));
  *az = atan2(sin(ha),cos(ha)*sin(lat)-tan(dec)*cos(lat));

  return;
}

// this function coverts the alt-az difference between the two spots
// to an elevation-cross-elevation difference. The expressions used
// are taken from the CNES documentation.
void ALTAZ_to_ELCE(double alt1, double az1, double alt2, double az2, double &el, double &ce){

  el = asin(sin(alt1)*cos(alt2)-cos(az1-az2)*cos(alt1)*sin(alt2)) *180/M_PI*3600;
  ce = atan2(sin(az1-az2)*cos(alt1),cos(az1-az2)*cos(alt1)*cos(alt2)+sin(alt1)*sin(alt2)) * 180/M_PI*3600;

};


void getLatLon(double *lat, double *lon){
  *lat = 31.761;
  *lon = -95.64;
};

void getPos(double &lat, double &lon){
  double latd, lond;
  lat = 31.761;
  lon = -95.64;

  if(isGpsActive()){
    
    lat = gps_latitude;
    lon = gps_longitude;
    latd = (int) (lat/100);
    lond = (int) (lon/100);
    lat = (lat-100*latd)/60.0 + latd;
    lon = (lon-100*lond)/60.0 + lond;
    if(gps_lat_reference=='S') lat*=-1;
    if(gps_long_reference=='W') lon*=-1;
  };
};
