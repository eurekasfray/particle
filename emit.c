#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "emit.h"
#include "file.h"
#include "vm.h"
#include "utils.h"
#include "debug.h"

// Emit byte

/*
void emitb(void *ptr, size_t size, byte data)
{
    void *new_ptr;
    new_ptr = erealloc(ptr, (size + sizeof(byte)) * sizeof *ptr);
    new_ptr[size+sizeof(byte)] = data;
    return new_ptr;
}

void emitw(void *ptr, size_t size, word data)
{

}

void emitd(void *ptr, size_t size, dword data)
{

}
*/

void emit(File *file, const char *format, ... )
{
    char *s;
    s = (char*)emalloc(strlen(format)+1);
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
    fprintf(file->handle, "%s", s);
    //free(s);
}

void emitln(File *file, const char *format, ... )
{
    char *s;
    s = (char*)emalloc(strlen(format)+1);
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
    //fseek(file->handle,0,SEEK_CUR);
    fprintf(file->handle, "%s", s);
    free(s);
}
