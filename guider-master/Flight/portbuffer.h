/*
PortBuffer class.

sets up a port buffer of a particularsize, a start character and 
allows for functions to access the members to check for valid messages
*/

#pragma once

#include "messages.h"
#include "ringbuffer.h"

class PortBuffer{
 public:
  RingBuffer ring;
  char message[255];
  unsigned short count;
  char firstchar;
  char lastchar;
  unsigned short maxlength;


  PortBuffer(){
    // there are no messages longer than 60 characters. 
    maxlength = 60;
    
  };
  ~PortBuffer(){};

  void setFirstChar(char fc){
    firstchar = fc;
  };

  void setLastChar(char lc){
    lastchar = lc;
  };

  void setMaxLength(unsigned short ml){
    maxlength = ml;
  };


  // finds the first instance of the first character
  // and removes everything preceding it.
  // finds the first instance of the last character. 
  // i.e. say all messages start with '$' and end with 
  // '\n', then this cookie cutter can mark the string 
  
  int findFirst(){
    int i;
    int j=0;
    for(i=0;i<ring.used;i++){
      if(ring.buffer[i] == firstchar){
	j=i;
	break;
      };
    };
    ring.strip(i);

    return 0;
  }; 


  int findLast(){
    int i;
    int j=0;
    for(i=0;i<ring.used;i++){
      if(ring.buffer[i] == lastchar){
	j=i;
	break;
      };
    };

    return j;

  };

  
};


