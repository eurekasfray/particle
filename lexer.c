#include "lexer.h"
#include "global.h"

//==============================================================================
// Scanner
//==============================================================================

// Create lexer

struct lexer *init_lexer(FILE *srcfile)
{
    struct lexer *p;
    p = emalloc(sizeof(struct lexer));
    p->input = 0;
    p->srcfile = srcfile;
    return p;
}

// Get next character from source file

int get_next_char(FILE *f)
{
    int c;

    c = fgetc(f);
    if (ferror(f)) {
        fail("Unable to read character from source file.");
    }
    return c;
}

// Get next token

struct token *get_next_token(struct lexer *lexer)
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
    1               eol                 3           do nothing
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
    7.1             eol                 0           do nothing
    7.1             eof                 0           do nothing
    7.1             anything else       7.1         ignore input; get next input
    8               whitespace          0           do nothing
    8               symbol              0           do nothing
    8               eol                 0           do nothing
    8               eof                 0           do nothing
    8               sqmark              0           do nothing
    8               dqmark              0           do nothing
    8               comment initiator   0           do nothing
    8               anything else       8           push input to lexeme; get next input
    0               lexeme              -           ...
    -------------   ------------------  ----------  ------------------------------------
    */

    typedef enum state {
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
    } state;

    state next_state;
    state current_state;
    bool done; // indicates the end of the tokenization process
    struct token *token;

    token = create_token();
    flush_lexeme(token);
    done = false;
    next_state = S1;

    while (!done) {
        current_state = next_state;
        switch (current_state) {
            // scanner:
            case S1:
                if (is_whitespace(lexer->input)) {
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = current_state;
                }
                else if (is_symbol(lexer->input)) {
                    next_state = S2;
                }
                else if (is_eol(lexer->input)) {
                    next_state = S3;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S4;
                }
                else if (is_sqmark(lexer->input)) {
                    next_state = S5;
                }
                else if (is_dqmark(lexer->input)) {
                    next_state = S6;
                }
                else if (is_comment_initiator(lexer->input)) {
                    next_state = S7;
                }
                else {
                    next_state = S8;
                }
                break;
            case S2:
                if (lexer->input == ':') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_1;
                }
                else if (lexer->input == '|') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_2;
                }
                else if (lexer->input == '&') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_3;
                }
                else if (lexer->input == '=') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_4;
                }
                else if (lexer->input == '!') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_5;
                }
                else if (lexer->input == '<') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_6;
                }
                else if (lexer->input == '>') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S_7;
                }
                else if (is_symbol(lexer->input)) {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                break;
            case S2_1:
                if (lexer->input == ':') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_2:
                if (lexer->input == '|') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_3:
                if (lexer->input == '&') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_4:
                if (lexer->input == '=') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_5:
                if (lexer->input == '=') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_6:
                if (lexer->input == '<') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S2_6_1;
                }
                else if (lexer->input == '=') }
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                else {
                    next_state = S0;
                }
                break;
            case S2_6_1:
                if (lexer->input == '<') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S2_7:
                if (lexer->input == '>') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S2_6_1;
                }
                else if (lexer->input == '=') }
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                else {
                    next_state = S0;
                }
                break;
            case S2_7_1:
                if (lexer->input == '>') {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S0;
                }
                else {
                    next_state = S0;
                }
                break;
            case S3:
                token->eol = true;
                push_to_lexeme(token,'[');
                push_to_lexeme(token,'E');
                push_to_lexeme(token,'O');
                push_to_lexeme(token,'L');
                push_to_lexeme(token,']');
                lexer->input = get_next_char(lexer->srcfile);
                next_state = 0;
                break;
            case S4:
                token->eof = true;
                push_to_lexeme(token,'[');
                push_to_lexeme(token,'E');
                push_to_lexeme(token,'O');
                push_to_lexeme(token,'F');
                push_to_lexeme(token,']');
                lexer->input = get_next_char(lexer->srcfile);
                next_state = 0;
                break;
            case S5:
                push_to_lexeme(token, lexer->input);
                lexer->input = get_next_char(lexer->srcfile);
                next_state = S5_1;
                break;
            case S5_1:
                if (is_backslash(lexer->input)) {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S5_1_1;
                }
                else if (is_eol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S0;
                }
                else if (is_sqmark(lexer->input)) {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->input);
                    next_state = S0;
                }
                else {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->input);
                    next_state = S5_1;
                }
                break;
            case S5_1_1:
                if (is_eol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S0;
                }
                else {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->input);
                    next_state = S5_1;
                }
                break;
            case S6:
                push_to_lexeme(token, lexer->input);
                lexer->input = get_next_char(lexer->srcfile);
                next_state = S6_1;
                break;
            case S6_1:
                if (is_backslash(lexer->input)) {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = S6_1_1;
                }
                else if (is_eol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S0;
                }
                else if (is_dqmark(lexer->input)) {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->input);
                    next_state = S0;
                }
                else {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->input);
                    next_state = S6_1;
                }
                break;
            case S6_1_1:
                if (is_eol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S0;
                }
                else {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->input);
                    next_state = S6_1;
                }
                break;
            case S7:
                lexer->input = get_next_char(lexer->input);
                next_state = S7_1;
                break;
            case S7_1:
                if (is_eol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S0;
                }
                else {
                    next_state = current_state;
                }
                break;
            case S8:
                if (is_whitespace(lexer->input)) {
                    next_state = S0;
                }
                else if (is_symbol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eol(lexer->input)) {
                    next_state = S0;
                }
                else if (is_eof(lexer->input)) {
                    next_state = S0;
                }
                else if (is_sqmark(lexer->input)) {
                    next_state = S0;
                }
                else if (is_dqmark(lexer->input)) {
                    next_state = S0;
                }
                else if (is_comment_initiator(lexer->input)) {
                    next_state = S0;
                }
                else {
                    push_to_lexeme(token, lexer->input);
                    lexer->input = get_next_char(lexer->srcfile);
                    next_state = current_state;
                }
                break;
            // tokenizer:
            case S0:
                done = true;
                if (token->eol) {
                    token_type = t_eol;
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
                else if (is_terminal(":", token->lexeme)) {
                    token->type = t_colon;
                }
                else if (is_terminal("::", token->lexeme)) {
                    token->type = t_base_op;
                }
                else if (is_terminal("|", token->lexeme)) {
                    token->type = t_bitwise_or_op
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
                    token->type = t_eq_op
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
                    token->intval = eval_sqstr(token->lexeme);
                }
                else if (is_dqstr(token->lexeme)) {
                    token->type = t_sqstr;
                    token->intval = eval_dqstr(token->lexeme);
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

int eval_bin(const char *s)
{
    // Remove appended symbol and evaluate binary number
    char *p;
    p = dupstr(s);
    p[strlen(p)-1] = '\0';
    return eval(p,2);
}

// Evaluate octal

int eval_oct(const char *s)
{
    // Remove appended symbol and evaluate octal number
    char *p;
    p = dupstr(s);
    p[strlen(p)-1] = '\0';
    return eval(p,8);
}

// Evaluate decimal

int eval_dec(const char *s)
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

int eval_hex(const char *s)
{
    char *p;
    p = dupstr(s);
    p[strlen(p)-1] = '\0';
    return eval(p,16);
}

// Evaluators
int eval(const char *s, int base)
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

int eval_digit(int c)
{
    // Get the value of any digit. To do this, we use a lookup table,
    // a simple character array. This array is a map. Each digit (array element)
    // maps to its corresponding integer value (array index).

    int i; // indexer
    char d[] = "0123456789abcdef"; // table map

    // Search table for digit and returns its corresponding integer value
    for (i=0; i<strlen(d); i++) {
        if (lowercase(c) == d[i]) {
            return i;
        }
    }

    // Return an error indicator if no match
    return -1;
}

// Evaluate single-quote string

char *eval_sqstr(char *s)
{
    // Simple string. Only remove quotation marks.
    char *p;
    p = substr(s+1,strlen(s)-2);
    return p;
}

// Evaluate double-quote string
char *eval_dqstr(char *s)
{
    return eval_sqstr(s);
}

//==============================================================================
// Recognizers
//==============================================================================

// TERMINAL RECOGNIZER

// Match token lexeme to terminal

bool is_terminal(const char *terminal, const char *token)
{
    if (strcmp(terminal,token) == 0) {
        return true;
    }
    return false;
}

// TOKEN RECOGNIZERS

// Recognize identifier

book is_id(const char *s)
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
            case 0;
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

bool is_int(const char *s)
{
    if (is_bin(s) || is_oct(s) || is_dec(s) || is_hex(s)) {
        return true;
    }
    return false;
}

// Recognize binary numerals

bool is_bin(const char *s)
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

bool is_oct(const char *s)
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

bool is_dec(const char *s)
{
    int current_state;
    int next_state;
    int c;
    int i;

    i = 0;
    next_state = 2;

    while (true)) {
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

bool is_hex(const char *s)
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

bool is_sqstr(const char *s)
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
                // Accept any visible ASCII character, except for the
                // single-quotation mark; or accept the backslash character;
                // or accept closing single-quotation mark. Deny anything else.
                if (is_visible_ascii_character(c) && c != '\'') {
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

bool is_dqstr(const char *s)
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
                // Accept any visible ASCII character except for the
                // double-quotation mark; or accept the backslash character;
                // or accept closing double-quotation mark. Deny anything else.
                if (is_visible_ascii_character(c) && c != '"') {
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

bool is_bindigit(int c)
{
    if (c == '0' || c == '1') {
        return true;
    }
    return false;
}

// Recognize octal digit

bool is_octdigit(int c)
{
    if (is_digit(c) && (c < '8')) {
        return true;
    }
    return false;
}

// Recognize decimal digit

bool is_decdigit(int c)
{
    if (is_digit(c)) {
        return true;
    }
    return false;
}

// Recognize hexadecimal digit

bool is_hexdigit(int c)
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

bool is_digit(int c)
{
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

// Recognize letter

bool is_letter(int c)
{
    c = lowercase(c);
    if (c >= 'a' && c <= 'z') {
        return true;
    }
    return false;
}

// Recognize any visible ASCII character

bool is_visible_ascii_character(int c)
{
    // Visible ASCII characters run a value range of 32 - 126
    if (c >= 32 && c <= 126) {
        return true;
    }
    return false;
}

// ATOM/CHARACTER-LEVEL HELPER RECOGNIZERS
//
// These recognizers assist in recognition and are not explicitly
// part of the grammar.

// Recognize end-of-string character

bool is_eos(int c)
{
    if (c == '\0') {
        return true;
    }
    return false;
}

// Recognize end-of-line character

bool is_eol(int c)
{
    if (c == '\n') {
        return true;
    }
    return false;
}

// Recognize end-of-file indicator

bool is_eof(int c)
{
    if (c == EOF) {
        return true;
    }
    return false;
}

// Recognize binary notation symbol

bool is_binsym(int c)
{
    if (lowercase(c) == 'b') {
        return true;
    }
    return false;
}

// Recognize octal notation symbol

bool is_octsym(int c)
{
    if (lowercase(c) == 'o') {
        return true;
    }
    return false;
}

// Recognize decimal notation symbol

bool is_decsym(int c)
{
    if (lowercase(c) == 'd') {
        return true;
    }
    return false;
}

// Recognize hexadecimal notation symbol

bool is_hexsym(int c)
{
    if (lowercase(c) == 'h') {
        return true;
    }
    return false;
}

// Recognize comment initiator

bool is_comment_initiator(int c)
{
    if (c == '#') {
        return true;
    }
    return false;
}

// Recognize underscore character

bool is_underscore(int c)
{
    if (c == '_') {
        return true;
    }
    return false;
}

// Recognize single quotation mark

bool is_sqmark(int c)
{
    if (c == '\'') {
        return true;
    }
    return false;
}

// Recognize double quotation mark

bool is_dqmark(int c)
{
    if (c == '"') {
        return true;
    }
    return false;
}

// Recognize backslash or escape sequence initiator

bool is_backslash(int c)
{
    if (c == '\\') {
        return true;
    }
    return false;
}

// Recognize a symbol

bool is_symbol(int c)
{
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '$':
        case '&':
        case '~':
        case ',':
        case '(':
        case ')':
        case '[':
        case ']':
        case '^':
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

bool is_whitespace(int c)
{
    // All non-visible ASCII characters except NEWLINE are considered whitespace.
    if (!is_visible_ascii_character(c) && !is_eol(c)) {
        return true;
    }
}
