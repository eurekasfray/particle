#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "utils.h"
#include "token.h"
#include "file.h"

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
    char s[256];
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
    error("%s", s);
    exit(EXIT_FAILURE);
}

void report(File *file, Token *token, const char *format, ... )
{
    char s[256];
    va_list args;
    va_start(args,format);
    vsprintf(s,format,args);
    va_end(args);
    fail("%s:%d:%d: %s", file->name, token->lineno, token->colno, s);
}

// Report what as expected and Abort
void expected(File *file, Token *token, const char *format, ... )
{
    char s[256];
    va_list args;
    va_start(args,format);
    vsprintf(s,format,args);
    va_end(args);
    report(file, token, "Expected %s; found %s `%s'", s, token_meaning(token->type), token->lexeme);
}
