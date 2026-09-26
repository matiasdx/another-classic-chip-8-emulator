#include <stdint.h>

#define PC_INIT 0x200
#define PC_END 0xFFF

#define DISPLAY_HEIGHT 32
#define DISPLAY_LENGTH 64

struct system_chip8 {
    // Registers for general purpose referred to as V[i], where i is a hexadecimal number (0-15)
    uint8_t V[16];
    // The V[15] register should not be used by any program, as it is used as a flag by some instructions
    //  The I register is generally used to store memory address
    uint16_t I;
    // Chip8 has two special purpuse 8 bit register, for the delay and sound timers. When these register
    // Are non-zero, they are automatically decrementd at a rate of 60hz.
    uint8_t DT;
    uint8_t ST;
    // The stack pointer (SP) is used to point to the topmost level of the stack
    uint8_t SP;
    // The program counter (PC) is used to store the currently executing addresing
    uint16_t PC;
    // The stack is used to store the address that the interpreter should return to when finished with a subroutine.
    uint16_t stack[16];

    //memory of 4k x 1byte, 0x000 to 0x1FF reserved to interpreter. 0x200 to 0xFFF data space.
    uint8_t RAM[4096];
    
    // keyboard, 0-F. 1 if the i-th key is pressed, 0 otherwise
    uint8_t keyboard[16];

    // display, used a 64x32-pixel monochrome
    uint8_t framebuffer[DISPLAY_HEIGHT][DISPLAY_LENGTH];
};