#include "instruction.h"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
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

#define HEIGHT ROWS * 20
#define WIDTH COLUMNS * 20

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Chip8 chip;
static std::ifstream rom;
constexpr double ms_per_second = 1000.0 / 700.0;
constexpr double frames_per_second = 1000.0 / 60.0;
static SDL_AudioStream *stream = NULL;
static int current_sine_sample = 0;

static void SDLCALL FeedAudioStream(void *userdata, SDL_AudioStream *astream, int additional_amount, int total_amount) {
    additional_amount /= sizeof(float);
    while (additional_amount > 0) {
        float samples[128];
        const int total = SDL_min(additional_amount, SDL_arraysize(samples));
        for (int i = 0; i < total; i++) {
            const int freq = 440;
            const float phase = current_sine_sample * freq / 8000.0f;
            current_sine_sample++;
        }

        current_sine_sample %= 8000;
        SDL_PutAudioStreamData(astream, samples, total * sizeof(float));
        additional_amount -= total;
    }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (argc < 2)
        throw FILE_NOT_RECEIVED;
    SDL_AudioSpec spec;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
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
    rom.open(argv[1], std::ios::binary);
    if (!rom) {
        SDL_Log("Coudn't load file.");
        return SDL_APP_FAILURE;
    }
    chip.addProgram(&rom);
    spec.channels = 1;
    spec.format = SDL_AUDIO_F32;
    spec.freq = 8000;
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, FeedAudioStream, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void send_key_up(SDL_Event *event) {
    if(chip.waiting_for_up_key)
        chip.key_up = chip.get_key(event->key.scancode);
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_KEY_UP:
            send_key_up(event);
            break;
    }
    return SDL_APP_CONTINUE;
}

void instruction0(Instruction instruction) {
    switch (instruction.first_byte) {
        case 0x00:
            switch (instruction.second_byte) {
                case 0xe0:
                    chip.cls(renderer);
                    break;
                case 0xee:
                    chip.return_from_subroutine();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void instruction8(Instruction instruction) {
    switch (instruction.forth_nibble()) {
        case 0x0:
            chip.set_VX_to_VY(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x1:
            chip.binary_or(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x2:
            chip.binary_and(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x3:
            chip.logical_xor(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x4:
            chip.add_VX_carry(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x5:
            chip.subtract_VX_VY(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x6:
            chip.shift_right(instruction.second_nibble(), instruction.third_nibble());
            break;
         case 0x7:
            chip.subtract_VY_VX(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0xE:
            chip.shift_left(instruction.second_nibble(), instruction.third_nibble());
            break;
        default:
            break;
    }
}

void instructionE(Instruction instruction) {
    switch (instruction.third_nibble()) {
        case 0x9:
            chip.is_pressed(instruction.second_nibble());
            break;
        case 0xA:
            chip.is_not_pressed(instruction.second_nibble());
            break;
        default:
            break;
    }
}

void instructionF(Instruction instruction) {
    switch (instruction.second_byte) {
        case 0x07:
            chip.set_VX_to_delay(instruction.second_nibble());
            break;
        case 0x0A:
            chip.set_VX_key_pressed(instruction.second_nibble());
            break;
        case 0x15:
            chip.set_delay_to_VX(instruction.second_nibble());
            break;
        case 0x18:
            chip.set_sound_timer_to_VX(instruction.second_nibble());
            break;
        case 0x1E:
            chip.add_I(instruction.second_nibble());
            break;
        case 0x29:
            chip.set_I_to_sprite(instruction.second_nibble());
            break;
        case 0x33:
            chip.decimal_convertion(instruction.second_nibble());
            break;
        case 0x55:
            chip.store(instruction.second_nibble());
            break;
        case 0x65:
            chip.load(instruction.second_nibble());
            break;
        default:
            break;
    }
}

#ifdef _DEBUG
void print_instruction(Instruction instruction) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(instruction.first_byte);
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(instruction.second_byte) << '\n';
}
void print_chip_information() {
    std::cout << "PC = " << static_cast<int>(chip.pc) << std::endl;
    std::cout << "I = " << static_cast<int>(chip.i) << std::endl;
    std::cout << "Registers: ";
    for (int i = 0; i < chip.registers.size(); i++)
        std::cout << "registers[" << i << "] " << static_cast<int>(chip.registers[i]) << std::endl;
}
#endif

SDL_AppResult SDL_AppIterate(void* appstate) {
    SDL_PauseAudioStreamDevice(stream);
    uint64_t last_time, current_time;
    Instruction instruction = chip.get_instruction();
#ifdef _DEBUG
    print_instruction(instruction);
    print_chip_information();
    bool wait_for_n = true;
    SDL_Event event;
    while (wait_for_n) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                return SDL_APP_SUCCESS;
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_N) {
                std::cout << event.key.scancode << std::endl;
                wait_for_n = false;
                send_key_up(&event);
          }
        }
    }
    print_chip_information();
#endif
    static uint64_t delay_last_time = SDL_GetPerformanceCounter();
    last_time = SDL_GetPerformanceCounter();
    switch (instruction.first_nibble()) {
        case 0x0:
            instruction0(instruction);
            break;
        case 0x1:
            chip.jmp(instruction.get_address(3));
            break;
        case 0x2:
            chip.call_subroutine(instruction.get_address(3));
            break;
        case 0x3:
            chip.skip_if_equal(instruction.second_nibble(), instruction.get_address(2));
            break;
        case 0x4:
            chip.skip_if_not_equal(instruction.second_nibble(), instruction.get_address(2));
            break;
        case 0x5:
            chip.skip_if_reg_equal(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0x6:
            chip.set_VX(instruction.second_nibble(), instruction.second_byte);
            break;
        case 0x7:
            chip.add_VX(instruction.second_nibble(), instruction.second_byte);
            break;
        case 0x8:
            instruction8(instruction);
            break;
        case 0x9:
            chip.skip_if_reg_not_equal(instruction.second_nibble(), instruction.third_nibble());
            break;
        case 0xa:
            chip.set_I(instruction.get_address(3));
            break;
        case 0xb:
            chip.jump(instruction.get_address(3));
            break;
        case 0xc:
            chip.set_VX_random(instruction.second_nibble(), instruction.get_address(2));
            break;
        case 0xd:
            chip.draw(renderer, instruction.second_nibble(), instruction.third_nibble(), instruction.forth_nibble());
            break;
        case 0xe:
            instructionE(instruction);
            break;
        case 0xf:
            instructionF(instruction);
            break;
        default:
            break;
    }
    current_time = SDL_GetPerformanceCounter();
    double delay_frame_ticks = (double)((current_time - delay_last_time) * 1000) / SDL_GetPerformanceFrequency();
    if (delay_frame_ticks >= frames_per_second) {
        chip.decrementTimers();
        delay_last_time = current_time;
    }
    if (chip.sound_timer != 0) {
        SDL_ResumeAudioStreamDevice(stream);
    }
    double frame_ticks = (double)((current_time - last_time) * 1000) / SDL_GetPerformanceFrequency();
    if (frame_ticks < ms_per_second)
        SDL_Delay((Uint64)(ms_per_second - frame_ticks));
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
}
