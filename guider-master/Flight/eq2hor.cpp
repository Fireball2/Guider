#include <math.h>
#include "skytime.h"
#include <iostream>
#include <stdio.h>


//#define VERBOSE

// translation of idl eq2hor routine to c(++)
// RA and DEC are entered in 
void eq2hor(double _ra, double _dec, double jd, double lat, double lon, double &alt, double &az){
  static double ra, dec;
  static double J_now;
  ra = _ra *180/M_PI;
  if(ra<0) ra+=180;
  dec = _dec = dec * 180/M_PI;




#ifdef VERBOSE
  printf(" INPUT\n");
  printf("=======\n");
  printf("J2000 =\t %lf\n",jd);   
  printRA(ra/15);
  printDec(dec);
  printf(" PROCESSING\n");
  printf("============\n");
#endif
    // compute the current equinox.
  J_now = (jd - 2451545.0)/365.25+2000;
  // precess the coordinates to the current epoch
  precess(ra,dec,2000.0, J_now);
#ifdef VERBOSE
  printf("( precessed )\n");
  printf("JD    =\t %4.4lf\n",J_now);   
  printRA(ra/15);
  printDec(dec);
#endif

  // make corrections for nutation and aberrations

  static double dra1, dra2, ddec1, ddec2;
  double eps, d_psi, d_eps;

  //  dra1 = dra2 = eps = d_psi = d_eps = 0;
  co_nutate(jd, ra, dec,  dra1, ddec1, eps, d_psi,d_eps);
  co_aberration(jd, ra, dec, dra2, ddec2, eps);


  //  printf("%lf %lf\n",eps,d_psi);

  ra = ra + (dra1+ dra2)/3600.;
  dec = dec + (ddec1 + ddec2)/3600.;

#ifdef VERBOSE
  printf("( fully corrected )\n");
  printf("JD    =\t %4.4lf\n",J_now);   
  printRA(ra/15);
  //  printf("RA    =\t %lf\n",ra);
  printDec(dec);
#endif



  double lmst, last;

  lmst = ct2lst(lon, jd); // get LST (in hours, this is independen
  // of timezone  as we are giving JD.

  // calculate local apparent sidereal time
  last = lmst + d_psi * cos(eps)/3600./15;
  //  printf("d_psi = %lf\t eps=%lf\n",d_psi, eps);

#ifdef VERBOSE
  printf("LMST");
  printDEGtoH(lmst/15);
  printf("LAST");
  printDEGtoH(last/15);
#endif


    // find the hour angle
    static double ha;
  ha = 15*last - ra;
  if(ha<0) ha+= 360;
  ha = fmod(ha,360);


#ifdef VERBOSE
  printf("HA");
  printDEGtoH(ha/15);
#endif

  // now do spherical trig to get APPARENT alt,az
  hadec2altaz(ha,dec, lat,alt,az);

#ifdef VERBOSE
  printf("\nALT");
  printDEGtoH(alt);
  //=\t %.6lf\n",alt);
  printf("AZ");
  printDEGtoH(az/15);
#endif


};



void hadec2altaz(double ha, double dec, double lat, double &alt, double &az){

  static double sh, ch, sd, cd, sl, cl;
  static double x, y,z;
  double r; 
  
  sh = sin(ha*DEGtoRAD);
  ch = cos(ha*DEGtoRAD);
  sd = sin(dec*DEGtoRAD);
  cd = cos(dec*DEGtoRAD);
  sl = sin(lat*DEGtoRAD);
  cl = cos(lat*DEGtoRAD);
  
  x = - ch * cd*sl + sd*cl;
  y = -sh * cd;
  z = ch * cd * cl + sd * sl;
  r = sqrt(x*x+y*y);
    
  az = atan2(y,x) / DEGtoRAD;
  alt = atan2(z,r) / DEGtoRAD;
  
  if(az<0) az+=360.0;

return;
};


