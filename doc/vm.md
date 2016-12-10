# The Particle virtual machine specification 

# Overview

This is a process virtual machine. The machine is stack-based. This machine can process values of 32-bits, and has 4MB of memory which suggests an address line of 22 bits.

# Concepts 

- A *byte* is a unit of 8 bits.
- A *word* is a unit of 16 bits. Furthermore, it reflects a fixed-size of data that the instruction set handles.
- A *dword* (or double word) is a unit of 2 words.

# Endianness

Reverse endian.

# Components 

## Expression stack

TBD

## Call stack

TBD

## Memory

  - Has 2^22 (= 4,194,304 = 4MB) cells. This machine has an address line of 22 bits.
  - Each cell stores a unit of 8-bits.
  - A cell address is a non-integer value. 
  - A cell starts at address 0.
  - Memory is divided into segments. The expression and call stacks live in memory. By keeping the stacks in memory, the program may access and manipulate the stacks.
    - Program+data: Stores program and program data.
    - Heap: Serves as additional memory for program use.
    - Expression stack:
    - Call stack: Stores the return address for subroutine calls.

## Registers

  - Registers can are 32 bits wide.
  - pc: program counter -- points to the next instruction.
  - mar: memory address register -- stores the memory address for a memory operation.
  - mdr: memory data register -- stores data for a memory operation.
  - cir: current instruction register -- stores the current instruction to be decoded and executed.
  - sp: stack pointer -- points to the top of the stack.
  - fp: frame pointer -- points to the base of the current stack frame.
  - t0 .. t2: three temporary registers -- stores temporary values.

# Instruction set

## Conventions

- TOS means "top of stack".

## Design

- Every instruction is opcode. Instructions are not encoded with additional information, and only contain opcode information. All instructions are a byte wide.

## Stack

pushbi - move byte integer operand to TOS.
pushwi - move word integer operand to TOS.
pushdi - move dword integer operand to TOS.
popbi - discard byte integer from TOS.
popwi - discard word integer from TOS.
popdi - discard dword integer from TOS.
dupbi - copy byte integer on TOS and push it to TOS.
dupwi -
dupdi -
overbi - …
overwi -
overdi -
swapbi -
swapwi -
swapdi -
rollbi -
rotbi -
rotcbi -
peekbi -
pushsp - Push the (dword) value of SP to TOS.
pushfp - Push the (dword) value of FP to TOS.

## Memory

loadbi - move byte integer from memory to stack.
pullbi - move byte integer fron stack to memory.

## Jump

jmp
jz
jnz
je
jne
call
ret

## Math

addbi
subbi
mulbi
divbi
modbi

## Bitwise

andbi
orbi
xorbi
notbi

## Misc

nop: no operation. 0x00
