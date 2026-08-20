#include "chip8.h"
#include "instruction.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sys/types.h>

#define PIXEL_SIZE 20

#define REGISTER_ERROR 1
#define MEMORY_ERROR 2

Chip8::Chip8() {
    pc = START_ADDRESS;
    i = 0;
    delay_timer = 0;
    sound_timer = 0;
    registers.fill(0);
    for (int j = 0; j < font.size(); j++) {
        memory[j] = font[j];
    }
};

void Chip8::cls(SDL_Renderer *renderer) {
    SDL_RenderClear(renderer);
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < ROWS; j++)
            display[i][j] = 0;
    }
}

void Chip8::jmp(uint16_t address) {
    pc = address;
}

void Chip8::set_VX(uint8_t x, uint8_t nn) {
    if (x > 15) {
        throw REGISTER_ERROR;
    }
    registers[x] = nn;
}

void Chip8::add_VX(uint8_t x, uint8_t nn) {
    if (x > 15) {
        throw REGISTER_ERROR;
    }
    registers[x] += nn;
}

void Chip8::set_I(uint16_t nnn) {
    i = nnn;
}

void Chip8::jump(uint16_t nnn) {
    pc = nnn + registers[0];
}

void Chip8::set_VX_random(uint8_t reg_x, uint8_t nn) {
    srand(time(0));
    int random = rand();
    registers[reg_x] = random & nn;
}

std::string integer_to_bit_string(uint8_t n) {
   return std::bitset<8>(n).to_string();
}

void Chip8::render(SDL_Renderer *renderer) {
    SDL_FRect rect[COLUMNS][ROWS];
    for (uint8_t x = 0; x < COLUMNS; x++) {
        for (uint8_t y = 0; y < ROWS; y++) {
            rect[x][y].w = PIXEL_SIZE;
            rect[x][y].h = PIXEL_SIZE;
            rect[x][y].x = x * PIXEL_SIZE;
            rect[x][y].y = y * PIXEL_SIZE;
            SDL_SetRenderDrawColor(renderer, 120, 0, 20, SDL_ALPHA_OPAQUE);
            if (display[x][y] == 1) {
                SDL_SetRenderDrawColor(renderer, 193, 18, 31, SDL_ALPHA_OPAQUE);
            }
            SDL_RenderFillRect(renderer, &rect[x][y]);
        }
    }
    SDL_RenderPresent(renderer);
}

void Chip8::draw(SDL_Renderer *renderer, uint8_t reg_x, uint8_t reg_y, uint8_t n) {
    if (reg_x > 15 || reg_y > 15) {
        throw REGISTER_ERROR;
    }
    uint8_t x = registers[reg_x];
    uint8_t y = registers[reg_y];
    x %= COLUMNS;
    y %= ROWS;
    uint8_t initial_x = x;
    registers[15] = 0;
    for (int i = 0; i < n; i++) {
        uint8_t sprite = memory[this->i + i];
        for (int j = 0; j < 8; j++) {
            uint8_t sprite_bit = (sprite >> (7 - j)) & 0x1;
            if(display[x][y] && sprite_bit)
                registers[15] = 1;
            display[x][y] ^= sprite_bit;
            x++;
        }
        x = initial_x;
        y++;
    }
    render(renderer);
}

SDL_Scancode Chip8::get_scancode(uint8_t x) {
    switch (x & 0xf) {
        case 0x1:
            return SDL_SCANCODE_1;
            break;
         case 0x2:
            return SDL_SCANCODE_2;
            break;
        case 0x3:
            return SDL_SCANCODE_3;
            break;
        case 0xC:
            return SDL_SCANCODE_4;
            break;
        case 0x4:
            return SDL_SCANCODE_Q;
            break;
        case 0x5:
            return SDL_SCANCODE_W;
            break;
        case 0x6:
            return SDL_SCANCODE_E;
            break;
        case 0xD:
            return SDL_SCANCODE_R;
            break;
        case 0x7:
            return SDL_SCANCODE_A;
            break;
        case 0x8:
            return SDL_SCANCODE_S;
            break;
        case 0x9:
            return SDL_SCANCODE_D;
            break;
        case 0xE:
            return SDL_SCANCODE_F;
            break;
        case 0xA:
            return SDL_SCANCODE_Z;
            break;
        case 0x0:
            return SDL_SCANCODE_X;
            break;
        case 0xB:
            return SDL_SCANCODE_C;
            break;
        case 0xF:
            return SDL_SCANCODE_V;
            break;
        default:
            return SDL_SCANCODE_UNKNOWN;
    }
}

