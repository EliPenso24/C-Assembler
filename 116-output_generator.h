/*
  output_generator.h
  Purpose: Export interface for the assembler's output file generation.
  This file include headers of functions meant to write the output files. write_ob_file goes over
  the code and data images together, and prints 4 bytes per line with a decimal address. write_ent_file and write_ext_file each go over their lists and print one
  line for each entry.

  The functions receive an open file pointer and the data (arrays
  or lists) made by the first pass and symbol table.
  It returns a file written in the format the project requires.
*/

#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include <stdio.h>
#include "globals.h"
#include "symbol_table.h"
#include "first_pass.h"

#define SUCCESS 0 /*code for success */
#define ERROR 1 /* code for error */


/*
Writes the .ob file with a header and a memory image.
It receives the output file pointer, the code image array, the code length(bytes), the data image array, the data length in bytes.
It returns SUCCESS after writing the file.
It writes the header, then goes through the code image and the data image as one byte stream, and prints 4 bytes in each line with a decimal address with 4 digits.
*/
int write_ob_file(FILE *fp, unsigned int *code_img, int ic_size, unsigned char *data_img, int dc_size);


/*
Writes the .ent file by writing one line for every entry symbol. The line includes its name and address.
It receives the output file pointer  and the head of the symbol table.
It returns SUCCESS after writing the file.
*/
int write_ent_file(FILE *fp, SYMBOL *head);


/*
Writes the .ext fileby writing one line per usage of an external symbol. The line includes its name and the address where it was used.
It receives the output file pointer and the head of the external usage list.
It returns SUCCESS after writing the file.
*/
int write_ext_file(FILE *fp, EXTERNAL_USAGE *head);


#endif




