# Particle compiler spec

# Introduction

This is the Particle compiler spec. Its role is to document the design and implementation of the compiler.

The Particle compiler is uses the technique of syntax-directed translation.

# Compilation process

## Front-end

```
               +----------+                  +------------+
character ---> | lexical  | ---> token  ---> | syntax     | ---> intermediate
stream         | analyzer |      stream      | directed   |      representation
               +----------+                  | translator |
                                             +------------+
```

The compiler is built using the concepts of units and ADTs.

A unit is modular.

## Translator

### Units

Tokenizer (unit)
- Takes a stream of characters and outputs a token

Translator (unit)
- Takes a stream of tokens and performs direct syntax translation to it. The translation emits an assembly source data structure.

## Assembler

Assembler (unit)
- Takes an assembly source as input and outputs a binary file

## VM

VM (unit)
- Takes a binary file as input and executes it

### ADT

Source

```
#include 'translator/
```

An assembly source is a linked list. Each node stores an assembly source line.

# Command-line design

```
Usage: particle [options] file
Options
  -a --assemble  Treats the source file as assembly

```
