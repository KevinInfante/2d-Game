#!bin/bash

libs=-luser32 # I think this is including a library for removing warnings
warnings = -Wno-writable-strings # getting rid of a warning

clang src/main.cpp -o2d-game.exe $libs $warnings
# -g after the clang++

