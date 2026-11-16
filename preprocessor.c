/*
  preprocessor.c
  Purpose: The macro pre-processor.
  This code makes two passes over the source file. The first one builds a
  list of every macro defined between "mcro" and "mcroend" with the name and   body line. The second pass rewrites the by changing every macro with its lines and it skips the macro definitions so that they donwt reach the output.

  It receives an open .as source file.
  It returns an .am file with every macro call replaced by its body.

  It assumes macros are not nested, every "mcro" has a matching "mcroend",
  and that the  macro calls only come after their definition in the file.
 */



#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

#include "preprocessor.h"


/*
 Removes first and last whitspaces from a string.
 Receives a pointer to a string and returns a pointer to the start of the fixed string.
It moves a pointer past the first spaces and then moves a pointer back fro, the end past the last spaces and ends the string there.
it assumes the input is a valid string.

 */
static char *trim( char *str)
{
    char *end;
    /*skip first spaces. */
    while (isspace((unsigned char)*str) )
    {
        str++;
    }
    /* check if the whole line was empty or just whitespace.*/
    if ( *str== '\0')
    {
        return str;
    }

    /*cut off last spaces*/
    end =str + strlen(str)-1;
    while (end> str&&isspace((unsigned char) *end))
    {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}


/*
 Checks if a line opens with the word "mcro" separately.
It receives a pointer to a line and returns TRUE if it starts with a macro definition and FALSE if not.
It trims the line, compares the first 4 characters to "mcro" exactly.
It assumes the line is not longer than MAX_LINE_LEN.

 */
int is_mcro_start( const char *line)
{
    char buf [MAX_LINE_LEN] ;
    char *trimmed;
    /*create a local copy to not change the original string. */
    strcpy(buf, line);
    trimmed = trim(buf);

    /*makes sure it is long enough and starts correctly.*/
    if (strncmp( trimmed,"mcro", 4)!=0 )
    {
        return FALSE;
    }

    /* make sure "mcro" is the whole word and not a part of another word*/
    if (trimmed[4]!= '\0'&&!isspace( (unsigned char) trimmed[ 4] ))
    {
        return FALSE;
    }
    
    
    return TRUE;
}


/*
 Checks if a line is "mcroend".
It receives a pointer to a line and returns TRUE if the line is "mcroend" and FALSE otherwise.
it trims the line and compares it to "mcroend".
It assumes the line is not longer than MAX_LINE_LEN.
 */
 
int is_mcro_end(const char *line)
{

    char buf[MAX_LINE_LEN];
    char *trimmed;
    /*create a local copy to not change the original string. */
    strcpy(buf, line);
    trimmed = trim(buf);

    if (strcmp (trimmed ,"mcroend")== 0)
    {
        return TRUE;
    }
    return FALSE;
}



static const  char *reserved_words[] =
{
    "add","sub","and","or","nor", 
    "move","mvhi", "mvlo",
    "addi","subi", "andi","ori", "nori",
    "bne", "beq", "blt", "bgt",
    "lb","sb", "lw", "sw","lh", "sh",
    "jmp", "la", "call", "hlt" ,
    ".db", ".dh", ".dw", ".asciz",
    ".entry", ".extern",
    "mcro", "mcroend",
    NULL
};

/*
 Checks if a name is identical to a word from the reserved words.
It receives a pointer to the name.
It returns true if it is and false if not.
IT goes through all the words until the pointer points to null.
 It assumes the name and the reserved words are valid and not null.
 */
static int is_reserved_word(const char *name)
{
    int i;
    for (i=0;reserved_words[i ]!= NULL;i++ )
    {
        if (strcmp(name,reserved_words[i])==0)
            return TRUE;
    }
    
    return FALSE;
}




/*
 Extracts the macro name out of a line that starts with "mcro".
 Creates a new MACRO node with the name we recieved.
It receives a pointer to the macro name.
It returns a pointer to the new MACRO ,or null if the allocation failed.
IT trims the line, skips the "mcro" and the spaces and then it copies the rest into name_out.
 It assumes the caller already checked the line with is_mcro_start.
 */
int extract_macro_name(const char*line ,char *name_out , size_t max_len)
{
    char buf[ MAX_LINE_LEN];
    char *trimmed;
    char *name_start;

    strcpy(buf, line);
    trimmed = trim(buf);
    /* the name starts after "mcro"*/
    name_start= trimmed +4;

    /* skip the whitespace between "mcro" and the name*/
    while(isspace(( unsigned char)*name_start) )
    {
        name_start ++;
    }

    /* if nothing is written after mcro*/
    if (*name_start =='\0')
    {
        return ERROR;
    }
    /* if the name is too long for the buffer*/
    if (strlen(name_start ) >=max_len)
    {
        return ERROR;
    }
    
    
    if (is_reserved_word(name_start))
    {
    return ERROR;
    }
    strcpy( name_out,name_start);
    return SUCCESS;
}

/*
Creates a new MACRO node with the name we recieved.
It receives a pointer to the macro name.
It returns a pointer to the new MACRO ,or null if the allocation failed.
It allocates memory for one MACRO, copies the input into it, and sets the rest to NULL.
It assumes the name is not longer than MAX_NAME_LEN.
*/

MACRO *create_macro(const char *name)
{
    MACRO *new;
    new =(MACRO *)malloc(sizeof( MACRO));
    if (new==NULL)
    {
        fprintf(stderr, "Error:memory allocation failed for macro '%s'\"\n",name) ;
        return NULL;
    }
    strcpy(new->name,name);
    new->lines = NULL;
    
    new->next = NULL;
    return new;
}


/*
Adds a new line to the end of a macro.
It receives a pointer to the macro and a pointer to the line.
It returns SUCCESS if the line was added,and ERROR if not.
It allocates memory for one MACRO_LINE, copies the content into it and adds it to the end of the macros lines.
It assumes the given macro pointer is not NULL and the line is not longer than MAX_LINE_LEN.
*/
int add_line_to_macro(MACRO *macro, const char *line)
{
    MACRO_LINE *new_line;
    MACRO_LINE *current;
    new_line=(MACRO_LINE *)malloc( sizeof(MACRO_LINE));

    if(new_line== NULL)
    {
        fprintf(stderr,"Error:  memory allocation failed for macro line\n");
        return ERROR;
    }
    strcpy(new_line->content ,line);
    new_line->next =NULL;
    if (macro->lines== NULL)
    {
        macro->lines = new_line;
        return SUCCESS;
    }
    current= macro->lines;
    while (current->next !=NULL)
    {
        current= current->next ;
    }
    current->next = new_line;
    return SUCCESS;
}


/*
Adds a macro to the end of the macro list.
It receives a pointer to the head pointer of the list, and a pointer to the macro to add.
It adds the new macro.
It sets the last pointer to the given macro.
It assumes the new macro pointer is full.
*/


void add_macro_to_list(MACRO **head, MACRO *new)
{
    MACRO *current;
    if (*head==NULL)
    {
        *head= new;
        return;
    }

    current= *head;
    while (current->next !=NULL)
    {
        current =current->next;
    }
    current->next = new;
}


/*
Creates a new MACRO node from the name in the given line.
It receives a line that opens a macro.
It returns a pointer to the new MACRO or NULL if there was an error.
It calls extract_macro_name to get the name and then uses create_macro.
It assumes the line was already checked with is_mcro_start.

*/
static MACRO *create_macro_from_line( const char *line, int line_number)
{
    char name [MAX_NAME_LEN];
    if(extract_macro_name(line,name, MAX_NAME_LEN)!= SUCCESS)
    {
        fprintf(stderr,"Error: Invalid macro definition on line %d: %s\n", line_number, line);
        return NULL;
    }
    return create_macro(name);
}


/*
Reads a file and builds a list of all the macro in it.
It receives a file pointer and the address of the list.
It returns SUCCESS if the list was built or ERROR if there was an invalid macro.
It reads the file line by line. At the begining of a macro it creates a new macro and at the end it adds it.
It assumes the file pointer is valid.
*/
int build_macro_list(FILE *src_file, MACRO  **head)
{
    char line[MAX_LINE_LEN];
    MACRO *current_macro =NULL ;
    int in_macro=FALSE;
    int line_number = 0;

    while (fgets (line,sizeof(line) , src_file)!= NULL)
    {
        line_number++;
        if (!in_macro)
        {
            if (!is_mcro_start (line ))
            {
                continue;
            }
            current_macro =create_macro_from_line(line, line_number);
            if(current_macro== NULL)
                return ERROR;
            in_macro= TRUE;
            continue;
        }

        if(is_mcro_end( line))
        {
            add_macro_to_list(head, current_macro);
            current_macro= NULL;
            in_macro =FALSE;
            continue;
        }

        if (is_mcro_start( line))
        {
            fprintf(stderr,"Error: Macro inside another macro on line %d: %s", line_number, line);
            return ERROR;
        }

        if (add_line_to_macro ( current_macro, line) !=SUCCESS)
        {
            fprintf(stderr, "Error: Failed to add line to macro on line %d: %s",line_number, line);
            return  ERROR;
        }
    }

    if( in_macro)
    {
        fprintf(stderr  ,"Error: Missing mcroend at the end of the file on line %d: %s\n", line_number, line);
        return ERROR;
    }

    return SUCCESS;
}

/*
Searches for a macro with a certain name.
It receives the head of the macro list and the name to search for.
It returns a pointer to the right MACRO, or NULL if it wasn't found.
It compares each macro's name to the wanted name.
It assumes head points to a valid list.
*/

static MACRO *find_macro(MACRO *head ,const char *name)
{
    MACRO *current =head;
    while (current != NULL)
    {
        if (strcmp(current->name , name)== 0)
            return current;
        current=current->next;
    }
    return NULL;
}


/*
Reads a source file and writes an .am file with expanded macros.
It receives the source file, the .am file and the macro list.
It returns SUCCESS after finishing.
It replaces macro's with their body lines and skips the definitions of macros.
It assumes head was already built by build_macro_list.
*/

int expand_macros(FILE *src_file, FILE *am_file,MACRO *head )
{
    char line[MAX_LINE_LEN ];
    char buf [MAX_LINE_LEN] ;
    char *trimmed ;
    char *colon;
    char *macro_name;
    MACRO *called_macro ;
    MACRO_LINE  *body_line;
    int in_macro= FALSE;
    int has_label;

    while ( fgets( line,sizeof(line ),src_file) !=NULL)
    {

        if (!in_macro &&is_mcro_start( line))
        {
            in_macro =TRUE ;
            continue;
        }
        if ( in_macro)
        {
            if (is_mcro_end( line) )
                in_macro=FALSE;
            continue;
        }

        strcpy(buf, line);
        trimmed =trim(buf) ;
        called_macro= find_macro(head,trimmed);
        has_label = FALSE;
        colon = strchr(trimmed, ':');
        if (called_macro == NULL && colon != NULL)
        {
            *colon = '\0';
            macro_name = trim(colon + 1);
            called_macro = find_macro(head, macro_name);
            has_label = called_macro != NULL;
        }

        if (called_macro==NULL)
        {
            fputs( line,am_file);
            continue;
        }

        body_line= called_macro-> lines;
        if (has_label)
        {
            fputs(trimmed, am_file);
            fputs(": ", am_file);
        }
        while(body_line !=NULL)
        {
            fputs( body_line ->content,am_file) ;
            body_line= body_line-> next;
            if (has_label)
            {
                has_label = FALSE;
            }
        }
    }
    return SUCCESS;
    
}


/*
Frees all memory used by the macro list.
It receives the head of the macro list.
For each macro it frees the lines and then frees the macro node itself.
It assumes the list was built using create_macro and add_line_to_macro and that all the pointers in the macros are valid.
*/


void free_macro_list(MACRO  *head)
{
    MACRO *macro_temp ;
    MACRO_LINE  *line_current;
    MACRO_LINE *line_temp;

    while (head!=NULL)
    {
        line_current= head->lines ;
        while ( line_current!= NULL)
        {
            line_temp =line_current;
            line_current= line_current->next ;
            free (line_temp) ;
        }
        macro_temp  =head;
        head =head-> next;
        free( macro_temp);
        
    }
}


