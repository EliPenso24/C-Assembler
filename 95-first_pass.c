/*
  first_pass.c
  Purpose: The first pass.
  This code defines every line as a label, directive, or
  instruction. It adds the Labels to the symbol table at the current address.
  It checks if instructions are R/I/J, parses the operands,
  and the result is packed into a 32-bit word according to each
  type's bit layout. The Branch distances is found immediately when the
  target label is known. or the instruction is encoded
  and the label is saved as unresolved for the second pass.
  It receives an open .am file and the address of the symbol table's head pointer.
  It returns a filled code image array, and it saves unresolved
  label references and every external symbol usage.
  
  It assumes all operand syntax matches the project instruction, and that registers and immediates are validated before encoding.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "globals.h"
#include "symbol_table.h"
#include "first_pass.h"

/* Images written to the object file after both assembler passes work. */
static unsigned int code_image[CODE_IMAGE_WORDS];
static unsigned char data_image[MEMORY_SIZE];
static int ic = IC_START;
static int dc = DC_START;

/* Lists used by the second pass and by the .ext output writer. */
static UNRESOLVED_REF *unresolved_head = NULL;
static EXTERNAL_USAGE *external_head = NULL;

/*ntry directives can legally precede the definition of their symbol. */
static char entry_names[100][MAX_NAME_LEN];
static int entry_line_numbers[100];
static char entry_lines[100][MAX_LINE_LEN];
static int entry_count = 0;

/* Instruction tables taken from the assignment specification. */
static const char *r_names[] = {
    "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo", NULL
};
static const int r_opcode[] = {0, 0, 0, 0, 0, 1, 1, 1};
static const int r_funct[] = {1, 2, 3, 4, 5, 1, 2, 3};

static const char *i_names[] = {
    "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt",
    "bgt", "lb", "sb", "lw", "sw", "lh", "sh", NULL
};
static const int i_opcode[] = {
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
};

static const char *j_names[] = {"jmp", "la", "call", "hlt", NULL};
static const int j_opcode[] = {30, 31, 32, 63};


/*
Frees the linked lists used for unresolved references and external usages.
It does not receive or return.
It goes through the unresolved_head and external_head lists, freeing each node and setting the heads to NULL.
It assumes the lists were allocated.
*/
static void reset_dynamic_lists(void)
{
    UNRESOLVED_REF *unresolved;
    UNRESOLVED_REF *next_unresolved;
    EXTERNAL_USAGE *external;
    EXTERNAL_USAGE *next_external;

    for (unresolved = unresolved_head; unresolved != NULL;
         unresolved = next_unresolved) {
        next_unresolved = unresolved->next;
        free(unresolved);
    }

    for (external = external_head; external != NULL; external = next_external) {
        next_external = external->next;
        free(external);
    }

    unresolved_head = NULL;
    external_head = NULL;
}


/*
Finds the index of an instruction in an instruction table.
It receives the mnemonic string to search for and an array of string pointers.
It returns the index of the mnemonic if found, or -1.
It does an linear search through the array using strcmp.
It assumes the array is without null.
*/
static int find_name(const char *name, const char *names[])
{
    int i;

    for (i = 0; names[i]!= NULL; i++ ) {
        if (strcmp(name, names[i])== 0) {
            return i;
        }
    }
    return  -1;
}



/*
Finds what the instruction type is.
It receives a pointer to the opcode.
It returns one the types (R/I/J) or unknown.
It compares the received opcode with the patterns of the different types
It assumes the opcode is shorter or equal to MAX_NAME_LEN/
*/
int classify_instruction(const char *opcode)
{
    if (find_name(opcode, r_names) >= 0) {
        return TYPE_R;
    }
    if (find_name(opcode, i_names) >= 0) {
        return TYPE_I;
    }
    if (find_name(opcode, j_names) >= 0) {
        return TYPE_J;
    }
    return TYPE_UNKNOWN;
}


