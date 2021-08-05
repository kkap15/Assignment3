#include "client.h"

/*
 * Function to check if the authfile given in the command line 
 * is valid or not.
 *
 * @param filename: the name of the file given in the command line
 *
 * return: bool indicating if the filename is valid or not.
*/
bool valid_authfile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        return false;
    }
    fclose(file);
    return true;
}

/*
 * Function to get the auth string from the auth file.
 *
 * @param authfile: the name of the file to open.
 *
 * return's the auth string.
*/
char *get_auth_string(char *authFile) {
    FILE *file = fopen(authFile, "r");
    char *line = read_line(file);
    if (line == NULL) {
        fclose(file);
        return NULL;
    }
    fclose(file);
    return line;
}

/*
 * Function to initialise a new Client.
 *
 * @param name: Name of the client given in the command line.
 *
 * @param authFile: Name of the file given in the command line.
 *
 * @param port: name of the port that the client will connect to.
 *
 * return's a Client struct.
*/
Client *new_client(char *name, char *authFile, char *port) {
    Client *client = (Client *) malloc(sizeof(Client));
    client->name = malloc(sizeof(char) * strlen(name));
    strcpy(client->name, name);
    client->authString = get_auth_string(authFile);
    client->port = port;
    client->nameSuffix = "0";
    client->fromUser = stdin;
    pthread_mutex_init(&client->clientLock, NULL);
    pthread_mutex_init(&client->serverLock, NULL);
    return client;
}

/*
 * Function to get an OK message from server when client send's an AUTH:
 * message.
 *
 * @param fromServer: a FILE * that read's from the server socket
 *
 * return's nothing.
*/
void get_auth_ok_message_from_server(FILE *fromServer) {
    char first = fgetc(fromServer);
    ServerMessage msg;
    msg.message = NULL;
    msg.messID = S_INVALID;
    if (first == 'O') {
        msg = parse_ok_message_from_server(fromServer);
    } else {
        fprintf(stderr, "Authentication error\n");
        fflush(stderr);
        exit(AUTH_ERR);
    }
    if (msg.messID != S_OK) {
        fprintf(stderr, "Authentication error\n");
        fflush(stderr);
        exit(AUTH_ERR);
    }
}

/*
 * Function to parse a server message. 
 * The first character is used to identify the message sent by the server.
 * Further checks in comms.c
 *
 * @param fromServer: The FILE * that reads from the server socket.
 *
 * return's a ServerMessage struct with the information of the message 
 * sent by the server.
*/
ServerMessage parse_server_messages(FILE *fromServer) {
    ServerMessage msg;
    msg.message = NULL;
    msg.messID = S_INVALID;
    char first = fgetc(fromServer);
    if (first <= 0) {
        fprintf(stderr, "Communications error\n");
        fflush(stderr);
        exit(COMMS_ERR);
    }
    switch (first) {
        case 'A':
            msg = parse_auth_message_from_server(fromServer);
            break;
        case 'M':
            msg = parse_msg_message_from_server(fromServer);
            break;
        case 'N':
            msg = parse_name_taken_message_from_server(fromServer);
            break;
        case 'W':
            msg = parse_who_message_from_server(fromServer);
            break;
        case 'E':
            msg = parse_enter_message_from_server(fromServer);
            break;
        case 'L':
            first = fgetc(fromServer);
            if (first == 'E') {
                msg = parse_leave_message_from_server(fromServer);
            }
            if (first == 'I') {
                msg = parse_list_message_from_server(fromServer);
            }
            break;
        case 'K':
            msg = parse_kick_message_from_server(fromServer);
            break;
        case 'O':
            msg = parse_ok_message_from_server(fromServer);
            break;
        default:
            break;
    }
    return msg;
}

