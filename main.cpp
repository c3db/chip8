#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include "chip8.h"

#define FILE_NOT_RECEIVED 1

#define WIDTH ROWS * 20
#define HEIGHT COLLUMNS * 20

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Chip8 *chip = NULL;
static std::ifstream rom;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (argc < 2)
        throw FILE_NOT_RECEIVED;

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
    chip = new Chip8();
    rom.open(argv[1], std::ios::binary);
    chip->addProgram(&rom);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void instruction0(Instruction instruction) {
    switch (instruction.first_byte) {
        case 0x00:
            switch (instruction.second_byte) {
                case 0xe0:
                    chip->cls(renderer);
                    break;
                default:
                    return;
            }
            break;
        default:
            return;
    }
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    Instruction instruction = chip->getInstruction();
    switch (instruction.first_byte >> 4) {
        case 0x0:
            instruction0(instruction);
            break;
        case 0x1:
            chip->jmp((instruction.first_byte & 0x0f) * 16 + instruction.second_byte);
            break;
        case 0x6:
            chip->setVX(instruction.first_byte & 0x0f, instruction.second_byte);
            break;
        case 0x7:
            chip->addVX(instruction.first_byte & 0x0f, instruction.second_byte);
            break;
        case 0xa:
            chip->setI(((instruction.first_byte & 0x0f) << 8) + instruction.second_byte);
            break;
        case 0xd:
            chip->draw(renderer, instruction.first_byte & 0x0f, instruction.second_byte >> 4, instruction.second_byte & 0x0f);
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
}
