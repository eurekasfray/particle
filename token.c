#include "token.h"

// Create new token
struct token *create_token()
{
    struct token *p;

    p = emalloc(sizeof(struct token));
    p->eol = false;
    p->eof = false;

    return p;
}

// Push character to lexeme

void push_to_lexeme(struct token *p, int c)
{
    if (p->top == TOKEN_LEXEME_SIZE) {
        fail("Overflow occurred on lexeme stack");
    }
    p->lexeme[p->top++] = c;
    p->lexeme[p->top] = '\0';
}

// Pop character from lexeme

int pop_from_lexeme(struct token *p)
{
    int c;

    if (p->top == -1) {
        fail("Underflow occurred on lexeme stack");
    }
    p->top--;
    c = p->lexeme[p->top];
    p->lexeme[p->top] = '\0';

    return c;
}

// Flush lexeme

void flush_lexeme(struct token *p)
{
    p->top = 0;
    p->lexeme[p->top] = '\0';
}
