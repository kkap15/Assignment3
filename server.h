#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <semaphore.h>
#include "shared.h"
#include "client.h"
#include "comms.h"

#define CLIENT_HEAD "@CLIENTS@\n"
#define SERVER_HEAD "@SERVER@\n"

typedef struct Server Server;

typedef struct Clients Clients;

// struct to store the client message count
typedef struct {
    int msgCount;
    int kickCount;
    int listCount;
} ClientMessageCount;

// struct to store the server message count.
typedef struct {
    int authCount;
    int nameCount;
    int msgCount;
    int kickCount;
    int listCount;
    int leaveCount;
} ServerMessageCount;

// The server struct
struct Server {
    char *authString;
    char *host;
    char *port;
    char *name;
    
    bool status;
    
    int socket;
    int actualPort;
    int clientCount;
    
    Clients *clients;
    ServerMessageCount messageCount;
    
    FILE *serverOut;
    
    pthread_t threadId;

    pthread_mutex_t serverLock;
};

// The client struct
struct Clients {
    char *name;
    
    bool isDeleted;
    
    int socket;
    int portNumber;
    int id;
    
    FILE *toClient;
    FILE *fromClient;
    
    ClientMessageCount messageCount;

    
    Clients *next;
    Clients *prev;
    
    Server *server;
};

// Enum to store different errors encoutered by the server.
typedef enum {
    NORMAL_EXIT = 0,
    BAD_AGS_NUMBER = 1,
    BAD_AUTH_FILE = 1,
    COMMS_ERROR = 2
} ExitCodes;

#endif //ass4_server_h
