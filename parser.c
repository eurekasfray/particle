static Token *look;
static FILE *srcfile;

//==============================================================================
// Accessors
//==============================================================================

// Set look

void parser_set_look(struct token *token)
{
    look = token;
}

// Get look

struct token *parser_get_look()
{
    return look;
}

// Set srcfile

void parser_set_srcfile(FILE *fp)
{
    srcfile = fp;
}

// Get srcfile

FILE *parser_get_srcfile()
{
    return srcfile;
}

//==============================================================================
// Translate
//==============================================================================

void *translate()
{
    // Get first character for the lexer to start with
    lexer = init_lexer();
    lexer->input = get_next_char();
}

bool match(TokenType expected_type)
{
    if (look->type == expected_type) {
        look = get_next_token();
        return true;
    }
    else {
        return false;
    }
}