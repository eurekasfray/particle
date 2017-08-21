#include <stdio.h>
#include <stdlib.h>
//#include "string.h"
//#include "wrappers.h"
#include "utils.h"
#include "file.h"
#include "error.h"
#include "debug.h"

struct File *file_open(const char *name, const char *mode)
{
    File *file;

    file = (File*)emalloc(sizeof(*file));
    file->handle = efopen(name, mode);
    file_reset(file);
    file->name = dupstr(name);
    file->lineno = 1;
    file->colno = 1;
    return file;
}

int file_getc(File *file)
{
    int c;
    c = fgetc(file->handle);
    // maintain the line and column numbers for the compiler to display
    // and on every newline char, increment the line number and reset the column number
    if (ferror(file->handle)) {
        fail("Unable to read character from file ");
    }
    file->colno++;
    if (c == '\n') {
        file->lineno++;
        file->colno = 0; // reset column number
    }
    return c;
}

int file_reset(File *file)
{
    return fseek(file->handle, 0, SEEK_SET);
}

long int file_size(File *file)
{
    long int previous_offset;
    long int size;

    previous_offset = ftell(file->handle);
    if (fseek(file->handle,0,SEEK_END) != 0) {
        fail("Unable to get file size");
    }
    size = ftell(file->handle);
    if (fseek(file->handle, previous_offset, SEEK_SET) != 0) {
        fail("Unable to reset file position indicator to its original state");
    }
    return size;
}

void file_close(File *file)
{
    fclose(file->handle);
    free(file);
}
