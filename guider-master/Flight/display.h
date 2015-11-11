#pragma once

#include "QCam.h"
#include "frameblob.h"


extern short zoom_type;

void StartDisplay();

void RestoreDisplay();

void SetVPalette(unsigned char Br, unsigned char Ct, unsigned char Ga);


void FontOverwrite();
void FontMask();


// puts a small box in the appropriate place
void PlaceSmallBox(QCamera *cam, unsigned char *buffer, void *spots, int error, int qID);
void PlaceFullChip(QCamera *cam, unsigned char *buffer, frameblob &fblob, int error);
void PlaceFullChip(QCamera *cam, unsigned char *buffer, frameblob &fblob, double rms, int error);


// This coordinates given here are IN CHIP COORDINATES!!!!
void DrawDiamond(unsigned short x, unsigned short y, unsigned short sz, unsigned int color, bool offset);
void DrawSquare(unsigned short x, unsigned short y, unsigned short sz, unsigned int color, bool offset);
void DrawRectangle(unsigned short x, unsigned short y, unsigned short dx, unsigned short dy, unsigned int color, bool offset);
void DrawCircle(unsigned short x, unsigned short y, unsigned short r, unsigned int color, bool offset);
void DrawCross(unsigned short x, unsigned short y, unsigned short sz, unsigned int color, bool offset);
//The coords given below are in screen coordinates

void gl_drawSquare(unsigned short x, unsigned short y, unsigned short sz, unsigned int color);
void gl_drawRectangle(unsigned short x, unsigned short y, unsigned short dx, unsigned short dy, unsigned int c);
void gl_drawDiamond(unsigned short x, unsigned short y, unsigned short sz, unsigned int color);
void gl_drawCross(unsigned short x, unsigned short y, unsigned short sz, unsigned int color);

int gl_gray(unsigned char gry);

void TestColors();

void ClearScreen();

typedef struct Point{
  float  x,y;
};


