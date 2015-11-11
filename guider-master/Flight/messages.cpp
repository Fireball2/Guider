#include "messages.h"
#include "ringbuffer.h"

//#include "globals.h"
#include <stdio.h>

#include <mcheck.h>



/*
messages.cpp
------------

Class for constructing messages passed between the various computers on the gondola system


Written by Matt Matuszewski, 2007.
*/


//--- Simple constructor for a message.
message::message(){
  length = 0;
};


//--- Constructor for message with a string and string length specified. no terminating /0
message::message(char *bod, unsigned short lng){
  length = lng;
  int i;

  for(i=0;i<lng;i++){
    body[i] = bod[i];
  };
};

//--- the destructor.
message::~message(){
};


//-- Compute the CRC for the message by XORing the appropriate bytes in the message
unsigned char message::FindCRC(){
  unsigned char crc;
  int i;

  crc = body[1];
  for(i=2;i<length-2;i++){
    crc = crc ^ body[i];
  }

#ifdef TESTMODE
  printf("Found CRC = %d\n",crc);
#endif
  return crc;
};


//--- Checks whether a passed string constitutes a valid message. 
unsigned char IsValid(unsigned char *str, unsigned short lng){
  message mess;
  int i;
  
  for(i=0; i<lng; i++) mess.body[i]=str[i];
  mess.length = lng;
  
  return mess.IsValid();
};

unsigned char IsValid(RingBuffer &rb, unsigned short lng){
  message mess;
  int i;
  for(i=0; i<lng; i++) mess.body[i] = rb[i];
  mess.length = lng;
  
  return mess.IsValid();

};


//--- Checks if a message is valid, returns 0 if Not Valid, 1 Otherwise.
unsigned char message::IsValid(){
#ifdef MEMCHECK
mtrace();
#endif
  unsigned char crc;
  unsigned short lng;
  unsigned short lg;
  unsigned short *plng;
  unsigned char *c1, *c2;

//--- front sync byte?
  if(body[0] != 0x55){
    return 0;
  };
//-- end sync byte?
  if(body[length-1] != 0xAA){
    //    printf("%d, %d\n",body[length-1],0xAA);
    //    printf("Err 2\n");
    return 0;
  };

  c1 = (unsigned char *)&lng;
  c2 = (unsigned char *)&lng+1 ;

  *c1 = body[3];
  *c2 = body[4];
    
  plng = (unsigned short *)&body[3];


  lng = *plng;
//--- valid length?
  if(length != (lng) + 7){
    return 0;
    
  };

//--- CRC OK?
  crc = FindCRC();
  if (crc != body[length-2]){
    //    printf("Read CRC: %d, Found CRC: %d\n",body[length-2],crc);
    //    printf("Err 4\n");
    return 0;
  };

  
  return 1;
};


//--- Copies one message to another.
void CopyMessage(message &dest, message source){
#ifdef MEMCHECK
mtrace();
#endif
  int i;

  dest.length = source.length;
  dest.ID = source.ID;
  dest.valid = source.valid;
  dest.source = source.source;
  
  for(i=0;i<256;i++){
    dest.body[i] = source.body[i];
  };
  

  return;
};





