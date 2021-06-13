#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "printMes.h"
#include "receiver.h"
#include "getMes.h"
#include "sender.h"


#define MSG_MAX_LEN 500 


static pthread_t print_Thread;
static pthread_cond_t* pCond2;
static pthread_mutex_t* pMutex;
static List* pList2;
static bool stop;


void* printThread(void* unused){


    while(1){

        pthread_mutex_lock(pMutex);
        {
            pthread_cond_wait(pCond2,pMutex);
            char* message = List_remove(pList2);

            if(strcmp(message, "!") == 0){
                stop = true;
            }

            puts(message);
            free(message);
            message = NULL;
		}

    	pthread_mutex_unlock(pMutex);

        if(stop){
            break;
        }
    }

    printer_shutdown_sequence();

}

void Printer_init(pthread_mutex_t* mutex, pthread_cond_t* cond2, List* list2){
    stop = false;
    pCond2 = cond2;
    pMutex = mutex;
    pList2 = list2;

    pthread_create(&print_Thread, NULL, printThread, NULL);
}

void print_Mes_shutDown(void){
    pthread_cancel(print_Thread);
}

void printer_shutdown_sequence(void){
    print_Mes_shutDown();
    Receiver_shutDown();
    Sender_shutDown();
    get_Mes_shutDown();
    pthread_mutex_destroy(pMutex);
}

void print_Mes_join(void){
    pthread_join(print_Thread, NULL);
}
