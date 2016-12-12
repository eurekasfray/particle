static FILE *srcfile;

//==============================================================================
// Accessors
//==============================================================================

void set_srcfile(FILE *fp)
{
    srcfile = fp;
}

FILE *get_srcfile()
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
    lexer->input = get_next_char(lexer->srcfile);
}
