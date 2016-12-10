#include "string.h"

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
