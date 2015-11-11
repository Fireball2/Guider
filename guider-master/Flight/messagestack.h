#ifndef MESSAGESTACK_H_
#define MESSAGESTACH_H_


#include "messages.h"


//void SetupMessageQueue();

int AddToMessageQueue(message m);
int GetFromMessageQueue(message &m);

int MessageQueueCount();


#endif
