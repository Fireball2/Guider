#ifndef DAC_H_
#define DAC_H_

extern float TrackerXOffset, TrackerYOffset, CardXOffset, CardYOffset;
extern unsigned short adcValue;
extern  float leakylvdt;


void InitDAC();

void SetELMultiplier();
void SetCEMultiplier();


bool getOutput();


float getAdcValue();

// arguments passed here are in arcseconds and are converted to 
// a number. 
void SendOutEL(float el);
void SendOutCE(float ce);


void setOutputZero();
void setOutputTrue();

void outPortB(unsigned char b);
void outPortA(unsigned char a);

#endif
