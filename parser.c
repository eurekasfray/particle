
//==============================================================================
// Translate
//==============================================================================

void *translate()
{
    struct source *source;

    // Get first character for the lexer to start with
    lexer = init_lexer();
    lexer->input = get_next_char(lexer->srcfile);
}
