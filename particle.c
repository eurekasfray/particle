// The Particle compiler

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>

#include "particle.h"
#include "globals.h"
#include "error.h"
#include "lexer.h"
#include "asm.h"
#include "parser.h"
#include "vm.h"
#include "utils.h"
#include "file.h"
#include "debug.h"

// Input language options
#define PARTICLE_INPUT_LANGUAGE_PARTICLE 1
#define PARTICLE_INPUT_LANGUAGE_ASSEMBLY 2
#define PARTICLE_INPUT_LANGUAGE_MACHINE  3

// Externally exposed
int particle_input_language = PARTICLE_INPUT_LANGUAGE_PARTICLE; // the input language
char *particle_asmfile_name = NULL;
char *particle_objfile_name = NULL;

static int opt; // stores opt character from getopt()
static int i; // counter

//==============================================================================
// Main
//==============================================================================

int main(int argc, char *argv[])
{
    File *srcfile;
    File *asmfile;
    File *objfile;
    void *objcode;

    // NOTE: This options parser needs to be strengthened. It has many flaws.

    // Display usage if no options nor files were supplied on command line
    if (argc == 1) {
        display_usage();
        return 0;
    }

    // Process options
    while ((opt = getopt(argc,argv,"x:a:m:h")) != -1) {
        switch (opt) {
            case 'h':
                display_usage();
                return 0;
                break;
            case 'x':
                if (strcmp(optarg,"particle") == 0) {
                    particle_input_language = PARTICLE_INPUT_LANGUAGE_PARTICLE;
                }
                else if (strcmp(optarg,"assembly") == 0) {
                    particle_input_language = PARTICLE_INPUT_LANGUAGE_ASSEMBLY;
                }
                else if (strcmp(optarg,"machine") == 0) {
                    particle_input_language = PARTICLE_INPUT_LANGUAGE_MACHINE;
                }
                else {
                    fail("option -x: unknown input language specified: `%s'", optarg);
                }
                break;
            case 'a':
                particle_asmfile_name = dupstr(optarg);
                break;
            case 'm':
                particle_objfile_name = dupstr(optarg);
                break;
            case '?':
                return 0;
                break;
            default:
                abort();
                break;
        }
    }

    // There should only be one non-option argument: the file. If there are too
    // many non-option arguments, then report the error
    if ((argc - 1) > optind) {
        error("options: too many arguments. Found illegal dangling arguments:");
        for (i = optind + 1; i<argc; i++) {
            error("  - `%s'", argv[i]);
        }
        return 0;
    }
    else if (optind != (argc - 1)) {
        fail("options: too few arguments.");
    }

    srcfile = file_open((const char *)argv[optind],"rb");
    if (particle_input_language == PARTICLE_INPUT_LANGUAGE_PARTICLE) {
        asmfile = parse(srcfile);
        file_close(srcfile);
        objfile = assemble(asmfile);
        file_close(asmfile);
    }
    if (particle_input_language == PARTICLE_INPUT_LANGUAGE_ASSEMBLY) {
        objfile = assemble(srcfile);
        file_close(srcfile);
    }
    if (particle_input_language == PARTICLE_INPUT_LANGUAGE_MACHINE) {
        execute(srcfile);
    }

    // Exit on good terms
    return 0;
}

//==============================================================================
// Display usage
//==============================================================================

void display_usage()
{
    // The convention here to to let the usage copy be no more than 80 character wide
    const char usage[] =
        "Usage: particle [options] file\n\n"
        "Description:\n"
        "  Interprets particle files, assembles particle assembly files,\n"
        "  or runs particle machine code.\n\n"
        "Options:\n"
        "  -h           Display this information\n"
        "  -a FILE      Output generated assembly code to FILE\n"
        "  -m FILE      Output generated machine code to FILE\n"
        "  -x LANGUAGE  Specify the language of the input file.\n"
        "               Can be: particle (default), assembly, or machine.\n"
        "  \n"
        ;
    printf("%s", usage);
}
