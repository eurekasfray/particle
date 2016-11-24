# Particle spec

# Introduction

* This document is the specification for the Particle programming language.

* The style of this document was inspired by [Dick's November 1996 Working Paper C++ standards draft](http://www.csci.csusb.edu/dick/c++std/cd2/index.html). Concepts, ideas, and gotchas of the language are written as points called articles. Each article expresses a single idea about the programming language with the good intention to enhance the reading experience with simple consumable concepts.

# Design

## Goals

* My goal is to develop a simple, useable programming language whose core concept is based the ideas behind the assembly languages (x86 specifically). The language shall allow a programmer to build a program using three basic data types (bytes, words, and double words). Let's see the possibilities.

## Basics

* The constructs of the language are designed to satisfy the following requirements:

* 1) Turing-complete. To do this, the language will need:
  - Control-flow constructs:
  - Storage manipulation: the ability to write/read values to/from objects, and the ability to get the address of an object.

* 2) A similarity to assembly:
  - Direct addressing. In assembly a programmer can write directly to memory by a instruction such as

    ```
    mov [0xA000], 100
    ```

    The language's assignment statement should facilitate direct addressing like

    ```
    0xA000 = 100
    ```

# Lexical conventions

## Character set

* A program shall only contain characters specified by the character set.

### Source encoding

* The encoding of the input source shall support 7-bit ASCII, and shall not support any other encoding.

### Alphabets

```
Uppercase characters

A  B  C  D  E  F  G  H  I  J  K  L  M
N  O  P  Q  R  S  T  U  V  W  X  Y  Z
```

```
Lowercase characters

a  b  c  d  e  f  g  h  i  j  k  l  m
n  o  p  q  r  s  t  u  v  w  x  y  z
```

```
Symbol character

_   underscore
```

### Digits

```
0  1  2  3  4  5  6  7  8  9
```


### Special characters

```
+   plus sign           /   division sign           %   percentage sign

-   minus sign          *   multiplication sign     &   ampersand

|   vertical bar        (   left parenthesis        )   right parenthesis

{   left curly bracket  }   right curly bracket     ,   comma

:   colon               ;   semicolon               =   equal sign

<   left-than sign      >   greater-than sign       !   exclamation mark

~   tilde               ^   caret                   #   hash/pound sign

@   at sign
```

### Whitespace characters

```
\n  newline             \r  carriage return         \v  vertical tab

\t  horizontal tab      \0  null                    \b  blank space

\'  single quote        \"  double quote
```

### Execution characters

```
[Put escape characters here]
```

## Translation phases

TBD

## Keywords

TBD

## Token

* A *token* is structure that represents a lexeme. A token provides the translator with basic information about its lexeme. The information a token provides shall include the following: 1) The *lexeme* which is a unit of characters. 2) The *type* which a unique value that is used to classify a lexeme. 3) The *int value* which stores the evaluated integer value of the lexeme if the token type is t_int. 4) The *string value* which stores the evaluated value of a string literal if the token type is either t_dqstr or t_sqstr. Other information a token may provide includes the following: 1) The *line number* on which the token was collected by the translator. The line number should be maintained by keeping a count of the number of newline characters a the translator encounters in the input stream. 2) The *column number* on which the token was collected by the translator. The column number should be counted by counting each character read from the input stream. However, the counter shall be reset to the value `1` if the newline character is read.

* Whitespace characters shall delimit tokens. The lexical analyzer ignores whitespace characters entirely.

* Tokens are groups into [insert number here] classes.

* Apart from the above, other tokens include t_eof, t_char.

* Tokenization is the process of recognizing lexemes and creating the appropriate token for the lexeme.

* The translator shall recognize the following token types:

  ```
  t_int t_eof t_newline t_unknown
  [ place token types here along side their syntax/pattern; meaning, what syntax makes t_int an int? ]
  ```

* If a type cannot recognization a matching type for a lexeme, then the lexeme shall be assigned the `t_unknown` type.

* For a token whose type is `t_unknown` the translator shall raise an error to report the unknown lexeme.

