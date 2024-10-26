//this platform.h file will be platform agnostic, it will declare functions
// that will have platform-specific implementations in other files
#pragma once

// ############################################################
//                        PLATFORM GLOBALS
// ############################################################
static bool running = true;

// ############################################################
//                        PLATFORM FUNCTIONS
// ############################################################

bool platform_create_window(int width, int height, char* title);
void platform_update_window();
void* platform_load_gl_function(char* funName);