#pragma once
#include <math.h>

#define DEGtoRAD (M_PI/180.)
#define RADtoDEG (180./M_PI)
#define HRtoRAD (M_PI/12.0)
#define HRtoDEG 15.0
#define DEGtoAS  (M_PI/180/3600.)
#define SECtoRAD (M_PI/180/3600.)


double JulianDay();

void eq2hor(double ra, double dec, double jd, double lat, double lon,  double &alt, double &az);


void hadec2altaz(double ha, double dec, double lat, double &alt, double &az);

double poly(double x, double *c, int n);
void cirrange(double angle);

void nutate(double jd, double &nut_long, double &nut_obliq);


void co_nutate(double jd, double ra, double dec, double &dra, double &ddec, double &eps, double &dpsi, double &deps);

void co_aberration(double jd, double ra, double dec, double &dra, double &ddec, double eps);

void precess(double &ra, double &dec, double yr, double jd);

double ct2lst(double lon, double jd);

void sunpos(double jd, double &ra, double &dec, double &longmed);

void premat(double equinox1, double equinox2, double *r);

void printRA(double ra);

void printDec(double dec);

void printDEGtoH(double ra);
