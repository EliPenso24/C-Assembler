#ifndef GLOBALS_H
#define GLOBALS_H

/*memory limits */
#define IC_START 100 /*start address for instructions*/

#define DC_START  0 /* data counter*/
#define MEMORY_SIZE 524288 /*max RAM (512KB) */

#define MAX_LINE_LEN 82 /* max line length including \n,\0 at the end*/
#define MAX_NAME_LEN 31 /* max length for names*/
#define NUM_REGISTERS 32 /*number of registers (r0-r31)*/

/* file extensions*/
#define AS_EXTENSION ".as" /* input file extension  */

#define AM_EXTENSION ".am"/*the extension after the macro expansion */
#define OB_EXTENSION ".ob" /* machine code extension */
#define ENT_EXTENSION ".ent"/*entries list extension */
#define EXT_EXTENSION ".ext" /* external list extension*/

#define IC_START 100 /*Address of the beginning of the instruction code*/

/* status codes*/
#define SUCCESS 0 /*code for success */
#define ERROR 1 /* code for error */

/*boolean type for the project*/
typedef enum { FALSE=0, TRUE=1 } BOOLEAN;

#endif




