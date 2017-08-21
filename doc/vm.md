# The M32 virtual machine specification

# The machine

## General design

- **Machine type.** This is a stack-based process virtual machine with a load/store architecture
- **Size of data line.** The machine can process integer values of up to 32-bits.
- **Size of address line.** The machine has 4MB of memory which suggests an address line of 22 bits. A memory of that size should be enough for a programming language of esoteric origin.
- **All data live in memory.** The program, program data, call stack, and expression stack all live in memory. The decision to keep the stacks in memory and not a separate component was chosen in order to give the program the power to access and manipulate the stacks. The decision also allowed for simpler implementation.

## Concepts

- A *byte* is a unit of 8 bits.
- A *word* is a unit of 16 bits. Furthermore, it reflects a fixed-size of data that the instruction set handles.
- A *dword* (or double word) is a unit of 2 words.
- A *float* is a 32-bit single-precision floating point. A float represents a real number.

# Memory model

- **How data is stored in memory.** Binary values in memory are stored in big endian. Data is stored in memory as is, so that the manipulation of data is simple and unabstracted.
- **Total number of cells.** M has 2^22 (= 4,194,304 = `0x400000` = 4MB) cells. This machine has an address line 22 bits wide. It is safe to say a hexadecimal number shall only be 6 digits long because memory is only 4MB.
- **Cell size.** Each cell stores a unit of 8-bits. Therefore
  - a *byte* occupies 1 cell
  - a *word* occupies 2 cells
  - a *dword* occupies 4 cells.
- **Cell address.**
  - A cell address is a non-negative integer value.
  - A cell starts at address `0x000000`.
- **Segmentation.** Memory is divided into segments. Segments are divided into equal parts.
  - **Size of a segment.** Each segment is 1MB long.
  - **Arrangement of segments in memory.** Segments are arranged in memory as follows:
    1. Code: Stores program and program data.
    2. Heap: Serves as additional memory for program use.
    3. Expression stack: Stores operands for operations and operation results.
    4. Call stack: Stores the return address and stack frames for subroutine calls.
  - A visual display:
    ```
    <----------- 4MB ----------->

    +------+------+------+------+
    |  1MB |  1MB |  1MB |  1MB |
    +------+------+------+------+
      code   heap   expr   call
                    stack  stack
    ```

# Registers

- **Why use registers.** The machine has several internal registers. Even though the machine is stack-based, it uses registers to accomplish operations. Whereas, the stack is used to store operation operands and results.
- **Size of registers.** Registers are 32 bits wide.
- **How values are stored in registers.** Binary values in registers are stored in big endian.
- **List of internal registers.**
    - `pc`: program counter (dword) -- points to the next instruction.
    - `mar`: memory address register (dword) -- stores the memory address for a memory operation.
    - `mdr`: memory data register (dword) -- stores data for a memory operation.
    - `cir`: current instruction register (dword) -- stores the current instruction to be decoded and executed.
    - `ep`: expression stack pointer (dword) -- points to the top of the expression stack.
    - `cp`: code stack pointer (dword) -- points to the top of the expression stack.
    - `fp`: frame pointer (dword) -- points to the base of the current stack frame.
    - `t0` .. `t2`: three temporary registers (dword)  -- stores temporary values.
    - `znvc`: flag register -- z:zero; n:negative, v:overflow; c:carry

# Instruction set

## Conventions

- TOES means "top of expression stack". The TOES refers to the value of the EP.
- SOES means "second on expression stack". The SOES refers to the value of the EP minus a data size. The data size differs between operations. Data size may be a byte, word, or dword. If the operation being performed is a byte operation then the data size will be a word.
- TOCS means "top of call stack".
- SOCS means "second on call stack"
- [deprecated] TOES<size> means the size at TOES. For instance, the expression TOES<byte> means "the byte at TOES", where a byte at the address EP is read. If the expression were TOES<word>, the expression would mean "the word at TOES", where at a word is read from base address EP.
- Appended 'b' means 'integer byte'
- Appended 'w' means 'integer word'
- Appended 'd' means 'integer dword'
- Appended 'f' means 'floating point'
- Appended 's' means 'signed'
- **int** represents an integer value
- **addr** is an integer value. It represents a cell address in memory. It is no different from an int.

## Design

- Every instruction just is an opcode, because an instruction encoding only contains opcode information and it is not encoded with additional information.
- All instructions are a word wide.
- The VM only supports direct and immediate addressing.

