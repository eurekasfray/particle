Status: File contains outdated information but may be useful for when writing up new assembler documentation

---

Syntax of a line:

- [label] [instruction]

The Assembler:

- process the source file line by line:
  - for each line, we use a finite-state machine to process each token of the line. (comments are ignored entirely by the lexer. the lexer ignores all inputs until it encounters either the NEWLINE or EOF characters.)

Label?
- at Label branch

Mnemonic?
- add Instruction branch if is instruction
- add Directive branch if is directive


build token buffer():
    token = next_token();
    while not newline or eof
        insert token into token buffer
        increment buffer_size_counter by 1
    endwhile
    insert token into token buffer
    increment buffer_size_counter by 1

parse tokens():
    check if token_buffer[0] is id and token_buffer[1] is colon; if so:
        add label to symbol table (name, value, type) as (token->lexeme, lc, SYMTYPE_LABEL)
        (add branch to intermediate representation)
        set has_label = true;
    check if token at token_buffer[0] is instruction; if so:
        get some info about the instruction, so we can determine the syntax of the line:
            instruction_size = the size of the instruction
            has_operand = does instruction have an operand? (yes or no)
            operand_size = the number of bytes the operand occupies in memory?
        endget
        if (has_operand == yes) then
            instruction_size += operand_size
            expect token at token_buffer[1] to be either an int or str
            if token is int then  
                if operand_size == 1 AND token's intval > 0xFF then
                    report error
                else if operand_size ==  2 AND token's intval > 0xFFFF then
                    report error
                else if operand_size == 4 AND token's intval > 0xFFFFFFFF then
                    report error
                endif
            else if token is str then
                if operand_size == 1 AND length of token's strval > 1 then
                    report error
                else if operand_size == 2 AND length of token's strval > 2 then
                    report error
                else if operand_size == 4 AND length of token's strval > 4 then
                    report error
                endif
            endif
        endif
        set lc += instruction_size;
