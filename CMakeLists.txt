cmake_minimum_required(VERSION 3.28)
project(huffman2 C)

set(CMAKE_C_STANDARD 11)

add_executable(huffman2 main.c
        structs.h
        code.h
        decode.h)
