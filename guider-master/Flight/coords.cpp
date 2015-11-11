#include "coords.h"
#include <math.h>


Coords::Coords(){
};


Coords::Coords(double ra, double dec){

  RA = ra;
  DEC = dec;
};


Coords::~Coords(){
};


void Coords::Set(double ra, double dec){
  
  RA = ra;
  DEC = dec;

  return;
};

void Coords::SetRA(double ra){

  RA = ra;
  return;
};

void Coords::SetDEC(double dec){

  DEC = dec;

  return;
};

void Coords::GetFloat(double &ra, double &dec){

  ra=RA;
  dec = DEC;
  
  return;

};

double  Coords::GetFloatRA(){
  return RA;
};

double  Coords::GetFloatDEC(){
  return DEC;
};


void Coords::operator=(Coords c){
  RA = c.RA;
  DEC = c.DEC;
  return;
};

char Coords::operator==(Coords c){
  return ( (RA == c.RA) && (DEC = c.DEC));
};




