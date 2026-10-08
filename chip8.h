#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "display_tools.h"

static const uint8_t chip8_fontset[80] = {
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
    bool keyboard[16];

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

// Instruction set

// Dxyn, Display n-byte sprite starting at memory location I at (V[x] X[y]), set VF = collision

void exec_DXYN(struct system_chip8 *chip8, uint16_t instr);

// Annn, Set I = nnn: te value of register I is set to nnn

void exec_ANNN(struct system_chip8 *chip8, uint16_t instr);

// 7xkk, Set Vx = Vx + kk: adds to the value of register Vx, then stores result in Vx

void exec_7XKK(struct system_chip8 *chip8, uint16_t instr);

// 6xkk, Set Vx = kk: puts the value kk into register V[x]

void exec_6XKK(struct system_chip8 *chip8, uint16_t instr);

// 1nnn, jump: sets the program counter (pc) to nnn

void exec_1NNN(struct system_chip8 *chip8, uint16_t instr);


void exec_0XXX(struct system_chip8 *chip8, uint16_t instr);

// 3xkk, SE Vx, byte: skip next instruction if Vx = kk

void exec_3XKK(struct system_chip8 *chip8, uint16_t instr);

// 4xkk, SNE Vx, byte: skip next instrctuion if Vx != kk

void exec_4XKK(struct system_chip8 *chip8, uint16_t instr);

// 5xy0, SE Vx, Vy: skip next instruction if Vx = Vy

void exec_5XY0(struct system_chip8 *chip8, uint16_t instr);

// 9xy0, SNE Vx, Vy: skip next instruction if Vx != Vy

void exec_9XY0(struct system_chip8 *chip8, uint16_t instr);

// 2nnn, CALL addr: call subroutine at nnn.

void exec_2NNN(struct system_chip8 *chip8, uint16_t instr);

// Fx55, LD [I], Vx: Store registers V0 through Vx in memory starting at location I

void exec_FX55(struct system_chip8 *chip8, uint16_t instr);

// Fx65, LD Vx, [I]: Read registers V0 through Vx from memory starting at location I

void exec_FX65(struct system_chip8 *chip8, uint16_t instr);

// 8xy0, LD Vx, Vy: Stores the value of register Vy in Vx

void exec_8XY0(struct system_chip8 *chip8, uint16_t instr);

// 8xy1, OR Vx, Vy: Set Vx = Vx OR Vy 

void exec_8XY1(struct system_chip8 *chip8, uint16_t instr);

// 8xy2, AND Vx, Vy: Set Vx = Vx AND Vy

void exec_8XY2(struct system_chip8 *chip8, uint16_t instr);

// 8xy3, XOR Vx, Vy: Set Vx = Vx XOR Vy

void exec_8XY3(struct system_chip8 *chip8, uint16_t instr);

//  8xy4, ADD Vx, Vy: Set Vx = Vx + Vy, Set VF = Carry.

void exec_8XY4(struct system_chip8 *chip8, uint16_t instr);

// 8xy5, SUB Vx, Vy: Set Vx = Vx - Vy, Set VF = NOT borrow

void exec_8XY5(struct system_chip8 *chip8, uint16_t instr);

// 8xy6, SHR Vx, {, Vy}: Set Vx = Vx SHR 1

void exec_8XY6(struct system_chip8 *chip8, uint16_t instr);

// 8xy7, SUBN Vx, Vy: Set Vx = Vy - Vx, Set VF = NOT borrow

void exec_8XY7(struct system_chip8 *chip8, uint16_t instr);

// 8xyE, SHL Vx {, Vy}: Set Vx = Vx SHL 1

void exec_8XYE(struct system_chip8 *chip8, uint16_t instr);

// 8XXX, Execute operations starting with the '8' nibble

void exec_8XXX(struct system_chip8 *chip8, uint16_t instr);

// Ex9E, SKP Vx: Skip next instruction if key with the value of Vx is pressed

void exec_EX9E(struct system_chip8 *chip8, uint16_t instr);

// ExA1, SKNP Vx: Skip next instruction if key with the value of Vx is not pressed

void exec_EXA1(struct system_chip8 *chip8, uint16_t instr);

// EXXX, Execute operations starting with the 'E' nibble

void exec_EXXX(struct system_chip8 *chip8, uint16_t instr);

// Fx15, LD ST, Vx: Set sound timer = Vx

void exec_FX15(struct system_chip8 *chip8, uint16_t instr);

// Fx18, LD ST, Vx: Set sound timer = Vx

void exec_FX18(struct system_chip8 *chip8, uint16_t instr);

// Fx07, LD Vx, DT: Set Vx = delay timer value

void exec_FX07(struct system_chip8 *chip8, uint16_t instr);

// Fx33, LD B, Vx: Store representation of Vx in memory locations I, I+1 and I+2.

void exec_FX33(struct system_chip8 *chip8, uint16_t instr);

// Fx13, ADD I, Vx: The values of I and Vx are added, and the result stored in I

void exec_FX1E(struct system_chip8 *chip8, uint16_t instr);

// Fx29, LD F, Vx: Set I = Location of sprite for digit Vx

void exec_FX29(struct system_chip8 *chip8, uint16_t instr);

// Fx04, Wait for a key press, store the value of the key in Vx

void exec_FX0A(struct system_chip8 *chip8, uint16_t instr);

// Jump to location nnn + V0

void exec_BNNN(struct system_chip8 *chip8, uint16_t instr);

// Cxkk - RND Vx, byte: Set Vk = random byte AND kk.

void exec_CXKK(struct system_chip8 *chip8, uint16_t instr);


// FXXX, Execute operations starting with the 'F' nibble

void exec_FXXX(struct system_chip8 *chip8, uint16_t instr);
