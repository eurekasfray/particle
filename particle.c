// The Particle compiler

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "particle.h"
#include "globals.h"
#include "lexer.h"
#include "etraps.h"

//==============================================================================
// Main
//==============================================================================

int main(int argc, char *argv[])
{
    FILE *srcfile;
    void *objcode;

    if (argc != 2) {
        display_usage(argv[0]);
        return 0;
    }
    else {
        srcfile = efopen(argv[1],"rb");
        objcode = translate(srcfile);
        vm_run();
    }
    return 0;
}

//==============================================================================
// Display usage
//==============================================================================

void display_usage(const char *self)
{
    printf("Particle usage: %s <filename>\n", self);
}
