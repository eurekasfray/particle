#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "debug.h"
#include "error.h"

//=============================================================================
// String utilities
//=============================================================================

// Convert a lowercase to uppercase

int uppercase(int c)
{
    if (c >= 'a' && c <= 'z') {
        c -= 32;
    }
    return c;
}

// Convert an uppercase to lowercase

int lowercase(int c)
{
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c;
}

// Get substring

char *substr(const char *s, size_t len)
{
    char *p;
    if (len == 0) {
        p = emalloc(1);
        strcpy(p,"");
    }
    else {
        p = emalloc(len+1);
        memcpy(p,s,len);
        p[len] = '\0'; // add string terminator
    }
    return p;
}

// Duplicate string

char *dupstr(const char *s)
{
    char *p;
    p = emalloc(strlen(s)+1);
    strcpy(p,s);
    return p;
}


//=============================================================================
// Error-trapped functions
//=============================================================================

// fopen()

FILE *efopen(const char *name, const char *mode)
{
    FILE *p;
    p = fopen(name,mode);
    if (p == NULL) {
        fail("Unable to open file %s", name);
    }
    return p;
}

// malloc()

void *emalloc(size_t size)
{
    void *p;
    p = malloc(size);
    if (p == NULL) {
        fail("Unable to allocate memory");
    }
    return p;
}

// realloc()

void *erealloc(void *ptr, size_t new_size)
{
    void *new_ptr;
    new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        fail("Unable to reallocate memory");
    }
    return new_ptr;
}
