#include "chip8.h"
#include <stdlib.h>
#include <string.h>

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
        if(!read_events(display)){
            break;
        }
        uint16_t instr = fetch_instruction(chip8);
        uint8_t nibble =  (instr >> 12) & 0x000F;
        switch(nibble){
            case 0x0:
                if ((0x000F & instr) == 0){
                    memset(&chip8->framebuffer, 0, sizeof(chip8->framebuffer));
                } else if ((0x000F & instr) == 0xE){
                    chip8->SP = chip8->SP - 1;
                    if (chip8->SP > 15){ // maybe underflow 
                        perror("Segmentation fault");
                        return ;
                    }
                    chip8->PC = chip8->stack[chip8->SP];
                }
                break;
            
            //1nnn, jump: sets the program counter (pc) to nnn
            case 0x1:
                chip8->PC = instr & 0x0FFF;
                break;

            // 6xkk, Set Vx = kk: puts the value kk into register V[x]
            case 0x6:
                chip8->V[(instr & 0x0F00) >> 8] = instr & 0x00FF;
                break;

            // 7xkk, Set Vx = Vx + kk: adds to the value of register Vx, then stores result in Vx
            case 0x7:
                chip8->V[(instr & 0x0F00) >> 8] = chip8->V[(instr & 0x0F00) >> 8] + (instr & 0x00FF);
                break;
            
            // Annn, Set I = nnn: te value of register I is set to nnn
            case 0xA:
                chip8->I = instr & 0x0FFF;
                break;

            // Dxyn, Display n-byte sprite starting at memory location I at (V[x] X[y]), set VF = collision
            case 0xD: {
                uint8_t reg_X = (instr & 0x0F00) >> 8;
                uint8_t reg_Y = (instr & 0x00F0) >> 4;  
                uint8_t sprite_height = instr & 0x000F;
                uint8_t flag_sprite;  
                uint8_t temp_bit;
                chip8->V[15] = 0;
                for (uint16_t y = 0; y < sprite_height; y++){
                    flag_sprite = chip8->RAM[chip8->I + y];
                    for (uint8_t x = 0; x < 8; x++){ 
                        temp_bit = ((flag_sprite >> (7 - x)) & 1);
                        uint8_t coord_x = (chip8->V[reg_X] + x) % DISPLAY_LENGTH;
                        uint8_t coord_y = (chip8->V[reg_Y] + y) % DISPLAY_HEIGHT;
                        if (temp_bit){
                            if (chip8->framebuffer[coord_y][coord_x]){
                                chip8->V[15] = 1;
                            }
                            chip8->framebuffer[coord_y][coord_x] = chip8->framebuffer[coord_y][coord_x] ^ 1;
                        }   
                    }
                }   
                break;
            }
        } 
        render_frame(chip8->framebuffer, display);
        delay_display(2);
    }
    destroy_display(display);
}


int main(void){
    struct system_chip8 chip8;
    init_system(&chip8);
    uint8_t exit_load = load_rom(&chip8, "/home/matiasdx/Code/hobby/chip8/roms/ibm.ch8"); 
    if (exit_load){
        return EXIT_FAILURE;
    }
    decode_instruction(&chip8);
}