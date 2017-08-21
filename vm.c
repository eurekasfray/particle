//==============================================================================
// Includes
//==============================================================================

#include <stdio.h>
#include <stdbool.h>
#include "vm.h"
#include "opcode.h"
#include "error.h"
#include "utils.h"

//==============================================================================
// VM data sizes
//==============================================================================

#define uint8 unsigned char
#define uint16 unsigned short int
#define uint32 unsigned long int

typedef uint8 byte;
typedef uint16 word;
typedef uint32 dword;

//==============================================================================
// Instruction encoding masks and values
//==============================================================================

#define ADDRESS_MASK                    0x3fffff   // mask for 22-bit address
#define OPCODE_CLASS_MASK               0xfffffff0 // unmakes bits 5 to 31 in the instruction encoding where the opcode is located
#define OPCODE_CLASS_NORMALIZER         0xf  // 0xf = 1111b: used to normalize the opcode class value to its actual value
#define OPERAND_SIZE_CLASS_MASK         12  // 12 = 1100b: unmasks bits 2 and 3 in the instruction encoding where opcode size is located
#define OPERAND_SIZE_CLASS_NORMALIZER   3 // 3 = 0011b: used to normalize the operand-size class  to its actual value
#define ADDRESSING_MODE_CLASS_MASK      2 // 2 = 0001b: unmasks bit 1 in the instruction encoding where the addressing mode of the instruction is located

#define ADDRESSING_MODE_IMMEDIATE 0
#define ADDRESSING_MODE_IMMEDIATE 0

#define OPERAND_SIZE_BYTE  1
#define OPERAND_SIZE_WORD  2
#define OPERAND_SIZE_DWORD 3

//==============================================================================
// Memory
//==============================================================================

// Defs

#define BYTE                     1
#define WORD                     (2 * BYTE)
#define DWORD                    (4 * BYTE)
#define KB                       (1024 * BYTE)
#define MB                       (KB * KB)
#define MEMORY_SIZE              (4 * MB)
#define MEMORY_MIN_ADDRESS       0
#define MEMORY_MAX_ADDRESS       (MEMORY_SIZE - 1)
#define SEGMENT_COUNT            4 // number of segments in memory
#define SEGMENT_SIZE             (MEMORY_SIZE / SEGMENT_COUNT)
#define CODE_SEGMENT_START       MEMORY_MIN_ADDRESS
#define CODE_SEGMENT_END         (CODE_SEGMENT_START + (SEGMENT_SIZE - 1))
#define CODE_SEGMENT_SIZE        SEGMENT_SIZE
#define HEAP_SEGMENT_START       (CODE_SEGMENT_END + 1)
#define HEAP_SEGMENT_END         (HEAP_SEGMENT_START + (SEGMENT_SIZE - 1))
#define HEAP_SEGMENT_SIZE        SEGMENT_SIZE
#define EXPR_STACK_SEGMENT_START (HEAP_SEGMENT_END + 1)
#define EXPR_STACK_SEGMENT_END   (EXPR_STACK_SEGMENT_START + (SEGMENT_SIZE - 1))
#define EXPR_STACK_SEGMENT_SIZE  SEGMENT_SIZE
#define CALL_STACK_SEGMENT_START (EXPR_STACK_SEGMENT_END + 1)
#define CALL_STACK_SEGMENT_END   (CALL_STACK_SEGMENT_START + (SEGMENT_SIZE - 1))
#define CALL_STACK_SEGMENT_SIZE  SEGMENT_SIZE

// Memory data structure

static char mem[MEMORY_SIZE];

//==============================================================================
// CPU FSM
//==============================================================================

// CPU FSM states

typedef enum CpuState {
    // instructions
    I_NOP = OC_NOP,
    I_HALT = OC_HALT,

    // CPU fetch-decode-execute sequence
    S_INIT,
    S_FETCH,
    S_DECODE,
    S_COUNTER_INCREMENT,

    // error
    E_UNKNOWN_INSTRUCTION
} CpuState;

// Error

#define CPUERR_MEMORY_WRITE_UNDERFLOW 1

// Prototypes