## Instruction encoding

- The instruction encoding of this machine stores little information, because majority of the instructions in this instruction set does not carry an operand and also the machine only has two addressing modes. Therefore an instruction does need to carry a hoard of information for the machine to interpret.

- **Instruction information**
  - An instruction carries the following information:
    - Opcode
    - Operand-size
    - Addressing mode
  - How the classes occupy an instruction:
    ```
     31                                             8 7      4 3  2 1  0
    +------------------------------------------------+--------+----+----+
    |                       oc                       |   sc   |    | ac |
    +--------------------------------------------------------------+----+
    ```
    - oc - opcode class: occupies 24 bits at bits 8 - 31
    - pc - operand-size class (or simply size class): occupies 2 bits at bits 4 - 3
      - When *pc* = `3`, then operand size is dword
      - When *pc* = `2`, then operand size is word
      - When *pc* = `1`, then operand size is byte
      - When *pc* = `0`, then instruction does not support operands
    - ac - addressing-mode class: occupies 1 bit at bits 0 - 1
      - When *ac* = `1`, then addressing mode is direct
      - When *ac* = `0`, then addressing mode is immediate
    - Bits 2 - 3 are reserved because we want the instruction to be aligned in nibbles. The opcode class for instance is exactly several nibbles long.


- When an instruction is stored in memory, it is stored in a specific format. This format is stored in memory in the form of two fields:
  - Instruction field: This field is 1 word wide.
  - Immediate field: This field is either nonexistent or 1 byte to 1 dword wide, depending on the type of operation.

- **Opcode lead number**
  - This number helps to divide instructions in sections semantically for the VM developer to add new instructions under the same section. The opcode lead number has nothing to do with the machine and does not affect how the machine functions. Instead, it instructions from being scattered across the opcode numbers. Instead it helps to semantically group opcode together.

## Expression stack

- **Opcode lead number:**
  - `0x01--`

- **movb int** (Move Byte)
  - Moves immediate byte integer to TOES
  - Opcode - `0x01`
  - Operation:
    - mem[ep] := int
  - Addressing mode
  - Flags affected

- **movw int** (Move Word)
  - Moves immediate word integer to TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **movd int** (Move Dword)
  - Moves immediate dword integer to TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **dropb** (Drop Byte)
  - Discards byte integer from TOES.
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **dropw** (Drop Word)
  - Discards word integer from TOES.
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **dropd** (Drop Dword)
  - Discards dword integer from TOES.
  - Opcode - `0x01`
  - Operation -

- **dupb** (Duplicate Byte)
  - Discards byte integer on TOES and push it to TOES.
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **dupw** (Duplicate Word)
  - Copy word integer on TOES and push it to TOES.
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **dupd** (Duplicate Dword)
  - Copy dword integer on TOES and push it to TOES.
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **overb** (Over Byte)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **overw** (Over Word)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **overd** (Over Dword)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **swapb** (Swap Byte)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **swapw** (Swap Word)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **swapd** (Swap Dword)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **rollb int** (Roll Byte)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **rotb int** (Rotate Byte)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **rotcb int** (Counter-Rotate Byte)
  - ...
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **pushep** (Push EP)
  - Copies the dword value of the EP register to TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **popep** (Pop EP)
  - Copies the dword at the TOES to the EP register
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **pushcp** (Push CP)
  - Copies the dword value of the CP register to TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **popcp** (Pop CP)
  - Copies the dword at TOES to the CP register
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **pushfp** (Push FP)
  - Copies the dword value of the FP register to TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **popfp** (Pull FP)
  - Copies the dword at the TOES to the FP register
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

## Memory

- **Opcode lead number:**
  - `0x02--`

- **lodb addr** (Load Byte)
  - Moves byte integer from memory to TOES
  - Opcode - `0x02`
  - Operation - *mem[ep] := mem[addr]*
  - Addressing mode
  - Flags affected

- **lodw addr** (Load Word)
  - Move word integer from memory to TOES
  - Opcode - `0x02`
  - Operation - *[TBD insert operation]*
  - Addressing mode
  - Flags affected

- **lodd addr** (Load Dword)
  - Move dword integer from memory to TOES
  - Opcode - `0x02`
  - Operation - *[TBD insert operation]*
  - Addressing mode
  - Flags affected

- **stob addr** (Store Byte)
  - Move byte integer at address in memory to TOES
  - Opcode - `0x02`
  - Operation - *mem[addr] := mem[EP]*
  - Addressing mode
  - Flags affected

