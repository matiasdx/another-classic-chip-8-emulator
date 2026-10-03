#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SCALE 10

typedef struct display* Display;

Display init_display(void);

void destroy_display(Display display);

bool read_events(Display);

void render_frame(uint8_t framebuffer[32][64], Display display);

void delay_display(int ms);