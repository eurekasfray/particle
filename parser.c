#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "error.h"
#include "lexer.h"
#include "token.h"
#include "vm.h"
#include "emit.h"
//#include "wrappers.h"
#include "utils.h"
#include "file.h"
#include "debug.h"

// Prototypes
static Token *parser_next_token(Lexer *);
static bool match(TokenType);
static TokenType lookahead();
static void program();
static void entry_point_specifier();
static void var_block();
static void var_definition_list();
static void var_definition();
static void var_declaration();
static void var_type_specifier();
static void var_declarator();
static void var_actual_declarator();
static void var_size_declarator();
static void var_initializer_list();
static void var_initializer();
static void func_definition();
static void func_declaration();
static void func_type_specifier();
static void func_declarator();
static void func_actual_declarator();
static void func_parameter_list();
static void func_parameter_declaration();
static void nonempty_type();
static bool is_nonempty_type();
static void empty_type();
static bool is_empty_type();
static void stmt_list();
static void stmt();
static bool is_stmt();
static void expr_stmt();
static bool is_expr_stmt();
static void break_stmt();
static void continue_stmt();
static void next_stmt();
static void return_stmt();
static void if_stmt();
static void while_stmt();
static void for_stmt();
static void expr();
static void term();
static void factor();
static void constant();
static bool is_constant();

static Token *look; // stores the lookahead
static File *asmfile;
static Lexer *lexer;

//==============================================================================
// Parse
//==============================================================================

File *parse(File *srcfile)
{
    // Prepare assembly output file
    asmfile = file_open("particle.asm","wb+");

    // Get first character for the lexer to start with
    lexer = lexer_create();
    lexer->file = srcfile;
    lexer->input = lexer_next_char(lexer);
    look = lexer_next_token(lexer, false);
    program();
    file_reset(asmfile);
    return asmfile;
}

//==============================================================================
// Recursive-descent parser, using syntax-directed-translation technique
// The functions have the same name as their corresponding production rules.
//==============================================================================

// Returns TRUE if the give type matches the lookahead; otherwise FALSE is returned

static bool match(TokenType type)
{
    if (look->type == type) {
        token_destroy(look); // we need to remove our garbage from memory
        look = lexer_next_token(lexer, false);
        return true;
    }
    else {
        return false;
    }
}

// Returns value of the lookahead

static TokenType lookahead()
{
    return look->type;
}

static void program()
{
    entry_point_specifier();

    if (look->type == t_var) {
        var_block();
    }

    while (look->type == t_def) {
        func_definition();
    }

    if (!match(t_eof)) {
        expected(lexer->file, look, "%s", token_meaning(t_eof));
    }
}

static void entry_point_specifier()
{
    if (!match(t_entry)) {
        expected(lexer->file, look, "entry point specifier; begins with %s", token_meaning(t_entry));
    }

    emitln(asmfile, "jmp %s", look->lexeme);

    if (!match(t_id)) {
        expected(lexer->file, look, "entry point specifier; ends with %s", token_meaning(t_id));
    }
}

static void var_block()
{
    if (!match(t_var)) {
        expected(lexer->file, look, "var block initiator (%s)", token_meaning(t_var));
    }

    var_definition_list();

    if (!match(t_endvar)) {
        expected(lexer->file, look, "var block terminator (%s)", token_meaning(t_endvar));
    }
}

static void var_definition_list()
{
    var_definition();

    while (is_nonempty_type()) {
        var_definition();
    }
}

static void var_definition()
{
    var_declaration();
    var_declarator();
}

static void var_declaration()
{
    var_type_specifier();
}

static void var_type_specifier()
{
    nonempty_type();
}

static void var_declarator()
{
    var_actual_declarator();
    var_size_declarator();
    if (lookahead() == t_colon) {
        var_initializer_list();
    }
}

static void var_actual_declarator()
{
    if (!match(t_id)) {
        expected(lexer->file, look, "%s", token_meaning(t_id));
    }
}

static void var_size_declarator()
{
    if (!match(t_lbracket)) {
        expected(lexer->file, look, "%s", token_meaning(t_lbracket));
    }

    if (is_constant()) {
        constant();
    }

    if (!match(t_rbracket)) {
        expected(lexer->file, look, "%s", token_meaning(t_rbracket));
    }
}

static void var_initializer_list()
{
    var_initializer();

    while (lookahead() == t_comma) {
        var_initializer();
    }
}

