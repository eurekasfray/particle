#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token.h"
#include "error.h"
#include "utils.h"
#include "debug.h"

// Create new token
Token *token_create()
{
    Token *ptr;

    ptr = (Token*)emalloc(sizeof(Token));
    ptr->eol = false;
    ptr->eof = false;
    return ptr;
}

// Destroy token
void token_destroy(Token *token)
{
    free(token);
}

// Push character to lexeme

void token_push_to_lexeme(Token *token, int c)
{
    if (token->top == TOKEN_LEXEME_SIZE) {
        fail("Overflow has occurred on token lexeme stack. Culprit: `%s'. To avoid an overflow,"
             "keep the length of your tokens under %d characters", token->lexeme, TOKEN_LEXEME_SIZE);
    }
    token->lexeme[token->top++] = c;
    token->lexeme[token->top] = '\0';
}

// Pop character from lexeme

int token_pop_from_lexeme(Token *token)
{
    int c;

    if (token->top == -1) {
        fail(
            "Underflow has occurred on token lexeme stack. Something went wrong."
            "There is a bug creeping around inside the compiler");
    }
    token->top--;
    c = token->lexeme[token->top];
    token->lexeme[token->top] = '\0';

    return c;
}

// Flush lexeme

void token_flush_lexeme(Token *token)
{
    token->top = 0;
    token->lexeme[token->top] = '\0';
}

char *token_meaning(TokenType type)
{
    char *s;
    switch (type) {
        case t_id:
            s = dupstr("identifier");
            break;
        case t_int:
            s = dupstr("integer");
            break;
        case t_byte:
            s = dupstr("byte");
            break;
        case t_word:
            s = dupstr("word");
            break;
        case t_dword:
            s = dupstr("dword");
            break;
        case t_null:
            s = dupstr("null");
            break;
        case t_false:
            s = dupstr("false");
            break;
        case t_true:
            s = dupstr("true");
            break;
        case t_void:
            s = dupstr("void");
            break;
        case t_entry:
            s = dupstr("entry");
            break;
        case t_def:
            s = dupstr("def`");
            break;
        case t_enddef:
            s = dupstr("enddef");
            break;
        case t_var:
            s = dupstr("var");
            break;
        case t_endvar:
            s = dupstr("endvar");
            break;
        case t_body:
            s = dupstr("body");
            break;
        case t_end:
            s = dupstr("end");
            break;
        case t_break:
            s = dupstr("break");
            break;
        case t_continue:
            s = dupstr("continue");
            break;
        case t_next:
            s = dupstr("next");
            break;
        case t_ret:
            s = dupstr("ret");
            break;
        case t_if:
            s = dupstr("if");
            break;
        case t_else:
            s = dupstr("else");
            break;
        case t_elseif:
            s = dupstr("elseif");
            break;
        case t_endif:
            s = dupstr("endif");
            break;
        case t_while:
            s = dupstr("while");
            break;
        case t_endwhile:
            s = dupstr("endwhile");
            break;
        case t_for:
            s = dupstr("for");
            break;
        case t_endfor:
            s = dupstr("endfor");
            break;
        case t_sqstr:
            s = dupstr("single-quote string");
            break;
        case t_dqstr:
            s = dupstr("double-quote string");
            break;
        case t_assign_op:
            s = dupstr("assignment operator");
            break;
        case t_add_op:
            s = dupstr("addition operator");
            break;
        case t_sub_op:
            s = dupstr("subtraction operator");
            break;
        case t_mul_op:
            s = dupstr("multiplication operator");
            break;
        case t_div_op:
            s = dupstr("division operator");
            break;
        case t_mod_op:
            s = dupstr("modulus operator");
            break;
        case t_sizeof_op:
            s = dupstr("sizeof operator");
            break;
        case t_addrof_op:
            s = dupstr("address-of operator");
            break;
        case t_bitwise_neg_op:
            s = dupstr("bitwise NEG operator");
            break;
        case t_bitwise_or_op:
            s = dupstr("bitwise OR operator");
            break;
        case t_bitwise_xor_op:
            s = dupstr("bitwise XOR operator");
            break;
        case t_bitwise_and_op:
            s = dupstr("bitwise AND operator");
            break;
        case t_bitwise_shl_op:
            s = dupstr("bitwise SHL operator");
            break;
        case t_bitwise_shr_op:
            s = dupstr("bitwise SHR operator");
            break;
        case t_bitwise_rol_op:
            s = dupstr("bitwise ROL operator");
            break;
        case t_bitwise_ror_op:
            s = dupstr("bitwise ROR operator");
            break;
        case t_eq_op:
            s = dupstr("equal operator");
            break;
        case t_neq_op:
            s = dupstr("not-equal operator");
            break;
        case t_lt_op:
            s = dupstr("less-than operator");
            break;
        case t_lte_op:
            s = dupstr("less-than-equal operator");
            break;
        case t_gt_op:
            s = dupstr("greater-than operator");
            break;
        case t_gte_op:
            s = dupstr("greater-than-equal operator");
            break;
        case t_logical_and_op:
            s = dupstr("logical AND operator");
            break;
        case t_logical_or_op:
            s = dupstr("logical OR operator");
            break;
        case t_logical_neg_op:
            s = dupstr("logical NEG operator");
            break;
        case t_comma:
            s = dupstr("comma");
            break;
        case t_lparen:
            s = dupstr("left parentheses");
            break;
        case t_rparen:
            s = dupstr("right parentheses");
            break;
        case t_lbracket:
            s = dupstr("left bracket");
            break;
        case t_rbracket:
            s = dupstr("right bracket");
            break;
        case t_colon:
            s = dupstr("colon");
            break;
        case t_semicolon:
            s = dupstr("semicolon");
            break;
        case t_base_op:
            s = dupstr("base operator");
            break;
        case t_newline:
            s = dupstr("newline");
            break;
        case t_eof:
            s = dupstr("end-of-file");
            break;
        case t_eol:
            s = dupstr("end-of-line");
            break;
        case t_unknown:
            s = dupstr("unknown");
            break;
    }
    return s;
}
