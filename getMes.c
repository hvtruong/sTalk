#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "getMes.h"


#define MSG_MAX_LEN 500 

static pthread_t get_Mes_Thread;
static pthread_mutex_t* pMutex;
static pthread_cond_t* pCond1;
static char buffer[MSG_MAX_LEN];
List* pList1;


void* getMesThread(void* unused){

	while(1){
		
		memset(buffer,0,strlen(buffer));
		fgets(buffer, MSG_MAX_LEN, stdin);

		int strlen = 0;
		while(buffer[strlen] != '\n'){
			strlen += 1;
		}

		char* message = (char*)malloc(strlen*sizeof(char));

		for(int i = 0; i < strlen; i++){
			message[i] = buffer[i];
		}

		pthread_mutex_lock(pMutex);
		{
			List_append(pList1, message);
			pthread_cond_signal(pCond1);
			
		}
    	pthread_mutex_unlock(pMutex);
	}

}

void get_Mes_init(pthread_mutex_t* mutex, pthread_cond_t* cond1, List* list1){

	pMutex = mutex;
	pCond1 = cond1;
	pList1 = list1;

    pthread_create(&get_Mes_Thread, NULL, getMesThread, NULL);
}

void get_Mes_shutDown(void){
	pthread_cancel(get_Mes_Thread);
	pthread_cond_destroy(pCond1);
}

void get_Mes_join(void){
	pthread_join(get_Mes_Thread, NULL);
}