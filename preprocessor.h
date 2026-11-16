/*
  preprocessor.h
  Purpose: this file includes the files meant for processing the macros. 
  the code in these functions makes two passes over the source file. The first one builds a
  list of every macro defined between "mcro" and "mcroend" with the name and   body line. The second pass rewrites the by changing every macro with its lines and it skips the macro definitions so that they donwt reach the output.

  It receives an open .as source file.
  It returns an .am file with every macro call replaced by its body.
*/

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "globals.h"
#include <stdio.h>

/*struct for macro content*/
typedef struct MacroLine {
    char content[MAX_LINE_LEN];
    struct MacroLine *next;
} MACRO_LINE;

/*struct for macros*/
typedef struct Macro {
    char name[MAX_NAME_LEN];
    MACRO_LINE *lines;/* head of macro content lines*/
    struct Macro *next; /*next macro*/
} MACRO;


/*
 Checks if a line opens with the word "mcro" separately.
It receives a pointer to a line and returns TRUE if it starts with a macro definition and FALSE if not.
*/
int is_mcro_start(const char *line);

/*
 Checks if a line is "mcroend".
It receives a pointer to a line and returns TRUE if the line is "mcroend" and FALSE otherwise.
*/
int is_mcro_end(const char *line);


/*
Extracts the macro name from a line starting with "mcro".
It receives a pointer to a line, a pointer to the wanted name and the max len (int).
It returns SUCCESS and the name is copied into name_out if the line is valid or ERROR if the name is missing or too long.
 */
int extract_macro_name(const char *line, char *name_out, size_t max_len);

/*
Creates a new Macro node with the name we recieved.
It receives a pointer to the macro name.
It returns a pointer to the new Macro ,or null if the allocation failed.
*/
MACRO *create_macro (const char *name);


/*
Adds a new line to the end of a macro.
It receives a pointer to the macro and a pointer to the line.
It returns SUCCESS if the line was added,and ERROR if not.
*/
int add_line_to_macro(MACRO *macro, const char *line);

/*
Adds a macro to the end of the macro list.
It receives a pointer to the head pointer of the list, and a pointer to the macro to add.
It adds the new macro.
*/
void add_macro_to_list(MACRO **head, MACRO *new_macro);

/*
Reads a file and builds a list of all the macro in it.
It receives a file pointer and the address of the list.
It returns SUCCESS if the list was built or ERROR if there was an invalid macro.
*/
int build_macro_list(FILE *src_file, MACRO  **head);


/*
Reads a source file and writes an .am file with expanded macros.
It receives the source file, the .am file and the macro list.
It returns SUCCESS after finishing.
*/
int expand_macros(FILE *src_file, FILE *am_file,MACRO *head );


/*
Frees all memory used by the macro list.
It receives the head of the macro list.
*/
void free_macro_list(MACRO  *head);

#endif




