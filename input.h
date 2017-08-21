#ifndef __PARTICLE_INPUT_H__
#define __PARTICLE_INPUT_H__

#include <stdio.h>
#include "file.h"

// Input represents an input from a file.
typedef struct Input {
    char c;
    unsigned int lineno; // stores the line number where input was found
    unsigned int colno;  // stores the column number where input was found
    File *file;          // stores the parent file
} Input;

Input *input_create();
void input_destroy(Input *);

#endif /* __PARTICLE_INPUT_H__ */
