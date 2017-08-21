#ifndef __PARTICLE_GLOBALS_H__
#define __PARTICLE_GLOBALS_H__

#include "lexer.h"
#include "token.h"

struct lexer *__srclexer;      // source file lexer
void *__asmcode;       // pointer to assembly code
void *__intermcode;    // pointer to object code
struct token *__look;  // pointer to look

#endif /* __PARTICLE_GLOBALS_H__ */
