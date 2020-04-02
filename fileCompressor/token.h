/*
 * tokenize.h
 *
 * Utility functions for reading from files/directories.
 */

#ifndef FinalToken_H
#define FinalToken_H

#include "DLL.h"

/*
 * Read the contents of the file into a string.
 * Returns NULL on error.
 */
char *toString(char *file_name);


/*
 * Tokenize the string according to assignment requirements and return a list
 * of tokens.
 */
DLL * string_to_tokens( char *string);


/*
 * Return a list of all files in the directory and all of its subdirectories.
 */
DLL * it_dir(char * directory_name);

#endif
