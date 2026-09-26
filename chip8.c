#include "chip8.h"

struct system_chip8 chip8;

void init_system(struct system_chip8 *chip8){
    chip8->PC = PC_INIT;
     
    memset(chip8->RAM, 0, sizeof(chip8->RAM));

    for (uint16_t addr = 0; addr < 80; addr++){
        chip8->RAM[addr] = chip8_fontset[addr];
    }     
}