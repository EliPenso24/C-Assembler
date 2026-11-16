/*
error_handler.c
the purpose of thia file is to report thw errors that found in the source code and manage the error flag
it receives error messages with file name and line number, prints them to stderr, sets the error flag, and allows checking or resetting the error status

it receives filename string line_number, and error message text and returns the error flag value 1 if error found and 0 if successed
*/

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
/*
this function is about to report the rror in our code by printing them, the function 
nain params that she receives are filename string, line number, and error message text
the function also sets a error flag
*/
void report_error(const char *filename, int line_number, const char *message);

/* the function returns teh error flag from our code so we can know if there is error or not
she doesnt the params */
int get_error_flag(void);

/*
this function resets the error flag to 0 for a new file we would get
*/
void reset_error_flag(void);

#endif



