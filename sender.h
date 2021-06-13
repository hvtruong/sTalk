#ifndef _SENDER_H
#define _SENDER_H
#include <pthread.h>
#include <sys/socket.h>
#include "list.h"

void* sendThread(void* unused);

void Sender_init(pthread_mutex_t* mutex, pthread_cond_t* cond1, List* list1, int* socketDescriptor, struct sockaddr* sinRemote);

void Sender_shutDown(void);

void sender_shutdown_sequence(void);

void Sender_join(void);

#endif