/*
 * Function that append's a digit at the end of the name of the client.
 *
 * @param client: Client struct which has the name of the client.
 *
 * return's nothing.
*/
void generate_new_name(Client *client) {
    bool digitPresent = false;
    int len = strlen(client->name);
    client->name = realloc(client->name, sizeof(char) * 
            strlen(client->name + 2));
    for (int i = 0; i < len; ++i) {
        if (isdigit(client->name[i])) {
            digitPresent = true;
            break;
        }
    }
    if (digitPresent) {
        sprintf(client->name + (len - 1), "%s", client->nameSuffix);
    } else {
        sprintf(client->name + len, "%s", client->nameSuffix);
    }
    int number = atoi(client->nameSuffix);
    number++;
    client->nameSuffix = int_to_string(number);
}

/*
 * Function which print's the ENTER message to client's standard out.
 *
 * @param output: The standard output of the client.
 *
 * @param name: The name of the client which has entered the chat.
 *              return's nothing.
*/
void print_enter_message(FILE *output, char *name) {
    fprintf(output, "(%s has entered the chat)\n", name);
    fflush(output);
}

/*
 * Functions which print's the incoming message from the server.
 *
 * @param output: The standard output of the client.
 *
 * @param message: The message sent by the server.
 *
 * return's nothing.
*/
void print_incoming_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

/*
 * Function which print's the list of chatters in the chat.
 *
 * @param output: The standard output of the client.
 *
 * @param message: The message sent by the server.
 *
 * return's nothing.
*/
void print_client_list(FILE *output, char *message) {
    fprintf(output, "(current chatters: %s)\n", message);
    fflush(output);
}

/*
 * Function which print's the name of the leaving client.
 *
 * @param output: The standard output of the client.
 *
 * @param name: The name of the leaving client.
 *
 * return's nothing.
*/
void print_client_left(FILE *output, char *name) {
    fprintf(output, "(%s has left the chat)\n", name);
    fflush(output);
}

/*
 * Function which shut's down client if it recieves a KICK: message from 
 * server.
 *
 * @param client: The Client struct which stores all the client info.
 *
 * return's nothing and exit's with a valid error code.
*/
void shutdown_client(Client *client) {
    free(client->authString);
    free(client->name);
    fclose(client->fromServer);
    fclose(client->toServer);
    pthread_mutex_destroy(&client->clientLock);
    pthread_mutex_destroy(&client->serverLock);
    fprintf(stderr, "Kicked\n");
    fflush(stderr);
    exit(KICKED);
}

/*
 * Function which is used to perform necessary actions once proper
 * classification of server messages is performed. 
 *
 * @param clientInfo: The client struct containing the client
 *                    information.
 *
 * return's nothing.
*/
void handle_connection(Client *clientInfo) {
    Client *client = (Client *) clientInfo;
    ServerMessage msg;
    memset(&msg, 0, sizeof(ServerMessage));
    while (1) {
        while (msg.messID != S_KICK) {
            msg = parse_server_messages(client->fromServer);
            pthread_mutex_lock(&client->clientLock);
            switch (msg.messID) {
                case S_MSG:
                    print_incoming_message(stdout, msg.message);
                    break;
                case S_ENTER:
                    print_enter_message(stdout, msg.message);
                    break;
                case S_LIST:
                    print_client_list(stdout, msg.message);
                    break;
                case S_LEAVE:
                    print_client_left(stdout, msg.message);
                    break;
                case S_KICK:
                    shutdown_client(client);
                    break;
                case S_INVALID:
                default:;
            }
            pthread_mutex_unlock(&client->clientLock);
        }
    }
    return;
}

/*
 * Function which process's input by the user and decides if
 * the input is a command or a SAY: message.
 *
 * @param line: The line entered by the user. 
 *
 * @param toServer: The FILE * which is used by the client to send messages to
 *                  the server.
 * return's nothing.
*/
void process_input_from_user(char *line, FILE *toServer) {
    if (line[0] == '*') {
        line = line + 1;
        send_client_command_to_server(line, toServer);
    } else {
        send_input_as_client_message(line, toServer);
    }
}

