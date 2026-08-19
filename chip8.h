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
#define ROWS 32
#define COLUMNS 64
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
        uint8_t display[COLUMNS][ROWS];
        bool waiting_for_up_key;
        int8_t key_up = -1;
    public:
        Chip8();

        void cls(SDL_Renderer *); // 00E0
        void return_from_subroutine(); // 00EE
        void jmp(uint16_t); // 1NNN
        void call_subroutine(uint16_t address); //2NNN
        void skip_if_equal(uint8_t reg_x, uint8_t nn); // 3XNN
        void skip_if_not_equal(uint8_t reg_x, uint8_t nn); // 4XNN
        void skip_if_reg_equal(uint8_t reg_x, uint8_t reg_y); // 5XY0
        void set_VX(uint8_t x, uint8_t nn); // 6XNN
        void add_VX(uint8_t x, uint8_t nn); // 7XNN
        void set_VX_to_VY(uint8_t reg_x, uint8_t reg_y); // 8XY0
        void binary_or(uint8_t reg_x, uint8_t reg_y); // 8XY1
        void binary_and(uint8_t reg_x, uint8_t reg_y); // 8XY2
        void logical_xor(uint8_t reg_x, uint8_t reg_y); // 8XY3
        void add_VX_carry(uint8_t reg_x, uint8_t reg_y); // 8XY4
        void subtract_VX_VY(uint8_t reg_x, uint8_t reg_y); //8XY5
        // AMBIGOUS INSTRUCTION
        void shift_right(uint8_t reg_x); // 8XY6
        void subtract_VY_VX(uint8_t reg_x, uint8_t reg_y); //8XY7
        // AMBIGOUS INSTRUCTION
        void shift_left(uint8_t reg_x); // 8XYE
        void skip_if_reg_not_equal(uint8_t reg_x, uint8_t reg_y); // 9XY0
        void set_I(uint16_t address); // ANNN
        void jump(uint16_t addresss); // BNNN
        void set_VX_random(uint8_t reg_x, uint8_t nn); // CXNN
        void draw(SDL_Renderer *renderer, uint8_t reg_x, uint8_t reg_y, uint8_t n); // DXYN
        void is_pressed(uint8_t reg_x); // EX9E
        void is_not_pressed(uint8_t reg_x); // EXA1
        void set_VX_to_delay(uint8_t reg_x); // FX07
        void set_VX_key_pressed(uint8_t reg_x); // FX0A
        void set_delay_to_VX(uint8_t reg_x); // FX15
        void set_sound_timer_to_VX(uint8_t reg_x); // FX18
        void add_I(uint8_t reg_x); //FX1E
        void set_I_to_sprite(uint8_t reg_x); //FX29
        void decimal_convertion(uint8_t reg_x); // FX33
        // AMBIGOUS INSTRUCTION
        void store(uint8_t reg_x); // FX55
        // AMBIGOUS INSTRUCTION
        void load(uint8_t reg_x); // FX65

        void render(SDL_Renderer *renderer);
        void addProgram(std::ifstream *rom);
        Instruction get_instruction();
        void decrementTimers();
        SDL_Scancode get_scancode(uint8_t x);
        uint8_t get_key(SDL_Scancode scancode);
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
