#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include "chip8.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetHint("SDL_HINT_MAIN_CALLBACK_RATE", "60");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Coudn't initialize sdl: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("window title", WIDTH, HEIGHT,
                                     SDL_WINDOW_BORDERLESS, &window,
                                     &renderer)) {
        SDL_Log("Coudn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    SDL_SetRenderDrawColor(renderer, 255, 0.0, 0.0, 0);

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
}
