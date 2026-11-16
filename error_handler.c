/*
error_handler.c
the purpose of thia file is to report thw errors that found in the source code and manage the error flag
it receives error messages with file name and line number, prints them to stderr, sets the error flag, and allows checking or resetting the error status

it receives filename string line_number, and error message text and returns the error flag value 1 if error found and 0 if successed
it also assumes that filename and message are valid null terminated strings and the line number is positive integer
*/
#include <stdio.h>
#include "error_handler.h"

/* flag to track thr errors in the code we got*/
static int flag_of_errors  = 0;


/* the main function to report the errors in the code by printing them and sets a error flag in the function
the main params are filename string, line number, error message text and flag_of_errors that tracks if there are errors in the code
the function algorithm is just to sets the flag of errors to 1 and print error format to stderr and the assumptions are that filename and message are valid null terminated strings and line_number is positive integer
*/
void report_error(const char *filename, int line_number, const char *message) {
    flag_of_errors = 1; /*sets the flag on */
    fprintf(stderr, "Error: %s (line %d) -> %s\n", filename, line_number, message);
}
/* the goal of this function is to return errors flag
and she doesnt has assumptions or algorithm' she just returns the error flag
*/
int get_error_flag(void) {
    return flag_of_errors ;
}

/*this function reset the flag of errors in our code */
void reset_error_flag(void) {
    flag_of_errors  = 0;
}


