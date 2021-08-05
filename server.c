#include "server.h"

bool sigHup = false; // Global variable to indicate if SigHup happend

/*
 * Function to update a SAY: message count of the client and server.
 *
 * @param server: The server struct.
 *
 * @param client: The client struct.
 *
 * return's nothing
*/
void update_say_msg_count(Server *server, Clients *client) {
    server->messageCount.msgCount++;
    client->messageCount.msgCount++;
}

/*
 * Function to update a NAME: message count of the server.
 *
 * @param server: The server struct.
 *
 * return's nothing.
*/
void update_name_message_count(Server *server) {
    server->messageCount.nameCount++;
}

/*
 * Function to update a AUTH: message count of the server.
 *
 * @param server: The server struct.
 *
 * return's nothing.
*/
void update_auth_message_count(Server *server) {
    server->messageCount.authCount++;
}

/*
 * Function to update a LIST: message count of the client and server.
 *
 * @param server: The server struct.
 *
 * @param client: The client struct.
 *
 * return's nothing
*/
void update_list_message_count(Server *server, Clients *client) {
    server->messageCount.listCount++;
    client->messageCount.listCount++;
}

/*
 * Function to update a LEAVE: message count of the server.
 *
 * @param server: The server struct.
*/
void update_leave_message_count(Server *server) {
    server->messageCount.leaveCount++;
}

/*
 * Function to update a KICK: message count of the client and server.
 *
 * @param server: The server struct.
 *
 * @param client: The client struct.
 *
 * return's nothing
*/
void update_kick_message_count(Server *server, Clients *client) {
    server->messageCount.kickCount++;
    client->messageCount.kickCount++;
}

/*
 * Function to broadcast a message to the clients.
 *
 * @param server: The server struct which has all the information of the
 *                clients.
 *
 * @param chat: The chat message sent.
 *
 * @param name: The name of the client that sent the message.
 *
 * return nothing.
*/
void broadcast_chat_message(Server *server, char *chat, char *name) {
    Clients *temp = server->clients;
    for (; temp != NULL; temp = temp->prev) {
        if (!temp->isDeleted) {
            send_chat_message_to_clients(temp->toClient, "MSG", name, chat); 
        }
    }
}

/*
 * Function to send the clients an ENTER: message.
 *
 * @param clients: the clients that are connected.
 *
 * return's nothing.
*/
void send_enter_message_to_clients(Clients *clients, char *name) {
    Clients *temp = clients;
    for (; temp != NULL; temp = temp->prev) {
        send_enter_message(temp->toClient, "ENTER", name);
    }
}

/*
 * Function to send the clients a LEAVE: message.
 *
 * @param server: The server struct.
 *
 * @param leavingClient: The struct of the client which is leaving.
 *
 * return's nothing.
*/
void send_left_message_to_clients(Server *server, Clients *leavingClient) {
    Clients *temp = server->clients;
    char *name = leavingClient->name;
    for (; temp != NULL; temp = temp->prev) {
        send_leave_message(temp->toClient, "LEAVE", name);
    }
}

/*
 * Function to print the name of the entering client.
 *
 * @param output: The FILE* indicating the standard output of the server.
 *
 * @param name: The name of the client which is entering.
 *
 * return's nothing.
*/
void print_client_name(FILE *output, char *name) {
    fprintf(output, "(%s has entered the chat)\n", name);
    fflush(output);
}

/*
 * Function to print the leaving client's name to standard out.
 *
 * @param output: The FILE* indicating the standard output of the server.
 *
 * @param name: The name of the client which is leaving.
 *
 * return's nothing.
*/
void print_left_client_info(FILE *output, char *name) {
    fprintf(output, "(%s has left the chat)\n", name);
    fflush(output);
}

/*
 * Function to print the message sent by the client in the chat.
 *
 * @param output: The FILE * indicating the standard output of the server.
 *
 * @param name: The name of the client who has sent the message.
 *
 * @param message: The message of the client.
 * return's nothing.
*/
void print_chat_message(FILE *output, char *name, char *message) {
    fprintf(output, "%s: %s\n", name, message);
    fflush(output);
}

/*
 * Function to compare name.
 *
 * @param n1: name 1
 *
 * @param n2: name 2
 *
 * return's an int indicating which name is smaller.
*/
int compare_names(const void *n1, const void *n2) {
    return strcmp(*((char **) n1), *((char **) n2));
}