uint8_t Chip8::get_key(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_1:
            return 0x1;
            break;
         case SDL_SCANCODE_2:
            return 0x2;
            break;
        case SDL_SCANCODE_3:
            return 0x3;
            break;
        case SDL_SCANCODE_4:
            return 0xC;
            break;
        case SDL_SCANCODE_Q:
            return 0x4;
            break;
        case SDL_SCANCODE_W:
            return 0x5;
            break;
        case SDL_SCANCODE_E:
            return 0x6;
            break;
        case SDL_SCANCODE_R:
            return 0xD;
            break;
        case SDL_SCANCODE_A:
            return 0x7;
            break;
        case SDL_SCANCODE_S:
            return 0x8;
            break;
        case SDL_SCANCODE_D:
            return 0x9;
            break;
        case SDL_SCANCODE_F:
            return 0xE;
            break;
        case SDL_SCANCODE_Z:
            return 0xA;
            break;
        case SDL_SCANCODE_X:
            return 0x0;
            break;
        case SDL_SCANCODE_C:
            return 0xB;
            break;
        case SDL_SCANCODE_V:
            return 0xF;
            break;
        default:
            return SDL_SCANCODE_UNKNOWN;
    }
}

void Chip8::is_pressed(uint8_t reg_x) {
    uint8_t x = registers[reg_x];
    const bool *key_states = SDL_GetKeyboardState(NULL);
    SDL_Scancode scancode = get_scancode(x);
    if (key_states[scancode])
        pc+=2;
}

void Chip8::is_not_pressed(uint8_t reg_x) {
    uint8_t x = registers[reg_x];
    const bool *key_states = SDL_GetKeyboardState(NULL);
    SDL_Scancode scancode = get_scancode(x);
    if (!key_states[scancode])
        pc+=2;
}

void Chip8::set_VX_to_delay(uint8_t reg_x) {
    registers[reg_x] = delay_timer;
}

void Chip8::set_VX_key_pressed(uint8_t reg_x) {
    waiting_for_up_key = true;
    bool pressed = false;
    if (key_up != -1) {
        registers[reg_x] = key_up;
        pressed = true;
    }
    if(!pressed)
        pc -= 2;
    else {
        waiting_for_up_key = false;
        key_up = -1;
    }
}

void Chip8::set_delay_to_VX(uint8_t reg_x) {
    delay_timer = registers[reg_x];
}

void Chip8::set_sound_timer_to_VX(uint8_t reg_x) {
    sound_timer = registers[reg_x];
}

void Chip8::call_subroutine(uint16_t address) {
    stack.push(pc);
    pc = address;
}

void Chip8::return_from_subroutine() {
    pc = stack.top();
    stack.pop();
}

void Chip8::skip_if_equal(uint8_t reg_x, uint8_t nn) {
    if (registers[reg_x] == nn) {
        pc += 2;
    }
}

void Chip8::skip_if_not_equal(uint8_t reg_x, uint8_t nn) {
    if (registers[reg_x] != nn) {
        pc += 2;
    }
}

void Chip8::skip_if_reg_equal(uint8_t reg_x, uint8_t reg_y) {
    if (registers[reg_x] == registers[reg_y]) {
        pc += 2;
    }
}

void Chip8::skip_if_reg_not_equal(uint8_t reg_x, uint8_t reg_y) {
    if (registers[reg_x] != registers[reg_y]) {
        pc += 2;
    }
}