static int run();
static void mem_writeb(dword, byte);
static byte mem_readb(dword);
static void mem_writew(dword, word);
static word mem_readw(dword);
static void mem_writed(dword, dword);
static dword mem_readd(dword);
/*
static void es_pushb();
static byte es_pullb();
static es_pushw();
static es_pullw();
static es_pushd();
static es_pulld();
static toes_setb(byte);
static byte toes_getb();
static toes_setw(word);
static word toes_getw();
static toes_setd(dword);
static dword toes_getd();*/

// Status flags

static bool sz; // zero flag
static bool sn; // negative flag
static bool sv; // overflow flag
static bool sc; // carry flag

// Internal registers

static dword pc;  // program counter
static dword cir; // current instruction register
static dword mar; // memory address register - stores the address for a memory operation
static dword mdr; // memory data register - stores the data for a memory operation
static dword ep;  // expression stack pointer
static dword cp;  // call stack pointer`
static dword fp;  // frame pointer
static dword t0;  // temporary register 0
static dword t1;  // temporary register 1
static dword t2;  // temporary register 2

//==============================================================================
// Entry point
//==============================================================================

void execute(File *file)
{
    long int filesize;
    size_t bytes_read;

    filesize = file_size(file);
    if (filesize > CODE_SEGMENT_SIZE) {
        fail("Object file size (%dB) exceeds VM memory size of %dB", filesize, MEMORY_SIZE);
    }

    bytes_read = fread((char*)mem, 1, filesize, file->handle);
    if (bytes_read != filesize) {
        fail("Unable to read entire file. Read %d bytes of file containing %d bytes", bytes_read, filesize);
    }
    file_close(file);

    run();
}

