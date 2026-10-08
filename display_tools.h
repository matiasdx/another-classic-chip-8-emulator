#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SCALE 10

typedef struct display* Display;

Display init_display(void);

void destroy_display(Display display);

bool read_events(Display, bool keyboard[16]);

void render_frame(uint8_t framebuffer[32][64], Display display);

void delay_display(int ms);

void update_timers(uint8_t *delay_timer, uint8_t *sound_timer);

void init_frame(Display display);

void sync_frames(Display display);