void Chip8::set_VX_to_VY(uint8_t reg_x, uint8_t reg_y) {
    registers[reg_x] = registers[reg_y];
}

void Chip8::binary_or(uint8_t reg_x, uint8_t reg_y) {
    registers[reg_x] |= registers[reg_y];
}

void Chip8::binary_and(uint8_t reg_x, uint8_t reg_y) {
    registers[reg_x] &= registers[reg_y];
}

void Chip8::logical_xor(uint8_t reg_x, uint8_t reg_y) {
    registers[reg_x] ^= registers[reg_y];
}

void Chip8::add_VX_carry(uint8_t reg_x, uint8_t reg_y) {
    uint16_t result = registers[reg_x] + registers[reg_y];
    registers[reg_x] = static_cast<uint8_t>(result);
    registers[15] = 0;
    if (result > 255)
        registers[15] = 1;
}

void Chip8::subtract_VX_VY(uint8_t reg_x, uint8_t reg_y) {
    uint8_t x = registers[reg_x];
    uint8_t y = registers[reg_y];
    registers[reg_x] = x - y;
    registers[15] = 0;
    if (x >= y)
        registers[15] = 1;
}

void Chip8::shift_right(uint8_t reg_x, uint8_t reg_y) {
//AMBIGOUS
//    registers[reg_x] = registers[reg_y];
    uint8_t x = registers[reg_x];
    registers[reg_x] >>= 1;
    registers[15] = 0;
    if (x & 0x1)
        registers[15] = 1;
}

void Chip8::shift_left(uint8_t reg_x, uint8_t reg_y) {
//AMBIGOUS
//    registers[reg_x] = registers[reg_y];
    uint8_t x = registers[reg_x];
    registers[reg_x] <<= 1;
    registers[15] = 0;
    if (x & 0x80)
        registers[15] = 1;
}

void Chip8::subtract_VY_VX(uint8_t reg_x, uint8_t reg_y) {
    uint8_t x = registers[reg_x];
    uint8_t y = registers[reg_y];
    registers[reg_x] = y - x;
    registers[15] = 0;
    if (y >= x)
        registers[15] = 1;
}

void Chip8::store(uint8_t reg_x) {
    for (int i = 0; i <= reg_x; i++) {
        memory[this->i + i] = registers[i];
    }
//AMBIGOUS
//    i += reg_x + 1;
}

void Chip8::decimal_convertion(uint8_t reg_x) {
    uint8_t x = registers[reg_x];
    for (int i = 2; i >= 0; i--) {
        uint8_t value = x % 10;
        memory[this->i + i] = value;
        x = (x - value) / 10;
    }
}

void Chip8::add_I(uint8_t reg_x) {
    uint8_t x = registers[reg_x];
    i += x;
    if (i > MEMORY_SIZE)
        registers[15] = 1;
}

void Chip8::set_I_to_sprite(uint8_t reg_x) {
    uint8_t x = registers[reg_x] & 0xf;
    i = x * 5;
}

void Chip8::load(uint8_t reg_x) {
    for (int i = 0; i <= reg_x; i++) {
        registers[i] = memory[this->i + i];
    }
//AMBIGOUS
//    i += reg_x + 1;
}

void Chip8::addProgram(std::ifstream *rom) {
    uint8_t instruction;
    uint16_t iterator = PROGRAM_ADDRESS;
    while ((*rom).read(reinterpret_cast<char*>(&instruction), sizeof(char))) {
        memory.at(iterator) = instruction;
        iterator++;
    }
}

Instruction Chip8::get_instruction() {
    if (pc > MEMORY_SIZE)
        throw MEMORY_ERROR;
    Instruction instruction(memory[pc], memory[pc + 1]);
    pc += 2;
    return instruction;
}

void Chip8::decrementTimers() {
    if (delay_timer > 0)
        delay_timer--;
    if (sound_timer > 0)
        sound_timer--;
}