static int run()
{
    CpuState next_state;
    CpuState current_state;
    bool done;

    uint32 opcode_class = 0; // stores opcode class
    uint32 oprsize_class = 0; // stores operand size class
    uint32 addrmode_class = 0; // stores addressing mode class

    done = false;
    next_state = S_INIT;
    while (!done) {
        current_state = next_state;
        switch (current_state) {

            // Fetch-Decode sequence

            case S_INIT:
                // the init state initializes the machine
                pc  = CODE_SEGMENT_START;
                sz  = false;
                sn  = false;
                sv  = false;
                sc  = false;
                t0  = 0;
                t1  = 0;
                t2  = 0;
                cir = 0;
                mar = 0x000000;
                mdr = 0;
                ep  = EXPR_STACK_SEGMENT_END;
                cp  = CODE_SEGMENT_END;
                fp  = 0x000000;
                next_state = S_FETCH;
                break;

            case S_FETCH:
                // the fetch state retrieves the next instruction from memory
                // The PC always points to the next instruction.

                // get instruction and store it in the CIR
                cir = mem_readw(pc);
                next_state = S_COUNTER_INCREMENT;
                break;

            case S_COUNTER_INCREMENT:
                // the counter-increment state increments the PC too the next instruction

                // go to next instruction. all instructions are a word wide
                pc += WORD;
                next_state = S_DECODE;
                break;

            case S_DECODE:
                // the decode state decodes the instruction in the CIR

                // because an instruction encoding only contains the opcode,
                // we can jump straight to an instruction without actually
                // decoding. We let each instruction handle its own decoding

                // get opcode from the CIR
                opcode_class = cir & OPCODE_CLASS_MASK;
                opcode_class >>= OPCODE_CLASS_NORMALIZER;

                // the operand size determines whether an instruction has an
                // operand or not. So first we check the operand size to see if
                // the instruction has an operand.
                oprsize_class = cir & OPERAND_SIZE_CLASS_MASK;
                oprsize_class >>= OPERAND_SIZE_CLASS_NORMALIZER;

                // the addressing mode tells us how to retrieve the operand
                // and tells use more importantly how interpret the
                // immediate field. If the addressing mode is 'immediate'
                // then it means that value is to be passed. If the
                // addressing mode is 'direct' then it means that the
                // immediate field stores the address of where the operand
                // is located in memory
                addrmode_class = cir & ADDRESSING_MODE_CLASS_MASK;

                // if this instruction supports operands then lets retrieve the operand
                if (oprsize_class != 0) {
                    // handle immediate addressing
                    if (addrmode_class == 0) {
                        // if operand is byte then
                        if (oprsize_class == OPERAND_SIZE_BYTE) {
                            mdr = mem_readb(pc); // get byte operand from immediate field
                            pc += BYTE; // goto next instruction
                        }
                        // if operand is word then
                        if (oprsize_class == OPERAND_SIZE_WORD) {
                            mdr = mem_readw(pc); // get word operand from immediate field
                            pc += WORD; // goto next instruction
                        }
                        // if operand is dword then
                        if (oprsize_class == OPERAND_SIZE_DWORD) {
                            mdr = mem_readd(pc); // get dword operand from immediate field
                            pc += DWORD; // goto next instruction
                        }
                    }
                    // handle direct addressing
                    else {
                        // if operand is byte then
                        if (oprsize_class == OPERAND_SIZE_BYTE) {
                            mar = mem_readd(pc); // we need the address of operand
                            mdr = mem_readb(mar); // we need the byte operand to pass to the operation
                            pc += DWORD; // advance the PC by a dword because the immediate feild stores a dword-sized address
                        }
                        // if operand is word then
                        if (oprsize_class == OPERAND_SIZE_WORD) {
                            mar = mem_readd(pc);
                            mdr = mem_readw(mar);
                            pc += DWORD;
                        }
                        // if operand is dword then
                        if (oprsize_class == OPERAND_SIZE_DWORD) {
                            mar = mem_readd(pc);
                            mdr = mem_readd(mar);
                            pc += DWORD;
                        }
                    }
                }

                // go execute the operation
                next_state = opcode_class;
                break;

            // Expression stack instructions
            /*
            case I_PUSHB:
                es_pushb(mdr);
                break;

            case I_PUSHW:
                break;

            case I_PUSHD:
                break;

            case I_POPB:
                break;

            case I_POPW:
                break;

            case I_POPD:
                break;

            case I_DUPB:
                break;

            case I_DUPW:
                break;

            case I_DUPD:
                break;

            case I_OVERB:
                break;

            case I_OVERW:
                break;

            case I_OVERD:
                break;

            case I_SWAPB:
                break;

            case I_SWAPW:
                break;

            case I_SWAPD:
                break;

            case I_ROLLB:
                break;

            case I_ROTCB:
                break;

            case I_PUSHEP:
                break;
                break;

            case I_PULLEP:
                break;

            case I_PUSHCP:
                break;

            case I_PULLCP:
                break;

            case I_PUSHFP:
                break;

            case I_PULLFP:
                break;

            // Memory instruction

            case I_LODB:
                break;
                break;

            case I_LODW:
                break;
                break;

            case I_LODD:
                break;
                break;

            case I_PULLB:
                break;
                break;

            case I_PULLW:
                break;
                break;

            case I_PULLD:
                break;

            case I_PEEKB:
                break;
                break;

            case I_PEEKW:
                break;
                break;

            case I_PEEKD:
                break;

            // Jump instructions

            case I_JMP:
                break;

            case I_JZ:
                break;

            case I_JNZ:
                break;

            case I_JE:
                break;

            case I_JNE:
                break;

            case I_CALL:
                break;

            case I_RET:
                break;
                */

            case I_NOP:
                printf("\n\nIF YOU SEE THIS THEN THE VM IS WORK\n\n");
                break;

            case I_HALT:
                done = true;
                break;

            case E_UNKNOWN_INSTRUCTION:
                error("vm: unknown instruction (%x)", cir);
                break;
        }
    }
}

//==============================================================================
// Memory operations
//==============================================================================

/**
 * Writes a byte of data to memory
 *
 * byte  data: The data to write
 * dword addr: The address to write to
 *
 * Returns nothing
 */
static void mem_writeb(dword addr, byte data)
{
    if (addr > MEMORY_MAX_ADDRESS) {
        fail("Overflow on memory");
    }
    else if (addr < MEMORY_MIN_ADDRESS) {
        fail("Underflow on memory");
    }

    // write the byte to memory
    mem[addr] = data;
}

/**
 * Reads a byte of data from memory and returns it
 */
static byte mem_readb(dword addr)
{
    if (addr > MEMORY_MAX_ADDRESS) {
        fail("Overflow on memory");
    }
    else if (addr < MEMORY_MIN_ADDRESS) {
        fail("Underflow on memory");
    }

    // read the byte from memory
    return mem[addr];
}

