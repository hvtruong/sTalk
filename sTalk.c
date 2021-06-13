#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "getMes.h"
#include "sender.h"
#include "receiver.h"
#include "printMes.h"
#include "list.h"

#define MSG_MAX_LEN 500 


int main(int argc, char** argv){

    int myPort = atoi(argv[1]);
    char* otherIP = argv[2];
    int otherPort = atoi(argv[3]);

    if(myPort > 65535 || myPort <= 1024 || otherPort > 65535 || otherPort <= 1024){
        printf("Invalid Port(s)! \n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

    int error1 = pthread_mutex_init(&mutex, NULL);
    int error2 = pthread_cond_init(&cond1, NULL);
    int error3 = pthread_cond_init(&cond2, NULL);

    if(error1 != 0 || error2 != 0 || error3 != 0){
        printf("Mutex/condition variable error! \n");
        exit(EXIT_FAILURE);
    }

    List* list1 = List_create();
    List* list2 = List_create();

    if(list1 == NULL || list2 == NULL){
        printf("Linked list error! \n");
        exit(EXIT_FAILURE);
    }

    // setup socket and local port
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // user IP address of the local machine
    sin.sin_port = htons(myPort);


    // remote port information 
    struct addrinfo hints;
    struct addrinfo* res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM;
    int error4 = getaddrinfo(otherIP, argv[3], &hints, &res);

    if(error4 != 0){
        printf("IP address error! \n");
        exit(EXIT_FAILURE);
    }


    // Other method, not used
    // struct sockaddr_in sinRemote;  
    // memset(&sinRemote, 0, sizeof(sinRemote));
    // sinRemote.sin_family = AF_INET;
    // inet_pton(AF_INET, res->ai_addr->sa_data, &(sinRemote.sin_addr)); // use IP address of other machine 
    // // sinRemote.sin_addr.s_addr = htonl(res->ai_addr->sa_data); // user IP address of the local machine
    // sinRemote.sin_port = htons(otherPort);


    unsigned int sin_len = sizeof(sin);
    unsigned int remoteSin_len = sizeof(res->ai_addr);


    // see if socket properly initalized, bind 
    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0); 
    if(socketDescriptor == -1){
        printf("Socket failed! \n");
        exit(EXIT_FAILURE);
    }

    int error5 = bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(struct sockaddr_in)); 

    if(error5 != 0){
        printf("Binding failed! \n");
        exit(EXIT_FAILURE);
    }


    printf("Connecting: \n");

    // create the threads
    // Sender_init(&mutex, &cond1, list1, &socketDescriptor, &sinRemote); // other method, not used
    Sender_init(&mutex, &cond1, list1, &socketDescriptor, res->ai_addr);
    Printer_init(&mutex, &cond2, list2);
    get_Mes_init(&mutex, &cond1, list1);
    // Receiver_init(&mutex, &cond2, list2, &socketDescriptor, &sinRemote); // othher method, not used
    Receiver_init(&mutex, &cond2, list2, &socketDescriptor, res->ai_addr);


    // join threads
    Sender_join();   
    print_Mes_join();
    get_Mes_join();
    Receiver_join();    


    // close socket 
    close(socketDescriptor); 

    free(res);

    return 0;
}