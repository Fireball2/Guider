#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "ringbuffer.h"

class message{
 public:
  message();
  message(char *bod, unsigned short lng);
  ~message();
  
 


  unsigned char body[256];			// the body of the message
  unsigned short length;			// the length of the message
  unsigned char ID;					// message ID
  unsigned char valid;				// is the CRC okay?
  unsigned char source;				/* the source of the message. This might not be used.
								   0: MPF NUM
								   1: Ground (TMTC)
								   2: detector computer (probably unused)
								   3: generated by guider 
									4: other.
									*/


  unsigned char ReadID();			//Returns the message ID.
  unsigned char IsValid();			//Checks the validity of the message
  unsigned char FindCRC();			//Finds the CRC of the message



  /********************************************/

};

  void CopyMessage(message &dest, message source);					// copies the source message to the destination
unsigned char IsValid(unsigned char *str, unsigned short lng);		// checks whether the char buffer pointed to by str
unsigned char IsValid(RingBuffer &rb, unsigned short lng); // check if a ringbuffer contains a valid message in the first lng characters

																	// is a valid message




#endif
