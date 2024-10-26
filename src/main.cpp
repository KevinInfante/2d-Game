#include"KevinEngine_lib.h"
#include<iostream> //note, he doesn't have this line
#include "platform.h"

// ############################################################
//                        WINDOWS PLATFORM 
// ############################################################
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.h"

int main(){
    platform_create_window(1200, 700, "Kevin Engine");
    while(running){
        //update
        platform_update_window();//every loop iteration, we update the window,& we check for input
    }
    return 0;
}