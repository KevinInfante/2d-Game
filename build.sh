#!/bin/bash

libs=-"luser32 -lopengl32 -lgdi32" # 
# the -l flag is for including a library. It looks for .lib (or .a) files which are either static /
# libraries or import libraries. Import libraries will reference a corresponding .dll file.
# In this case, it's the latter. user32.lib is an import library for user32.dll
# user32.dll is supposedly a windows library file that contains instructions for graphical elements
# gdi32 is related to openGL, i think it is needed for ChoosePixelFormat, DescribePixelFormat...

# adding the -Wno flag to writable-strings
# note, here it is necessary to do 0 spaces with operator =
warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations" 
includes="-Ithird_party -Ithird_party/Include" #-I is to add search path to header files
    # note: the -I flag allows us to use angle brackets to include header files in the directory
    #"the /Include means we're including the directories to be searched for C++ or header files"

clang++ $includes -g src/main.cpp -o2d-game.exe $libs $warnings
# -g after the clang++ creates debug symbols (files) for us