/*
 * Function to get a client list.
 *
 * @param server: The server struct which has all the clients.
 *
 * return's a string of the names of clients in a lexicographic order.
*/
char *get_client_list(Server *server) {
    char *buffer = (char *) malloc(sizeof(char) * 128);
    int j = 0, names = 0;
    Clients *clients = server->clients;
    for (; clients != NULL; clients = clients->prev) {
        if (!clients->isDeleted && clients->name != NULL) {
            names++;
        }
    }
    int pos = 0;
    char *clientNames[names];
    clients = server->clients;
    for (; clients != NULL; clients = clients->prev) {
        if (!clients->isDeleted && clients->name != NULL) {
            clientNames[pos++] = clients->name;
        }
    }
    size_t size = sizeof(clientNames) / sizeof(char *);
    qsort(clientNames, size, sizeof(char *), compare_names);
    j = sprintf(buffer, "%s", "LIST:");
    for (int i = 0; i < names; ++i) {
        if (i) {
            j += sprintf(buffer + j, "%s", ",");
        }
        j += sprintf(buffer + j, "%s", clientNames[i]);
    }
    buffer[strlen(buffer)] = '\0';
    return buffer;
}

/*
 * Function to send a client a kick message.
 *
 * @param server: The server struct.
 *
 * @param name: The name of the client which is to be kicked.
 *
 * return's nothing.
*/
void send_kick_message_to_client(Server *server, char *name) {
    Clients *temp = server->clients;
    if (name == NULL) {
        return;
    }
    for (; temp != NULL; temp = temp->prev) {
        if (strcmp(temp->name, name) == 0) {
            send_kick_message(temp->toClient, "KICK:");
            close(temp->socket);
            send_left_message_to_clients(server, temp);
            print_left_client_info(server->serverOut, name);
            temp->isDeleted = true;
            delete_client(server, temp);
            return;
        }
    }
}

/*
 * Function to update the message count on the basis of id of the message.
 *
 * @param client: The client struct which holds the client info.
 *
 * @param server: The server struct which holds the server info.
 *
 * @param messType: The message ID whose count has to be updated.
 *
 * return's nothing.
*/
void update_message_count(Clients *client, Server *server, ClientID messType) {
    switch (messType) {
        case C_SAY: 
            update_say_msg_count(server, client);
            break;
        case C_LIST:
            update_list_message_count(server, client);
            break;
        case C_LEAVE:
            update_leave_message_count(server);
            break;
        case C_KICK:
            update_kick_message_count(server, client);
        default:;
    }
    
}

/*
 * Function to delete a client from the server.
 *
 * @param server: The server struct which holds all the clients
 *
 * @param client: The client struct who is to be deleted.
 *
 * return's nothing.
*/
void delete_client(Server *server, Clients *client) {
    Clients *temp = server->clients, *prev;
    if (temp != NULL && temp->id == client->id) {
        server->clients = temp->prev;
        free(temp);
        server->clientCount--;
        return;
    }
    while (temp != NULL && temp->id != client->id) {
        prev = temp;
        temp = temp->prev;
    }
    if (temp == NULL) {
        return;
    }
    prev->prev = temp->prev;

    free(temp);
    server->clientCount--;
    return;
}

/*
 * Function to parse a message from the client on a first character basis.
 *
 * @param input: The FILE * of the client socket to read from.
 *
 * @param authString: The authString of the server.
*/
ClientMessage parse_message_from_client(FILE *input, char *authSting) {
    ClientMessage msg;
    msg.message = NULL;
    msg.messID = C_INVALID;
    char first = fgetc(input);
    switch (first) {
        case 'A':
            msg = parse_auth_message(input, authSting);
            break;
        case 'N':
            msg = parse_name_message(input);
            break;
        case 'S':
            msg = parse_say_message(input);
            break;
        case 'L':
            first = fgetc(input);
            if (first == 'E') {
                msg = parse_leave_message(input);
            }
            if (first == 'I') {
                msg = parse_list_message(input);
            }
            break;
        case 'K':
            msg = parse_kick_message(input);
            break;
        default:
            break;
    }
    return msg;
}