static void var_initializer()
{
    constant();

    if (lookahead() == t_base_op) {
        match(t_base_op);
        constant();
    }
}

static void func_definition()
{
    if (!match(t_def)) {
        expected(lexer->file, look, "function definition (%s)", token_meaning(t_def));
    }

    func_declaration();
    func_declarator();

    if (!match(t_colon)) {
        expected(lexer->file, look, "%s", token_meaning(t_colon));
    }

    if (lookahead() == t_var) {
        var_block();
    }

    stmt_list();

    if (!match(t_enddef)) {
        expected(lexer->file, look, "function epilogue (%s)", token_meaning(t_enddef));
    }
}

static void func_declaration()
{
    func_type_specifier();
}

static void func_type_specifier()
{
    if (is_empty_type()) {
        empty_type();
    }
    else if (is_nonempty_type()) {
        nonempty_type();
    }
    else {
        expected(lexer->file, look, "empty type (%s) or non-empty type (%s or %s or %s)", token_meaning(t_void), token_meaning(t_byte), token_meaning(t_word), token_meaning(t_dword));
    }
}

static void func_declarator()
{
    func_actual_declarator();
}

static void func_actual_declarator()
{
    if (!match(t_id)) {
        expected(lexer->file, look, "%s", token_meaning(t_id));
    }

    if (!match(t_lparen)) {

        expected(lexer->file, look, "%s", token_meaning(t_lparen));
    }

    func_parameter_list();

    if (!match(t_rparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_rparen));
    }
}

static void func_parameter_list()
{
    func_parameter_declaration();

    if (lookahead() == t_comma) {
        func_parameter_declaration();
    }
}

static void func_parameter_declaration()
{
    if (is_empty_type()) {
        match(t_void);
    }
    else {
        var_declaration();
        var_actual_declarator();
    }
}

// Lookahead

static void nonempty_type()
{
    if (lookahead() == t_byte) {
        match(t_byte);
    }
    else if (lookahead() == t_word) {
        match(t_word);
    }
    else if (lookahead() == t_dword) {
        match(t_dword);
    }
    else {
        expected(lexer->file, look, "non-empty type (%s or %s or %s)", token_meaning(t_byte), token_meaning(t_word), token_meaning(t_dword));
    }
}

static bool is_nonempty_type()
{
    if (lookahead() == t_byte || lookahead() == t_word || lookahead() == t_dword) {
        return true;
    }
    else {
        return false;
    }
}

static void empty_type()
{
    if (!match(t_void)) {
        expected(lexer->file, look, "empty type (%s)", token_meaning(t_void));
    }
}

// Lookahead

static bool is_empty_type()
{
    if (lookahead() == t_void) {
        return true;
    }
    else {
        return false;
    }
}

static void stmt_list()
{
    while (is_stmt()) {
        stmt();
    }
}

static void stmt()
{
    if (is_expr_stmt()) {
        expr_stmt();
    }
    else if (lookahead() == t_break) {
        break_stmt();
    }
    else if (lookahead() == t_continue) {
        continue_stmt();
    }
    else if (lookahead() == t_next) {
        next_stmt();
    }
    else if (lookahead() == t_ret) {
        return_stmt();
    }
    else if (lookahead() == t_if) {
        if_stmt();
    }
    else if (lookahead() == t_while) {
        while_stmt();
    }
    else if (lookahead() == t_for) {
        for_stmt();
    }
    else {
        expected(lexer->file, look, "statement");
    }
}

// Look ahead

static bool is_stmt()
{
    // lookahead for EXPR stmt
    if (is_expr_stmt()) {
        return true;
    }
    // lookahead for BREAK stmt
    else if (lookahead() == t_break) {
        return true;
    }
    // lookahead for CONTINUE stmt
    else if (lookahead() == t_continue) {
        return true;
    }
    // lookahead for NEXT stmt
    else if (lookahead() == t_next) {
        return true;
    }
    // lookahead for RET stmt
    else if (lookahead() == t_ret) {
        return true;
    }
    // lookahead for IF stmt
    else if (lookahead() == t_if) {
        return true;
    }
    // lookahead for WHILE stmt
    else if (lookahead() == t_while) {
        return true;
    }
    // lookahead for FOR stmt
    else if (lookahead() == t_for) {
        return true;
    }
    else {
        return false;
    }
}

static void expr_stmt()
{
    expr();
}

