#include "comms.h"

/*
 * Function to parse the AUTH: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_auth_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "UTH:", strlen("UTH:")) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    } else {
        msg.messID = S_AUTH;
        msg.message = "AUTH:";
        free(line);
        return msg;
    }
}

/*
 * Function to parse the WHO: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_who_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "HO:", 3) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.message = "WHO:";
    msg.messID = S_WHO;
    free(line);
    return msg;
}

/*
 * Function to parse the ENTER: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_enter_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "NTER:", 5) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    char *name = strtok(&line[strlen("NTER:")], "\0");
    if (name == NULL) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.message = (char *) malloc(sizeof(char) * strlen(name));
    strcpy(msg.message, name);
    msg.messID = S_ENTER;
    free(line);
    return msg;
}

/*
 * Function to parse the NAME_TAKEN: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_name_taken_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "AME_TAKEN:", 10) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.messID = S_NAME_TAKEN;
    msg.message = "NAME_TAKEN:";
    free(line);
    return msg;
}

/*
 * Function to construct a message as per the spec.
 * This message will be displayed on the standard output of the client
 * with the client's name and the message sent by the client.
 * @param name: The name of the client which sent the message.
 * @param message: The message sent by the client.
 * return's a string with the name and the message of the client. 
*/
char *construct_message(char *name, char *message) {
    char *line = (char *) malloc(sizeof(char) * (strlen(name) + 
            strlen(message)));
    int j = 0;
    j = sprintf(line, "%s", name);
    j += sprintf(line + j, "%s", ":");
    j += sprintf(line + j, " %s", message);
    return line;
}

/*
 * Function to parse the MSG: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_msg_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "SG:", 3) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    char *name = strtok(&line[strlen("SG:")], ":");
    if (name == NULL) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    char *message = strtok(&line[strlen("SG:") + strlen(name) + 1], "\0");
    if (message == NULL) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.message = construct_message(name, message);
    msg.messID = S_MSG;
    free(line);
    return msg;
}

/*
 * Function to parse the LEAVE: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_leave_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "AVE:", 4) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    char *name = strtok(&line[strlen("AVE:")], "\0");
    if (name == NULL) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.messID = S_LEAVE;
    msg.message = (char *) malloc(sizeof(char) * strlen(name));
    strcpy(msg.message, name);
    free(line);
    return msg;
}

/*
 * Function to parse the KICK: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_kick_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "ICK:", 4) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.message = "KICK:";
    msg.messID = S_KICK;
    free(line);
    return msg;
}

/*
 * Function to parse the OK: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_ok_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "K:", 2) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.messID = S_OK;
    msg.message = "OK:";
    free(line);
    return msg;
}

/*
 * Function to parse the LIST: message from the server.
 * Check's if the message sent by the server is valid or not
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the server's socket.
 * return's ServerMessage struct containing the information of the message
 * recieved from the server.
*/
ServerMessage parse_list_message_from_server(FILE *input) {
    ServerMessage msg;
    char *line = read_line(input);
    if (strncmp(line, "ST:", 3) != 0) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    char *list = strtok(&line[strlen("ST:")], "\0");
    if (list == NULL) {
        msg.message = 0;
        msg.messID = S_INVALID;
        free(line);
        return msg;
    }
    msg.messID = S_LIST;
    msg.message = (char *) malloc(sizeof(char) * strlen(list));
    strcpy(msg.message, list);
    free(line);
    return msg;
}

/*
 * Function to parse the AUTH: message from the client.
 * Check's if the message and the auth string sent by the client is 
 * valid or not and assigns the messID accordingly.
 * @param input: The FILE * used to read from the client's socket.
 * @param authString: The authString stored in the server.
 * return's ClientMessage struct containing the information of the message
 * recieved from the client.
*/
ClientMessage parse_auth_message(FILE *input, char *authString) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *line = read_line(input);
    char *auth = (char *) malloc(sizeof(char) * strlen(authString));
    if (line == NULL) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    if (strncmp(line, "UTH:", strlen("UTH:")) != 0) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    int check = sscanf(line + strlen("UTH:"), "%s", auth);
    if (check == 0) {
        msg.message = 0;
        msg.messID = C_AUTH;
        free(line);
        return msg;
    }
    auth[strlen(auth)] = '\0';
    msg.message = auth;
    msg.messID = C_AUTH;
    free(line);
    return msg;
}

/*
 * Function to parse the NAME: message from the client.
 * Check's if the message sent by the client is valid or not 
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the client's socket.
 * return's ClientMessage struct containing the information of the message
 * recieved from the client.
*/
ClientMessage parse_name_message(FILE *input) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *line = read_line(input);
    if (line == NULL) {
        msg.message = 0;
        msg.messID = C_LEAVE;
        free(line);
        return msg;
    }
    if (strncmp(line, "NAME:", 5) != 0) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    char *name = strtok(&line[5], "\0");
    if (name == NULL) {
        msg.messID = C_NAME_TAKEN;
        msg.message = 0;
        free(line);
        return msg;
    }
    msg.message = (char *) malloc(sizeof(char) * strlen(name));
    strcpy(msg.message, name);
    msg.messID = C_NAME;
    free(line);
    return msg;
}

