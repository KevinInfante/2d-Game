#pragma once //I presume this is so that this header isn't defined more than once per file
#define _CRT_SECURE_NO_WARNINGS
//added the above line myself, it essentially gets rid of a lot of warnings related to
// deprecated functions like fopen() and so so forth

#include<stdio.h> //using this to write to console 
#include<stdlib.h> //for malloc
#include<string.h> //for memset
#include<sys/stat.h>//used to get the edit timestamp of files

// ############################################################
//                        DEFINES
// ############################################################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

#define BIT(x) 1 << (x) //you call this macro, and it shifts number 1 to the left by x bits
#define KB(x) ((unsigned long long) 1024 * x)
#define MB(x) ((unsigned long long) 1024 * KB(x))
#define GB(x) ((unsigned long long) 1024 * MB(x))

// ############################################################
//                        LOGGING
// ############################################################

//"the reason I put the underscore is bc I don't want it to be invoked directly"
enum TextColor{ //"these are basically all the colors you can set on windows,
// and you can combine them"
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

template<typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args... args){
    static char* TextColorTable[TEXT_COLOR_COUNT] = {
    "\x1b[30m",    // TEXT_COLOR_BLACK,
    "\x1b[31m",     // TEXT_COLOR_RED,
    "\x1b[32m",     // TEXT_COLOR_GREEN,
    "\x1b[33m",     // TEXT_COLOR_YELLOW,
    "\x1b[34m",      // TEXT_COLOR_BLUE,
    "\x1b[35m",      // TEXT_COLOR_MAGENTA,
    "\x1b[36m",      // TEXT_COLOR_CYAN,
    "\x1b[37m",      // TEXT_COLOR_WHITE,
    "\x1b[90m",      // TEXT_COLOR_BRIGHT_BLACK,
    "\x1b[91m",      // TEXT_COLOR_BRIGHT_RED,
    "\x1b[92m",      // TEXT_COLOR_BRIGHT_GREEN,
    "\x1b[93m",      // TEXT_COLOR_BRIGHT_YELLOW,
    "\x1b[94m",      // TEXT_COLOR_BRIGHT_BLUE,
    "\x1b[95m",      // TEXT_COLOR_BRIGHT_MAGENTA,
    "\x1b[96m",      // TEXT_COLOR_BRIGHT_CYAN,
    "\x1b[97m"       // TEXT_COLOR_BRIGHT_WHITE,
    };
    
    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);
    //the first %s is the color change, the second the color change, the third the msg
    //the \033[0m closes the handle, to undo the color change for future trace msgs

    char textBuffer[8192] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__); 
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__); 
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__); 