/*
 * Function which adds a client to the chat lobby and starts chatting.
 *
 * @param server: the server struct which has all the clients stored.
 *
 * @param verifiedClient: The client which has been verified.
 *
 * return's a bool indicating wether to close the connection or not.
*/
bool add_client_to_chat_lobby(Server *server, Clients *verifiedClient) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *list = NULL;
    while (1) {
        if (verifiedClient->isDeleted) {
            break;
        }
        msg = parse_message_from_client(verifiedClient->fromClient,
                server->authString);
        switch (msg.messID) {
            case C_SAY:
                update_message_count(verifiedClient, server, C_SAY);
                print_chat_message(server->serverOut, verifiedClient->name, 
                        msg.message);
                broadcast_chat_message(server, msg.message, 
                        verifiedClient->name);
                break;
            case C_LIST:
                update_message_count(verifiedClient, server, C_LIST);
                list = get_client_list(server);
                send_list_to_client(verifiedClient->toClient, list);
                break;
            case C_KICK:
                update_message_count(verifiedClient, server, C_KICK);
                pthread_mutex_lock(&server->serverLock);
                send_kick_message_to_client(server, msg.message);
                pthread_mutex_unlock(&server->serverLock);
                break;
            case C_LEAVE:
                update_message_count(0, server, C_LEAVE);
                send_left_message_to_clients(server, verifiedClient);
                print_left_client_info(server->serverOut, 
                        verifiedClient->name);
                return false;
            default: 
                msg.messID = C_INVALID;
                break;
        }
        usleep(100000);
    }
    return false;
}

/*
 * Function which stores a client name into the client struct.
 *
 * @param client: a client struct.
 *
 * @param name: the name which is to be stored into the client.
 *
 * return's nothing
*/
void store_client_name(Clients *client, char *name) {
    client->name = malloc(sizeof(char) * strlen(name));
    strcpy(client->name, name);
}

/*
 * Function to check if the name given by a client name is unique or not.
 *
 * @param server: The server struct
 *
 * @param name: The name given by a connecting client.
 *
 * return's a bool indicating if the name is unique or not.
*/
bool is_name_unique(Server *server, char *name) {
    int count = 0;
    if (name == NULL) {
        return false;
    }
    Clients *temp = server->clients;
    for (; temp != NULL; temp = temp->prev) {
        if (temp->name == NULL || temp->isDeleted) {
            continue;
        }
        if (strcmp(temp->name, name) == 0) {
            count++;
        }
    }
    if (count > 0) {
        return false;
    }
    return true;
}

/*
 * Ask name from the client by sending a WHO: message.
 *
 * @param client: The client whose name is to be asked.
 *
 * return's a ClientMessage struct indicating the message type recieved from
 * the client.
*/
ClientMessage ask_name_from_client(Clients *client) {
    send_who_message(client->toClient, "WHO:");
    ClientMessage msg = parse_name_message(client->fromClient);
    if (msg.messID == C_NAME_TAKEN) {
        return msg;
    } else if (msg.messID == C_LEAVE) {
        return msg;
    }
    return msg;
}

/*
 * Function to check the auth status after sending an AUTH: message to client.
 *
 * @param client: The client struct to whom the message is to be sent.
 *
 * @param authString: The authString to get accepted.
 *
 * return's an int indicating successfull auth or not.
 * 0 is success and 1 is failure.
*/
int get_auth_status(Clients *client, char *authString) {
    send_auth_message_to_client(client->toClient, "AUTH:");
    ClientMessage auth = parse_message_from_client(client->fromClient, 
            authString);
    if (auth.messID == C_AUTH) {
        if (strlen(authString) == 0 && auth.message == 0) {
            return 0;
        } else if (strcmp(auth.message, authString) == 0) {
            return 0;
        } else {
            return 1;
        }
    }
    return 1;
}

/*
 * Function which handles a client connection after a client connects
 *
 * @param serverInfo: The info of the sever passed on by the thread.
 *
 * return's NULL.
*/
void *handle_client(void *serverInfo) {
    Server *server = (Server *) serverInfo;
    Clients *current = server->clients;
    int authStatus = get_auth_status(current, server->authString);
    bool holdConnect = true;
    switch (authStatus) { 
        case 0:
            update_auth_message_count(server);
            send_ok_message(current->toClient, "OK:");
            ClientMessage msg = ask_name_from_client(current);
            if (msg.messID == C_LEAVE) {
                holdConnect = false;
                break;
            }
            while (!is_name_unique(server, msg.message) || 
                msg.message == NULL) {
                send_name_taken_message(current->toClient, "NAME_TAKEN:");
                msg = ask_name_from_client(current);
            }
            send_ok_message(current->toClient, "OK:");
            update_name_message_count(server);
            store_client_name(current, msg.message);
            send_enter_message_to_clients(server->clients, current->name);
            print_client_name(server->serverOut, current->name);
            holdConnect = add_client_to_chat_lobby(server, current);
            break;
        case 1:
            update_auth_message_count(server);
            holdConnect = false;
            break;
    }
    if (!holdConnect) {
        close(current->socket);
        delete_client(server, current);
    }
    return NULL;
}

