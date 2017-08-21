#ifndef __PARTICLE_OPCODE_H__
#define __PARTICLE_OPCODE_H__

// Opcodes

// Stack (00-2F)
#define OC_PUSHBI    0x01
#define OC_PUSHWI    0x02
#define OC_PUSHDI    0x03
#define OC_PUSHSP    0x04
#define OC_PUSHFP    0x05
#define OC_POPBI     0x06
#define OC_POPWI     0x07
#define OC_POPDI     0x08
#define OC_DUPBI     0x09
#define OC_DUPWI     0x0a
#define OC_DUPDI     0x0b
#define OC_OVERBI    0x0c
#define OC_OVERWI    0x0d
#define OC_OVERDI    0x0e
#define OC_SWAPBI    0x0f
#define OC_SWAPWI    0x10
#define OC_SWAPDI    0x11
#define OC_ROLLBI    0x12
#define OC_ROLLWI    0x13
#define OC_ROLLDI    0x14
#define OC_ROTBI     0x15
#define OC_ROTWI     0x16
#define OC_ROTDI     0x17
#define OC_ROTCBI    0x18
#define OC_ROTCWI    0x19
#define OC_ROTCDI    0x1a

// Memory (30-3F)
#define OC_LOADBI    0x30
#define OC_LOADWI    0x31
#define OC_LOADDI    0x32
#define OC_PULLBI    0x33
#define OC_PULLWI    0x34
#define OC_PULLDI    0x35

// Jumps (40-5F)
#define OC_JMP       0x40
#define OC_JZ        0x41
#define OC_JNZ       0x42
#define OC_JE        0x43
#define OC_JNE       0x44
#define OC_CALL      0x45
#define OC_RET       0x46

// Math (60-7F)
#define OC_ADDBI     0x60
#define OC_ADDWI     0x61
#define OC_ADDDI     0x62
#define OC_SUBBI     0x63
#define OC_SUBWI     0x64
#define OC_SUBDI     0x65
#define OC_MULBI     0x66
#define OC_MULWI     0x67
#define OC_MULDI     0x68
#define OC_DIVBI     0x69
#define OC_DIVWI     0x6a
#define OC_DIVDI     0x6b
#define OC_MODBI     0x6c
#define OC_MODWI     0x6d
#define OC_MODDI     0x6e

// Bitwise (80-9F)
#define OC_ANDBI     0x80
#define OC_ANDWI     0x81
#define OC_ANDDI     0x82
#define OC_ORBI      0x83
#define OC_ORWI      0x84
#define OC_ORDI      0x85
#define OC_XORBI     0x86
#define OC_XORWI     0x87
#define OC_XORDI     0x88
#define OC_NOTBI     0x89
#define OC_NOTWI     0x8a
#define OC_NOTDI     0x8b
#define OC_SHLBI     0x8c
#define OC_SHLWI     0x8d
#define OC_SHLDI     0x8e
#define OC_SHRBI     0x8f
#define OC_SHRWI     0x90
#define OC_SHRDI     0x91

// Machine (special)
#define OC_NOP       0xFF
#define OC_HALT      0x00

#endif /* __PARTICLE_OPCODE_H__ */