#define SM_ASSERT(x, msg, ...)          \
{                                       \
    if(!(x)){                           \
        SM_ERROR(msg, ##__VA_ARGS__);   \
        DEBUG_BREAK();                  \
        SM_ERROR("Assertion hit!");     \
    }                                   \
}      


// ############################################################
//                       Bump Allocator
// ############################################################
struct BumpAllocator{
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator make_bump_allocator(size_t size){
    BumpAllocator ba = {};
    ba.memory = (char*)malloc(size); //malloc takes size in bytes, i believe
    if(ba.memory){
        ba.capacity = size;  
        memset(ba.memory, 0, size); //setting memory to 0 before returning      
    }
    else{
        SM_ASSERT(false, "failed to allocated memory!"); 
    }
    return ba;
}

//function to allocate memory from the bump allocator
char* bump_alloc(BumpAllocator* bumpAllocator, size_t size){
    char* result = nullptr;
    size_t alignedSize = (size + 7) & ~7; //bitwise AND with bitwise NOT..
    // 7, which ensures first 4 bits are set to 0. 
    //I'm not sure why, I would think that ~0111 = 1000, so first 3 bits
    if(bumpAllocator->used + alignedSize <= bumpAllocator->capacity){
        result = bumpAllocator->memory + bumpAllocator->used; 
        bumpAllocator->used += alignedSize;
    }
    else{
        SM_ASSERT(false, "bump allocator is full");
    }

    return result;
}

// ############################################################
//                        File I/O
// ############################################################
long long get_timestamp(char* file){
    struct stat file_stat = {};
    stat(file, &file_stat); //fills in the struct
    return file_stat.st_mtime; //return the mtime, 
    //the time in miliseconds, when the file was created
}

bool file_exists(char* filePath){
    SM_ASSERT(filePath, "No filePath supplied!");
    auto file = fopen(filePath, "rb"); //fopen is in stdio.h
    //chose fopen bc it's cross-platfrom and in the std library
    if(!file){
        return false;
    }
    fclose(file);
    return true;
}

long get_file_size(char* filePath){
    SM_ASSERT(filePath, "No filePath supplied!"); //to catch errors..
    // during development
    long fileSize = 0;
    auto file = fopen(filePath, "rb");
    if(!file){
        SM_ERROR("Failed opening file: %s", filePath);
        return 0;
    }

    fseek(file, 0, SEEK_END); //seek to the end
    fileSize = ftell(file); //to see how big file is
    fseek(file, 0, SEEK_SET); //seek to the beginning
    fclose(file); //close the file
    
    return fileSize;
}

/*
* Reads a file into a supplied buffer. We manage our own
* memory and therefore want mre control over where it
* is allocated
*/
char* read_file(char* filePath, int* fileSize, char* buffer){
    SM_ASSERT(filePath, "No filePath supplied");
    SM_ASSERT(filePath, "No fileSize suppplied");
    SM_ASSERT(buffer, "No buffer supplied"); 

    *fileSize = 0;
    auto file = fopen(filePath, "rb"); //read in binary
    if(!file){
        SM_ERROR("Failed opening file: %s", filePath);
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(buffer, 0, *fileSize+1); //don't have to do this,
    //but it's to null terminate the file
    fread(buffer, sizeof(char), *fileSize, file);
    //fread to write into the buffer using the size of a character,
    //then we supply the size of the file, and the handle to the file

    fclose(file);
    return buffer;
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* bumpAllocator){
    char* file = nullptr;
    long fileSize2 = get_file_size(filePath);

    if(fileSize2){
        char* buffer = bump_alloc(bumpAllocator, fileSize2+1); 
        file = read_file(filePath, fileSize, buffer); //other read_file overload
    }
    return file;
}

void write_file(char* filePath, char* buffer, int size){
    SM_ASSERT(filePath, "No filePath supplied!");
    SM_ASSERT(buffer, "No buffer supplied!");
    auto file = fopen(filePath, "wb");
    if(!file){
        SM_ERROR("Failed opening file: %s", filePath);
        return;
    }
    fwrite(buffer, sizeof(char), size, file);
    fclose(file);
}

bool copy_file(char* fileName, char* outputName, char* buffer){
    int fileSize = 0;
    char* data = read_file(fileName, &fileSize, buffer);

    auto outputFile = fopen(outputName, "wb");
    if(!outputFile){
        SM_ERROR("Failed opening File %s", outputName);
        return false;
    }
    int result = fwrite(data, sizeof(char), fileSize, outputFile);
    if(!result){
        SM_ERROR("Failed opening File: %s", outputName);
        return false;
    }
    fclose(outputFile);
    return true;
}

bool copy_file(char* fileName, char* outputName, BumpAllocator* bumpAllocator){
    char* file = 0;
    long fileSize2 = get_file_size(fileName);
    if(fileSize2){
        char* buffer = bump_alloc(bumpAllocator, fileSize2+1);
        return copy_file(fileName, outputName, buffer);
    }
    return false;
}

//"that is why we have the bump allocator here, because it allows us to supply..
// any memory we want to those functions, and we don't need to call new or malloc
// every time we call them. [when you manage your own memory] you actually know
// what's going on. Reusing the same memory makes your program faster."