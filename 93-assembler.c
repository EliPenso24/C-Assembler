/*
   assembler.c
   
   Purpose: This part includes the main function for the two-pass assembler.
   This code translates assembly source files into binary machine
   instructions and data images for execution on a hypothetical target processor defined by our university course.
   It does a process with 4 phases for each file. The first phase is the macro expansion into a .am file,
   than the first pass symbol table construction and the calculation of IC and DC,
   after that the second pass machine code encodes and takes care of label resolution, and lastly the output file generation.

   It receives assembly source file paths theough the cmd
   and creates the expanded version(.am), object code( .ob), entry symbol (.ent), and
   external reference (.ext) files if there are no errors.

   It assumes input line lengths stay within MAX_LINE_LEN limit, and that critical
   syntax or phase errors immediately halt processing for the affected file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "preprocessor.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "output_generator.h"
#include "error_handler.h"



/*
this function appends an extension string to a base filename.
It receives the destination buffer, the base name, and the extension string.
It returns nothing.
*/
void add_extension(char *dest,  const char *name , const char *ext)
{
    strcpy(dest ,name);
    strcat(dest, ext);
}


/*
Checks if the symbol table contains any entry symbols.
It receives the pointer to the head of the symbol table.
It returns TRUE if an entry symbol is found, or FALSE.
*/
static int has_entry_symbols(SYMBOL  *symbol_table) {
    while (symbol_table !=NULL) {
        if ( symbol_table->type &SYMBOL_ENTRY) return TRUE;
        symbol_table= symbol_table->next;
    }
    return FALSE;
}


/*
It validates the .as file extension and extracts the base file name.
It receives the raw input path string, the destination buffer for the .as filename, and the buffer for the base name.
It returns SUCCESS if the extension is valid, or eRROR if it is'nt.
*/
static int split_input_name(const char *input, char *filename_as, char *base_name) {
    size_t length =strlen(input);
    if (length <=strlen(AS_EXTENSION)|| strcmp(input+ length-strlen(AS_EXTENSION),AS_EXTENSION) !=0) {
        return ERROR;
    }
    strcpy(filename_as,input);
    strncpy(base_name,input, length - strlen(AS_EXTENSION));
    base_name[ length- strlen(AS_EXTENSION)] = '\0';
    return SUCCESS;
}



int main(int argc,char *argv[]) {
    int i;
    if (argc <2) {
        fprintf(stderr,"Usage: %sfile1 file2 ...\n",argv[0]);
        return ERROR ;
    }

    for (i= 1; i < argc; i++ ) {
        char filename_as[ MAX_LINE_LEN],filename_am[MAX_LINE_LEN] , filename_out[MAX_LINE_LEN] , base_name[ MAX_LINE_LEN];
        FILE *as_file, *am_file, *ob_file, *ent_file, *ext_file;
        MACRO *macro_head = NULL;
        SYMBOL *symbol_table =NULL;

        printf("\n--- Starting assembler for: %s ---\n", argv[i]);
        reset_error_flag() ;

        if (split_input_name(argv[i], filename_as, base_name) != SUCCESS) {
            fprintf(stderr, "Error: input file must have the %s extension: %s\n",AS_EXTENSION, argv[ i]);
            continue ;
        }
        as_file = fopen(filename_as,"r");
        if (!as_file) {
            fprintf(stderr,"Error: Could not open %s\n", filename_as);
            continue ;
        }


        add_extension(filename_am, base_name, AM_EXTENSION);

        if (build_macro_list(as_file, &macro_head) == SUCCESS) {
            rewind(as_file); /*Resets the pointer to the start of the file for macro expansion  */
            am_file = fopen(filename_am, "w");
            if (am_file) {
                expand_macros(as_file, am_file, macro_head);
                fclose(am_file);
            }
        } 
        else {
            printf("Error: Preprocessor failed for %s\n", filename_as);

            fclose(as_file);
            free_macro_list(macro_head);
            continue; 
        }
        fclose(as_file);
        free_macro_list(macro_head);

        if (get_error_flag()) continue;

        am_file = fopen(filename_am, "r");
        if (!am_file) continue;
        
        if (first_pass(am_file,&symbol_table )== SUCCESS) {
            rewind(am_file);
            if (second_pass(am_file, symbol_table)  ==  SUCCESS&& !get_error_flag()) {
                
                add_extension(filename_out, base_name, OB_EXTENSION);
                ob_file =fopen( filename_out,"w");
                if (ob_file){
                    /*  Subtract IC_START's base offset to pass the actual instruction count*/
                    write_ob_file(ob_file, get_code_image(), get_ic()-IC_START, get_data_image(), get_dc()); 
                    fclose(ob_file) ;
                }

                if ( has_entry_symbols(symbol_table)) {
                    add_extension(filename_out, base_name, ENT_EXTENSION);
                    ent_file= fopen(filename_out, "w");
                    if (ent_file) {
                        write_ent_file( ent_file,symbol_table);
                        fclose(ent_file) ;
                    }
                }

                if ( get_external_usage_list() !=NULL)
                {
                    add_extension(filename_out,base_name, EXT_EXTENSION);
                    ext_file =fopen(filename_out , "w");
                    if ( ext_file)
                    {
                        write_ext_file( ext_file, get_external_usage_list());
                        fclose( ext_file);
                    }
                }
                
                printf("\n---Successfully assembled file: %s---\n", argv[i]);
            }
            else {
                fprintf(stderr ,"Error: second pass failed for %s\n",filename_am ) ;
            }
        }
        else {
            fprintf(stderr,"Error: first pass failed for %s\n", filename_am);
        }
        
        fclose(am_file ) ;
        free_symbol_table( symbol_table);
    }
    return SUCCESS;
}
