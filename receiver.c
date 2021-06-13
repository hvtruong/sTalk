#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include "receiver.h"

#define MSG_MAX_LEN 500 


static pthread_t receive_Thread;
static pthread_mutex_t* pMutex;
static pthread_cond_t* pCond2;
static List* pList2;
static int* pSocketDescriptor;
static struct sockaddr* pSinRemote; 
static char messageRx[MSG_MAX_LEN];


void* receiveThread(void* notUsed){


    while(1){

        memset(messageRx,0,strlen(messageRx));

        // blocking call - waiting to receive from sender 

        int sin_len = sizeof(struct sockaddr_in);
        
        int bytesRx = recvfrom(*pSocketDescriptor, messageRx, MSG_MAX_LEN, 0, (struct sockaddr *) pSinRemote, &sin_len);


        // dynamically allocate memory for the string 
        // copy it into the string
        // put the pointer to string into the queue
        char* receivedMessage = (char*)malloc((bytesRx+1)*sizeof(char));
        for(int i = 0; i < bytesRx; i++){
            receivedMessage[i] = messageRx[i];
        }
        receivedMessage[bytesRx] = '\0';

       
        pthread_mutex_lock(pMutex);
		{
            List_append(pList2, receivedMessage);
            pthread_cond_signal(pCond2);
		}
    	pthread_mutex_unlock(pMutex);
    }
  
  
}


void Receiver_init(pthread_mutex_t* mutex, pthread_cond_t* cond2, List* list2, int* socketDescriptor, struct sockaddr* sinRemote){

    pMutex = mutex;
    pCond2 = cond2;
    pList2 = list2;
    pSocketDescriptor = socketDescriptor;
    pSinRemote = sinRemote;

    pthread_create(&receive_Thread, NULL, receiveThread, NULL);
}

void Receiver_shutDown(void){
    pthread_cancel(receive_Thread);
    pthread_cond_destroy(pCond2);
}

void Receiver_join(void){
    pthread_join(receive_Thread, NULL);
}