#pragma once

#include "frameblob.h"
#include "QCam.h"




extern bool filesaving;

char StartLog();

char WriteToLog(char *logentry);
char WriteToLog(char *function, char *format, ...);
char FrameToLog(unsigned int framenumber, unsigned short width, unsigned short height, unsigned char *buffer);
void ReopenLog();
char ExtraFrameToLog(unsigned int framenumber, unsigned short x, unsigned short y,unsigned short width, unsigned short height, float bx, float by,unsigned int expt, unsigned char *buffer);


void InfoToLog(QCamera *qcam, frameblob *fb, unsigned long fn, unsigned short tx, unsigned short ty, float cx, float cy);

char FrameInfoToLog(unsigned int framenumber, unsigned short x, unsigned short y, unsigned short dx, unsigned short dy, unsigned short tx, unsigned short ty, unsigned char tt, unsigned int et, frameblob *fb);

void FlushLog();


char CloseLog();

#pragma pack(push)
#pragma pack(1)

typedef struct imageInfo{
  unsigned long framenumber; // the framenumber
  unsigned short expT; // the image exposure time
  unsigned char trigT; // the image trigger time 
  unsigned short targetX, targetY; // the target x and y for the image
  unsigned short wx, wy, wdx, wdy; // the window location and size
  unsigned char blobs;
  double bx[10], by[10]; // the blob coordinates
  float gpsTime; // the gps time
  float cpuTime; // computer time
};
#pragma pack(pop)