## Comments

* A comment is initiated by the hash character (`#`) and is terminated by either the t_newline token or t_eof token.

* The translator shall not tokenize a comment. A comment is ignored entirely by the compiler. That is, the compiler ignores any sequence of characters that follows the hash character. The compiler does this until either a new line or the end of the file is met.


# Concepts

## Basics

* This part introduces the basic concepts of the programming language.

* An *entity* is a value or object.

* A *value* is an expression that cannot be evaluated further.

* An *lvalue* is an expression that refers to an address in memory.

* An *object* is a location in memory. An object is either a variable or function.

* A *variable* is a storage location. (A variable shall have an lvalue and rvalue)?. (Access to a variable's lvalue and rvalue is associated with the variable's identifier)?.

* A *storage element* is a unit of storage. It is a contiguous sequence of cells in memory that is treated as a unit of data. A variable can have zero or more storage elements. Storage elements are stored as a contiguous sequence in memory. The first storage element in the sequence is called the *zeroeth element*. When an identifier is declared, the lvalue and rvalue of the variable are associated with the first (or zeroeth) storage element.

* A *function* is a location of a unit of instructions.

* An *identifier* is a name used to identify an object.

* A *type* determines the meaning of each storage element of an object. There are four types: `byte`, `word`, `dword`, and `void`.

* `byte`: A variable declared as `byte` can store 8 bits of data. This is the smallest unit of data. It occupies one cell of memory. For the declared object, each storage element is interpreted as units of 8 bits of data.

* `word`: A variable declared as `word` can store 16 bits (or 2 bytes) of data. It is the size of the data path of the machine. It occupies two contiguous cells of memory. For the declared object, each of the object's storage element is interpreted as units of 16 bits of data.

* `dword`: A variable declared as `dword` (double word) can store 32 bits (or double the size of a word). It occupies four contiguous cells of memory. For the declared object, each of the object's storage element is interpreted as units of 32 bits of data.

* `void`: A variable declared as `void` carries an empty value. The declared object has no storage element and the object is interpreted as incomplete (why?).


# Declarations

* A *declaration* specifies how identifiers are interpreted.

## Declarators

* A *declarator* specifies the identifier of an object under a declaration.

* A *variable declarator* specifies the identifier and the number of the storage element(s) for a variable, and causes storage to be reserved for that variable.

* A *function declarator* specifies the identifier and formal parameters of a function.

## Initializer

* A declarator can specify the initial value(s) of the identifier being declared.

* The initializers of an identifier for a variable declares the initial values of each storage element of a variable.

## Definition

* The definition of a function includes the function body.


# Expressions

## Unary operators

* The `$` operator returns the size of its operand in bytes. The operand of this operator must only be a variable. The result of this operation is a `word` value.

* The `&` operator returns the address of its operand as a `word`. The operand shall be an identifier.

* The `*` operator treats its operand as an address and returns the value stored at the address. The operand shall be an expression.

## Literals

* The values `true`, `false`, and `null` are representations of the integer values values `0` and `1`. The `null` and `false` numerals is a representation for the integer value `0`. The `true` value numerals is a representation for the integer value `1`.


# Statements

## Expression statement

TBD

## The `if` statement

TBD

## The `while` statement

TBD

## The `for` statement

TBD

## The `break` statement

TBD

## The `continue` statement

TBD

## The `return` statement

TBD

## The `next` statement

TBD


# Built-in functions

## The `get` function

* The `get()` reads a character from the input stream.

* The prototype of this function is as follows: `byte get(void)`.

* It takes no arguments and returns a `byte` value.

* TBD

## The `put()` function

* The `put()` outputs a character to the output stream.

* The prototype of this function is as follows: `void out(byte)`.

* TBD

## The `malloc()` function

* The `malloc()` TBD

* The prototype of this function is as follows: `word malloc(word)`.

* The function takes a `word` argument and returns a `word` argument.

## The `free()` function

* The `free()` TBD

* The prototype of this function is as follows: `void free(word)`.
