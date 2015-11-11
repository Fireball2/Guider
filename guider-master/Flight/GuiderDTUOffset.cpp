#include "GuiderDTUOffset.h"
#include "math.h"
#include "coords.h"

double _GuiderDTUOffsetRA, _GuiderDTUOffsetDEC;

Coords _GuiderDTUOffset;

void SetGuiderDTUOffset(double ra, double dec){
  _GuiderDTUOffset.RA = ra;
  _GuiderDTUOffset.DEC = dec;
};

void GetGuiderDTUOffset(double &ra, double &dec){
  ra = _GuiderDTUOffset.RA;
  dec = _GuiderDTUOffset.DEC;
};

double GetGuiderDTUOffsetRA(){
  return _GuiderDTUOffset.RA;
};

double GetGuiderDTUOffsetDEC(){
  return _GuiderDTUOffset.DEC;
};
