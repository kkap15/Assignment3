#include "shared.h"
#include <string.h>

/*
 * Functions which read's line character by character.
 *
 * @param file: The FILE * of the file whose line is to be read.
 *
 * return's a null terminated string (char *).
*/
char *read_line(FILE *file) {
    int buffSize = BUFFSIZE;
    char *buffer = (char *) malloc(sizeof(char) * buffSize);
    int numOfReads = 0;
    int next;
    while (1) {
        next = fgetc(file);
        if (next == EOF && numOfReads == 0) {
            free(buffer);
            return NULL;
        }
        if (numOfReads == buffSize - 1) {
            buffSize *= 2;
            buffer = (char *) realloc(buffer, sizeof(char) * buffSize);
        }
        if (next < 32) {
            buffer[numOfReads] = '?';
        }
        if (next == '\n' || next == EOF) {
            buffer[numOfReads] = '\0';
            break;
        }
        buffer[numOfReads++] = next;
    }
    //fflush(file);
    return buffer;
}

/*
 * Function which convert's an integer to a string.
 *
 * @param number: Takes in an integer 
 *
 * return's a number in the string format.
*/
char *int_to_string(int number) {
    int length = 1;
    int duplicate = number;
    while (duplicate /= 10) {
        length++;
    }
    char *result = (char *) malloc(sizeof(char) * length);
    sprintf(result, "%d", number);
    return result;
}

/*
 * Function to count slash in a string.
 *
 * @param string: The string in which to count the slashes in.
 *
 * return's the number of slashes in a string.
*/
int get_slash_count(char *string) {
    int count = 0;
    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] == '/') {
            count++;
        }
    }
    return count;
}
