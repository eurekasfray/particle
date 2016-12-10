#include <stdio.h>
#include <stdlib.h>
#include "wrappers.h"
#include "error.h"

//=============================================================================
// Error-trapped functions
//=============================================================================

// fopen()

FILE *efopen(const char *filename, const char *mode)
{
    FILE *p;
    p = fopen(filename,mode);
    if (!p) {
        fail("Unable to open file %s", filename);
    }
    return p;
}

// malloc()

void *emalloc(size_t size)
{
    void *p;
    p = malloc(size);
    if (!p) {
        fail("Unable to allocate memory");
    }
    return p;
}
