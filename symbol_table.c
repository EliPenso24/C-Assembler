/*
symbol_table.c
the purpose of this file id tp manage the symbol table linked list for labels and variables
it adds new symbols to the list, searches for existing labels by name, updates data symbol addresses after first pass, and frees all memory at the end
it receives head pointer of symbol table, symbol name string, address number, and type flags
and returns pointer to found symbol or success/error status
it assumes that symbol names are valid null terminated strings and memory allocation is available
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symbol_table.h"

/* the function inserts new symbol to the linked list, the main params she receives are head pointer to head of list, name string, addr number and type of symbol, it use node variable for new symbol and p pointer to proggres on list, return 1 for successed or 0 if fail
the function main algoritem is to check if symbol exists with find_symbol, allocates memory for node and copying details to node, search for end of list with p and attach new node
the assumption she gets are that head is valid pointer to pointer, name is valid null terminated string and addr is not negative number */
int insert_symbol(SYMBOL **head, const char *name, int addr, int type) {
    SYMBOL *p, *node;

    if (find_symbol(*head, name)) { /* making sure the symbol isnt already there */
      return ERROR; /* error duplicate symbol */
    }
    
    node = (SYMBOL *)malloc(sizeof(SYMBOL)); /* allocate memory for new node */
    if (!node) {
      printf("memory allocation failed for symbol table!\n");
      return ERROR; /* allocation failed */
}

    /* fill in the symbol details inside node */
    strncpy(node->name, name, MAX_NAME_LEN);
    node->name[MAX_NAME_LEN -1] = '\0';
    node->address = addr;
    node->type = type;
    node->next = NULL;

    if (*head ==NULL) {
      *head = node;}
    else {
      p = *head; /* p pointer start from head to find the end */
      while (p->next!= NULL) {
        p = p->next; }
      p->next = node; 
}
    return SUCCESS; /* successed */
}

/* the function search for symbol in the list, the main params she receives are head pointer to start of list and name string to look for, it use p pointer to progres on the list, return pointer to symbol if found or NULL if not found */
SYMBOL *find_symbol(SYMBOL *head, const char *name) {
    SYMBOL *p = head;
    while (p !=NULL) { /* loop that progres on list until the end */
      if (strcmp(p->name,name) == 0) { /* check if name is matched */
      return p;}
    p = p->next;
    }
    return NULL;
}

/* the function is update all data symbol addresses after first pass finished
the main params she receives are head pointer to start of list and final_ic number that added to every data symbol, it use p pointer to progres on the list
the function main algorithm is to loop on list with p pointer, check if symbol type is data symbol and add final_ic number to symbol address
and the assumptions she gets are that head is valid pointer to symbol table or null if list empty and final_ic is non negative number */
void update_data_symbol_addresses(SYMBOL *head, int final_ic) {
    SYMBOL *p = head; 
    while (p != NULL) {
      if ( p->type & SYMBOL_DATA) {
        p->address += final_ic;}
    p = p->next;}
}

/* this function free all the memory we allocated for teh symbol table list, the main params she receives is head pointer to start of list, it use temp pointer to save current node before she frees it
the function main algorithm is to loop on list while head is not null, save head in temp, move head to next node and free temp memory
the assumptions she gets are that head is valid pointer to symbol table or null if list empty */
void free_symbol_table(SYMBOL *head) {
    SYMBOL *temp;
    while (head != NULL) {
      temp = head;
      head = head->next;
      free(temp);}
}


