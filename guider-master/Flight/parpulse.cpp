#include <sys/io.h>
#include <stdlib.h>
#include <stdio.h>

#define PARBASE 0x378

void ParPulse(unsigned char value, unsigned int length){
	unsigned int i,j;
	ioperm(PARBASE,24,1);

	outb(value,PARBASE);
	for(i=0; i<length; i++){
	 for(j=0;j<length;j++);
	};
	outb(0,PARBASE);

};
