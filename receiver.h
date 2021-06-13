#ifndef _RECEIVER_H
#define _REVEIVER_H
#include <pthread.h>
#include <sys/socket.h>
#include "list.h"

void* receiveThread(void* notUsed);

void Receiver_init(pthread_mutex_t* mutex, pthread_cond_t* cond2, List* list2, int* socketDescriptor, struct sockaddr* sinRemote);

void Receiver_shutDown(void);

void Receiver_join(void);

#endif