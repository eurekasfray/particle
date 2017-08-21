#include <stdlib.h>
#include "input.h"
#include "debug.h"
//#include "wrappers.h"
#include "utils.h"

Input *input_create()
{
    Input *ptr;
    ptr = (Input*)emalloc(sizeof(*ptr));
    ptr->c = '\0';
    ptr->lineno = 0;
    ptr->colno = 0;
    ptr->file = (void*)0;
    return ptr;
}

void input_destroy(Input *input)
{
    free(input);
}
