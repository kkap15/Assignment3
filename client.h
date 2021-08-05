#ifndef CLIENT_H
#define CLIENT_H

#include "comms.h"
#include "shared.h"
#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <sys/socket.h>
#include <ctype.h>
#include <semaphore.h>

//Client struct declaration
//Client struct that holds the information of client.
typedef struct {
    char *name;
    char *port;
    char *authString;
    
    FILE *fromServer;
    FILE *toServer;
    FILE *fromUser;
    
    int serverSocket;
    pthread_mutex_t clientLock;
    pthread_mutex_t serverLock;
    sem_t clientSem;
    char *nameSuffix;
    int okCount;
} Client;

// Enum to store different error codes for the Client.
typedef enum {
    OK = 0,
    BAD_ARGS = 1,
    COMMS_ERR = 2,
    KICKED = 3,
    AUTH_ERR = 4
} ClientError;

#endif //ass4_client_h
