#include "chip8.h"
#include <string.h>
#include <stdlib.h>


void exec_DXYN(struct system_chip8 *chip8, uint16_t instr){
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
}

void exec_ANNN(struct system_chip8 *chip8, uint16_t instr){
    chip8->I = instr & 0x0FFF;
}

void exec_7XKK(struct system_chip8 *chip8, uint16_t instr){
    chip8->V[(instr & 0x0F00) >> 8] = chip8->V[(instr & 0x0F00) >> 8] + (instr & 0x00FF);
}

void exec_6XKK(struct system_chip8 *chip8, uint16_t instr){
    chip8->V[(instr & 0x0F00) >> 8] = instr & 0x00FF;
}

void exec_1NNN(struct system_chip8 *chip8, uint16_t instr){
    chip8->PC = instr & 0x0FFF;
}

void exec_0XXX(struct system_chip8 *chip8, uint16_t instr){
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
}

void exec_3XKK(struct system_chip8 *chip8, uint16_t instr){
    if (chip8->V[(0x0F00 & instr) >> 8] == (0x00FF & instr)){
        chip8->PC = chip8->PC + 2;
    }
}

void exec_4XKK(struct system_chip8 *chip8, uint16_t instr){
    if (chip8->V[(0x0F00 & instr) >> 8] != (0x00FF & instr)){
        chip8->PC = chip8->PC + 2;
    }
}

void exec_5XY0(struct system_chip8 *chip8, uint16_t instr){
    if (chip8->V[(0x0F00 & instr) >> 8] == chip8->V[(0x00F0 & instr) >> 4]){
        chip8->PC = chip8->PC + 2;
    }
}

void exec_9XY0(struct system_chip8 *chip8, uint16_t instr){
    if (chip8->V[(0x0F00 & instr) >> 8] != chip8->V[(0x00F0 & instr) >> 4]){
        chip8->PC = chip8->PC + 2;
    }
}

void exec_2NNN(struct system_chip8 *chip8, uint16_t instr){
    chip8->stack[chip8->SP] = chip8->PC;
    chip8->SP = chip8->SP + 1;
    if (chip8->SP >= 16){
        perror("Stack overflow");
        return ;
    }
    chip8->PC = instr & 0x0FFF;
}

void exec_FX55(struct system_chip8 *chip8, uint16_t instr){
    uint16_t X = ((0x0F00 & instr) >> 8);
    for (uint16_t i = 0; i < X; i++){
        if (chip8->I + i < MEMORY_LENGTH){
            chip8->RAM[chip8->I + i] = chip8->V[i];
        }
    }
}

void exec_FX65(struct system_chip8 *chip8, uint16_t instr){
    uint16_t X = ((0x0F00 & instr) >> 8);
    for (uint16_t i = 0; i < X; i++){
        if (chip8->I + i < MEMORY_LENGTH){
            chip8->V[i] = chip8->RAM[chip8->I + i];
        }
    }
}


void exec_8XY0(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    chip8->V[X] = chip8->V[Y];
}

void exec_8XY1(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    chip8->V[X] = chip8->V[Y] | chip8->V[X];
}

void exec_8XY2(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    chip8->V[X] = chip8->V[Y] & chip8->V[X];
}

void exec_8XY3(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    chip8->V[X] = chip8->V[Y] ^ chip8->V[X];
}

void exec_8XY4(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    uint16_t temp = chip8->V[Y] + chip8->V[X];
    uint8_t carry = temp > 255;
    chip8->V[X] = (uint8_t)(temp & 0x00FF);
    chip8->V[0xF] = carry;
}

void exec_8XY5(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    uint8_t not_borrow = (chip8->V[X] >= chip8->V[Y]);
    chip8->V[X] = chip8->V[X] - chip8->V[Y];
    chip8->V[0xF] = not_borrow;
}

void exec_8XY6(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t LSB_VX = chip8->V[X] & 0x0001;
    chip8->V[X] = chip8->V[X] >> 1;
    chip8->V[0xF] = LSB_VX;
}

void exec_8XY7(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t Y = (instr & 0x00F0) >> 4;
    uint8_t not_borrow = (chip8->V[Y] >= chip8->V[X]);
    chip8->V[X] = chip8->V[Y] - chip8->V[X];
    chip8->V[0xF] = not_borrow;
}

