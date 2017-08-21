#ifndef __PARTICLE_TOKEN_H__
#define __PARTICLE_TOKEN_H__

#include <stdbool.h>

// Constant

#define TOKEN_LEXEME_SIZE 256

// Token types

typedef enum TokenType {
    // ...
    t_id,               //
    t_int,              //
    // keywords
    t_word,             //
    t_byte,             //
    t_dword,            //
    t_null,            //
    t_false,            //
    t_true,            //
    t_void,             //
    t_entry,
    t_def,
    t_enddef,
    t_var,
    t_endvar,
    t_body,
    t_end,
    t_break,
    t_continue,
    t_next,
    t_ret,
    t_if,
    t_else,
    t_elseif,
    t_endif,
    t_while,
    t_endwhile,
    t_for,
    t_endfor,
    // string
    t_sqstr,            // single-quote string
    t_dqstr,            // double-quote string
    // operators
    t_assign_op,        // assignment operator
    t_add_op,           //
    t_sub_op,           //
    t_mul_op,           //
    t_div_op,           //
    t_mod_op,           //
    t_sizeof_op,        //
    t_addrof_op,        //
    t_bitwise_neg_op,   //
    t_bitwise_or_op,    //
    t_bitwise_xor_op,   //
    t_bitwise_and_op,   //
    t_bitwise_shl_op,   //
    t_bitwise_shr_op,   //
    t_bitwise_rol_op,   //
    t_bitwise_ror_op,   //
    t_eq_op,            //
    t_neq_op,           //
    t_lt_op,            //
    t_lte_op,           //
    t_gt_op,            //
    t_gte_op,           //
    t_logical_and_op,   //
    t_logical_or_op,    //
    t_logical_neg_op,   //
    // punctuators
    t_comma,            //
    t_lparen,           //
    t_rparen,           //
    t_lbracket,         //
    t_rbracket,         //
    t_colon,            //
    t_semicolon,        //
    t_base_op,          //
    // misc
    t_newline,          //
    t_eof,              //
    t_eol,              //
    t_unknown           //
} TokenType;

// Token

typedef struct Token {
    char lexeme[TOKEN_LEXEME_SIZE]; // string that's treated like a stack; it stores the lexeme captured from source
    int top;              // used by stack operations; points to top of lexeme
    bool eol;             // special flag used to represent an EOL token
    bool eof;             // also a special flag that represents the EOF token
    TokenType type;       // token type
    int intval;           // store evaluated value of an integer literal
    char *strval;         // store evaluated value of a string literal
    unsigned int lineno;  // the line number on which the token was found
    unsigned int colno;   // the column number on which the token was found
} Token;

// Token operations

Token *token_create();
void token_destroy(Token *);
void token_push_to_lexeme(Token *, int);
int token_pop_from_lexeme(Token *);
void token_flush_lexeme(Token *);
char *token_meaning(TokenType);

#endif /* __PARTICLE_TOKEN_H__ */