/*
 * Function to create a new Client struct.
 *
 * @param server: Takes in the server struct.
 *
 * return's a new Client struct.
*/
Clients *new_client(Server *server) {
    Clients *client = (Clients *) malloc(sizeof(Clients));
    client->prev = server->clients; // store the last client's info.
    client->next = NULL;
    client->server = server;
    client->name = NULL;
    client->isDeleted = false;
    memset(&client->messageCount, 0, sizeof(ClientMessageCount));
    if (server->clients != NULL) {
        server->clients->next = client;
    }
    return client;
}

/*
 * Function which accept's the client connection.
 *
 * @param server: The server struct containign the server info.
 *
 * return's nothing.
*/
void accept_client_connections(Server *server) {
    struct sockaddr_in client;
    socklen_t socklen = sizeof(client);
    int socket = accept(server->socket, (struct sockaddr *) &client, &socklen);
    if (socket == -1) {
        fprintf(stderr, "Communications error\n");
        exit(COMMS_ERROR);
    }
    pthread_mutex_lock(&server->serverLock);
    Clients *newClient = new_client(server);
    newClient->socket = socket;
    newClient->portNumber = ntohs(client.sin_port);
    newClient->toClient = fdopen(newClient->socket, "w");
    newClient->fromClient = fdopen(newClient->socket, "r");
    newClient->id = server->clientCount;
    server->clientCount++;
    server->clients = newClient;
    pthread_mutex_unlock(&server->serverLock);
    pthread_t clientThread;
    pthread_create(&clientThread, NULL, handle_client, (void *) server);
    pthread_detach(clientThread);
}

/*
 * Function to start the server. Print's the port number to stderr.
 *
 * @param server: Takes in the server struct.
 *
 * return's a bool indicating if the server could be started or not.
*/
bool start_server(Server *server) {
    struct sockaddr_in socket;
    socklen_t socklen = sizeof(socket);
    if (getsockname(server->socket, (struct sockaddr *) &socket,
            &socklen) == -1) {
        return false;
    }
    server->actualPort = ntohs(socket.sin_port);
    server->port = int_to_string(server->actualPort);
    
    pthread_mutex_lock(&server->serverLock);
    fprintf(stderr, "%s\n", server->port);
    fflush(stderr);
    pthread_mutex_unlock(&server->serverLock);
    return true;
}

/*
 * Function to see if the server is listening or not.
 *
 * @param server: Takes in the Server struct.
 *
 * return's a bool indicating if the server ha sstarted listening for
 * connections or not.
*/
bool is_server_listening(Server *server) {
    struct addrinfo hints, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = DEFAULT_PROTOCOL;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(server->host, server->port, &hints,
            &result) != 0) {
        return false;
    }
    for (struct addrinfo *attempt = result; attempt != NULL; 
            attempt = attempt->ai_next) {
        server->socket = socket(attempt->ai_family, attempt->ai_socktype,
                attempt->ai_protocol);
        if (server->socket == -1) {
            continue;
        }
        int v = 1;
        setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
        if (bind(server->socket, attempt->ai_addr, 
                attempt->ai_addrlen) != 0) {
            close(server->socket);
            continue;    
        }
    }
    if (listen(server->socket, SOMAXCONN)) {
        perror("Listen");
        return false;
    }
    return true;
}

/*
 * Function which run's a server and performs various steps into making
 * a server struct perform like a server.
 *
 * @param server: The server struct.
 *
 * return's nothing.
*/
void run_server(Server *server) {
    if (!is_server_listening(server)) {
        fprintf(stderr, "Communications error\n");
        exit(COMMS_ERROR);
    }
    if (!start_server(server)) {
        fprintf(stderr, "Communications error\n");
        exit(COMMS_ERROR);
    }
    while (1) {
        accept_client_connections(server);
    }
}

/*
 * Function to start off a server thread.
 *
 * @param serverInfo: The server struct passed on to the thread.
 *
 * return's nothing.
*/
void *make_server(void *serverInfo) {
    Server *server = (Server *) serverInfo;
    run_server(server);
    return NULL;
}

/*
 * The function which print's the stats of the currently connected clients.
 * The clients are shown in a lexicographical order.
 *
 * @param server: The main server struct.
 *
 * return's nothing.
*/
void print_client_stats(Server *server) {
    pthread_mutex_lock(&server->serverLock);
    int names = 0;
    Clients *client = server->clients;
    for (; client != NULL; client = client->prev) {
        if (!client->isDeleted && client->name != NULL) {
            names++;   
        }
    }
    int pos = 0;
    char *clientNames[names];
    client = server->clients;
    for (; client != NULL; client = client->prev) {
        if (!client->isDeleted && client->name != NULL) {
            clientNames[pos++] = client->name;   
        }
    }
    size_t size = sizeof(clientNames) / sizeof(char *);
    qsort(clientNames, size, sizeof(char *), compare_names);
    
    int i = 0;
    while (i < pos) {
        client = server->clients;
        for (; client != NULL; client = client->prev) {
            if (strcmp(client->name, clientNames[i]) == 0) {
                ClientMessageCount count = client->messageCount;
                fprintf(stderr, "%s:SAY:%d:KICK:%d:LIST:%d\n", clientNames[i],
                        count.msgCount, count.kickCount, count.listCount);
                fflush(stderr);
                i++;
            }
        }
    }
    pthread_mutex_unlock(&server->serverLock);
}

