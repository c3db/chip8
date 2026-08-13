cmake_minimum_required(VERSION 4.4.0)
project(chip8_emu)
add_executable(${PROJECT_NAME} main.cpp)
if(_DEBUG)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
endif()

add_library(chip8 SHARED chip8.cpp)
add_library(instruction SHARED instruction.cpp)

find_package(SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 chip8 instruction)
