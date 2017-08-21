Translation stages:

- parse src to asm
- parse asm to interm.rep then to obj.code
- run obj.code

# Assembler

Assembly line format:

- [label] [mnemonic operands] [comment]

Possible assembly lines:

label:
movb 24
; comment
label: movb 24

Pass 1:

The lexer: The lexer brakes each line into tokens. It stores each token into a buffer called the *token buffer*. The token buffer should be a fixed-size array. An array with 256 indecies -- if so many -- should be used.

The syntax checker: This part checks the syntax of the current line.
* The checker reads each token from the token buffer.
* The syntax checker expects either an empty line, or the following in the order defined below:
  * label (optional): if no label is present, then the checker looks for an instruction.
  * instruction (optional): if no instruction is present, then line is empty if no other tokens are present. However, if unexpected tokens are present, report a syntax error.
* What is an empty line? An empty line is a line filled with whitespace and only contains a NEWLINE token. Lines with comments are also considered empty, because comments are ignored entirely by the compiler.
* If there is a mnemonic, we look it up and get its size in order to increment the LC.
  * If there is a label definition, we save a tuple to the symbol table as {label identifier and the value of LC}.
* We generate a branch to the immediate representation, in memory, based on the current line. The branch must have nodes containing the following data: the LC value for the current line; pointer to the mnemonic in the mnemonic table; pointer to the identifier's entry in the symbol table if a label exists
  * If the line is a directive, execute the directive:
    * db (define byte):
      * This directive is executed in pass 1
      * Get total length operands; then increment LC by the length.
      * Generate 'db' branch with nodes containing the following: operand length, and concatenated operands. For instance​, if the instruction is `db "Hello, World!"`; then the length node shall contain the value 13, and the value node shall contain the byte string "Hello, World!" 
    * dw (define word):
      * Same goes for the 'dw' directive. The value is broken up into byte values.
    * dd (define dword):
      * …

Design of the intermediate representation:

* It's a tree
* Table entry:
  * Record type: instruction & directive
  * Has label indicator: Indicates whether line has label or not
  * LC value for line
  * Pointer to specific entry in the opcode table or directive table

Design of symbol table:
* It's a tree
* It keeps the following info:
  * name: 
  * type: can be LABEL or EQUATE
  * value: This is a `long int`

Opcode table:

* It keeps the following info:
  * mnemonic
  * opcode
  * number of operands
  * 