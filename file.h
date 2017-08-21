#ifndef __PARTICLE_FILE_H__
#define __PARTICLE_FILE_H__

#include <stdio.h>

// File
typedef struct File {
    char *name;          // File name
    FILE *handle;        // File handle
    unsigned int lineno; // store the line number of where the file indicator is
    unsigned int colno;  // store the column number of the where the file indicator is
} File;

// Prototypes
File *file_open(const char *, const char *);
int file_getc(File *);
int file_reset(File *);
long int file_size(File *);
void file_close(File *);

#endif /* __PARTICLE_FILE_H__ */
