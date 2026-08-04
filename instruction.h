#pragma once

#include <cstdint>

class Instruction {
public:
    uint8_t first_byte;
    uint8_t second_byte;
public:
    Instruction(uint8_t first_byte, uint8_t second_byte);

    uint8_t first_nibble();
    uint8_t second_nibble();
    uint8_t third_nibble();
    uint8_t forth_nibble();
    uint16_t get_address(uint8_t size);
};