/*
Validates a symbol name used by labels, .entry and .extern directives.
It recieves a string pointer representing the symbol name.
It returns TRUE if the name is valid or FALSE.
It checks that the first character is a valid, and the length is within MAX_NAME_LEN.
It assumes the string is null-terminated.
*/
static int valid_name( const char *name)
{
    int i ;

    if (name == NULL||
        (!isalpha((unsigned char)name[0] ) &&  name[ 0] !='_')) {
        return FALSE;
    }

    for (i = 1; name[i ]!= '\0'; i++) {
        if (!isalnum((unsigned char )name[i]) &&name[i] !='_') {
            return  FALSE;
        }
    }

    return strlen(name) < MAX_NAME_LEN;
}


/*
Parses a decimal integer token from a string.
It receives the text token and a pointer to a long variable to store the parsed value.
It returns SUCCESS if the parsing was completely successful, or ERROR otherwise.
It uses strtol to convert the text and checks if there is any trailing non-numeric garbage.
It assumes the text pointer is not null.
*/
static int parse_number(const char *text, long *value)
{
    char *end;

    if (text == NULL || *text == '\0') {
        return ERROR;
    }

    *value = strtol(text, &end, 10);
    return *end == '\0' ? SUCCESS : ERROR;
}



/*
Turns a register token of the form "$N" into its numeric value.
It receives the token string.
It returns the register number (0-31), or -1 if the token is not a valid register.
It checks for the leading '$' and converts the remaining digits, validating the range.
*/
static int parse_register(const char *text)
{
    long number;

    if (text == NULL || text[0] != '$' ||
        parse_number(text + 1, &number) != SUCCESS) {
        return -1;
    }

    return number >= 0 && number < NUM_REGISTERS ? (int)number : -1;
}


/*
Adds a new unresolved label reference to our list of holes.
It receives the address we need to patch later, the label name , and the patch type.
IT allocates a new node on the heap and links it to the very front of the list.
it assumes the label name isn't longer than MAX_NAME_LEN.

*/
static void add_unresolved(int address, const char *label, int patch_type)
{
    UNRESOLVED_REF *node;

    node = (UNRESOLVED_REF *)malloc(sizeof(UNRESOLVED_REF));
    if (node == NULL) {
        return;
    }

    strcpy(node->label, label);
    node->address = address;
    node->patch_type = patch_type;
    node->next = unresolved_head;
    unresolved_head = node;
}


/*
recoeds an occurrence of an external symbol usage for the .ext file.
It receives the name of the external label and the address where it was used.
It does not return a value.
It adds a new EXTERNAL_USAGE node and copies the data, and links it to the front of the external_head list.
It assumes the label name is shorter than MAX_NAME_LEN.
*/
void record_external_usage(const char *label, int address)
{
    EXTERNAL_USAGE *node;

    node = (EXTERNAL_USAGE *)malloc(sizeof(EXTERNAL_USAGE));
    if (node == NULL) {
        return;
    }

    strcpy(node->label, label);
    node->address = address;
    node->next =external_head;
    external_head  =node ;
}


/*
adds a single byte to the data image array.
It receives the long value that needs to be added.
It returns SUCCESS if the byte was added, or ERROR.
It casts the value to an unsigned char, stores it in the data_image at the current dc, and adds to dc.
It assumes dc is the right value.
*/
static int add_byte(long value)
{
    if (dc >=MEMORY_SIZE|| value  <-128 || value > 255) {
        return ERROR;
    }

    data_image[dc++ ] = (unsigned char)value;
    return SUCCESS;
}


