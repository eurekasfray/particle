#include <stdarg.h>
#include "error.h"
#include "etraps.h"

// Report error

void error(const char *format, ... )
{
    printf("Error: ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

// Report error and exit

void fail(const char *format, ... )
{
    char *s;
    s = emalloc(strlen(format)+1);
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
    error("%s", s);
    exit(EXIT_FAILURE);
}