void exec_8XYE(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t MSB_VX = (chip8->V[X] & 0x0080) >> 7;
    chip8->V[X] = chip8->V[X] << 1;
    chip8->V[0xF] = MSB_VX;
}

void exec_EX9E(struct system_chip8 *chip8, uint16_t instr){
    uint8_t key = (instr & 0x0F00) >> 8;
    chip8->PC = chip8->keyboard[chip8->V[key]] ? chip8->PC + 2 : chip8->PC;
}   

void exec_EXA1(struct system_chip8 *chip8, uint16_t instr){
    uint8_t key = (instr & 0x0F00) >> 8;
    chip8->PC = !chip8->keyboard[chip8->V[key]] ? chip8->PC + 2 : chip8->PC;
}

void exec_FX07(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    chip8->V[X] = chip8->DT;
}

void exec_FX15(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    chip8->DT = chip8->V[X];
}

void exec_FX18(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    chip8->ST = chip8->V[X];
}

void exec_FX33(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    chip8->RAM[chip8->I] = chip8->V[X] / 100;
    chip8->RAM[chip8->I + 1] = (chip8->V[X] / 10) % 10;
    chip8->RAM[chip8->I + 2] = chip8->V[X] % 10;
}


void exec_FX1E(struct system_chip8 *chip8, uint16_t instr){
       uint8_t X = (instr & 0x0F00) >> 8;
       chip8->I = chip8->I + chip8->V[X];
}

void exec_FX29(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    chip8->I = chip8->V[X] * 5; 
}

void exec_FX0A(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    bool key_pressed = false;

    for (uint8_t key = 0; key < 16; key++){
        if (chip8->keyboard[key]){
            chip8->V[X] = key;
            key_pressed = true;
            break;
        }
    }

    if (!key_pressed){
        chip8->PC = chip8->PC - 2;
    }
}

void exec_FXXX(struct system_chip8 *chip8, uint16_t instr){
    uint8_t op = instr & 0x00FF;
    switch(op){
        case(0x0A):
            exec_FX0A(chip8, instr);
            break;
        
        case(0x1E):
            exec_FX1E(chip8, instr);
            break;

        case(0x29):
            exec_FX29(chip8, instr);
            break;


        case(0x55):
            exec_FX55(chip8, instr);
            break;
        
        case(0x65):
            exec_FX65(chip8, instr);
            break;
        
        case(0x07):
            exec_FX07(chip8, instr);
            break;
        
        case(0x15):
            exec_FX15(chip8, instr);
            break;
        
        case(0x18):
            exec_FX18(chip8, instr);
            break;

        case(0x33):
            exec_FX33(chip8, instr);
            break;

        default:
            break;
    }
}

void exec_8XXX(struct system_chip8 *chip8, uint16_t instr){
    uint8_t op = instr & 0x000F; 
    switch(op){
        case(0x0):
            exec_8XY0(chip8, instr);
            break;
        case(0x1):
            exec_8XY1(chip8, instr);
            break;
        case(0x2):
            exec_8XY2(chip8, instr);
            break;
        case(0x3):
            exec_8XY3(chip8, instr);
            break;
        case(0x4):
            exec_8XY4(chip8, instr);
            break;
        case(0x5):
            exec_8XY5(chip8, instr);
            break;
        case(0x6):
            exec_8XY6(chip8, instr);
            break;
        case(0x7):
            exec_8XY7(chip8, instr);
            break;
        case(0xE):
            exec_8XYE(chip8, instr);
            break;
    }
}


void exec_EXXX(struct system_chip8 *chip8, uint16_t instr){
    uint8_t op = instr & 0x00FF;
    switch(op){
        case(0x9E):
            exec_EX9E(chip8, instr);
            break;
        case(0xA1):
            exec_EXA1(chip8, instr);
            break;
    }
}

void exec_BNNN(struct system_chip8 *chip8, uint16_t instr){
    uint16_t nnn = 0x0FFF & instr;
    chip8->PC = chip8->V[0] + nnn;
}

void exec_CXKK(struct system_chip8 *chip8, uint16_t instr){
    uint8_t X = (instr & 0x0F00) >> 8;
    uint8_t kk = (instr & 0x00FF);
    chip8->V[X] = (rand() % 256) & kk;
}