static bool is_expr_stmt()
{
    if (lookahead() == t_id || lookahead() == t_lparen || is_constant()) {
        return true;
    }
    else {
        return false;
    }
}

static void break_stmt()
{
    if (!match(t_break)) {
        expected(lexer->file, look, "%s", token_meaning(t_break));
    }
}

static void continue_stmt()
{
    if (!match(t_continue)) {
        expected(lexer->file, look, "%s", token_meaning(t_continue));
    }
}

static void next_stmt()
{
    if (!match(t_next)) {
        expected(lexer->file, look, "%s", token_meaning(t_next));
    }
}

static void return_stmt()
{
    if (!match(t_ret)) {
        expected(lexer->file, look, "%s", token_meaning(t_ret));
    }
}

static void if_stmt()
{
    if (!match(t_if)) {
        expected(lexer->file, look, "%s", token_meaning(t_if));
    }
    if (!match(t_lparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_lparen));
    }
    expr();
    if (!match(t_rparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_rparen));
    }
    if (!match(t_colon)) {
        expected(lexer->file, look, "%s", token_meaning(t_colon));
    }
    stmt_list();
    while (lookahead() == t_elseif) {
        if (!match(t_elseif)) {
            expected(lexer->file, look, "%s", token_meaning(t_elseif));
        }
        if (!match(t_lparen)) {
            expected(lexer->file, look, "%s", token_meaning(t_lparen));
        }
        expr();
        if (!match(t_rparen)) {
            expected(lexer->file, look, "%s", token_meaning(t_rparen));
        }
        if (!match(t_colon)) {
            expected(lexer->file, look, "%s", token_meaning(t_colon));
        }
        stmt_list();
    }
    if (lookahead() == t_else) {
        match(t_else);
        if (!match(t_colon)) {
            expected(lexer->file, look, "%s", token_meaning(t_colon));
        }
        stmt_list();
    }
    if (!match(t_endif)) {
        expected(lexer->file, look, "%s", token_meaning(t_endif));
    }
}

static void while_stmt()
{
    if (!match(t_while)) {
        expected(lexer->file, look, "%s", token_meaning(t_while));
    }
    if (!match(t_lparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_lparen));
    }
    expr();
    if (!match(t_rparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_rparen));
    }
    if (!match(t_colon)) {
        expected(lexer->file, look, "%s", token_meaning(t_colon));
    }
    stmt_list();
    if (!match(t_endwhile)) {
        expected(lexer->file, look, "%s", token_meaning(t_endwhile));
    }
}

static void for_stmt()
{
    if (!match(t_for)) {
        expected(lexer->file, look, "%s", token_meaning(t_for));
    }
    if (!match(t_lparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_lparen));
    }
    expr();
    if (!match(t_semicolon)) {
        expected(lexer->file, look, "%s", token_meaning(t_semicolon));
    }
    expr();
    if (!match(t_semicolon)) {
        expected(lexer->file, look, "%s", token_meaning(t_semicolon));
    }
    expr();
    if (!match(t_rparen)) {
        expected(lexer->file, look, "%s", token_meaning(t_rparen));
    }
    if (!match(t_colon)) {
        expected(lexer->file, look, "%s", token_meaning(t_colon));
    }
    stmt_list();
    if (!match(t_endfor)) {
        expected(lexer->file, look, "%s", token_meaning(t_end));
    }
}

static void expr()
{
    term();
}

static void term()
{
    factor();
}

static void factor()
{
    if (lookahead() == t_id) {
        match(t_id);
    }
    else if (is_constant()) {
        constant();
    }
    else if (lookahead() == t_lparen) {
        match(t_lparen);
        expr();
        if (!match(t_rparen)) {
            expected(lexer->file, look, "%s", token_meaning(t_rparen));
        }
    }
    else {
        expected(lexer->file, look, "identifier, literal constant, or left parentheses");
    }
}

static void constant()
{
    if (lookahead() == t_int) {
        match(t_int);
    }
    else if (lookahead() == t_sqstr) {
        match(t_sqstr);
    }
    else if (lookahead() == t_dqstr) {
        match(t_dqstr);
    }
    else {
        expected(lexer->file, look, "literal constant: expected int, sqstr, or dqstr");
    }
}

// lookahead

static bool is_constant()
{
    if  (lookahead() == t_int || lookahead() == t_sqstr || lookahead() == t_dqstr) {
        return true;
    }
    else {
        return false;
    }
}
