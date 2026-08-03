#pragma once

#include "instruction.h"
#include <SDL3/SDL_render.h>
#include <array>
#include <cstdint>
#include <stack>
#include <string>
#include <vector>

#define MEMORY_SIZE 0x1000
#define STACK_SIZE 16
#define REGISTER_COUNT 16
#define DELAY_HZ 60
#define START_ADDRESS 0x0200
#define ROWS 64
#define COLLUMNS 32
#define PROGRAM_ADDRESS 0x200
#define FONT_ADDRESS 0x050

class Chip8 {
    public:
        uint16_t pc;
        uint16_t i;
        std::stack<uint16_t> stack;
        uint8_t delay_timer;
        uint8_t sound_timer;
        std::array<uint8_t, REGISTER_COUNT> registers;
        std::array<uint8_t, MEMORY_SIZE> memory;
        uint8_t display[ROWS][COLLUMNS];
    public:
        Chip8();

        void cls(SDL_Renderer *); // 00E0
        void jmp(uint16_t); // 1NNN
        void set_VX(uint8_t x, uint8_t nn); // 6XNN
        void add_VX(uint8_t x, uint8_t nn); // 7XNN
        void set_I(uint16_t address); // ANNN
        void draw(SDL_Renderer *renderer, uint8_t reg_x, uint8_t reg_y, uint8_t n); // DXYN
        void call_subroutine(uint16_t address); //2NNN
        void return_from_subroutine(); // 00EE
        void skip_if_equal(uint8_t reg_x, uint8_t nn); // 3XNN
        void skip_if_not_equal(uint8_t reg_x, uint8_t nn); // 4XNN
        void skip_if_reg_equal(uint8_t reg_x, uint8_t reg_y); // 5XY0
        void skip_if_reg_not_equal(uint8_t reg_x, uint8_t reg_y); // 9XY0
        void set_VX_to_VY(uint8_t reg_x, uint8_t reg_y); // 8XY0
        void binary_or(uint8_t reg_x, uint8_t reg_y); // 8XY1
        void binary_and(uint8_t reg_x, uint8_t reg_y); // 8XY2
        void logical_xor(uint8_t reg_x, uint8_t reg_y); // 8XY3
        void add_VX_carry(uint8_t reg_x, uint8_t reg_y); // 8XY4
        void subtract_VX_VY(uint8_t reg_x, uint8_t reg_y); //8XY5

        void subtract_VY_VX(uint8_t reg_x, uint8_t reg_y); //8XY7

        void render(SDL_Renderer *renderer);
        void addProgram(std::ifstream *rom);
        Instruction getInstruction();
        void decrementTimers();
};

static const std::array<uint8_t, 80> font = {
0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
