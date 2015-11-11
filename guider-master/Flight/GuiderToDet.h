#pragma once 


#define GUIDERTRACKING 0x02
#define CAMERAERROR 0x01


void FrameNumberToDetector(unsigned int fn);
void StatusToDetector(bool gdrsensor, bool valid);
void GtoD_PortB(unsigned char b);
void GtoD_PortA(unsigned char a);

  unsigned short  getDetCount();


#pragma pack(push)
#pragma pack(1)
class DetectorHello{
 public:
  // message variables
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned char val;
  unsigned char crc;
  unsigned char syncend;
  // useful variables
  unsigned char totlength;
  unsigned char *ptr;
  
  DetectorHello();
  ~DetectorHello();
  void send(unsigned char vl);
  void setCRC();

};


class FrameInfo{
 public:
  // first the values that will be sent
  // the order of these in this structure MATTERS
  unsigned char syncstart;
  unsigned short count;
  unsigned short length;
  unsigned char id;
  unsigned int framenumber;
  float ELOffset;
  float CEOffset;
  unsigned int ExpT;
  unsigned char status;
  unsigned char crc;
  unsigned char syncend;
  // other variables
  unsigned char totlength;
  unsigned char *ptr;
  static unsigned short mcount;
  static bool initcount;
  short *psEL, *psCE;
  short sEL, sCE;


  FrameInfo();
  ~FrameInfo();
  //  void send(unsigned int fn, float el, float ce, unsigned int et, unsigned char valid);
  void send(unsigned int fn, float lvdt, float el, float ce, unsigned int et, unsigned char valid);
  void setCRC();
};




#pragma pack(pop)

