#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"
#include "symbol_table.h"

/*instruction type constants */
#define TYPE_R 0
#define TYPE_I 1

#define TYPE_J 2
#define TYPE_UNKNOWN -1

/* patch type constants */

#define PATCH_ABSOLUTE 0
#define PATCH_RELATIVE 1


/* maximum number of code words*/
#define CODE_IMAGE_WORDS ((MEMORY_SIZE/4)- ( IC_START/4))


typedef union {
    unsigned int value;
    struct {
        unsigned int immediate : 16; /* [15-0]  (16 bits) */
        unsigned int rt        : 5;  /* [20-16] (5 bits) */
        unsigned int rs        : 5;  /* [25-21] (5 bits) */
        unsigned int opcode    : 6;  /* [31-26] (6 bits) */
    } fields;
} IOpcode;


/*node representing an unresolved label */
typedef struct UnresolvedRef
{
    int address;
    char label[MAX_NAME_LEN ];
    int patch_type ;
    struct UnresolvedRef *next;
} UNRESOLVED_REF;


/* node for an externally used label*/
typedef struct ExternalUsage {
    char label[MAX_NAME_LEN ];
    int address;
    struct ExternalUsage *next;
} EXTERNAL_USAGE;


/*

Finds what the instruction type is.
It receives a pointer to the opcode.
It returns one the types (R/I/J) or unknown.
*/
int classify_instruction(const char *opcode);

/*
recoeds an occurrence of an external symbol usage for the .ext file.
It receives the name of the external label and the address where it was used.
It does not return a value.
*/

void record_external_usage(const char *label, int address);

/*
Run first pass over our expanded .am file so it can handle labels and instructions.
It receives the pointer to our open .am file and the address of the symbol table's beginning.
It returns SUCCESS if the pass finished or ERROR if a label or instruction was invalid.
*/

int first_pass(FILE *am_file, SYMBOL **symbols);

/*
Writes a value into the code image at the address that we want.
It receives the address to patch and the value we want to write.
*/
void patch_instruction(int address, int value);

/*

Returns the current value of the instruction counter.

It does not recieve.
It indicates how many words we encoded.
*/
int get_ic(void);


/*
Returns the current value of the data counter.
It shows how many bytes of data we handled.
*/

int get_dc(void);

/*
Returns the head of the list of unresolved labels (holes).
It does not recieve.
It returns a pointer to the first UNRESOLVED_REF node, or NULL if there are none.
*/
UNRESOLVED_REF *get_unresolved_list(void);


/*
Returns the head of the list of unresolved labels (holes).
It does not recieve.
It returns a pointer to the first EXTERNAL_USAGE node, or NULL if there are none.
*/
EXTERNAL_USAGE *get_external_usage_list(void);

/*
Returns a pointer to the code image array.
It does not recieve.
It returns the address of the code image.
*/

unsigned int *get_code_image(void);


/*

Returns a pointer to the data image array.
It gives access to the assembled bytes.
*/
unsigned char *get_data_image(void);


#endif
