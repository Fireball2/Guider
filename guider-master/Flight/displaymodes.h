#pragma once

// these classes contain methods for choosing which of the display
// parameters to show on the screen during each mode

class InitMD{
 private:
 public:
  InitMD(){};
  ~InitMD(){};

};


class AutocollimationMD{
 private:
 public:
  AutocollimationMD(){
    lastexists = false;
    zoom = true;
    };
  ~AutocollimationMD(){};
  float DTUposEL;
  float DTUposCE;
  float offsetEL, offsetCE;
  float offsetx, offsety;
  float xpos, ypos;
  float lastx, lasty;
  float rmsr;
  bool lastexists;
  bool zoom;
};

class AlignmentMD{
 private:
 public:
  AlignmentMD(){};
  ~AlignmentMD(){};
  bool inRange; // used to designate whether the sky coordinates are
		// within range of the chip
  float DTUpeEL, DTUpeCE;
  float offsetEL, offsetCE;
  float rmsx, rmsy;
  float centX, centY;
  float rsq;
  float bgnd;
  short winx, winy, wind;
  float rsqx, rsqy;
  float flux;
  unsigned char submodeID;
};

class SlewingMD{
 private:
 public:
  SlewingMD(){};
  ~SlewingMD(){};

  bool inRange; // as in alignment mode 
    float DTUpeEL, DTUpeCE;
};


class PointingMD{
 private:
 public:
  PointingMD(){};
  ~PointingMD(){};
  float DTUpeEL, DTUpeCE;
};

class DisplayMD{
 private:
 public:
  DisplayMD(){};
  ~DisplayMD(){};
  bool modeMismatch;
  bool sensorMismatch;
};
