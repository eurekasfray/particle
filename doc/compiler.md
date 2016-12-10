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
