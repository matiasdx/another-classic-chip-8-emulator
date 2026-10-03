#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "display_tools.h"

const uint8_t chip8_fontset[80] = {
    // 0
        0xF0, 0x90, 0x90, 0x90, 0xF0,
    // 1
        0x20, 0x60, 0x20, 0x20, 0x70,
    // 2
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
    // 3
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
    // 4
        0x90, 0x90, 0xF0, 0x10, 0x10,
    // 5
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
    // 6
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
    // 7
        0xF0, 0x10, 0x20, 0x40, 0x40,
    // 8
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
    // 9
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
    // A
        0xF0, 0x90, 0xF0, 0x90, 0x90,
    // B
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
    // C
        0xF0, 0x80, 0x80, 0x80, 0xF0,
    // D
        0xE0, 0x90, 0x90, 0x90, 0xE0,
    // E
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
    // F
        0xF0, 0x80, 0xF0, 0x80, 0x80
};

#define PC_INIT 0x200
#define PC_END 0xFFF

#define MEMORY_LENGTH 4096

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
    // The stack pointer (SP) points to the next free slot in the stack.
    // It also represents the number of elements currently stored.
    uint8_t SP;
    // The program counter (PC) is used to store the currently executing addresing
    uint16_t PC;
    // The stack is used to store the address that the interpreter should return to when finished with a subroutine.
    uint16_t stack[16];

    // memory of 4k x 1byte, 0x000 to 0x1FF reserved to interpreter. 0x200 to 0xFFF data space.
    uint8_t RAM[MEMORY_LENGTH];
    
    // keyboard, 0-F. 1 if the i-th key is pressed, 0 otherwise
    uint8_t keyboard[16];

    // display, used a 64x32-pixel monochrome
    uint8_t framebuffer[DISPLAY_HEIGHT][DISPLAY_LENGTH];
};


// Function that prepares the system by initializing the PC, clearing RAM, and copying fontsets into RAM
void init_system(struct system_chip8 *chip8);

// Function to process bits from a ROM and load them
uint8_t load_rom(struct system_chip8 *chip8, char *rom_dir);

// Fetch the next 16-bit instruction from memory and advance the program counter.
uint16_t fetch_instruction(struct system_chip8 *chip8);

// Decode and execute the current 16-bit instruction, modifying the system state
void decode_instruction(struct system_chip8 *chip8);