void co_nutate(double jd, double ra, double dec, double &dra, double  &ddec, double &eps, double &dpsi, double &deps){
  static double T;

  T = (jd-2451545.0)/36525.0 ; // julian centuries from J2000 of jd


  //obliquity of ecliptic

  nutate(jd,dpsi, deps);

  static double eps0;
  eps0 = 23.4392911*3600.0 - 46.8150*T - 0.00059*T*T + 0.001813*T*T*T;
  eps = (eps0+deps)/3600. * DEGtoRAD;

  static double ce,se;
  ce = cos(eps);
  se = sin(eps);
  static double x,y,z,x2,y2,z2,r,ra2,dec2,xyproj;
  
  x = cos(ra*DEGtoRAD)* cos(dec*DEGtoRAD);
  y = sin(ra*DEGtoRAD)* cos(dec*DEGtoRAD);
  z = sin(dec * DEGtoRAD);

  x2 = x - (y*ce + z*se)*dpsi*DEGtoAS;
  y2 = y + (x*ce*dpsi - z*deps)*DEGtoAS;
  z2 = z + (x*se*dpsi +y*deps)*DEGtoAS;

  r = sqrt(x2*x2+y2*y2+z2*z2);
  xyproj = sqrt(x2*x2+y2*y2);


  ra2 = x2* 0.0;
  dec2 = x2 *0.0;

  if(xyproj==0 && z!=0){
    dec2 = asin(z2/r);
    ra2 = 0;
  };

  if(xyproj !=0){
    ra2 =  atan2(y2,x2);
    dec2 = asin(z2/r);
  };

  ra2 = ra2/DEGtoRAD;
  dec2 = dec2/DEGtoRAD;

  if(ra2<0) ra2+=360.;

  dra = (ra2-ra)*3600;
  ddec = (dec2-dec)*3600;

  
  return;
};


 double coeff1[4] = {297.85036,  445267.111480, -0.0019142, 1./189474 };
 double coeff2[4] = {357.52772, 35999.050340, -0.0001603, -1./3e5 };
double   coeff3[4] = {134.96298, 477198.867398, 0.0086972, 1.0/5.625e4};
double  coeff4[4] = {93.27191, 483202.017538, -0.0036825, -1.0/3.27270e5 };
double coeff5[4] = {125.04452, -1934.136261, 0.0020708, 1./4.5e5};

double d_lng[63] = {0,-2,0,0,0,0,-2,0,0,-2,-2,-2,0,2,0,2,0,0,-2,0,2,0,0,-2,0,-2,0,0,2,-2,0,-2,0,0,2,2,0,-2,0,2,2,-2,-2,2,2,0,-2,-2,0,-2,-2,0,-1,-2,1,0,0,-1,0,0, 2,0,2};

double m_lng[63] = {0,0,0,0,1,0,1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,1,0,-1,0,0,0,1,1,-1,0,0,0,0,0,0,-1,-1,0,0,0,1,0,0,1,0,0,0,-1,1,-1,-1,0,-1};

double mp_lng[63] = {0,0,0,0,0,1,0,0,1,0,1,0,-1,0,1,-1,-1,1,2,-2,0,2,2,1,0,0,-1,0,-1, 0,0,1,0,2,-1,1,0,1,0,0,1,2,1,-2,0,1,0,0,2,2,0,1,1,0,0,1,-2,1,1,1,-1,3,0};

double  f_lng[63] = {0,2,2,0,0,0,2,2,2,2,0,2,2,0,0,2,0,2,0,2,2,2,0,2,2,2,2,0,0,2,0,0,0,-2,2,2,2,0,2,2,0,2,2,0,0,0,2,0,2,0,2,-2,0,0,0,2,2,0,0,2,2,2,2};

double om_lng[63] = {1,2,2,2,0,0,2,1,2,2,0,1,2,0,1,2,1,1,0,1,2,2,0,2,0,0,1,0,1,2,1,1,1,0,1,2,2,0,2,1,0,2,1,1,1,0,1,1,1,1,1,0,0,0,0,0,2,0,0,2,2,2,2};

double sin_lng[63] = {-171996, -13187, -2274, 2062, 1426, 712, -517, -386, -301, 217,  -158, 129, 123, 63, 63, -59, -58, -51, 48, 46, -38, -31, 29, 29, 26, -22,   21, 17, 16, -16, -15, -13, -12, 11, -10, -8, 7, -7, -7, -7,  6,6,6,-6,-6,5,-5,-5,-5,4,4,4,-4,-4,-4,3,-3,-3,-3,-3,-3,-3,-3 };

