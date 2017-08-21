#include <stdarg.h>
#include <string.h>
#include "asm.h"
#include "file.h"
#include "token.h"
#include "lexer.h"
#include "utils.h"
#include "error.h"
#include "debug.h"

static bool match(TokenType);
static void program();
static void linen();
static bool is_linen(TokenType);
static void line();
static bool is_line(TokenType);
static void constant();
static bool is_constant(TokenType);

static File *objfile;
static Lexer *lexer;
static Token *look;

File *assemble(File *file)
{
    objfile = file_open("particle.bin","wb+");
    lexer = lexer_create();
    lexer->file = file;
    lexer->input = lexer_next_char(lexer);
    look = lexer_next_token(lexer, true);
    program();
    return NULL;
}

static bool match(TokenType type)
{
    if (look->type == type) {
        token_destroy(look); // remove our garbage from memory
        look = lexer_next_token(lexer, true);
        return true;
    }
    else {
        return false;
    }
}

static void program()
{
    // Expect repeated linen productions
    while (look->type == t_id || look->type == t_eol) {
        linen();
    }

    // Expect <EOF>
    if (!match(t_eof)) {
        expected(lexer->file, look, "%s", token_meaning(t_eof));
    }
}

static void linen()
{
    if (look->type == t_eol) {
        match(t_eol);
        return;
    }
    else if (look->type == t_id) {
        line();
        return;
    }
    else {
        expected(lexer->file, look, "%s for a label or mnemonic; or %s for an empty line", token_meaning(t_id), token_meaning(t_eol));
    }
}

static bool is_linen(TokenType type)
{
    if (type == t_id || type == t_eol) {
        return true;
    }
    else {
        return false;
    }
}

static void line()
{
    // Expect an identifier for either a label or instruction
    if (!match(t_id)) {
        expected(lexer->file, look, "%s for a label or mnemonic", token_meaning(t_id));
    }

    // Expect either a colon to complete a label definition or an operand to complete
    // an the definition of an instruction. The current token determines whether we
    // treat it and the previous token as a label definition or an instruction.
    // To know, we check the current token. If the token is a colon, then we treat
    // the token couple as label; but, if the token is a constant then we treat
    // the token couple as an instruction.
    if (look->type == t_colon) {
        match(t_colon);
        // Expect identifier for instruction
        if (look->type == t_id) {
            match(t_id);
            if (is_constant(look->type)) {
                constant();
            }
        }
    }
    else if (is_constant(look->type)) {
        constant();
    }
    else if (look->type == t_eol) {
        match(t_eol);
    }
    else if (look->type == t_eof) {
        return;
    }
    else {
        expected(lexer->file, look, "%s to complete definition of a label; or %s or %s or %s to complete the definition of an instruction", token_meaning(t_colon), token_meaning(t_int), token_meaning(t_sqstr), token_meaning(t_dqstr));
    }
}

static bool is_line(TokenType type)
{
    if (type == t_id) {
        return true;
    }
    else {
        return false;
    }
}

static void constant() {
    if (look->type == t_id) {
        match(t_id);
    }
    else if (look->type == t_int) {
        match(t_int);
    }
    else if (look->type == t_sqstr) {
        match(t_sqstr);
    }
    else if (look->type == t_dqstr) {
        match(t_dqstr);
    }
    else {
        expected(lexer->file, look, "literal constant: expected int, sqstr, or dqstr");
    }
}

static bool is_constant(TokenType type)
{
    if (type == t_id || type == t_int || type == t_sqstr || type == t_dqstr) {
        return true;
    }
    else {
        return false;
    }
}
