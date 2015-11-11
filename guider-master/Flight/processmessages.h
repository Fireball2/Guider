//#define DONOTPROCESSMPF

message m;

unsigned char mID;


float tmpEL, tmpCE, tmpEL1, tmpCE1, tmpValue;

unsigned char tmpMeasType, tmpValid1, tmpOpState, tmpPointControl, tmpRoiFlag;

double tmpRA, tmpDEC, tmpPS, tmpRot;

int tX, tY, tDX, tDY;

unsigned short tmpRoiCX, tmpRoiCY, tmpRoiD, tmpRoiX, tmpRoiY, tmpRoiDX, tmpRoiDY;
unsigned char tmpTrigT, tmpBr, tmpCt, tmpGa;
unsigned int tmpExpT;

unsigned int tmpGain;

bool sameModeChange = false;

short int tmpOffset;


char tgEL, tgCE;

unsigned char tmpMode, tmpSatVal, tmpDACoutput;

short tmpMove;

double tmpFBGain, tmpThreshold, tmpReadoutOffset, tmpReadoutNoise;
int tmpDistTol;
unsigned int tmpGrid, tmpCenBox, tmpApBox;

unsigned char tmpBlob;

unsigned short tmptpx, tmptpy;

float tmpstp;


AscAttitude asc;

LockedFC lFC;
LockedROI lROI;
GuidingFC gFC;
GuidingROI gROI;
WindowTrack wt;

CamTime ct;
