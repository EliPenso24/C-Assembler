/*
  output_generator.c
  Purpose: The output file writers.
  This code writes the output files. write_ob_file goes over
  the code and data images together, and prints 4 bytes per line with a decimal address. write_ent_file and write_ext_file each go over their lists and print one
  line for each entry.

  The functions receive an open file pointer and the data (arrays
  or lists) made by the first pass and symbol table.
  It returns a file written in the format the project requires.

  It assumes the code image, data image, and symbol table are already complete and finalized by the time these functions are called.
 */



#include <stdio.h>
#include "globals.h"

#include "symbol_table.h"
#include "first_pass.h"

#include "output_generator.h"


/*
Writes the .ob file with a header and a memory image.
It receives the output file pointer, the code image array, the code length(bytes), the data image array, the data length in bytes.
It returns SUCCESS after writing the file.
It writes the header, then goes through the code image and the data image as one byte stream, and prints 4 bytes in each line with a decimal address with 4 digits.

*/
int write_ob_file(FILE *ob_file ,unsigned  int *code_image ,int code_len, unsigned char *data_image, int data_len)
{
    int  addr;
    int  total_len ;
    int i ;
    unsigned char byte ;
    fprintf(ob_file, "%d %d\n",code_len , data_len);
    total_len=code_len + data_len; 
    addr =IC_START;
    for (i =0; i< total_len;i+= 4)
    {
        int j ;
        fprintf(ob_file , "%04d",addr);
        for (j =0; j< 4 && (i + j)< total_len; j++)
        {
            int byte_offset;
            byte_offset= i + j;
            if ( byte_offset < code_len)
            {
                int word_index;
                int shift;
                word_index =byte_offset / 4;
                shift = (byte_offset % 4) * 8;
                byte =( unsigned char) ((code_image[word_index] >> shift) & 0xFF);
            }
            else
            {
                byte =data_image[ byte_offset-code_len];
            }
            fprintf(ob_file , " %02X",byte);
        }
        fprintf( ob_file,"\n");
        addr +=4;
    }
    return SUCCESS;
}


/*
Writes the .ent file by writing one line for every entry symbol. The line includes its name and address.
It receives the output file pointer  and the head of the symbol table.
It returns SUCCESS after writing the file.
It goes over the symbol table and prints every symbol that is SYMBOL_ENTRY.

*/
int write_ent_file(FILE *ent_file,  SYMBOL  *symbol_table)
{
    SYMBOL  *current;
    current=symbol_table ;
    while (current !=NULL)
    {
        if (current->type & SYMBOL_ENTRY)
        {
            fprintf(ent_file,"%s %04d\n", current-> name,current->address) ;
        }
        current= current-> next;
        
    }
    return  SUCCESS;
}


/*
Writes the .ext fileby writing one line per usage of an external symbol. The line includes its name and the address where it was used.
It receives the output file pointer and the head of the external usage list.
It returns SUCCESS after writing the file.
It goes over the external usage list and prints one line foe every node (occurrence).

*/
int write_ext_file(FILE *ext_file,EXTERNAL_USAGE *external_list)
{
    EXTERNAL_USAGE  *current;
    current =external_list;
    while (current!= NULL)
    {
        fprintf(ext_file,"%s %04d\n",current-> label, current->address);
        current= current  ->next;
    }
    return SUCCESS;
}


