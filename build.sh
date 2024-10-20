#!/bin/bash

libs=-luser32 # we're defining a variable, and including its contents later with $
# the -l flag is for including a library. It looks for .lib (or .a) files which are either static /
# libraries or import libraries. Import libraries will reference a corresponding .dll file.
# In this case, it's the latter. user32.lib is an import library for user32.dll
# user32.dll is supposedly a windows library file that contains instructions for graphical elements

# adding the -Wno flag to writable-strings
# note, here it is necessary to do 0 spaces with operator =
warnings=-Wno-writable-strings

g++ src/main.cpp -o2d-game2.exe $libs $warnings
# -g after the clang++

