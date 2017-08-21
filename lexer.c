#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "globals.h"
#include "error.h"
#include "lexer.h"
#include "token.h"
#include "input.h"
#include "debug.h"
#include "utils.h"

// Evaluators
static int eval_bin(const char *);
static int eval_oct(const char *);
static int eval_dec(const char *);
static int eval_hex(const char *);
static int eval(const char *, int);
static int eval_digit(int);
static char *eval_dqstr(char *);
static char *eval_sqstr(char *);

// Terminal recognizer
static bool is_terminal(const char *, const char *);

// Token recognizers
static bool is_id(const char *);
static bool is_int(const char *);
static bool is_bin(const char *);
static bool is_oct(const char *);
static bool is_dec(const char *);
static bool is_hex(const char *);
static bool is_sqstr(const char *);
static bool is_dqstr(const char *);

// Atom recognizers
static bool is_bindigit(int);
static bool is_octdigit(int);
static bool is_decdigit(int);
static bool is_hexdigit(int);
static bool is_digit(int);
static bool is_letter(int);
static bool is_visible_ascii_character(int);

// Atom helper recognizers
static bool is_space(int);
static bool is_eos(int);
static bool is_eol(int);
static bool is_eof(int);
static bool is_binsym(int);
static bool is_octsym(int);
static bool is_decsym(int);
static bool is_hexsym(int);
static bool is_comment_initiator(int);
static bool is_underscore(int);
static bool is_sqmark(int);
static bool is_dqmark(int);
static bool is_backslash(int);
static bool is_symbol(int);
static bool is_whitespace(int);

//==============================================================================
// Scanner
//==============================================================================

// Create lexer

Lexer *lexer_create()
{
    Lexer *ptr;
    ptr = emalloc(sizeof(*ptr));
    return ptr;
}

// Get next character from source file
//
// \param Lexer lexer: The lexer context
// \return
Input *lexer_next_char(Lexer *lexer)
{

    int c;
    Input *input;


    input = input_create();


    input->c = file_getc(lexer->file);


    if (ferror(lexer->file->handle)) {

        fail("Unable to read character from source file.");
    }


    input->file = lexer->file;
    input->colno = lexer->file->colno;
    input->lineno = lexer->file->lineno;

    return input;
}