/*
 * The function which prints the total server stats.
 *
 * @param server: The server struct.
 *
 * return's nothing.
*/
void print_server_stats(Server *server) {
    pthread_mutex_lock(&server->serverLock);
    ServerMessageCount count = server->messageCount;
    fprintf(stderr, "%s:AUTH:%d:NAME:%d:SAY:%d:KICK:%d:LIST:%d:LEAVE:%d\n", 
            server->name, count.authCount, count.nameCount, count.msgCount,
            count.kickCount, count.listCount, count.leaveCount);
    fflush(stderr);
    pthread_mutex_unlock(&server->serverLock);
}

/*
 * Function to get the extract the auth string from the file.
 *
 * @param fileName: The name of the file to be opened to extract the
 * authString
 *
 * return's a string containing the authstring.
*/
char *get_authrization_string(char *fileName) {
    FILE *authFile = fopen(fileName, "r");
    if (!authFile) {
        fprintf(stderr, "%s\n", "Usage: server authfile [port]");
        fflush(stderr);
        exit(BAD_AUTH_FILE);
    }
    char *line = read_line(authFile);
    fclose(authFile);
    return line;
}

/*
 * Function to handle SIGHUP.
 *
 * @param signal: The signal recieved.
 *
 * return's nothing.
*/
void handle_sighup(const int signal) {
    if (signal == SIGHUP) {
        sigHup = true;
    }
}

/*
 * Function to set the server name. Get's the slash count and then traverses
 * the length of the line to get to the name of the server.
 *
 * @param serverPath: the path to the server.
 *
 * return's a string containing the name of the server.
*/
char *set_server_name(char *serverPath) {
    char *serverName = (char *) malloc(sizeof(char) * BUFFSIZE);
    int countSlash = 0;
    int j = 0;
    countSlash = get_slash_count(serverPath);
    for (int i = 0; i < strlen(serverPath); ++i) {
        if (serverPath[i] == '/') {
            countSlash--;
        }
        if (countSlash == 0) {
            strcpy(&serverName[j], &serverPath[i + 1]);
            j++;
        }
    }
    return serverName;
}

/*
 * Function to create a new server struct.
 *
 * @param serverName: takes in the name of the server given on command line.
 *
 * return's a Server struct with all the information.
*/
Server *create_new_server(char *serverName) {
    Server *server = malloc(sizeof(Server));
    server->name = set_server_name(serverName);
    server->authString = NULL;
    server->actualPort = 0;
    server->host = LOCALHOST;
    server->port = DEFAULT_PORT;
    server->clientCount = 0;
    server->status = true;
    server->serverOut = stdout;
    server->clients = NULL;
    memset(&server->messageCount, 0, sizeof(ServerMessageCount));
    pthread_mutex_init(&server->serverLock, NULL);
    return server;
}

/*
 * main function. Handles sighup, creates a thread to start server.
*/
int main(int argc, char *argv[]) {
    bool isPortPresent = false;
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "%s\n", "Usage: server authfile [port]");
        fflush(stderr);
        exit(BAD_AGS_NUMBER);
    }
    if (argc == 3) {
        isPortPresent = true;
    }
    Server *server = create_new_server(argv[0]);
    struct sigaction sa;
    sa.sa_handler = handle_sighup;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sa, 0);
    signal(SIGPIPE, SIG_IGN);
    if (isPortPresent == true) {
        server->port = argv[2];
    } else {
        server->port = DEFAULT_PORT;
    }
    server->authString = get_authrization_string(argv[1]);
    pthread_t serverThread;
    pthread_create(&serverThread, NULL, make_server, (void *) server);
    
    while (1) {
        if (sigHup) {
            sigHup = false;
            fprintf(stderr, "%s", CLIENT_HEAD);
            fflush(stderr);
            print_client_stats(server);
            fprintf(stderr, "%s", SERVER_HEAD);
            fflush(stderr);
            print_server_stats(server);
        }
    }
}