- **stow addr** (Store Word)
  - Move word integer at address in memory to TOES
  - Opcode - `0x02`
  - Operation -
  - Addressing mode
  - Flags affected

- **stod addr** (Store Dword)
  - Move dword integer at address in memory to TOES
  - Opcode - `0x02`
  - Operation -
  - Addressing mode
  - Flags affected

- **peekb addr** (Peek Byte)
  - Copies byte at TOES to memory without dropping the TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **peekw addr** (Peek Word)
  - Copies word at TOES to memory without dropping the TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **peekd addr** (Peek Dword)
  - Copies dword at TOES to memory without dropping the TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **pushb** (Push Byte)
  - Move byte integer to TOES at the address specified at TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **pushw** (Push Word)
  - Move word integer to TOES at the address specified at TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **pushd** (Push Dword)
  - Move dword integer to TOES at the address specified at TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **popb** (Push Word)
  - Move byte integer at SOES to address specified at TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **popw** (Push Word)
  - Move word integer at SOES to address specified at TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

- **popd** (Push Dword)
  - Move dword integer at SOES to address specified at TOES
  - Opcode - `0x01`
  - Operation -
  - Addressing mode
  - Flags affected

## Jump

- **Opcode lead number:**
  - `0x03--`

- **jmp addr** (Unconditional Jump)
  - Jump to address in operand -- *pc := addr*
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

- **jz addr** (Jump If Zero)
  - Jump to address in operand, if TOES is zero -- *if mem[ep] == 0, then pc := addr*
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

- **jnz addr** (Jump If Not Zero)
  - Jump to address in operand, if TOES is not zero.
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

- **je addr** (Jump If Equal)
  - Jump to address in operand, if TOES and SOES are equal.
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

- **jne addr** (Jump If Not Equal)
  - ...
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

- **call addr** (Call)
  - push value of PC to CS
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

- **ret int** (Return)
  - Returns from a subroutine call, after it removes the stack-frame context created by the `call` operation, and pops the given number of bytes specified in the operation from the call stack
  - Opcode - `0x03`
  - Operation -
  - Addressing mode
  - Flags affected

## Integer arithmetic

- **Opcode lead number:**
  - `0x04--`

- **addb** (Add Byte):
  - Adds byte at TOES and byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **addbu** (Add Unsigned Byte):
  - Adds unsigned byte at TOES and unsigned byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **addw** (Add Word)
  - Adds word at TOES and word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **addwu** (Add Unsigned Word)
  - Adds unsigned word at TOES and unsigned word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **addd** (Add Dword)
  - Adds dword at TOES and dword SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **adddu** (Add Unsigned Dword)
  - Adds unsigned dword at TOES and unsigned dword SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **subb** (Subtract Byte)
  - Subtracts byte at TOES from byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **subbu** (Subtract Unsigned Byte)
  - Subtracts unsigned byte at TOES from unsigned byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **subw** (Subtract Word)
  - Subtracts word at TOES from word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **subwu** (Subtract Unsigned Word)
  - Subtracts unsigned word at TOES from unsigned word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **subd** (Subtract Dword)
  - Subtracts dword at TOES from dword at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **subdu** (Subtract Unsigned Dword)
  - Subtracts unsigned dword at TOES from unsigned dword at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **mulb** (Multiply Byte)
  - Multiplies byte at TOES by byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **mulbu** (Multiply Unsigned Byte)
  - Multiplies unsigned byte at TOES by unsigned byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **mulw** (Multiply Word)
  - Multiplies word at TOES by word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **mulwu** (Multiply Unsigned Word)
  - Multiplies unsigned word at TOES by unsigned word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **muld** (Multiply Dword)
  - Multiplies dword at TOES by dword at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **muldu** (Multiply Unsigned Dword)
  - Multiplies unsigned dword at TOES by unsigned dword at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **divb** (Divide Byte)
  - Divides byte at TOES into by byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **divbu** (Divide Unsigned Byte)
  - Divides unsigned byte at TOES into by unsigned byte at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **divw** (Divide Word)
  - Divides word at TOES into by word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **divwu** (Divide Unsigned Word)
  - Divides unsigned word at TOES into by unsigned word at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **divd** (Divide Dword)
  - Divides dword at TOES into by dword at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **divdu** (Divide Unsigned Dword)
  - Divides unsigned dword at TOES into by unsigned dword at SOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **modb** (Modulo Byte)
  - Performs modulo arithmetic on byte at SOES using the byte at TOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **modbu** (Modulo Unsigned Byte)
  - Performs modulo arithmetic on unsigned byte at SOES using the unsigned byte at TOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **modw** (Modulo Word)
  - Performs modulo arithmetic on word at SOES using the word at TOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **modwu** (Modulo Unsigned Word)
  - Performs modulo arithmetic on unsigned word at SOES using the unsigned word at TOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **modd** (Modulo Dword)
  - Performs modulo arithmetic on dword at SOES using the dword at TOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

