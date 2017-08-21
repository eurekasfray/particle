# Particle

## Description

Particle is a little programming language inspired by assembly. It features an interpreter that runs Particle. The interpreter compiles Particle into Particle assembly code, which is in turn assembled into machine code for a stack-based virtual machine to execute. The Particle interpreter is written in C and compiles with `gcc`.

## Author

Created by eurekasfray

## Progress

- Lexer (complete)
- Compiler or syntax-directed translator: (almost there)
  - Parser (almost complete)
  - Semantic analyzer (todo)
  - Code generation (todo)
  - Symbol table (todo)
- Assembler:
  - Parser (done; can pass an entire file)
  - Symbol table (todo)
  - Opcode table (todo)
  - Directive table (todo)
- Virtual machine:
  - Design (almost done)
  - Implementation (almost done)
- Testing (todo)
