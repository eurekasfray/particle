# Assembler

- Line format: `label mnemonic operand`

- A comment begins with the '#' (hash) character, because the lexer already sees '#' as the comment initiator. By using that character, we maintain consistency between the assembly syntax and Particle syntax.

- The assembler processes the source file line by line. Each line stands for itself;

  For each production -- label, mnemonic, and operand -- the FSM produces an ASR (abstract-syntax record) and stores it in the ASA.

- The semantic analyzer checks the meaning of the line represented by the ASA
  - mnemonic;
    - is it an instruction of directive?
  - operand;
    - does the operand type match its mnemonic
    -is a `string` being used where an `int` should be?
  - does the operand size match the mnemonic; for instance, is a word being used with a byte operation.
  The semantic analyzer produces a table. The table has a fixed format and has the following entries:
    - the record type
    - the LC value for the line
    - pointer to the instruction in the opcode table
  - The assembler interprets the use of a label based on the mnemonic. The mnemonic tells the assembler how to treat a label. If the mnemonic is an instruction then the label shall be treated as an *address symbol* and store as a symbol.
