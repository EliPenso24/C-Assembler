/*
second_pass.c
this file purpose is to run the second pass on the code to resolve missing label addresses and patch instructions
it loops on all unresolved references, searches symbols in table, checks if external to, record usage, calculates address values, and changes the instruction word in code image
it receives am_file pointer to expanded file and st pointer to symbol table
and returns success if finished clean error if symbol not found or error flag set
it assumes that st is valid symbol table or null, and code image and unresolved list are ready
*/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "symbol_table.h"
#include "parser.h"
#include "first_pass.h"
#include "second_pass.h"
#include "error_handler.h"

/* thia function run second pass to fix labels in instructions
the main params she receives are am_file pointer and st symbol table pointer, it also uses ref pointer, symbol pointer, image array, word variable and value number, she return sucess or error
the function main algorithm is to get code image and unresolved list, loop on list with ref pointer, find symbol in table, if external record it, calculate address difference or value, change bits in word, patch instruction in image and check the error flag at the end
the assumptions she gets are that the am_file is valid file pointer or null, st is valid symbol table or null, and code image and unresolved list are ready */
int second_pass(FILE *am_file, SYMBOL *st) {
    UNRESOLVED_REF *ref;
    SYMBOL *symbol;
    unsigned int *image;
    unsigned int word;
    int value;

    (void)am_file;
    image = get_code_image();
    ref = get_unresolved_list();
    while ( ref !=NULL) { /* loop on all unresolved references until end */
      symbol = find_symbol(st, ref->label);
      if (symbol== NULL) {
          return ERROR;} /* error if symbol not  */
      if (symbol->type& SYMBOL_EXTERNAL) {
        record_external_usage(ref->label, ref->address); /* record usage of external symbol */
        ref = ref->next;
        continue;}
      if (ref->patch_type == PATCH_RELATIVE) {
        value = symbol->address -ref->address;} /* calculate relative distance */
      else {
        value= symbol->address;} /* use absolute symbol address */
      word = image[(ref->address - IC_START) / 4]; /* get current instruction word from image*/
      if (ref->patch_type == PATCH_RELATIVE) { /* update word for relative patch */
        word = (word & 0xffff0000U)| ((unsigned int)value & 0xffffU);} 
      else { /* update word for absolute patch */
        word = (word & 0xfe000000U) | ((unsigned int)value & 0x01ffffffU);}
      patch_instruction(ref->address,(int)word); /* patch the instruction in code image */
      ref = ref->next;
}

    if (get_error_flag()) { /* check if errors found */
      return ERROR;
}
    else{
      return SUCCESS;} /* second pass finished successfully */
}


