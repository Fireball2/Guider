#include "skytime.h"
#include "ChipAndSky.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "gps_func.h"

/*
This program is a rewrite in c of the jdcnv.pro idl astro routine
*/

// inputs
// long int year, month, day 
// double ut

// outputs
// double julianday;

double JulianDay(){
  long int L;
  long int jd;
  double julianday;

  static time_t timenow;
  static tm *tim;  
  unsigned int year, month,day, hour, minute;
  double second;
  double ut;

  if(isGpsActive()){
    year = gps_year;
    month = gps_month;
    day = gps_day;
    ut = gps_utc_time;
    hour = (int) ut/10000;;
    minute = (int) ((ut-10000*hour)/100);
    second = (ut-10000*hour - 100*minute);
  } else {
    time(&timenow);
    tim = gmtime(&timenow);
    year = tim->tm_year + 1900;
    month = tim->tm_mon+1;
    day = tim->tm_mday;
    hour = tim->tm_hour %24;
    minute = tim->tm_min;
    second = (float) tim->tm_sec;
  };


  if(month<1 || month>12) return 0;
  if(day < 1 || day > 31) return 0;

  L = (month-14)/12;
  
  jd = day - 320751 + 14611*(year+4800+L)/4 + 3671*(month-2-L*12)/12- 3*((year+4900+L)/100)/4;

  julianday = 1.0*jd + (ut/24) - 0.5;
  
 
  return julianday;


};


#include <time.h>
#include <stdio.h>
/*
int main(){
  int startt, endt;
  double alt,az;
  int j;
  startt = clock(); 
  for(j=0;j<1;j++)
  eq2hor(6.682833,9.895555,2460107.250, 50.52666, 6.855,
alt,az); 
  endt = clock();

  double ra = 2.52952778;
  double dec = 89.264055;
  precess(ra,dec,2000,1985);

  printf("Calculation lasted: %lfs\n",(endt-startt)*1.0/CLOCKS_PER_SEC);
  printf("Alt = %lf\n",alt);
  printf("AZ = %lf\n",az);


  double d_ra, d_dec, eps, d_psi, d_eps;
  double jd;
  jd = JulianDay(2028,11,13,.19*24);
  printf("jd = %lf\n",jd);
  co_nutate(2462088.5,2.769814167*15,49.3484833, d_ra, d_dec, eps, d_psi, d_eps);

  printf("%lf, %lf, %lf, %lf, %lf\n",d_ra,d_dec, eps, d_psi, d_eps);
  
  co_aberration(2462088.5,2.769814167*15,49.3484833, d_ra, d_dec, eps);

  printf("%lf :: %lf\n",d_ra,d_dec);


  jd = JulianDay(1987,4,10,0);
  printf("jd = %lf\n",jd);
  d_psi = d_eps = 0;
  nutate(2446895.5, d_psi,d_eps);
  printf("%lf %lf\n",d_psi, d_eps);

  return 0;
};
*/
