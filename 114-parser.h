/*
parser.c
the purposeof this file is to parse the assembly source code lines into tokens, labels, and line types, it analyzes input lines to detect labels, extract label names, identify line types and splits text into smaller words or parts of words/operands separat by spaces or commas

it receives a line string or buffer from assembly source files
and returns line types, extracted label strings, or pointers to next token words.
*/

#ifndef PARSER_H
#define PARSER_H

#include "globals.h"

/* definitions of line types */
#define LINE_EMPTY 0
#define LINE_DIRECTIVE 1
#define LINE_LABEL 2
#define LINE_INSTRUCTION 3


/* the function check if the line start with valid label the main params she receives is line string, return 1 if label founf or 0 if not label */
int is_label(const char *line);

/* the function check what type of line we got like instruction or directive
the main params she receives is line string, return integer type of the line */
int get_line_type(const char *line);

/* the function skip the label in line and return pointer to start of text after label
the main params she receives is line string */
char *skip_label(const char *line);

/* the function extract the label name from line
the main params she receives are line string and label_out buffer to save the name inside, return 1 if successed or 0 if fail */
int extract_label(const char *line, char *label_out);

/* the function get the next word from line, the main params she receives are line string and rest pointer to rest of line, return pointer to token string */
char *get_next_token(const char *line, char **rest);

#endif



