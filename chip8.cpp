#include "chip8.h"

Chip8::Chip8() {
    this->pc = START_ADDRESS;
    this->i = 0;
    this->delay_timer = 0;
    this->sound_timer = 0;
    this->registers.reserve(REGISTER_COUNT);
    this->memory.reserve(MEMORY_SIZE);
};
