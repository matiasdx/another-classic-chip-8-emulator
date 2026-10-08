#include <SDL2/SDL.h>
#include "display_tools.h"
#include <stdlib.h>


struct display {
    SDL_Window* window;
    SDL_Renderer* render;
    uint32_t frame_init;
};

Display init_display(void){
    Display new_display = malloc(sizeof(struct display));   
    if (new_display == NULL){
        printf("Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    //  Initialize sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    } else {
        //  Create window
        new_display->window = SDL_CreateWindow("ohmygod", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH * DISPLAY_SCALE,
                              DISPLAY_HEIGHT * DISPLAY_SCALE, 0);
        if (new_display->window == NULL){
            printf("Display could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    new_display->render = SDL_CreateRenderer(new_display->window, -1, SDL_RENDERER_ACCELERATED);
    if (new_display->render == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    } else {
        // Initialize renderer color
        SDL_SetRenderDrawColor(new_display->render, 0x000, 0x000, 0x000, 0x000);
    }
    return new_display;
}

void destroy_display(Display display){
    if (display == NULL){
        return ;
    }
    SDL_DestroyRenderer(display->render);
    SDL_DestroyWindow(display->window);
    display->render = NULL;
    display->window = NULL;
    free(display);
    SDL_Quit();
}

bool read_events(Display display, bool keyboard[16]){
    (void)display;
    //  Handle events on queue
    SDL_Event event; 
    while (SDL_PollEvent(&event) != 0){
        //  User requests quit
                    if (event.type == SDL_QUIT) {
                        return false;
                    }
                    if (event.type == SDL_KEYDOWN){
                        switch (event.key.keysym.sym) {
                            case SDLK_1: 
                                keyboard[0x1] = true;
                                break;
                            case SDLK_2:
                                keyboard[0x2] = true;
                                break;
                            case SDLK_3:
                                keyboard[0x3] = true;
                                break;
                            case SDLK_4:
                                keyboard[0xC] = true;
                                break;
                            case SDLK_q:
                                keyboard[0x4] = true;
                                break; 
                            case SDLK_w:
                                keyboard[0x5] = true;
                                break;
                            case SDLK_e:
                                keyboard[0x6] = true;
                                break;
                            case SDLK_r:
                                keyboard[0xD] = true;
                                break;
                            case SDLK_a:
                                keyboard[0x7] = true;
                                break;
                            case SDLK_s:
                                keyboard[0x8] = true;
                                break;
                            case SDLK_d:
                                keyboard[0x9] = true;
                                break;
                            case SDLK_f:
                                keyboard[0xE] = true;
                                break;
                            case SDLK_z:
                                keyboard[0xA] = true;
                                break;
                            case SDLK_x:
                                keyboard[0x0] = true;
                                break;
                            case SDLK_c:
                                keyboard[0xB] = true;
                                break;
                            case SDLK_v:
                                keyboard[0xF] = true;
                                break;
                            }
                        } else if (event.type == SDL_KEYUP){
                            switch (event.key.keysym.sym){
                                case SDLK_1:
                                    keyboard[0x1] = false;
                                    break;
                                case SDLK_2:
                                    keyboard[0x2] = false;
                                    break;
                                case SDLK_3:
                                    keyboard[0x3] = false;
                                    break;
                                case SDLK_4:
                                    keyboard[0xC] = false;
                                    break;
                                case SDLK_q:
                                    keyboard[0x4] = false;
                                    break; 
                                case SDLK_w:
                                    keyboard[0x5] = false;
                                    break;
                                case SDLK_e:
                                    keyboard[0x6] = false;
                                    break;
                                case SDLK_r:
                                    keyboard[0xD] = false;
                                    break;
                                case SDLK_a:
                                    keyboard[0x7] = false;
                                    break;
                                case SDLK_s:
                                    keyboard[0x8] = false;
                                    break;
                                case SDLK_d:
                                    keyboard[0x9] = false;
                                    break;
                                case SDLK_f:
                                    keyboard[0xE] = false;
                                    break;
                                case SDLK_z:
                                    keyboard[0xA] = false;
                                    break;
                                case SDLK_x:
                                    keyboard[0x0] = false;
                                    break;
                                case SDLK_c:
                                    keyboard[0xB] = false;
                                    break;
                                case SDLK_v:
                                    keyboard[0xF] = false;
                                    break;
                            }
                        }
    }
    return true;
}

void render_frame(uint8_t framebuffer[32][64], Display display){
    SDL_SetRenderDrawColor(display->render, 0x00, 0x00,0x00, 0x00);        
    SDL_RenderClear(display->render);
    SDL_SetRenderDrawColor(display->render, 0xFF, 0xFF, 0xFF, 0xFF); 
    for (unsigned int y = 0; y < DISPLAY_HEIGHT; y++){
        for (unsigned int x = 0; x < DISPLAY_WIDTH; x++){
            if (framebuffer[y][x] == 1){
                SDL_Rect fill_rect = {x * DISPLAY_SCALE, y * DISPLAY_SCALE, DISPLAY_SCALE, DISPLAY_SCALE};
                SDL_RenderFillRect(display->render, &fill_rect);
            }
        }
    }
    SDL_RenderPresent(display->render);
}

void delay_display(int ms){
    SDL_Delay(ms);
}

void init_frame(Display display){
    display->frame_init = SDL_GetTicks();
}
void sync_frames(Display display){
    const uint32_t FRAME_TARGET_TIME = 16; 
    uint32_t frame_time = SDL_GetTicks() - display->frame_init;
    
    if (FRAME_TARGET_TIME > frame_time) {
        SDL_Delay(FRAME_TARGET_TIME - frame_time);
    }
}

void update_timers(uint8_t *delay_timer, uint8_t *sound_timer){
    static uint32_t last_time = 0;
    uint32_t current_time = SDL_GetTicks();

    if (current_time - last_time >= 16){
        if (*delay_timer > 0){
            (*delay_timer)--;
        }
        if (*sound_timer > 0){
            (*sound_timer)--;
        }
        last_time = current_time;
    }
}