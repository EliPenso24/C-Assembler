/*
parser.c
the purposeof this file is to parse the assembly source code lines into tokens, labels, and line types, it analyzes input lines to detect labels, extract label names, identify line types and splits text into smaller words or parts of words/operands separat by spaces or commas

it receives a line string or buffer from assembly source files
and returns line types, extracted label strings, or pointers to next token words.

it also assumes that the input line strings are valid null terminated strings and not over the line lenght limit
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "parser.h"

/* the function skip all space and tab characters in start of string
the main param she receives is str pointer to string and she return pointr to first character that not space
the function algorithm is to loop on the string while the char is space or tab and incresing str pointer, then return str pointer
the assumptions are str is valid null terminated string and not null
*/
static char *skip_spaces(char *str) {
    while (*str== ' ' || *str == '\t'){ /* loop until he find character that not space or tab and moving the pointer to the next char if he finds */
    str++; }
      
    return str; /* return pointer to clean string */
}


/* the function check if line have label inside of it
the main param she receives is line string, it use buf array to copy line and p pointer to skip spaces
she return 1 if label finded or 0 if not
the function algorithm is to copy line to buf, skip leading spaces with skip_spaces, and check if colon character exists in p using strchr
the assumptions are line is valid null terminated string and string length not over the max line lenght  */
int is_label(const char *line) {
    char buf[MAX_LINE_LEN];
    char *p;

    strcpy(buf, line); /* copy line to buf */
    p = skip_spaces(buf); /* skip spaces and tabs in start of buf */
    return (strchr(p, ':') != NULL); /* check if there is a colon exists in p */
}

/* the function extract the label name from line
the main params she receives are line string and label_out buffer, it use buf array p pointer colon pointer and len number
she returns success or error
her algorithm is to copy line to buf, skip leading spaces, search for colon, calculate label length, check if valid length and copy label name to label_out buffer with null terminator
the assumptions are line and label_out are valid pointers, line length not exceed MAX_LINE_LEN, and label_out has enough space
*/
int extract_label(const char *line, char *label_out) {
    char buf[MAX_LINE_LEN];
    char *p, *colon;
    int len;

    strcpy(buf, line); 
    p = skip_spaces(buf); /* skip spaces in start */
    colon = strchr(p, ':'); 

    if (!colon){
      return ERROR; } /* error if no colon finded */

    len = (int)(colon - p);
    if (len > MAX_NAME_LEN){
      return ERROR;} /* error if label too long */

    strncpy(label_out, p, len);
    label_out[len] = '\0';

    return SUCCESS; /* label extracted successfuly */
}

/* the function skip the label part in line
the main param she receives is line string, it use static buf array and colon pointer and she returns pointer to text after label or after spaces
the algorithm of the function is to copy line to static buf, search for colon character with strchr, if colon exists return pointer after colon skipped spaces using skip_spaces, else return buf skipped spaces
her assumptions are that line is valid null terminated string and string length not exceed MAX_LINE_LEN */
char *skip_label(const char *line) {
    static char buf[MAX_LINE_LEN];
    char *colon;

    strcpy(buf, line);
    colon = strchr(buf, ':');

    if (colon){
      return skip_spaces(colon + 1);} /* skip label and spaces after colon */
    return skip_spaces(buf); /* if no label just skip spaces */
}

/* the function check what type of line we got, the main param she receives is line string anf it uses buf array and p pointer, she returns the enum type of line
the function algorithm is to copy line to buf, skip leading spaces with skip_spaces, check if line is empty or comment, check if starts with dot for directive, check if starts with label using is_label, and if not one of this she returns LINE_INSTRUCTION
the assumptions are line is valid null terminated string and line length not over the max limit of the line */
int get_line_type(const char *line) {
    char buf[MAX_LINE_LEN];
    char *p;

    strcpy(buf, line); /* copy line to buf */
    p = skip_spaces(buf); /* skip spaces in start */

    if (*p == '\0' || *p == ';'){
      return LINE_EMPTY;} /* check if line empty or comment */
    if (*p == '.'){
      return LINE_DIRECTIVE;} /* check if line is directive */
    if (is_label(p)){
      return LINE_LABEL;} /* check if line start with label */

    return LINE_INSTRUCTION; /* if nothing else it id a instruction line */
}

/* the function get next word token from line, the main params she receives are line string and rest double pointer, it use static token array p pointer and i index, return pointer to token string or null
the main functiom algorithm is to skip spaces with skip_spaces, return nukk if empty, copy characters to static token array until space or comma, put null terminator at end, update rest pointer to point to next part of line after spaces, and return the token pointer
and the assumptions in the function are line is valid null terminated string and rest is valid pointer to pointer or null
*/
char *get_next_token(const char *line, char **rest) {
    static char token[MAX_LINE_LEN];
    char *p = (char *)line;
    int i = 0;

    p = skip_spaces(p);
    if (*p == '\0'){
      return NULL; /* return null if line finished */
}
    while (*p && !isspace(*p) && *p != ',') { /* loop that copy characters until space or comma */
        token[i++] = *p++;
    }
    token[i] = '\0'; /* put null terminator in end of token */

    if (rest) { /* check if rest pointer passed */
        if (*p == ','){
          p++;} /* skip comma if exists */
        *rest = skip_spaces(p);
    }

    return token; /* return the word we finded */
}



