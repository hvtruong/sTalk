#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include "printMes.h"
#include "receiver.h"
#include "getMes.h"
#include "sender.h"

#define MSG_MAX_LEN 500

static pthread_t send_Thread;
static pthread_mutex_t* pMutex;
static pthread_cond_t* pCond1;
static List* pList1;
static int* pSocketDescriptor;
static struct sockaddr* pSinRemote;
static bool stop;


void* sendThread(void* unused){
    while(1){

        pthread_mutex_lock(pMutex);
    	{

            pthread_cond_wait(pCond1,pMutex);
            char* message = List_remove(pList1);

            if(strcmp(message, "!") == 0){
                stop = true;
            }

            int size = strlen(message);
            sendto(*pSocketDescriptor, message, size, 0, (struct sockaddr *) pSinRemote, sizeof(struct sockaddr_in));
            memset(message, 0, size);
            free(message);
            message = NULL;
            
        }
    	pthread_mutex_unlock(pMutex);

        if(stop){
            break;
        }
    }

    sender_shutdown_sequence();
}



void Sender_init(pthread_mutex_t* mutex, pthread_cond_t* cond1, List* list1, int* socketDescriptor, struct sockaddr* sinRemote){
    stop = false;

    pMutex = mutex;
    pCond1 = cond1;
    pList1 = list1;
    pSocketDescriptor = socketDescriptor;
    pSinRemote = sinRemote;

    pthread_create(&send_Thread, NULL, sendThread, NULL);

}

void Sender_shutDown(void){
    pthread_cancel(send_Thread);
}

void sender_shutdown_sequence(void){
    Sender_shutDown();
    get_Mes_shutDown();
    print_Mes_shutDown();
    Receiver_shutDown();
    pthread_mutex_destroy(pMutex);
}

void Sender_join(void){
    pthread_join(send_Thread, NULL);
}