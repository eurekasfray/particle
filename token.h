#ifndef PARTICLE_TOKEN_H
#define PARTICLE_TOKEN_H

// Constant

#define TOKEN_LEXEME_SIZE 256

// Token types

typedef enum token_type {
    // ...
    t_id,               //
    t_int,              //
    // keywords
    t_word,             //
    t_byte,             //
    t_dword,            //
    t_void,             //
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
    t_base_op,          //
    // misc
    t_newline,          //
    t_eof,              //
    t_unknown           //
} token_type;

// Token

struct token {
    char lexeme[TOKEN_LEXEME_SIZE]; // string that's treated like a stack; it stores the lexeme captured from source
    int top;          // used by stack operations; points to top of lexeme
    bool eol;         // special flag used to represent an EOL token
    bool eof;         // also a special flag that represents the EOF token
    token_type type;  // token type
    int intval;       // store evaluated value of an integer literal
    char *strval      // store evaluated value of a string literal
};

// Token operations

struct token *create_token();
void push_to_lexeme(struct token *, int);
int pop_from_lexeme(struct token *);
void flush_lexeme(struct token *);
