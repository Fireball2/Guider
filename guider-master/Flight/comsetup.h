#ifndef COMSETUP_H_
#define COMSETUP_H_

#define MOTORACCCURRENT 40
#define MOTORRUNCURRENT 30
#define MOTORMUNIT 10

#include "messages.h"

extern bool datatompf;

int ComSetup();
void serial_signal_IO(int status);
void ComClose();

void SetMessageLengths();

int SendToMPFNum(message m);
int SendToTMTC(message m);
int SendToDet(message m);
int SendToDet(unsigned char *ptr, unsigned int lngth);
int SendToStage(char *msg, short lng);

void ReadMPF();
void ReadTMTC();


void MPFOpen();
void MPFClose();
void GPSRead();


void MoveStage(short mv);
void RunSillyProg();
void HomeCamera();
#endif