double sdelt[63] = {-174.2, -1.6, -0.2, 0.2, -3.4, 0.1, 1.2, -0.4, 0, -0.5, 0, 0.1, 0,0,0.1, 0,-0.1,0,0,0,0,0,0,0,0,0,0, -0.1, 0, 0.1, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


double cos_lng[63] = {92025, 5736, 977, -895, 54, -7, 224, 200, 129, -95,0,-70,-53,0,  -33, 26, 32, 27, 0, -24, 16,13,0,-12,0,0,-10,0,-8,7,9,7,6,0,5,3,-3,0,3,3,0,-3,-3,3,3,0,3,3,3, 0,0,0,0,0,0,0,0,0,0,0,0,0,0};

double  cdelt[63] = {8.9, -3.1, -0.5, 0.5, -0.1, 0.0, -0.6, 0.0, -0.1, 0.3, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };



void nutate(double jd, double &nut_long, double &nut_obliq){
  static double t;
  
  t = (jd - 2451545)/36525.0; 
  
  double d;
  d = poly(t,coeff1,4)*DEGtoRAD;
  cirrange(d);
  
  double M;
  M = poly(t,coeff2,4)*DEGtoRAD;
  cirrange(M);

  double mprime;
  mprime = poly(t,coeff3,4)*DEGtoRAD;
  cirrange(mprime);

  double f;
  f = poly(t,coeff4,4)*DEGtoRAD;
  cirrange(f);

  double omega;
  omega = poly(t,coeff5,4)*DEGtoRAD;
  cirrange(omega);
  
  double arg[63];
  int i;
  double sarg[63], carg[63];
  nut_long = nut_obliq = 0;
  for(i=0;i<63;i++){
    arg[i] = d*d_lng[i]+M*m_lng[i]+mprime*mp_lng[i]+f*f_lng[i]+omega*om_lng[i];
    sarg[i] = sin(arg[i]);
    carg[i] = cos(arg[i]);
    nut_long +=  (sdelt[i]*t+sin_lng[i])*sarg[i];
    nut_obliq += (cdelt[i]*t+cos_lng[i])*carg[i];
  }

  nut_long *= 0.0001;
  nut_obliq *= 0.0001;
  
};



void cirrange(double angle){
  angle = fmod(angle, 2*M_PI);
  if(angle<0) angle+=2*M_PI;
  return;
};


double poly(double x, double *c, int n){
  static double val;
  static int i;
  val = c[n-1];
  for(i = n-1;i>0;i--) val= val*x+c[i-1];
  return val;  
};

double cst[4] = {280.46061837, 360.98564736629, 0.000387933, 38710000.0 };


double ct2lst(double lon, double jd){
  static double theta,t0,t,ret;
  t0 = jd-2451545.0;
  t = t0/36525.0;

  theta = cst[0] + (cst[1] * t0) + t*t*(cst[2] - t/ cst[3] );

  ret = (theta + lon)/15.0;
 ret = fmod(ret,24);
 if(ret<0) ret+= 24;

return ret;
}



void co_aberration(double jd, double ra, double dec, double &dra, double &ddec, double eps){

   double T,k;
   T = (jd-2451545.0)/36525.0;
   double e,pi;
   double sunlon, sunra, sundec;

   sunpos(jd,sunra,sundec,sunlon);



e = 0.016708634 - 0.000042037*T - 0.0000001267*T*T;

pi = 102.93735 + 1.71946*T + 0.00046*T*T;
 k = 20.49552; //;constant of aberration, in arcseconds

static double cd,sd,ce,te,cp,sp,cs,ss,ca,sa;
static double term1,term2,term3,term4;

   cd = cos(dec*DEGtoRAD);
   sd = sin(dec*DEGtoRAD);
   ce = cos(eps);
   te = tan(eps);
   cp = cos(pi*DEGtoRAD);
   sp = sin(pi*DEGtoRAD);
   cs = cos(sunlon*DEGtoRAD);
   ss = sin(sunlon*DEGtoRAD);
   ca = cos(ra*DEGtoRAD);
   sa = sin(ra*DEGtoRAD);

term1 = (ca*cs*ce+sa*ss)/cd;
term2 = (ca*cp*ce+sa*sp)/cd;
term3 = (cs*ce*(te*cd-sa*sd)+ca*sd*ss);
term4 = (cp*ce*(te*cd-sa*sd)+ca*sd*sp);

dra = -k * term1 + e*k * term2;
ddec = -k * term3 + e*k * term4;



  return;
};

void sunpos(double jd, double &ra, double &dec, double &longmed){
 
  static double t;
  t = (jd - 2415020)/36525.0;

  //;  form sun's mean longitude
  static double l;
 l = (279.696678+fmod((36000.768925*t),360.0))*3600.0;

 //;  allow for ellipticity of the orbit (equation of centre)
 //;  using the Earth's mean anomoly ME
 double me,ellcor;
 me = 358.475844 + fmod((35999.049750*t), 360.0);
 ellcor  = (6910.1 - 17.2*t)*sin(me*DEGtoRAD) + 72.3*sin(2.0*me*DEGtoRAD);
 l = l + ellcor;

 //; allow for the Venus perturbations using the mean anomaly of Venus MV
 static double mv, vencorr;
 mv = 212.603219 + fmod((58517.803875*t),360.0);
 vencorr = 4.8 * cos((299.1017 + mv - me)*DEGtoRAD) + 5.5 * cos((148.3133 +  2.0 * mv  -  2.0 * me )*DEGtoRAD) +  2.5 * cos((315.9433 +  2.0 * mv  -  3.0 * me )*DEGtoRAD) + 1.6 * cos((345.2533 +  3.0 * mv  -  4.0 * me )*DEGtoRAD) + 1.0 * cos((318.15   +  3.0 * mv  -  5.0 * me )*DEGtoRAD);
l = l + vencorr;

//;  Allow for the Mars perturbations using the mean anomaly of Mars MM

static double mm, marscorr; 
 mm = 319.529425  +  fmod(( 19139.858500 * t) ,  360.0 );
 marscorr = 2.0 * cos((343.8883 -  2.0 * mm  +  2.0 * me)*DEGtoRAD ) +   1.8 * cos((200.4017 -  2.0 * mm  + me) * DEGtoRAD);
 l = l + marscorr;
 //; Allow for the Jupiter perturbations using the mean anomaly of
 //; Jupiter MJ

static double mj, jupcorr;
 mj = 225.328328  +  fmod(( 3034.6920239 * t) ,  360.0 );
 jupcorr = 7.2 * cos(( 179.5317 - mj + me )*DEGtoRAD) + 2.6 * cos((263.2167  -  mj ) *DEGtoRAD) + 2.7 * cos(( 87.1450  -  2.0 * mj  +  2.0 * me ) *DEGtoRAD) + 1.6 * cos((109.4933  -  2.0 * mj  +  me ) *DEGtoRAD);
 l = l + jupcorr;

 //; Allow for the Moons perturbations using the mean elongation of
 //; the Moon from the Sun D

static  double d, mooncorr;
 d = 350.7376814  + fmod(( 445267.11422 * t),360.0 );
 mooncorr  = 6.5 * sin(d*DEGtoRAD);
 l = l + mooncorr;

 //; Allow for long period terms
 static double longterm;
 longterm  = + 6.4 * sin(( 231.19  +  20.20 * t )*DEGtoRAD);
 l  =    l + longterm;
 l  =  fmod(( l + 2592000.0),  1296000.0);
 longmed = l/3600.0;

 //; Allow for Aberration

 l  =  l - 20.5;

 //; Allow for Nutation using the longitude of the Moons mean node OMEGA

 static double omega;
 omega = 259.183275 - fmod(( 1934.142008 * t ) , 360.0 );
 l  =  l - 17.2 * sin(omega*DEGtoRAD);

 //; Form the True Obliquity
 static double oblt;
 oblt  = 23.452294 - 0.0130125*t + (9.2*cos(omega*DEGtoRAD))/3600.0;

 //; Form Right Ascension and Declination

 l = l/3600.0;
 ra  = atan2( sin(l*DEGtoRAD) * cos(oblt*DEGtoRAD) , cos(l*DEGtoRAD) );

 if(ra<0) ra+=2*M_PI;
	   

 dec = asin(sin(l*DEGtoRAD) * sin(oblt*DEGtoRAD));
 
 
 ra = ra/DEGtoRAD;
 dec = dec/DEGtoRAD;
 return;
};

// premat

void premat(double equinox1, double equinox2, double r[3][3]){
  double t;
  t = 0.001*( equinox2 - equinox1);

  static double st,a,b,c;
  st = 0.001*( equinox1 - 1900);
  //;  Compute 3 rotation angles
  
  a = SECtoRAD * t * (23042.53 + st*(139.75 +0.06*st) + t*(30.23 - 0.27*st+18.0*t));
   
  b = SECtoRAD * t * t * (79.27 + 0.66*st + 0.32*t) + a;
  
  c = SECtoRAD * t * (20046.85 - st*(85.33 + 0.37*st)+t*(-42.67 - 0.37*st -41.8*t));
  
  static double sina,sinb,sinc, cosa,cosb,cosc;
  
  sina = sin(a);
  sinb = sin(b);
  sinc = sin(c);
  cosa = cos(a);
  cosb = cos(b);
  cosc = cos(c);
  
  r[0][0] = cosa*cosb*cosc-sina*sinb;
  r[1][0] = sina*cosb+cosa*sinb*cosc;
  r[2][0] = cosa*sinc;
  
  r[0][1] = -cosa*sinb-sina*cosb*cosc;
  r[1][1] =  cosa*cosb-sina*sinb*cosc;
  r[2][1] = -sina*sinc;
   
  r[0][2] = -cosb*sinc;
  r[1][2] =  -sinb*sinc;
  r[2][2] =  cosc;
  
};



void precess(double &ra, double &dec, double equinox1, double equinox2){
  // convert RA DEC to radians


  static double ra_rad, dec_rad;
  ra_rad = 15*ra*DEGtoRAD;
  dec_rad = dec*DEGtoRAD;
  

  static double a;
  a = cos( dec_rad );

  static double x[3];

  x[0]=a*cos(ra_rad);
  x[1]= a*sin(ra_rad);
  x[2]= sin(dec_rad);
 

  double r[3][3];
 

  //; Use PREMAT function to get precession matrix from Equinox1 to Equinox2

  premat(equinox1, equinox2, r);


  static double x2[3];

  x2[0]=x2[1]=x2[2]=0;

  static int i;

  for(i=0;i<3;i++){
    x2[0] += r[0][i]* x[i];
    x2[1] += r[1][i]* x[i];
    x2[2] += r[2][i]* x[i];
  };


  ra= atan2(x2[1],x2[0]);
  dec = asin(x2[2]);

  dec = dec/DEGtoRAD;
  ra = ra/DEGtoRAD;
  
  if(ra<0) ra+=360;
  /*
    #ifdef VERBOSE
    printf("\n precess()\n");
    printf("===========\n");
    printRA(ra/15);
    printf("DEC  =\t%lf\n\n",dec);
    #endif
  */
  
	      
};


void printRA(double ra){
  static int h,m;
  float s;
  h = (int) ra;
  ra = ra-h;
  m = (int) (60.0*ra);
  ra = ra-m/60.;
  s = ra*3600.;
  printf("RA    =\t%d h %d m %02.2f s\n",h,m,s);
};

void printDEGtoH(double ra){
  static int h,m;
  float s;
  h = (int) ra;
  ra = ra-h;
  m = (int) (60.0*ra);
  ra = ra-m/60.;
  s = ra*3600.;
  printf("    =\t%d h %d m %02.2f s\n",h,m,s);
};

void printDec(double dec){
  static int h,m;
  float s;
  h = (int) dec;
  dec = dec-h;
  m = (int) (60.0*dec);
  dec = dec-m/60.;
  s = dec*3600.;
  printf("DEC   =\t%d d %d m %02.2f s\n",h,m,s);
};

