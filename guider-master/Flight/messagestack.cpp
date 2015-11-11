#include "messagestack.h"
#include "writelog.h"
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include "messages.h"
#include <stdio.h>
#include "defines.h"


#include <mcheck.h>
#define MEMCHECK


QQueue<message> MessageQueue;
QMutex MessageQueueMutex;

int AddToMessageQueue(message m){
#ifdef MEMCHECK
mtrace();
#endif
    QMutexLocker locker(&MessageQueueMutex);
    MessageQueue.enqueue(m);
    int i;

#ifdef TESTMODE
    for(i=0;i<m.length;i++) printf("%d ",m.body[i]);
    printf("\n");
#endif

    //    printf("Stacked %d\n",MessageQueue.count());
    return 0;
};

int GetFromMessageQueue(message &m){
#ifdef MEMCHECK
mtrace();
#endif
    QMutexLocker locker(&MessageQueueMutex);

    if(!MessageQueue.isEmpty()){
      CopyMessage(m, MessageQueue.dequeue());

    return 0;
    } else {
      return -1;
      };
};

int MessageQueueCount(){
#ifdef MEMCHECK
mtrace();
#endif
  return MessageQueue.count();
  return 0;
};



