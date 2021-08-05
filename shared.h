#ifndef SHARED_H
#define SHARED_H

#define BUFFSIZE 80
#define LOCALHOST "127.0.0.1"
#define DEFAULT_PORT "0"
#define DEFAULT_PROTOCOL 0

#include <stdio.h>
#include <stdlib.h>

char *read_line(FILE *file);
char *int_to_string(int number);
int get_slash_count(char *string);

#endif //ass4_shared_h