// Get next token (greedy tokenizer)
//
// \param Lexer lexer:    The lexer context
// \param bool using_eol: If TRUE the T_EOL token is scanned for; otherwise
//                        the T_EOF is not scanned for
Token *lexer_next_token(Lexer *lexer, bool using_eol)
{
    /*
    State transition table:

    Tokenization rules:
    * A string without a closing quotation mark is terminated by EOL or EOF
    * A comment is terminated by EOL

    -------------   ------------------  ----------  ------------------------------------
    current state   input               next state  action
    -------------   ------------------  ----------  ------------------------------------
    1               whitespace          1           ignore input; get next input
    1               symbol              2           do nothing
    1               eol && using_eol    3           do nothing
    1               eol && !using eol   1           ignore input; get next input
    1               eof                 4           do nothing
    1               sqmark              5           do nothing
    1               dqmark              6           do nothing
    1               comment initiator   7           do nothing
    1               anything else       8           do nothing
    2               "+"                 0           push input to lexeme; get next input
    2               "-"                 0           push input to lexeme; get next input
    2               "*"                 0           push input to lexeme; get next input
    2               "/"                 0           push input to lexeme; get next input
    2               "%"                 0           push input to lexeme; get next input
    2               "$"                 0           push input to lexeme; get next input
    2               "&"                 0           push input to lexeme; get next input
    2               "~"                 0           push input to lexeme; get next input
    2               ","                 0           push input to lexeme; get next input
    2               "("                 0           push input to lexeme; get next input
    2               ")"                 0           push input to lexeme; get next input
    2               "["                 0           push input to lexeme; get next input
    2               "]"                 0           push input to lexeme; get next input
    2               "^"                 0           push input to lexeme; get next input
    2               ";"                 0           push input to lexeme; get next input
    2               ":"                 2.1         push input to lexeme; get next input
    2               "|"                 2.2         push input to lexeme; get next input
    2               "&"                 2.3         push input to lexeme; get next input
    2               "="                 2.4         push input to lexeme; get next input
    2               "!"                 2.5         push input to lexeme; get next input
    2               "<"                 2.6         push input to lexeme; get next input
    2               ">"                 2.7         push input to lexeme; get next input
    2.1             ":"                 0           push input to lexeme; get next input
    2.1             anything else       0           do nothing
    2.2             "|"                 0           push input to lexeme; get next input
    2.2             anything else       0           do nothing
    2.3             "&"                 0           push input to lexeme; get next input
    2.3             anything else       0           do nothing
    2.4             "="                 0           push input to lexeme; get next input
    2.4             anything else       0           do nothing
    2.5             "="                 0           push input to lexeme; get next input
    2.5             anything else       0           do nothing
    2.6             "<"                 2.6.1       push input to lexeme; get next input
    2.6             "="                 0           push input to lexeme; get next input
    2.6             anything else       0           do nothing
    2.6.1           "<"                 0           push input to lexeme; get next input
    2.6.1           anything else       0           do nothing
    2.7             ">"                 2.7.1       push input to lexeme; get next input
    2.7             "="                 0           push input to lexeme; get next input
    2.7             anything else       0           do nothing
    2.7.1           ">"                 0           push input to lexeme; get next input
    2.7.1           anything else       0           do nothing
    3               eol                 0           push input to lexeme; get next input
    4               eof                 0           push input to lexeme; get next input
    5               sqmark              5.1         push input to lexeme; get next input
    5.1             backslash           5.1.1       push input to lexeme; get next input
    5.1             eol                 0           do nothing
    5.1             eof                 0           do nothing
    5.1             sqmark              0           push input to lexeme; get next input
    5.1             anything else       5.1         push input to lexeme; get next input
    5.1.1           eol                 0           do nothing
    5.1.1           eof                 0           do nothing
    5.1.1           anything else       5.1         push input to lexeme; get next input
    6               dqmark              6.1         push input to lexeme; get next input
    6.1             backslash           6.1.1       push input to lexeme; get next input
    6.1             dqmark              0           push input to lexeme; get next input
    6.1             eol                 0           do nothing
    6.1             eof                 0           do nothing
    6.1             anything else       6.1         push input to lexeme; get next input
    6.1.1           eol                 0           do nothing
    6.1.1           eof                 0           do nothing
    6.1.1           anything else       6.1         push input to lexeme; get next input
    7               comment initiator   7.1         ignore input; get next input
    7.1             eol                 1           do nothing
    7.1             eof                 1           do nothing
    7.1             anything else       7.1         ignore input; get next input
    8               whitespace          0           do nothing
    8               symbol              0           do nothing
    8               eol                 0           do nothing
    8               eof                 0           do nothing
    8               sqmark              0           do nothing
    8               dqmark              0           do nothing
    8               comment initiator   0           do nothing
    8               anything else       8           push input to lexeme; get next input
    0               lexeme              -           tokenize and evaluate
    -------------   ------------------  ----------  ------------------------------------
    */

    enum State {
        S0,
        S1,
        S2,
        S2_1,
        S2_2,
        S2_3,
        S2_4,
        S2_5,
        S2_6,
        S2_6_1,
        S2_7,
        S2_7_1,
        S3,
        S4,
        S5,
        S5_1,
        S5_1_1,
        S6,
        S6_1,
        S6_1_1,
        S7,
        S7_1,
        S8
    };

    enum State next_state;
    enum State current_state;
    bool done; // indicates the end of the tokenization process
    Token *token; // stores the token to return


    token = token_create();

    token_flush_lexeme(token);
    done = false;
    next_state = S1;

    while (!done) {


        current_state = next_state;
        switch (current_state) {
            // Scanner
            case S1:
                if (is_whitespace(lexer->input->c)) {
                    lexer->input = lexer_next_char(lexer);
                    next_state = current_state;
                }
                else if (is_symbol(lexer->input->c)) {
                    token->lineno = lexer->input->lineno; // store the location where the token was encountered
                    token->colno = lexer->input->colno; //   to the token, for the compiler to display
                    next_state = S2;
                }
                // if we are using EOL lets process it
                else if (using_eol && is_eol(lexer->input->c)) {

                    token->lineno = lexer->input->lineno;
                    token->colno = lexer->input->colno;
                    next_state = S3;
                }
                // if we are not using EOL then treat the EOL like whitespace and skip over it
                else if (!using_eol && is_eol(lexer->input->c)) {
                    lexer->input = lexer_next_char(lexer);
                    next_state = current_state;
                }
                else if (is_eof(lexer->input->c)) {
                    token->lineno = lexer->input->lineno;
                    token->colno = lexer->input->colno;
                    next_state = S4;
                }
                else if (is_sqmark(lexer->input->c)) {
                    token->lineno = lexer->input->lineno;
                    token->colno = lexer->input->colno;
                    next_state = S5;
                }
                else if (is_dqmark(lexer->input->c)) {
                    token->lineno = lexer->input->lineno;
                    token->colno = lexer->input->colno;
                    next_state = S6;
                }
                else if (is_comment_initiator(lexer->input->c)) {
                    next_state = S7;
                }
                else {
                    token->lineno = lexer->input->lineno;
                    token->colno = lexer->input->colno;
                    next_state = S8;
                }
                break;
            case S2:
                // Check for first character of a digraph symbol below
                if (lexer->input->c == ':') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_1;
                }
                else if (lexer->input->c == '|') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_2;
                }
                else if (lexer->input->c == '&') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_3;
                }
                else if (lexer->input->c == '=') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_4;
                }
                else if (lexer->input->c == '!') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_5;
                }
                else if (lexer->input->c == '<') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_6;
                }
                else if (lexer->input->c == '>') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_7;
                }
                // Check for regular symbol below
                else if (is_symbol(lexer->input->c)) {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                break;
            case S2_1:
                if (lexer->input->c == ':') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_2:
                if (lexer->input->c == '|') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_3:
                if (lexer->input->c == '&') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_4:
                if (lexer->input->c == '=') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_5:
                if (lexer->input->c == '=') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_6:
                if (lexer->input->c == '<') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_6_1;
                }
                else if (lexer->input->c == '=') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_6_1:
                if (lexer->input->c == '<') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_7:
                if (lexer->input->c == '>') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S2_6_1;
                }
                else if (lexer->input->c == '=') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_7_1:
                if (lexer->input->c == '>') {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S3:
                token->eol = true;
                token_push_to_lexeme(token,'[');
                token_push_to_lexeme(token,'E');
                token_push_to_lexeme(token,'O');
                token_push_to_lexeme(token,'L');
                token_push_to_lexeme(token,']');
                lexer->input = lexer_next_char(lexer);
                next_state = S0;
                break;
            case S4:
                token->eof = true;
                token_push_to_lexeme(token,'[');
                token_push_to_lexeme(token,'E');
                token_push_to_lexeme(token,'O');
                token_push_to_lexeme(token,'F');
                token_push_to_lexeme(token,']');
                next_state = S0;
                break;
            case S5:
                token_push_to_lexeme(token, lexer->input->c);
                lexer->input = lexer_next_char(lexer);
                next_state = S5_1;
                break;
            case S5_1:
                if (is_backslash(lexer->input->c)) {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S5_1_1;
                }
                else if (is_eol(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_sqmark(lexer->input->c)) {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S5_1;
                }
                break;
            case S5_1_1:
                if (is_eol(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input->c)) {
                    next_state = S0;
                }
                else {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S5_1;
                }
                break;
            case S6:
                token_push_to_lexeme(token, lexer->input->c);
                lexer->input = lexer_next_char(lexer);
                next_state = S6_1;
                break;
            case S6_1:
                if (is_backslash(lexer->input->c)) {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S6_1_1;
                }
                else if (is_eol(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_dqmark(lexer->input->c)) {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S0;
                }
                else {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S6_1;
                }
                break;
            case S6_1_1:
                if (is_eol(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input->c)) {
                    next_state = S0;
                }
                else {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = S6_1;
                }
                break;
            case S7:
                lexer->input = lexer_next_char(lexer);
                next_state = S7_1;
                break;
            case S7_1:
                if (is_eol(lexer->input->c)) {
                    next_state = S1;
                }
                else if (is_eof(lexer->input->c)) {
                    next_state = S1;
                }
                else {
                    lexer->input = lexer_next_char(lexer);
                    next_state = current_state;
                }
                break;
            case S8:
                if (is_whitespace(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_symbol(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_eol(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_sqmark(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_dqmark(lexer->input->c)) {
                    next_state = S0;
                }
                else if (is_comment_initiator(lexer->input->c)) {
                    next_state = S0;
                }
                else {
                    token_push_to_lexeme(token, lexer->input->c);
                    lexer->input = lexer_next_char(lexer);
                    next_state = current_state;
                }
                break;
            // tokenizer:
            case S0:
                done = true;
                if (token->eol) {
                    token->type = t_eol;
                }
                else if (token->eof) {
                    token->type = t_eof;
                }
                else if (is_terminal("+", token->lexeme)) {
                    token->type = t_add_op;
                }
                else if (is_terminal("-", token->lexeme)) {
                    token->type = t_sub_op;
                }
                else if (is_terminal("*", token->lexeme)) {
                    token->type = t_mul_op;
                }
                else if (is_terminal("/", token->lexeme)) {
                    token->type = t_div_op;
                }
                else if (is_terminal("%", token->lexeme)) {
                    token->type = t_mod_op;
                }
                else if (is_terminal("$", token->lexeme)) {
                    token->type = t_sizeof_op;
                }
                else if (is_terminal("&", token->lexeme)) {
                    token->type = t_addrof_op;
                }
                else if (is_terminal("~", token->lexeme)) {
                    token->type = t_bitwise_neg_op;
                }
                else if (is_terminal(",", token->lexeme)) {
                    token->type = t_comma;
                }
                else if (is_terminal("(", token->lexeme)) {
                    token->type = t_lparen;
                }
                else if (is_terminal(")", token->lexeme)) {
                    token->type = t_rparen;
                }
                else if (is_terminal("[", token->lexeme)) {
                    token->type = t_lbracket;
                }
                else if (is_terminal("]", token->lexeme)) {
                    token->type = t_rbracket;
                }
                else if (is_terminal("^", token->lexeme)) {
                    token->type = t_bitwise_xor_op;
                }
                else if (is_terminal(";", token->lexeme)) {
                    token->type = t_semicolon;
                }
                else if (is_terminal(":", token->lexeme)) {
                    token->type = t_colon;
                }
                else if (is_terminal("::", token->lexeme)) {
                    token->type = t_base_op;
                }
                else if (is_terminal("|", token->lexeme)) {
                    token->type = t_bitwise_or_op;
                }
                else if (is_terminal("||", token->lexeme)) {
                    token->type = t_logical_or_op;
                }
                else if (is_terminal("&", token->lexeme)) {
                    token->type = t_bitwise_and_op;
                }
                else if (is_terminal("&&", token->lexeme)) {
                    token->type = t_logical_and_op;
                }
                else if (is_terminal("=", token->lexeme)) {
                    token->type = t_assign_op;
                }
                else if (is_terminal("==", token->lexeme)) {
                    token->type = t_eq_op;
                }
                else if (is_terminal("!", token->lexeme)) {
                    token->type = t_logical_neg_op;
                }
                else if (is_terminal("!=", token->lexeme)) {
                    token->type = t_neq_op;
                }
                else if (is_terminal("<", token->lexeme)) {
                    token->type = t_lt_op;
                }
                else if (is_terminal("<<", token->lexeme)) {
                    token->type = t_bitwise_shl_op;
                }
                else if (is_terminal("<<<", token->lexeme)) {
                    token->type = t_bitwise_rol_op;
                }
                else if (is_terminal(">", token->lexeme)) {
                    token->type = t_gt_op;
                }
                else if (is_terminal(">>", token->lexeme)) {
                    token->type = t_bitwise_shr_op;
                }
                else if (is_terminal(">>>", token->lexeme)) {
                    token->type = t_bitwise_ror_op;
                }
                else if (is_terminal("entry", token->lexeme)) {
                    token->type = t_entry;
                }
                else if (is_terminal("def", token->lexeme)) {
                    token->type = t_def;
                }
                else if (is_terminal("enddef", token->lexeme)) {
                    token->type = t_enddef;
                }
                else if (is_terminal("var", token->lexeme)) {
                    token->type = t_var;
                }
                else if (is_terminal("endvar", token->lexeme)) {
                    token->type = t_endvar;
                }
                else if (is_terminal("body", token->lexeme)) {
                    token->type = t_body;
                }
                else if (is_terminal("end", token->lexeme)) {
                    token->type = t_end;
                }
                else if (is_terminal("void", token->lexeme)) {
                    token->type = t_void;
                }
                else if (is_terminal("byte", token->lexeme)) {
                    token->type = t_byte;
                }
                else if (is_terminal("word", token->lexeme)) {
                    token->type = t_word;
                }
                else if (is_terminal("dword", token->lexeme)) {
                    token->type = t_dword;
                }
                else if (is_terminal("null", token->lexeme)) {
                    token->type = t_null;
                }
                else if (is_terminal("false", token->lexeme)) {
                    token->type = t_false;
                }
                else if (is_terminal("true", token->lexeme)) {
                    token->type = t_true;
                }
                else if (is_terminal("break", token->lexeme)) {
                    token->type = t_break;
                }
                else if (is_terminal("continue", token->lexeme)) {
                    token->type = t_continue;
                }
                else if (is_terminal("next", token->lexeme)) {
                    token->type = t_next;
                }
                else if (is_terminal("ret", token->lexeme)) {
                    token->type = t_ret;
                }
                else if (is_terminal("if", token->lexeme)) {
                    token->type = t_if;
                }
                else if (is_terminal("endif", token->lexeme)) {
                    token->type = t_endif;
                }
                else if (is_terminal("else", token->lexeme)) {
                    token->type = t_else;
                }
                else if (is_terminal("elseif", token->lexeme)) {
                    token->type = t_elseif;
                }
                else if (is_terminal("endif", token->lexeme)) {
                    token->type = t_endif;
                }
                else if (is_terminal("while", token->lexeme)) {
                    token->type = t_while;
                }
                else if (is_terminal("endwhile", token->lexeme)) {
                    token->type = t_endwhile;
                }
                else if (is_terminal("for", token->lexeme)) {
                    token->type = t_for;
                }
                else if (is_terminal("endfor", token->lexeme)) {
                    token->type = t_endfor;
                }
                else if (is_id(token->lexeme)) {
                    token->type = t_id;
                }
                else if (is_bin(token->lexeme)) {
                    token->type = t_int;
                    token->intval = eval_bin(token->lexeme);
                }
                else if (is_oct(token->lexeme)) {
                    token->type = t_int;
                    token->intval = eval_oct(token->lexeme);
                }
                else if (is_dec(token->lexeme)) {
                    token->type = t_int;
                    token->intval = eval_dec(token->lexeme);
                }
                else if (is_hex(token->lexeme)) {
                    token->type = t_int;
                    token->intval = eval_hex(token->lexeme);
                }
                else if (is_sqstr(token->lexeme)) {
                    token->type = t_sqstr;
                    token->strval = eval_sqstr(token->lexeme);
                }
                else if (is_dqstr(token->lexeme)) {
                    token->type = t_dqstr;
                    token->strval = eval_dqstr(token->lexeme);
                }
                else {
                    token->type = t_unknown;
                }
                break;
        }
    }
    return token;
}

//==============================================================================
// Evaluators
//==============================================================================

// Evaluate binary

static int eval_bin(const char *s)
{
    // Remove appended symbol and evaluate binary number
    char *p;
    p = dupstr(s);
    p[strlen(p)-1] = '\0';
    return eval(p,2);
}

// Evaluate octal

static int eval_oct(const char *s)
{
    // Remove appended symbol and evaluate octal number
    char *p;
    p = dupstr(s);
    p[strlen(p)-1] = '\0';
    return eval(p,8);
}

// Evaluate decimal

static int eval_dec(const char *s)
{
    // Evaluate a decimal. Unlike the other numerical representations,
    // decimals are valid with or without the appended symbol. Check for
    // the symbol and remove it before performing evaluation.

    char *p;
    p = dupstr(s);
    if (lowercase(s[strlen(s)-1]) == 'd') {
        p[strlen(p)-1] = '\0';
    }
    return eval(p,10);
}

// Evaluate hexadecimal

static int eval_hex(const char *s)
{
    char *p;
    p = dupstr(s);
    p[strlen(p)-1] = '\0';
    return eval(p,16);
}

// Evaluators
static int eval(const char *s, int base)
{
    int i;       // loop counter
    int val;     // store integer value of digit
    int integer; // store converted integer
    int place;   // store place value

    integer = 0; // zero it
    place = 1;   // start at place value 1

    // Convert the given string to an integer value. Starting from end of
    // string and working our way back to the beginning, get the integer value
    // for each digit and calculate its corresponding place value.
    // Sum the (calculated) place value for all digits.
    for (i=strlen(s)-1; i>=0; i--) {
        val = eval_digit(s[i]); // get the actual value of the current digit
        integer += val * place; // put digit at its place value
        place *= base;          // go to next place value
    }
    return integer;
}

// Lookup digit value

static int eval_digit(int c)
{
    // Get the value of any digit. To do this, we use a lookup table,
    // a simple character array. This array is a map. Each digit (array element)
    // maps to its corresponding integer value (array index).

    int i; // indexer
    char d[] = "0123456789abcdef"; // table map
    int dlen = strlen(d); // stores the string length

    // Search table for digit and returns its corresponding integer value
    for (i=0; i<dlen; i++) {
        if (lowercase(c) == d[i]) {
            return i;
        }
    }

    // Return an error indicator if no match
    return -1;
}

// Evaluate single-quote string

static char *eval_sqstr(char *s)
{
    // Simple string. Only remove quotation marks.
    char *p;
    p = (char*)substr(s+1,strlen(s)-2);
    return p;
}

// Evaluate double-quote string
static char *eval_dqstr(char *s)
{
    return eval_sqstr(s);
}

//==============================================================================
// Recognizers
//==============================================================================

// TERMINAL RECOGNIZER

// Match token lexeme to terminal

static bool is_terminal(const char *terminal, const char *token)
{
    if (strcmp(terminal,token) == 0) {
        return true;
    }
    return false;
}

// TOKEN RECOGNIZERS

// Recognize identifier

static bool is_id(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept either a letter or an underscore; deny anything else.
                if (is_letter(c) || is_underscore(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept zero or more letters, digits or underscore characters;
                // deny anything else.
                if (is_eos(c)) {
                    next_state = 1;
                }
                else if (is_letter(c) || is_digit(c) || is_underscore(c)) {
                    next_state = current_state;
                }
                else {
                    next_state = 0;
                }
                break;
        }
    }
}

// Recognize any integer representation

static bool is_int(const char *s)
{
    if (is_bin(s) || is_oct(s) || is_dec(s) || is_hex(s)) {
        return true;
    }
    return false;
}

// Recognize binary numerals

static bool is_bin(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept one binary digits. Deny anything else.
                if (is_bindigit(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept zero or more binary digits. Deny anything else.
                if (is_bindigit(c)) {
                    next_state = current_state;
                }
                else if (is_binsym(c)) {
                    next_state = 4;
                }
                else {
                    next_state = 0;
                }
            case 4:
                // Accept EOS; deny anything else
                if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
                break;
        }
    }

}


// Recognize octal numeral

static bool is_oct(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept one octal digit; deny anything else.
                if (is_octdigit(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept zero or more octal digits; deny anything else.
                if (is_octdigit(c)) {
                    next_state = current_state;
                }
                else if (is_octsym(c)) {
                    next_state = 4;
                }
                else {
                    next_state = 0;
                }
                break;
            case 4:
                // Accept EOS; deny anything else.
                if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
                break;
        }
    }
}

// Recognize decimal numeral

static bool is_dec(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept one decimal digit; deny anything else.
                if (is_decdigit(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept zero or more decimal digits; deny anything else.
                if (is_decdigit(c)) {
                    next_state = current_state;
                }
                else if (is_decsym(c)) {
                    next_state = 4;
                }
                else if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
                break;
            case 4:
                // Accept EOS; deny anything else.
                if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
                break;
        }
    }
}

// Recognize hexadecimal numeral

static bool is_hex(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept one hex digit; deny anything else.
                if (is_hexdigit(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept zero or more hex digits; or accept hexadecimal symbol.
                // Deny anything else.
                if (is_hexdigit(c)) {
                    next_state = current_state;
                }
                else if (is_hexsym(c)) {
                    next_state = 4;
                }
                else {
                    next_state = 0;
                }
                break;
            case 4:
                // Accept EOS; deny anything else.
                if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
                break;
        }
    }
}

// Recognize single-quote string

static bool is_sqstr(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept opening single-quotation mark. Deny anything else.
                if (is_sqmark(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept any visible ASCII character and the space character, except for the
                // single-quotation mark; or accept the backslash character;
                // or accept closing single-quotation mark. Deny anything else.
                if ((is_visible_ascii_character(c) || is_space(c)) && !is_sqmark(c)) {
                    next_state = current_state;
                }
                else if (is_backslash(c)) {
                    next_state = 4;
                }
                else if (is_sqmark(c)) {
                    next_state = 5;
                }
                else {
                    next_state = 0;
                }
                break;
            case 4:
                // Accept any visible ASCII character (include the
                // single-quotation mark). Deny anything else.
                if (is_visible_ascii_character(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 5:
                // Accept EOS; deny anything else.
                if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
        }
    }
}

// Recognize double-quote string

static bool is_dqstr(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true) {
        c = s[i++];
        current_state = next_state;
        switch (current_state) {
            case 0:
                // Invalid
                return false;
            case 1:
                // Valid
                return true;
            case 2:
                // Accept opening double-quotation mark. Deny anything else.
                if (is_dqmark(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 3:
                // Accept any visible ASCII character and the space character, except for the
                // double-quotation mark; or accept the backslash character;
                // or accept closing double-quotation mark. Deny anything else.
                if ((is_visible_ascii_character(c) || is_space(c)) && !is_dqmark(c)) {
                    next_state = current_state;
                }
                else if (is_backslash(c)) {
                    next_state = 4;
                }
                else if (is_dqmark(c)) {
                    next_state = 5;
                }
                else {
                    next_state = 0;
                }
                break;
            case 4:
                // Accept any visible ASCII character (include the
                // double-quotation mark). Deny anything else.
                if (is_visible_ascii_character(c)) {
                    next_state = 3;
                }
                else {
                    next_state = 0;
                }
                break;
            case 5:
                // Accept EOS; deny anything else.
                if (is_eos(c)) {
                    next_state = 1;
                }
                else {
                    next_state = 0;
                }
        }
    }
}

// ATOM/CHARACTER-LEVEL RECOGNIZERS

// Recognize binary digit

static bool is_bindigit(int c)
{
    if (c == '0' || c == '1') {
        return true;
    }
    return false;
}

// Recognize octal digit

static bool is_octdigit(int c)
{
    if (is_digit(c) && (c < '8')) {
        return true;
    }
    return false;
}

// Recognize decimal digit

static bool is_decdigit(int c)
{
    if (is_digit(c)) {
        return true;
    }
    return false;
}

// Recognize hexadecimal digit

static bool is_hexdigit(int c)
{
    if (is_digit(c)) {
        return true;
    }
    else if (lowercase(c) >= 'a' && lowercase(c) <= 'f') {
        return true;
    }
    return false;
}

// Recognize digit

static bool is_digit(int c)
{
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

// Recognize letter

static bool is_letter(int c)
{
    c = lowercase(c);
    if (c >= 'a' && c <= 'z') {
        return true;
    }
    return false;
}

// Recognize any visible ASCII character

static bool is_visible_ascii_character(int c)
{
    // Visible ASCII characters run a value range of 32 - 126
    if (c > 32 && c < 127) {
        return true;
    }
    return false;
}

// ATOM/CHARACTER-LEVEL HELPER RECOGNIZERS
//
// These recognizers assist in recognition and are not explicitly
// part of the grammar.

// Recognize space character

static bool is_space(int c)
{
    if (c == ' ') {
        return true;
    }
    return false;
}

// Recognize end-of-string character

static bool is_eos(int c)
{
    if (c == '\0') {
        return true;
    }
    return false;
}

// Recognize end-of-line character

static bool is_eol(int c)
{
    if (c == '\n') {
        return true;
    }
    return false;
}

// Recognize end-of-file indicator

static bool is_eof(int c)
{

    if (c == EOF) {
        return true;
    }
    return false;
}

// Recognize binary notation symbol

static bool is_binsym(int c)
{
    if (lowercase(c) == 'b') {
        return true;
    }
    return false;
}

// Recognize octal notation symbol

static bool is_octsym(int c)
{
    if (lowercase(c) == 'o') {
        return true;
    }
    return false;
}

// Recognize decimal notation symbol

static bool is_decsym(int c)
{
    if (lowercase(c) == 'd') {
        return true;
    }
    return false;
}

// Recognize hexadecimal notation symbol

static bool is_hexsym(int c)
{
    if (lowercase(c) == 'h') {
        return true;
    }
    return false;
}

// Recognize comment initiator

static bool is_comment_initiator(int c)
{
    if (c == '#') {
        return true;
    }
    return false;
}

// Recognize underscore character

static bool is_underscore(int c)
{
    if (c == '_') {
        return true;
    }
    return false;
}

// Recognize single quotation mark

static bool is_sqmark(int c)
{
    if (c == '\'') {
        return true;
    }
    return false;
}

// Recognize double quotation mark

static bool is_dqmark(int c)
{
    if (c == '"') {
        return true;
    }
    return false;
}

// Recognize backslash or escape sequence initiator

static bool is_backslash(int c)
{
    if (c == '\\') {
        return true;
    }
    return false;
}

// Recognize a symbol

static bool is_symbol(int c)
{
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '$':
        case '~':
        case ',':
        case '(':
        case ')':
        case '[':
        case ']':
        case '^':
        case ';':
        case ':':
        case '|':
        case '&':
        case '=':
        case '!':
        case '<':
        case '>':
        return true;

        default:
        return false;
    }
}

// Recognize whitespace

static bool is_whitespace(int c)
{
    // The translator considers whitespace to be any non-ASCII character.
    // When checking for whitespace, this recognizer denies any character
    // that is not a visible ASCII character, not the the EOF indicator,
    // and not the EOL character.
    //
    // We specially include a check for the EOF and EOL because we if check
    // for visible ASCII characters only, then the EOF and EOL will be
    // implicitly considered whitespace. With the EOF, its value does not
    // fall in the range of visible ASCII characters. It will therefore be
    // seen as whitespace. Also with the EOL, its value falls within the
    // the range of non-visible ASCII characters. It will also be seen
    // as whitetspace. The exceptions applied by this recognizer allow
    // these two characters to be overlooked as whitespaces.
    if (!is_visible_ascii_character(c) && !is_eof(c) &&!is_eol(c)) {
        return true;
    }
}
