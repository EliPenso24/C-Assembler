/*
second_pass.c
this file purpose is to run the second pass on the code to resolve missing label addresses and patch instructions
it loops on all unresolved references, searches symbols in table, checks if external to, record usage, calculates address values, and changes the instruction word in code image
it receives am_file pointer to expanded file and st pointer to symbol table
and returns success if finished clean error if symbol not found or error flag set
*/

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include "symbol_table.h"

/*
the second pass function resolves label addresses and finishes the "machine" code
it assumes the symbol table is already full from the first pass
*/
int second_pass(FILE *am_file, SYMBOL *symbol_table);

#endif