/*
Encodes .db, .dh, and .dw in little-endian.
It receives the directive and numeric arguments strings.
It returns SUCCESS if parsing is, or ERROR.
It parses numbers that are seperated by commas and stores them.
It assumes the arguments string is not null.
*/
static int add_numbers(const char *directive, const char *arguments)
{
    char work[MAX_LINE_LEN];
    char *token;
    long value;
    int size;
    int i;

    size = strcmp(directive, ".db") == 0 ? 1 :
           (strcmp(directive, ".dh") == 0 ? 2 : 4);

    strcpy(work, arguments);
    token = strtok(work ,", \t\r\n");
    if (token ==NULL) {
        return ERROR;
    }

    while (token !=NULL)
    {
        if (parse_number(token, &value) !=SUCCESS ||
            (size==1 && (value <-128|| value > 255)) ||
            (size== 2 &&(value < -32768|| value > 65535))) {
            return ERROR;
        }

        for (i =0; i < size; i++) {
            if (add_byte((( unsigned long)value >> (8 * i)) & 255UL) != SUCCESS) {
                return ERROR;
            }
        }
        token=strtok(NULL, ", \t\r\n");
    }

    return SUCCESS;
}


/*
Encodes an .asciz string into the data image.
It receives arguments string.
It returns SUCCESS on valid insertion, or ERROR if there are missing quotes or full memory.
It stores characters between quotes as bytes.
It assumes the string is properly formed.
*/
static int add_string(const char *arguments)
{
    const char *text;

    while (isspace((unsigned char)*arguments)) {
        arguments++;
    }
    if (*arguments++!= '"') {
        return ERROR;
    }

    for (text = arguments; *text!='\0' && *text != '"';text++) {
        if (add_byte((unsigned char)*text)!= SUCCESS ) {
            return ERROR;
        }
    }
    if (*text !='"') {
        return  ERROR;
    }

    text++ ;
    while (isspace((unsigned char)*text)) {
        text++;
    }

    return (*text == '\0' || *text == ';') ? add_byte(0) : ERROR;
}


/*
Handles assembler directives.
It receives the directive opcode, arguments, symbol table pointer, and optional label.
It returns SUCCESS if valid, or ERROR.
It updates the symbol table.
It assumes the opcode string represents a directive.
*/
static int handle_directive(char *opcode, char *arguments, SYMBOL **symbols,
                            const char *label, int line_number,const char *original_line)
{
    char name[MAX_NAME_LEN ];

    if (strcmp(opcode, ".db" ) ==0 || strcmp(opcode, ".dh") == 0 ||
        strcmp(opcode, ".dw") ==0 || strcmp(opcode, ".asciz") == 0) {
        if (label != NULL &&
            insert_symbol(symbols , label,dc,SYMBOL_DATA) != SUCCESS) {
            return ERROR;
        }
        return strcmp(opcode, ".asciz" )==0 ?
               add_string(arguments):add_numbers( opcode, arguments);
    }

    if (sscanf(arguments,"%30s",name)!= 1 || !valid_name(name)) {
        return ERROR;
    }

    if (strcmp(opcode,".extern") ==0) {
        return find_symbol(*symbols, name) == NULL ?
               insert_symbol(symbols, name, 0, SYMBOL_EXTERNAL) : ERROR;
    }

    if (strcmp(opcode, ".entry") == 0 && entry_count < 100) {
        strcpy(entry_names[entry_count], name);
        entry_line_numbers[entry_count]=line_number;
        strcpy(entry_lines[entry_count] ,original_line);
        entry_count++ ;
        return SUCCESS;
    }

    return ERROR;
}


