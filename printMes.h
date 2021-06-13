#ifndef _PRINTMES_H
#define _PRINTMES_H
#include <pthread.h>
#include <sys/socket.h>
#include "list.h"

void* printThread(void* unused);

void Printer_init(pthread_mutex_t* mutex, pthread_cond_t* cond2, List* list2);

void print_Mes_shutDown(void);

void printer_shutdown_sequence(void);

void print_Mes_join(void);

#endif