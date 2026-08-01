#include "chip8.h"
#include <SDL3/SDL_render.h>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

#define PIXEL_SIZE 20
#define REGISTER_ERROR 1

Chip8::Chip8() {
    pc = START_ADDRESS;
    i = 0;
    delay_timer = 0;
    sound_timer = 0;
    registers.fill(0);
    for (int i = 0; i < font.size(); i++) {
        memory[i] = font[i];
    }
};

void Chip8::cls(SDL_Renderer *renderer) {
    SDL_RenderClear(renderer);
}

void Chip8::jmp(uint16_t address) {
    this->pc = address;
}

void Chip8::setVX(uint8_t x, uint8_t nn) {
    if (x > 15) {
        throw REGISTER_ERROR;
    }
    registers[x] = nn;
}

void Chip8::addVX(uint8_t x, uint8_t nn) {
     if (x > 15) {
        throw REGISTER_ERROR;
     }
     registers[x] += nn;
}

void Chip8::setI(uint8_t nnn) {
    i = nnn;
}

std::string integer_to_bit_string(uint8_t n) {
   return std::bitset<8>(n).to_string();
}

void Chip8::render(SDL_Renderer *renderer) {
    SDL_RenderClear(renderer);
    SDL_FRect rect[ROWS][COLLUMNS];
    for (uint8_t x = 0; x < ROWS; x++) {
        for (uint8_t y = 0; y < COLLUMNS; y++) {
                rect[x][y].w = PIXEL_SIZE;
                rect[x][y].h = PIXEL_SIZE;
                rect[x][y].x = x * PIXEL_SIZE;
                rect[x][y].y = y * PIXEL_SIZE;
                if (display[x][y] == 1) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 20, SDL_ALPHA_OPAQUE);
                }else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 20, SDL_ALPHA_OPAQUE);

            SDL_RenderFillRect(renderer, &rect[x][y]);
        }
    }
}

void Chip8::draw(SDL_Renderer *renderer, uint8_t reg_x, uint8_t reg_y, uint8_t n) {
    if (reg_x > 15 || reg_y > 15) {
        throw REGISTER_ERROR;
    }
    uint8_t x = registers.at(reg_x);
    uint8_t y = registers.at(reg_y);
    x %= ROWS;
    uint8_t initial_x = x;
    y %= COLLUMNS;
    uint8_t sprite = memory.at(i);
    registers[15] = 0;
    for (int i = 0; i < n; i++) {
        uint8_t sprite = memory[this->i + i];
        std::string sprite_bits = integer_to_bit_string(sprite);
        size_t sprite_bits_length = sprite_bits.length();
        if (y > COLLUMNS)
            break;
        for (size_t j = 0; j < sprite_bits_length; j++) {
            if (x > ROWS)
                break;
            else if (sprite_bits[j] == '1') {
                if (display[x][y] != 0) {
                 display[x][y] = 0;
                 registers[15] = 1;
                } else {
                    display[x][y] = 1;
                }
            } else {
                display[x][y] = 0;
            }
            x++;
        }
        x = initial_x;
        y++;
    }

    render(renderer);
}

void Chip8::addProgram(std::ifstream *rom) {
    char instruction;
    uint16_t iterator = PROGRAM_ADDRESS;
    while ((*rom).read(&instruction, sizeof(char))) {
        uint16_t h = static_cast<unsigned char>(instruction);
        memory.at(iterator) = (uint8_t)h;
        iterator++;
    }
}

std::string Chip8::getInstruction() {
    if (pc >= MEMORY_SIZE)
        return "";
    uint8_t first_byte = memory[pc];
    uint8_t second_byte = memory[pc + 1];
    pc += 2;

    std::stringstream stream;
    stream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(first_byte);
    stream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(second_byte);

    return stream.str();
}
