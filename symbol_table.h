/*
symbol_table.c
the purpose of this file id tp manage the symbol table linked list for labels and variables
it adds new symbols to the list, searches for existing labels by name, updates data symbol addresses after first pass, and frees all memory at the end
it receives head pointer of symbol table, symbol name string, address number, and type flags
and returns pointer to found symbol or success/error status
*/

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"

/* this is the possible types for symbols based on the lable
*/
typedef enum {
SYMBOL_CODE = 1,
SYMBOL_DATA = 2,
SYMBOL_EXTERNAL = 4,
SYMBOL_ENTRY = 8
} SYMBOL_TYPE;

/* this struct is hold the symbol info like name address type and pointer to next symbol in linked list
*/
typedef struct Symbol {
char name[MAX_NAME_LEN + 1];
int address;
int type;
struct Symbol *next;
} SYMBOL;


/* this function is insert new symbol to the linked list, the main params she receives are head pointer name string addr and type, it return 1 for good or 0 if fail */
int insert_symbol(SYMBOL **head, const char *name, int addr, int type);


/* this function search for symbol in the list
the params she receives is head pointer and name string to look for, it return pointer or NULL if she didnt found */
SYMBOL *find_symbol(SYMBOL *head, const char *name);

/* this function updates all data symbol addresses after pass 1 finished
the main params she receives are head pointer and final_ic number that added to every data symbol */
void update_data_symbol_addresses(SYMBOL *head, int final_ic);

/* this function is free all the memory we allocated for teh symbol table list
the param she receives is head pointer */
void free_symbol_table(SYMBOL *head);
#endif


