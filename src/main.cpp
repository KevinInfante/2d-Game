#include"KevinEngine_lib.h"
#include"input.h"
#include<iostream> //note, he doesn't have this line
#include "platform.h"

#define APIENTRY  //defining this to prevent glcore from redefining lean_and_mean
#define GL_GLEXT_PROTOTYPES //includes more function defs we need
#include "glcorearb.h" //this gives us all the defines, we will need it for 
// both the windows and linux implementation

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"

int main(){
    BumpAllocator transientStorage = make_bump_allocator(MB(50));
    platform_create_window(1200, 720, "Kevin Engine");
    input.screenSizeX = 1200;
    input.screenSizeY = 720;

    gl_init(&transientStorage);
    while(running){
        //update
        platform_update_window();//every loop iteration, we update the window,& we check for input
        gl_render();

        platform_swap_buffers();
    }
    return 0;
}