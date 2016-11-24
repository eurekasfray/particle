# Particle spec

# Introduction

* This document is the specification for the Particle programming language.

* The style of this document was inspired by the C++ specification <http://www.csci.csusb.edu/dick/c++std/cd2/index.html>. Language's concepts, ideas, and gotchas are written as points called articles. Each article expresses a single idea about the programming language with the good intention to enhance the reading experience with simple consumable concepts.

## Goals

* My aim goal is to develop a simple and useable programming language whose core concept is based the ideas behind the assembly languages (x86 especially). The language should allow a programmer to build a program using basic data types such as bytes, words and doubles words by manipulating data. Let's see the possibilities.


# Design

## Basics

* The constructs of the language are designed to satisfy the following requirements:

* Turing-complete. To do this, the language will need:
  - Control-flow constructs:
  - Storage manipulation: the ability to write/read values to/from objects, and the ability to get the address of an object.

* A similarity to assembly:
  - Direct addressing. In assembly a programmer can write directly to memory by a instruction such as

    ```
    mov [0xA000], 100
    ```

    The language's assignment statement should facilitate direct addressing like

    ```
    0xA000 = 100
    ```


# Concepts

## Basics

* This part introduces the basic concepts of the programming language.

* An *entity* is a value or object.

* A *value* is an expression that cannot be evaluated further.

* An *lvalue* is a value that points to an address in memory.

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

* A declaration specifies how identifiers are interpreted.

## Declarators

* A declarator specifies the identifier of an object under a declaration.

* A *variable declarator* specifies the identifier and the number of the storage element(s) for a variable, and causes storage to be reserved for that variable.

* A *function declarator* specifies the identifier and formal parameters of a function.

## Initializer

* A declarator can specify the initial value(s) of the identifier being declared.

* The initializers of an identifier for a variable declares the initial values of each storage element of a variable.

## Definition

* The definition of a function includes the function body.


# Expressions

## Unary operators

* The `$` operator returns the size of its operand in bytes. The operand shall only be a variable. The result of this operation is a `word` value.

* The `&` operator returns the address of its operand as a `word`. The operand shall be an identifier.

* The `*` operator treats its operand as an address and returns the value stored at the address. The operand shall be an expression.

## Literals

* The values `true`, `false`, and `null` are representations of the integer values values 0 and 1. The `null` and `false` numerals is a representation for the integer value 0. The `true` value numerals is a representation for the integer value 1.


# Statements

* The expression statement TBD

* The `if` statement TBD

* The `while` statement TBD

* The `for` statement TBD

* The `break` statement TBD

* The `continue` statement TBD

* The `return` statement TBD

* The `next` statement TBD


# Built-in functions

* The `get()` . This is the prototype: `byte get(void)`. It takes no arguments and returns a `byte` value. TBD

* The `put()` . This is the prototype: `void out(byte)`. TBD

* The `malloc()` . This is the prototype: `word malloc(word)`. It takes a `word` argument abd returns a `word` argument. TBD

* The `free()` . This is the prototype: `void free(word)`. TBD
