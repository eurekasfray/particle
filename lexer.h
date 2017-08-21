#ifndef __PARTICLE_LEXER_H__
#define __PARTICLE_LEXER_H__

#include <stdio.h>
#include <stdbool.h>
#include "input.h"
#include "file.h"
#include "token.h"

// Lexer context
typedef struct Lexer {
    Input *input;  // stores input read from file
    File *file;    // points to a source file
} Lexer;

// Lexer operations
Lexer *lexer_create();
Input *lexer_next_char(Lexer *);
Token *lexer_next_token(Lexer *, bool);

#endif /* __PARTICLE_LEXER_H__ */
