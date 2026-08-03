#include "instruction.h"
#include <cstdint>

#define ADDRESS_SIZE_ERROR 3

Instruction::Instruction(uint8_t first_byte, uint8_t second_byte) {
    this->first_byte = first_byte;
    this->second_byte = second_byte;
}

uint8_t Instruction::first_nible() {
    return first_byte >> 4;
}

uint8_t Instruction::second_nible() {
    return first_byte & 0x0f;
}

uint8_t Instruction::third_nible() {
    return second_byte >> 4;
}

uint8_t Instruction::forth_nible() {
    return second_byte & 0x0f;
}

uint16_t Instruction::get_address(uint8_t size) {
    if(size > 3)
        throw ADDRESS_SIZE_ERROR;
    uint16_t address = 0x0;
    if (size == 3) {
        address += ((first_byte & 0x0f) << 8);
    }
    return address + second_byte;
}
