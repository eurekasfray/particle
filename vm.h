    // Data sizes

#define uint8 unsigned char
#define uint16 unsigned short int
#define uint32 unsigned long int

typedef uint8 byte
typedef uint16 word;
typedef uint32 dword;

// Memory

#define KB                  (1024)                                  // what is a kilobyte?
#define MB                  (1024 * KB)                             // what is a megabyte?
#define MEM_SIZE            (4 * MB)                                // 4MB of memory!
#define MEM_SEG_COUNT       (4)                                     // number of segments in memory
#define MEM_SEG_LEN         (MEM_SIZE / MEM_SEG_COUNT)              // segments are divided into equal shares
#define HEAP_START          (0)                                     // the heap starts at address 0
#define HEAP_END            ((HEAP_START + MEM_SEG_LEN) - 1)        // address where the heap ends
#define EXPR_STACK_START    (HEAP_END + 1)                          // the expr stack falls after the heap
#define EXPR_STACK_END      ((EXPR_STACK_START + MEM_SEG_LEN) - 1)  // address where the expr stack ends
#define CALL_STACK_START    (EXPR_STACK_END + 1)                    // address where the call stack starts
#define CALL_STACK_END      ((CALL_STACK_START + MEM_SEG_LEN) - 1)  // address where the call stack ends
#define CODE_START          (CALL_STACK_END + 1)                    // address where the code segment starts
#define CODE_END            ((CALL_STACK_START + MEM_SEG_LEN) - 1)  // address where the code segment ends

// Masks

#define ADDRESS_MASK        0x3FFFFF   // mask for 22-bit address
#define BYTE1_MASK          0x000000FF
#define BYTE2_MASK          0x0000FF00
#define BYTE3_MASK          0x00FF0000
#define BYTE4_MASK          0xFF000000

// Machine states

#define S_INIT              -1
#define S_FETCH             -2
#define S_DECODE            -3
#define S_COUNTER_INCREMENT -4

// Memory

char *mem;

// Registers

word pc;            // program counter
word cir;           // current instruction register
word mar;           // memory address register
dword mdr;          // memory data register
dword t0,t1,t2;     // temp registers

word esp;           // expression-stack pointer
word csp;           // call-stack pointer
word fp;            // frame pointer
