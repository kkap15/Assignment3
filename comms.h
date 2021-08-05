#ifndef COMMS_H
#define COMMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

//ENUM to store the message id of different client messages.
typedef enum {
    C_AUTH,
    C_NAME,
    C_NAME_TAKEN,
    C_SAY,
    C_KICK,
    C_LIST,
    C_LEAVE,
    C_INVALID
} ClientID;

//Enum to store the message id of different server messages.
typedef enum {
    S_WHO,
    S_NAME_TAKEN,
    S_AUTH,
    S_OK,
    S_KICK,
    S_MSG,
    S_LIST,
    S_ENTER,
    S_LEAVE,
    S_INVALID
} ServerID;

// struct to store the message info sent by the clients. Used by the server.
typedef struct {
    ClientID messID; // the particular message id
    char *message; // the message sent by the client.
} ClientMessage;

// struct to store the message info sent by the server. Used by the client.
typedef struct {
    ServerID messID; // the particular message id
    char *message; // the message sent by the server.
} ServerMessage;

/* Function declarations of the functions used to send messages to the 
 * clients */
void send_auth_message_to_client(FILE *output, char *message);
void send_who_message(FILE *output, char *message);
void send_name_taken_message(FILE *output, char *message);
void send_ok_message(FILE *output, char *message);
void send_enter_message(FILE *output, char *message, char *name);
void send_chat_message_to_clients(FILE *output, char *fmt, char *name, 
        char *chat);
void send_leave_message(FILE *output, char *fmt, char *name);
void send_list_to_client(FILE *output, char *list);
void send_kick_message(FILE *output, char *message);

/* Function declarations of the functions used to parse messages from the
 * clients */
ClientMessage parse_auth_message(FILE *input, char *authString);
ClientMessage parse_name_message(FILE *input);
ClientMessage parse_say_message(FILE *input);
ClientMessage parse_list_message(FILE *input);
ClientMessage parse_leave_message(FILE *input);
ClientMessage parse_kick_message(FILE *input);

/* Function declarations used to parse messages from the server */
ServerMessage parse_who_message_from_server(FILE *input);
ServerMessage parse_auth_message_from_server(FILE *input);
ServerMessage parse_enter_message_from_server(FILE *input);
ServerMessage parse_leave_message_from_server(FILE *input);
ServerMessage parse_list_message_from_server(FILE *input);
ServerMessage parse_msg_message_from_server(FILE *input);
ServerMessage parse_kick_message_from_server(FILE *input);
ServerMessage parse_ok_message_from_server(FILE *input);
ServerMessage parse_name_taken_message_from_server(FILE *input);

/* Function Declarations used to send messages to the server */
void send_auth_string(FILE *output, char *authString);
void send_client_name(FILE *output, char *fmt, char *name);
void send_client_command_to_server(char *command, FILE *output);
void send_input_as_client_message(char *message, FILE *output);

#endif //ass4_comms_h
