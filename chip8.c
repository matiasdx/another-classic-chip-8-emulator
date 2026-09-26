#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

struct system_chip8 chip8;

void init_system(struct system_chip8 *chip8){
     
    memset(chip8, 0, sizeof(*chip8));

    chip8->PC = PC_INIT;

    for (uint16_t addr = 0; addr < 80; addr++){
        chip8->RAM[addr] = chip8_fontset[addr];
    }     
}

uint8_t load_rom(struct system_chip8 *chip8, char *rom_dir){
    FILE *rom = fopen(rom_dir, "rb");
    if (rom == NULL){
        perror("Error to open rom");
        return EXIT_FAILURE;
    }
    //// Verify that this number of bits does not exceed my available space (PC_END - PC_INIT)
    fseek(rom, 0L, SEEK_END);
    uint16_t rom_length = ftell(rom);
    fseek(rom, 0L, SEEK_SET);
    if (rom_length > PC_END - PC_INIT){
       perror("Error rom not valid");
       return EXIT_FAILURE;
    }
    // Dump the rom in ram
    uint16_t bytes_read = fread(&chip8->RAM[PC_INIT], sizeof(char), rom_length, rom);
    if (bytes_read != rom_length){
        perror("Error in lecture of bytes");
        return EXIT_FAILURE;
    } 
    fclose(rom);
    return EXIT_SUCCESS;
}