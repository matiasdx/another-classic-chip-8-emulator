#include <SDL2/SDL.h>
#include "display_tools.h"
#include <stdlib.h>


struct display {
    SDL_Window* window;
    SDL_Renderer* render;
};

Display init_display(void){
    Display new_display = malloc(sizeof(struct display));   
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

bool read_events(Display display){
    (void)display;
    //Handle events on queue
    SDL_Event event; 
    while (SDL_PollEvent(&event) != 0){
        //User requests quit
                    if(event.type == SDL_QUIT)
                    {
                        return false;
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