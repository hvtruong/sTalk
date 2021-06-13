#ifndef _GETMES_H
#define _GETMES_H
#include <pthread.h>
#include <sys/socket.h>
#include "list.h"

void* getMesThread(void* unused);

void get_Mes_init(pthread_mutex_t* mutex, pthread_cond_t* cond, List* list1);

void get_Mes_shutDown(void);

void get_Mes_join(void);

#endif