/*
 * Function which handles user input.
 *
 * @param clientInfo: The Client struct which contains the information of the
 *                    client. 
 * return's NULL
*/
void *handle_user_input(void *clientInfo) {
    Client *client = (Client *) clientInfo;
    char *line;
    while (1) {
        line = read_line(client->fromUser);
        pthread_mutex_lock(&client->serverLock);
        process_input_from_user(line, client->toServer);
        pthread_mutex_unlock(&client->serverLock);
    }
    return NULL;
}

/*
 * Function used to connect to the server.
 *
 * @param client: The client struct which contains the client info.
 *
 * return's a bool indicating successfull connection or not.
*/
bool connect_to_server(Client *client) {
    struct addrinfo *result;
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = DEFAULT_PROTOCOL;
    if (getaddrinfo(LOCALHOST, client->port, &hints, &result) != 0) {
        return false;
    }
    for (struct addrinfo *attempts = result; attempts != NULL; 
            attempts = attempts->ai_next) {
        client->serverSocket = socket(attempts->ai_family,
                attempts->ai_socktype, attempts->ai_protocol);
        if (client->serverSocket == -1) {
            continue;
        }
        int v = 1;
        setsockopt(client->serverSocket, SOL_SOCKET, SO_KEEPALIVE, &v, 
                sizeof(v));
        if (connect(client->serverSocket, attempts->ai_addr,
                attempts->ai_addrlen) == -1) {
            close(client->serverSocket);
            client->serverSocket = -1;
            continue;
        }
        break;
    }
    if (client->serverSocket == -1) {
        return false;
    }
    client->fromServer = fdopen(client->serverSocket, "r");
    client->toServer = fdopen(client->serverSocket, "w");
    return true;
}

/*
 * Function which start's the client, send's AUTH: and NAME: message to the
 * server and also starts a thread to handle user input after successfull 
 * AUTH: and NAME: negotiation from the server.
 *
 * @param client: The Client struct which has the client info.
 *
 * return's nothing.
*/
void start_client(Client *client) {
    if (!connect_to_server(client)) {
        fprintf(stderr, "Communications error\n");
        exit(COMMS_ERR);
    }
    
    ServerMessage msg = parse_server_messages(client->fromServer);
    if (msg.messID == S_AUTH) {
        send_auth_string(client->toServer, client->authString);
        get_auth_ok_message_from_server(client->fromServer);
    }
    msg = parse_server_messages(client->fromServer);
    if (msg.messID == S_WHO) {
        send_client_name(client->toServer, "NAME", client->name);   
    }
    
    while (1) {
        msg = parse_server_messages(client->fromServer);
        if (msg.messID == S_OK) {
            break;
        }
        if (msg.messID != S_OK) {
            if (msg.messID == S_NAME_TAKEN) {
                generate_new_name(client);
            } else {
                fprintf(stderr, "Communications error\n");
                exit(COMMS_ERR);
            }
        }
        msg = parse_server_messages(client->fromServer);
        if (msg.messID == S_WHO) {
            send_client_name(client->toServer, "NAME", client->name);
        } else {
            fprintf(stderr, "Communications error\n");
            exit(COMMS_ERR);
        }
    }
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, handle_user_input,
            (void *) client);
    handle_connection(client);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: client name authfile port\n");
        exit(BAD_ARGS);
    }
    if (!valid_authfile(argv[2])) {
        fprintf(stderr, "Usage: client name authfile port\n");
        exit(BAD_ARGS);
    }
    if (strlen(argv[1]) == 0 || strlen(argv[2]) == 0 || 
            strlen(argv[3]) == 0) {
        fprintf(stderr, "Usage: client name authfile port\n");
        exit(BAD_ARGS);
    }
    Client *client = new_client(argv[1], argv[2], argv[3]);
    start_client(client);
    return OK;
}
