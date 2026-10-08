chip8: chip8.c display_tools.c instructions.c
	$(CC) chip8.c display_tools.c instructions.c -o chip8 -Wall -Wextra -pedantic -std=c99 -lSDL2