/*
Encodes an instruction word and updates the instruction counter.
It receives the instruction opcode, arguments string, and symbol table.
It returns SUCCESS on valid encoding, or ERROR on syntax problem.
It parses the operands, fits the bits into a 32-bit word, add to the code image, and ups the IC by 4.
It assumes the opcode is a valid instruction.
*/
static int encode_instruction(char *opcode, char *arguments, SYMBOL *symbols)
{
    char work[MAX_LINE_LEN];
    char *first;
    char *second;
    char *third;
    int index;
    int rd;
    int rs;
    int rt;
    int type;
    long immediate;
    unsigned int word;
    SYMBOL *target;
    IOpcode iopcode;

    strcpy(work, arguments);
    first =strtok(work, ", \t\r\n");
    second= strtok(NULL, ", \t\r\n");
    third =strtok(NULL, ", \t\r\n");
    type = classify_instruction(opcode);

    if (type ==TYPE_R) {
        index  =find_name(opcode, r_names);
        rs = parse_register(first );
        rt = parse_register( second);
        rd = parse_register(third);

        if (index < 0 ||rs< 0||
            (index <= 4 &&(rt< 0 || rd < 0)) ||
            (index > 4&&  (rt< 0 ||third!= NULL ))) {
            return ERROR ;
        }

        word = ((unsigned int) r_opcode[index]<< 26)  |
               ((unsigned int)rs <<  21) |
               ((unsigned int)(index<= 4 ? rt : 0) << 16) |
               ((unsigned int)(index<= 4 ? rd : rt) << 11) |
               ((unsigned int)r_funct[index]<< 6);
    } else if (type == TYPE_I){
        index = find_name(opcode,i_names);
        rs = parse_register(first) ;
        rt = parse_register(index>= 5&& index <= 8 ? second : third);

        if (rs <  0|| rt < 0 ||
            (index >=  5 &&index<=8? third == NULL : second == NULL)) {
            return ERROR;
        }

        immediate = 0;
        if (index >= 5 && index <= 8) {
            target = find_symbol(symbols, third);
            if (target != NULL && !(target->type & SYMBOL_EXTERNAL)) {
                immediate = target->address - ic;
            } else if (target != NULL) {
                record_external_usage(third, ic);
            } else {
                add_unresolved(ic, third, PATCH_RELATIVE);
            }
        } else if (parse_number(second, &immediate) != SUCCESS ||
                   immediate < -32768 || immediate > 32767) {
            return ERROR;
        }

        

        iopcode.fields.immediate = immediate;
        iopcode.fields.rt = rt;
        iopcode.fields.rs = rs;
        iopcode.fields.opcode = i_opcode[index];
        word = iopcode.value;
        
        } else if (type == TYPE_J) {
            index = find_name(opcode, j_names);
            if ((index == 3 && first != NULL) || (index != 3 && first == NULL)) {
                return ERROR;
            }

            if (index == 3) {
                word = (unsigned int)j_opcode[index] << 26;
            } else if (index== 0 && first[0] =='$') {
                rd = parse_register(first);
                if (rd < 0) {
                    return ERROR;
                }
                word = ((unsigned int)j_opcode[index] << 26) |
                       (1U << 25) | (unsigned int)rd;
            } else {
            target = find_symbol(symbols, first);
            immediate = 0;
            if (target != NULL && !(target->type & SYMBOL_EXTERNAL)) {
                immediate = target->address;
            } else if (target != NULL) {
                record_external_usage(first, ic);
            } else {
                add_unresolved(ic, first, PATCH_ABSOLUTE);
            }
            word = ((unsigned int)j_opcode[index] << 26) |
                   ((unsigned int)immediate & 0x01ffffffU);
        }
    } else {
        return ERROR;
    }

    if ((ic - IC_START) / 4 >= CODE_IMAGE_WORDS) {
        return ERROR;
    }

    code_image[(ic -IC_START)/ 4]=word;
    ic += 4;
    return SUCCESS;
}


