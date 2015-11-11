// dither.h 
// fort sumner, nm. April 26th, 2009
// matt matuszewski (matmat@caltech.edu)

/* A way to add dithering and general motion control to the fireball
   guider software */




#pragma once

#define MAXDITHERSTEPS 10000


namespace dither { 


  
  extern bool isDithering; // are we actively dithering now?
  extern unsigned short patternID;

  void config(); 

  // warning -- in the interest of calling as few functions as
  // possible, there is no checking whether dithering is active when
  // 'add' is called. This check should be done in the calling function/
  void add(unsigned short &tx, unsigned short &ty); // alter the values of tx and ty 
  // to adjust for any dither that might be present. 
  void load(unsigned char pattern);
  void start();
  void stop();

  





}