/*
 * Function to parse the SAY: message from the client.
 * Check's if the message sent by the client is valid or not 
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the client's socket.
 * return's ClientMessage struct containing the information of the message
 * recieved from the client.
*/
ClientMessage parse_say_message(FILE *input) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *line = read_line(input);
    if (line == NULL) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    if (strncmp(line, "AY:", 3) != 0) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    char *chat = strtok(&line[strlen("AY:")], "\0");
    if (chat == NULL) {
        msg.message = 0;
        msg.messID = C_SAY;
        free(line);
        return msg;
    }
    msg.message = (char *) malloc(sizeof(char) * strlen(chat));
    strcpy(msg.message, chat);
    msg.messID = C_SAY;
    free(line);
    return msg;
}

/*
 * Function to parse the LIST: message from the client.
 * Check's if the message sent by the client is valid or not 
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the client's socket.
 * return's ClientMessage struct containing the information of the message
 * recieved from the client.
*/
ClientMessage parse_list_message(FILE *input) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *line = read_line(input);
    if (strncmp(line, "ST:", 3) != 0) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    msg.message = "LIST:";
    msg.messID = C_LIST;
    free(line);
    return msg;
}

/*
 * Function to parse the LEAVE: message from the client.
 * Check's if the message sent by the client is valid or not 
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the client's socket.
 * return's ClientMessage struct containing the information of the message
 * recieved from the client.
*/
ClientMessage parse_leave_message(FILE *input) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *line = read_line(input);
    if (strncmp(line, "AVE:", 4) != 0) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    msg.messID = C_LEAVE;
    msg.message = "LEAVE:";
    free(line);
    return msg;
}

/*
 * Function to parse the KICK: message from the client.
 * Check's if the message sent by the client is valid or not 
 * and assigns the messID accordingly.
 * @param input: The FILE * used to read from the client's socket.
 * return's ClientMessage struct containing the information of the message
 * recieved from the client.
*/
ClientMessage parse_kick_message(FILE *input) {
    ClientMessage msg;
    memset(&msg, 0, sizeof(ClientMessage));
    char *line = read_line(input);
    if (strncmp(line, "ICK:", 4) != 0) {
        msg.message = 0;
        msg.messID = C_INVALID;
        free(line);
        return msg;
    }
    msg.messID = C_KICK;
    char *name = strtok(&line[strlen("ICK:")], "\0");
    if (name == NULL) {
        msg.message = 0;
        msg.messID = C_KICK;
        free(line);
        return msg;
    }
    msg.message = (char *) malloc(sizeof(char) * strlen(name));
    strcpy(msg.message, name);
    free(line);
    return msg;
}

/*
 * Function to send the AUTH: message to the connecting client.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * return's nothing.
*/
void send_auth_message_to_client(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

/*
 * Function to send the WHO: message to the client.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * return's nothing.
*/
void send_who_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

/*
 * Function to send the NAME_TAKEN: message to the client.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * return's nothing.
*/
void send_name_taken_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

/*
 * Function to send the NAME_TAKEN: message to the client.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * return's nothing.
*/
void send_chat_message_to_clients(FILE *output, char *fmt, char *name, 
        char *chat) {
    fprintf(output, "%s:%s:%s\n", fmt, name, chat);
    fflush(output);
}

/*
 * Function to send the OK: message to the client.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * return's nothing.
*/
void send_ok_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

/*
 * Function to send the ENTER: message to the clients.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * @param name: The name of the entering client.
 * return's nothing.
*/
void send_enter_message(FILE *output, char *message, char *name) {
    fprintf(output, "%s:%s\n", message, name);
    fflush(output);
}

/*
 * Function to send the LEAVE: message to the clients.
 * @param output: The FILE * used to write to the client's socket.
 * @param fmt: The format message sent to the client.
 * @param name: The name of the client leaving the chat.
 * return's nothing.
*/
void send_leave_message(FILE *output, char *fmt, char *name) {
    fprintf(output, "%s:%s\n", fmt, name);
    fflush(output);
}

/*
 * Function to send the LIST: message to the client.
 * @param output: The FILE * used to write to the client's socket.
 * @param list: The list of the clients.
 * return's nothing.
*/
void send_list_to_client(FILE *output, char *list) {
    fprintf(output, "%s\n", list);
    fflush(output);
}

/*
 * Function to send the KICK: message to the client.
 * @param output: The FILE * used to write to the client's socket.
 * @param message: The message sent to the client.
 * return's nothing.
*/
void send_kick_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

//Clients
/*
 * Function to send the AUTH: message to the server.
 * @param output: The FILE * used to write to the server's socket.
 * @param authString: The authString sent to the client.
 * return's nothing.
*/
void send_auth_string(FILE *output, char *authString) {
    fprintf(output, "AUTH:%s\n", authString);
    fflush(output);
}

/*
 * Function to send the NAME: message to the server.
 * @param output: The FILE * used to write to the server's socket.
 * @param fmt: The format of the message.
 * @param name: The name of the client.
 * return's nothing.
*/
void send_client_name(FILE *output, char *fmt, char *name) {
    fprintf(output, "%s:%s\n", fmt, name);
    fflush(output);
}

/*
 * Function to send the command to the server given by the user.
 * @param output: The FILE * used to write to the server's socket.
 * @param command: The command input by the user.
 * return's nothing.
*/
void send_client_command_to_server(char *command, FILE *output) {
    fprintf(output, "%s\n", command);
    fflush(output);
}

/*
 * Function to send the SAY: message to the server.
 * @param output: The FILE * used to write to the server's socket.
 * @param message: The message input by the user.
 * return's nothing.
*/
void send_input_as_client_message(char *message, FILE *output) {
    fprintf(output, "SAY:%s\n", message);
    fflush(output);
}
