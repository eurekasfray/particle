#ifndef PARTICLE_LEXER_H
#define PARTICLE_LEXER_H

// Source file object
struct lexer {
    int input;      // store input read from file
    FILE *srcfile;     // store point to source file
};

// Lexer
struct lexer *init_lexer();
int get_next_char();
struct token *get_next_token();

// Evaluators
int eval_bin(const char *);
int eval_oct(const char *);
int eval_dec(const char *);
int eval_hex(const char *);
int eval(const char *, int);
int eval_digit(int);
char *eval_sqstr(char *);
char *eval_dqstr(char *);

// Terminal recognizer
bool is_terminal(const char *, const char *);

// Token recognizers
bool is_id(const char *);
bool is_int(const char *);
bool is_bin(const char *);
bool is_oct(const char *);
bool is_dec(const char *);
bool is_hex(const char *);
bool is_sqstr(const char *);
bool is_dqstr(const char *);

// Atom recognizers
bool is_bindigit(int);
bool is_octdigit(int);
bool is_decdigit(int);
bool is_hexdigit(int);
bool is_digit(int);
bool is_letter(int);
bool is_visible_ascii_character(int);

// Atom helper recognizers
bool is_eos(int);
bool is_eol(int);
bool is_eof(int);
bool is_binsym(int);
bool is_octsym(int);
bool is_decsym(int);
bool is_hexsym(int);
bool is_comment_initiator(int);
bool is_underscore(int);
bool is_sqmark(int);
bool is_dqmark(int);
bool is_backslash(int);
bool is_symbol(int);
bool is_whitespace(int);
