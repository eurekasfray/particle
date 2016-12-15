#include "file.h"

// Create new File struct

open_file(const char *filename)
{
    FILE *handle;
    File *p;
    handle = efopen(filename, "rb");
    p = emalloc(sizeof(File));
    p->name = dupstr(filename);
    p->handle = handle;
    p->lineno = 0;
    p->colno = 0;
    return p;
}

// Close file handle and destroy the File struct

close_file(File *p)
{
    FILE *handle;
    handle = p->handle;
    fclose(handle);
    free(p);
}