- **moddu** (Modulo Unsigned Dword)
  - Performs modulo arithmetic on unsigned dword at SOES using the unsigned dword at TOES
  - Opcode - `0x04`
  - Operation -
  - Addressing mode
  - Flags affected

## Floating-point arithmetic

- **Opcode lead number:**
  - `0x05--`

- *TBD*

## Bitwise

- **Opcode lead number:**
  - `0x06--`

- **andb**
  - Performs bitwise AND on byte at TOES, and replace byte at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **andw**
  - Performs bitwise AND on word at TOES, and replace word at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **andd**
  - Performs bitwise AND on dword at TOES, and replace dword at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **orb**
  - performs bitwise OR on byte at TOES, and replace byte at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **orw**
  - Performs bitwise OR on word at TOES, and replace word at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **ord**
  - Performs bitwise OR on dword at TOES, and replace dword at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **xorb**
  - Performs bitwise XOR on dword at TOES, and replace byte at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **xorw**
  - Performs bitwise XOR on word at TOES, and replace word at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **xord**
  - Performs bitwise XOR on dword at TOES, and replace dword at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **notb**
  - Performs bitwise NOT on byte at TOES, and replace byte at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **notw**
  - Performs bitwise NOT on word at TOES, and replace word at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

- **notd** -
  - Performs bitwise NOT on dword at TOES, and replace dword at TOES with result.
  - Opcode - `0x06`
  - Operation -
  - Addressing mode
  - Flags affected

## Human-interface

- **Opcode lead number:**
  - `0x0F--`

- **get** (Get)
  - Gets byte from standard input stream and stores it at TOES
  - Opcode - `0x0F`
  - Operation -

- **put** (Put)
  - Outputs byte from TOES to standard output stream
  - Opcode - `0x0F`
  - Operation -

## Machine

- **Opcode lead number:**
  - `0x00--`

- **nop** (No Operation):
  - Does absolutely nothing
  - Opcode - `0x00`
  - Operation -
  - Addressing mode
  - Flags affected

- **halt** (Halt):
  - Halts the CPU
  - Opcode - `0x00`
  - Operation -
  - Addressing mode
  - Flags affected

## Excluded instructions

The following instructions were excluded because they can be accomplished using other instructions.

### Data manipulation

- cbw - convert the byte at TOES to a word. This can be accomplished using a bitwise operation, and a few pulls and pushes.
- cbd - convert the byte at TOES to a dword. This can be accomplished using a bitwise operation, and a few pulls and pushes.
- cwb - convert the word at TOES to byte. Same as above.
- cdb - convert the word at TOES to byte. Same as above.

# Expression stack

TBD

# Call stack

TBD

# CPU - finite-state machine

-

# Assembler

## Directives

- **org** (Originate?)
  - Defines value of the LC. If defined, the LC is set to the value specified in the operand. During the assembly process at the next pass in the assembler, the instructions after the `org` directive are written to the value of the LC.
  - Format - **org int**
  - Example 1 - **org 100h**
- **equ** (Equate)
  - Defines a symbol. This line expects an defined label.
  - Format - **name equ int|str**
  - Example 1 - **COUNTRY_CODE EQU "ANU"**
  - Example 2 - **MAX EQU 99**
- **db** (Define Byte)
  - Defines a byte.
  - Format - **[label] db int|str**
  - Example 1 - **help db "Type --help for a comprehensive help guide"**
  - Example 2 - **alphabet_size db 26**
- **dw** (Define Dword)
  - Defines a word.
- **dd** (Define Dword)
  - Defines a dword.

## Special symbols

- **$** (Current LC Value) -
  - Represents the current value of the assembler's LC (Location Counter). This actively maintains the value of the LC.
- **$$** (LC Value After Pass 1)
  - Represents the value of LC after Pass 1.

## Size override

What are they and why are they used? They are used when an instruction defines a default size that it expects and you need to use a different size other than the default size.
