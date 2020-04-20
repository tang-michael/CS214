/*
 * util.h
 *
 * Utility functions for printing/writing to files.
 */


#ifndef UTIL_H
#define UTIL_H

/*
 * Write to a file.
 */
int write_string(int fd, char *s);

char * toString(char * file_name);

#endif