static void mem_writew(dword addr, word data)
{
    if (addr > MEMORY_MAX_ADDRESS) {
        fail("Overflow on memory");
    }
    else if (addr < MEMORY_MIN_ADDRESS) {
        fail("Underflow on memory");
    }

    // some vars
    byte hb; // stores low-order byte
    byte lb; // stores high-order byte

    // get the individual bytes from the given data
    hb = data & 0xf0; // extract high-order byte
    lb = data & 0x0f; // extract low-order byte

    // write the bytes to memory in big endian
    mem[addr]   = hb;
    mem[addr+1] = lb;
}

static word mem_readw(dword addr)
{
    if (addr > MEMORY_MAX_ADDRESS) {
        fail("Overflow on memory");
    }
    else if (addr < MEMORY_MIN_ADDRESS) {
        fail("Underflow on memory");
    }

    // some vars
    word hb;   // stores low-order byte
    word lb;   // stores high-order byte
    word data; // the data to return

    // initialize the vars
    hb = lb = 0;

    // read the data memory as big endian
    hb = (word)mem[addr];
    lb = (word)mem[addr+1];

    // shift the bytes for bitwise joining
    hb = hb << 8; // move the high-order byte into the high-order-byte position.

    // join the bytes into a word
    data = hb | lb;

    // return it
    return data;
}

static void mem_writed(dword addr, dword data)
{
    if (addr > MEMORY_MAX_ADDRESS) {
        fail("Overflow on memory");
    }
    else if (addr < MEMORY_MIN_ADDRESS) {
        fail("Underflow on memory");
    }

    // some vars
    byte hwhb; // stores high-order byte at the high-order word
    byte hwlb; // stores low-order byte at the high-order word
    byte lwhb; // stores high-order byte at the low-order word
    byte lwlb; // stores low-order byte at the low-order word

    // get individual bytes from the given data
    hwhb = data & 0xf000; // extract high-order byte at high-order word
    hwlb = data & 0x0f00; // extract low-order byte at high-order word
    lwhb = data & 0x00f0; // extract high-order byte at low-order word
    lwlb = data & 0x000f; // extract low-order byte at low-order word

    // write the bytes to memory in big endian
    mem[addr]   = hwhb;
    mem[addr+1] = hwlb;
    mem[addr+2] = lwhb;
    mem[addr+3] = lwlb;
}

static dword mem_readd(dword addr)
{
    if (addr > MEMORY_MAX_ADDRESS) {
        fail("Overflow on memory");
    }
    else if (addr < MEMORY_MIN_ADDRESS) {
        fail("Underflow on memory");
    }

    // some vars
    dword hwhb; // stores high-order byte at the high-order word
    dword hwlb; // stores low-order byte at the high-order word
    dword lwhb; // stores high-order byte at the low-order word
    dword lwlb; // stores low-order byte at the low-order word
    dword data; // the data to return

    // initialize bytes
    hwhb = hwlb = lwhb = lwlb = 0;

    // read the bytes from memory in big endian
    hwhb = (dword)mem[addr];   // get the high-order byte at the high-order word
    hwlb = (dword)mem[addr+1]; // get the low-order byte at the high-order word
    lwhb = (dword)mem[addr+2]; // get the high-order byte at the low-order word
    lwlb = (dword)mem[addr+3]; // get the low-order byte at the low-order word

    // shift the bytes for bitwise joining. `lwlb`1 is missing because it
    // already sits at its correct order position and does not need to be shifted
    hwhb = hwhb << 24; // move the HWHB into the HWHB position
    hwlb = hwlb << 16; // move the HWLB into the HWLB position
    lwhb = lwhb << 8;  // move the LWHB into LWHB position

    // join the bytes into a dword
    data = hwhb | hwlb | lwhb | lwlb;

    // return it
    return data;
}

//==============================================================================
// Expression stack operations
//==============================================================================

/*
static void es_pushb()
{

}

static byte es_pullb()
{

}

static es_pushw()
{

}

static es_pullw()
{

}

static es_pushd()
{

}

static es_pulld()
{

}

static toes_setb(byte data)
{

}

static byte toes_getb()
{

}

static toes_setw(word data)
{

}

static word toes_getw()
{

}

static toes_setd(dword data)
{

}

static dword toes_getd()
{

}*/
