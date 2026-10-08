#include "chip8.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INSTRUCTIONS_PER_FRAME 12

void init_system(struct system_chip8 *chip8){
    srand(time(NULL));
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
    // Verify that this number of bits does not exceed my available space (PC_END - PC_INIT)
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

uint16_t fetch_instruction(struct system_chip8 *chip8){
    uint16_t instr = (chip8->RAM[chip8->PC] << 8) | chip8->RAM[chip8->PC+1];
    chip8->PC = chip8->PC+2;
    return instr; 
}


void decode_instruction(struct system_chip8 *chip8){
    Display display = init_display();
    while (true){
        init_frame(display);
        if(!read_events(display, chip8->keyboard)){
            break;
        }
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            uint16_t instr = fetch_instruction(chip8);
            uint8_t nibble =  (instr >> 12) & 0x000F;
            switch(nibble){
                case 0x0:
                    exec_0XXX(chip8, instr);
                    break;
                
                case 0x1:
                    exec_1NNN(chip8, instr);
                    break;
                
                case 0x2:
                    exec_2NNN(chip8, instr);
                    break;

                case 0x3:
                    exec_3XKK(chip8, instr);
                    break;
                
                case 0x4:
                    exec_4XKK(chip8, instr);
                    break;

                case 0x5:
                    exec_5XY0(chip8, instr);
                    break;

                case 0x6:
                    exec_6XKK(chip8, instr);
                    break;

                case 0x7:
                    exec_7XKK(chip8, instr);
                    break;
                
                case 0x8:
                    exec_8XXX(chip8, instr);
                    break;

                case 0x9:
                    exec_9XY0(chip8, instr);
                    break;

                case 0xA:
                    exec_ANNN(chip8, instr);
                    break;

                case 0xB:
                    exec_BNNN(chip8, instr);
                    break;
                
                case 0xC:
                    exec_CXKK(chip8, instr);
                    break;

                case 0xD: {
                    exec_DXYN(chip8, instr);   
                    break;
                }

                case 0xE: {
                    exec_EXXX(chip8, instr);
                    break;
                }
                case 0xF:
                    exec_FXXX(chip8, instr);
                    break;

                default:
                    break;
            } 
        }
        update_timers(&chip8->DT, &chip8->ST);
        render_frame(chip8->framebuffer, display);
        sync_frames(display);
    }
    destroy_display(display);
}


int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Error: A required parameter is missing.\n");
        return EXIT_FAILURE;
    }
    struct system_chip8 chip8;
    init_system(&chip8);
    uint8_t exit_load = load_rom(&chip8, argv[1]); 
    if (exit_load){
        return EXIT_FAILURE;
    }
    decode_instruction(&chip8);
    return EXIT_SUCCESS;
}   