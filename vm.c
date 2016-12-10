// Help, please!

#include <stdio.h>
#include "vm.h"
#include "opcode.h"

void vm_run(void *obj_ptr, sizt_t obj_size)
{
    vm_init(obj_ptr, obj_size); // initialize the machine
    vm_cycle(); // start instruction cycle
}

void vm_init(void *obj, obj_size)
{
    mem = (char)emalloc(sizeof(char)*MEM_SIZE); // create memory
    memcpy(mem, obj_ptr, obj_size); // load object code into memory
}

int vm_cycle()
{
    int next_state;
    int current_state;

    next_state = S_INIT;
    while (true) {
        current_state = next_state;
        switch (current_state) {
            // instruction cycle
            case S_INIT:
                cir = 0;
                mar = 0;
                mdr = 0;
                esp = 0;
                csp = 0;
                fp = 0;
                pc = CODE_START;
                break;
            case S_FETCH:
                mar = pc;
                mdr = vm_readb();
                cir = mdr;
                next_state = cir;
                break;
            // stack operations
            case O_PUSHBI:
                break;
            case O_PUSHWI:
                break;
            case O_PUSHDI:
                break;
            case O_PUSHSP:
                break;
            case O_PUSHFP:
                break;
            case O_POPBI:
                break;
            case O_POPWI:
                break;
            case O_POPDI:
                break;
            case O_DUPBI:
                break;
            case O_DUPWI:
                break;
            case O_DUPDI:
                break;
            case O_OVERBI:
                break;
            case O_OVERWI:
                break;
            case O_OVERDI:
                break;
            case O_SWAPBI:
                break;
            case O_SWAPWI:
                break;
            case O_SWAPDI:
                break;
            case O_ROLLBI:
                break;
            case O_ROLLWI:
                break;
            case O_ROLLDI:
                break;
            case O_ROTBI:
                break;
            case O_ROTWI:
                break;
            case O_ROTDI:
                break;
            case O_ROTCBI:
                break;
            case O_ROTCWI:
                break;
            case O_ROTCDI:
                break;
            // memory operations
            case O_LOADBI:
                break;
            case O_LOADWI:
                break;
            case O_LOADDI:
                break;
            case O_PULLBI:
                break;
            case O_PULLWI:
                break;
            case O_PULLDI:
                break;
            // jump operations
            case O_JMP:
                break;
            case O_JZ:
                break;
            case O_JNZ:
                break;
            case O_JE:
                break;
            case O_JNE:
                break;
            case O_CALL:
                break;
            case O_RET:
                break;
            // math operations
            case O_ADDBI:
                break;
            case O_ADDWI:
                break;
            case O_ADDDI:
                break;
            case O_SUBBI:
                break;
            case O_SUBWI:
                break;
            case O_SUBDI:
                break;
            case O_MULBI:
                break;
            case O_MULWI:
                break;
            case O_MULDI:
                break;
            case O_DIVBI:
                break;
            case O_DIVWI:
                break;
            case O_DIVDI:
                break;
            case O_MODBI:
                break;
            case O_MODWI:
                break;
            case O_MODDI:
                break;
            // bitwise operations
            case O_ANDBI:
                break;
            case O_ANDWI:
                break;
            case O_ANDDI:
                break;
            case O_ORBI:
                break;
            case O_ORWI:
                break;
            case O_ORDI:
                break;
            case O_XORBI:
                break;
            case O_XORWI:
                break;
            case O_XORDI:
                break;
            case O_NOTBI:
                break;
            case O_NOTWI:
                break;
            case O_NOTDI:
                break;
            // machine operations
            case O_NOP:
                break;
            case O_HALT:
                break;
            // default
            default:
                // use for errors
                fail("Illegal instruction");
                break;
        }
    }
}


byte vm_readb()
{
    mdr = 0;
    mdr = mem[mar];
}

void vm_writeb()
{
    mem[mar] = mdr;
}

word vm_readw()
{
    mdr = 0;
}

void vm_writew()
{
    mdr = 0;
}

dword vm_readd()
{
    mdr = 0;
}

void vm_writed()
{
    mdr = 0;
}

dword vm_putbd(dword target, byte b, long int mask)
{

}