/*
Run first pass over our expanded .am file so it can handle labels and instructions.
It receives the pointer to our open .am file and the address of the symbol table's beginning.
It returns SUCCESS if the pass finished or ERROR if a label or instruction was invalid.
It reads every line (except for directive) in the fie while tracking the IC counter and puts any label into the symbol table. It finds the type and encodes each instruction into the code image. It also leaves a hole for any label not yet resolved. At the end it updates the IC.
It assumes the file pointer is valid and at the start.
*/
int first_pass(FILE *am_file, SYMBOL **symbols)
{
    char line[MAX_LINE_LEN];
    char original[MAX_LINE_LEN];
    char label[MAX_NAME_LEN];
    char *cursor;
    char *opcode;
    char *arguments;
    char *colon;
    int result;
    int line_number;
    int i;
    int error_counter = 0;
    SYMBOL *entry_symbol;

    reset_dynamic_lists();
    memset(code_image, 0, sizeof(code_image));
    memset(data_image, 0, sizeof(data_image));
    ic = IC_START;
    dc = DC_START;
    entry_count = 0;
    line_number = 0;

    while (fgets(line, sizeof(line), am_file) != NULL) {
        line_number++;
        strcpy(original, line);
        cursor = line;

        while (isspace((unsigned char)*cursor)) {
            cursor++;
        }
        if (*cursor == '\0' || *cursor == ';') {
            continue;
        }

        label[0] = '\0';
        colon = strchr(cursor, ':');
        if (colon != NULL) {
            *colon = '\0';
            if (!valid_name(cursor)) {
                fprintf(stderr, "Error: invalid label on line %d: %s",
                        line_number, original);
                error_counter++;
                continue;
            }
            strcpy(label, cursor);
            cursor = colon + 1;
            while (isspace((unsigned char)*cursor)) {
                cursor++;
            }
        }

        opcode =cursor;
        while (*cursor!= '\0' && !isspace((unsigned char)*cursor)) {
            cursor++;
        }
        if (*cursor !='\0') {
            *cursor++= '\0';
        }
        arguments = cursor;
        while (isspace(( unsigned char)*arguments)) {
            arguments++;
        }
        if (*opcode =='\0') {
            continue;
        }

        if (opcode[0] =='.'){
            result = handle_directive(opcode, arguments, symbols,
                                      label[0] == '\0' ? NULL : label, line_number,original);
        } else {
            if (label[0] != '\0' &&
                insert_symbol(symbols, label, ic, SYMBOL_CODE) != SUCCESS) {
                result = ERROR;
            } else {
                result = encode_instruction(opcode, arguments, *symbols);
            }
        }

        if (result !=SUCCESS) {
            fprintf(stderr, "Error: invalid statement on line %d: %s",
                    line_number, original);
                error_counter++;
                continue;;
        }
    }

    update_data_symbol_addresses(*symbols, ic);
    for (i = 0; i < entry_count; i++) {
        entry_symbol = find_symbol(*symbols, entry_names[i]);
        if (entry_symbol == NULL || (entry_symbol->type & SYMBOL_EXTERNAL)) {
            fprintf(stderr, "Error: invalid entry symbol %s on line %d: %s\n", entry_names[i],entry_line_numbers[i], entry_lines[i]);
                error_counter++;
                continue;;
        }
        entry_symbol->type|=SYMBOL_ENTRY;
    }
    if((error_counter>0)||(result != SUCCESS))
    {
      return ERROR;
    }
    return SUCCESS;
}


/*
Writes a value into the code image at the address that we want.
It receives the address to patch and the value we want to write.
IT overwrites whatever was there with the new word.
It assumes the address is in the code image.
*/
void patch_instruction(int address, int value)
{
    code_image[(address - IC_START) / 4] = (unsigned int)value;
}


/*
Returns the current value of the instruction counter.

It does not recieve.
It indicates how many words we encoded.
*/
int get_ic(void)
{
    return ic;
}

/*

Returns the current value of the data counter.
It shows how many bytes of data we handled.
*/

int get_dc(void)
{
    return dc;
}


/*
Returns the head of the list of unresolved labels (holes).
It does not recieve.
It returns a pointer to the first UNRESOLVED_REF node, or NULL if there are none.
*/
UNRESOLVED_REF *get_unresolved_list(void)
{
    return unresolved_head;
}


/*
Returns the head of the list of unresolved labels (holes).
It does not recieve.
It returns a pointer to the first EXTERNAL_USAGE node, or NULL if there are none.
*/
EXTERNAL_USAGE *get_external_usage_list(void)
{
    return external_head;
}


/*
Returns a pointer to the code image array.
It does not recieve.
It returns the address of the code image.
*/
unsigned int *get_code_image(void)
{
    return code_image;
}


/*
Returns a pointer to the data image array.
It gives access to the assembled bytes.
*/
unsigned char *get_data_image(void)
{
    return data_image;
}
