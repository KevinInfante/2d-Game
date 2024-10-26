#include"KevinEngine_lib.h"
// ############################################################
//                        PLATFORM GLOBALS
// ############################################################
#include<iostream>

// ############################################################
//                        PLATFORM FUNCTIONS
// ############################################################

bool platform_create_window(int width, int height, char* title);
void platform_update_window();

static bool running = true;

// ############################################################
//                        WINDOWS PLATFORM 
// ############################################################
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include<windows.h> 

//in vs code, it is necessary to link user32.lib,
//(which references user32.dll) in order to include<windows.h>

// ############################################################
//                        windows GLOBALS
// ############################################################
static HWND window; //has to be global so that update_window() can access it

// ############################################################
//                        PLATFORM IMPLEMENTATION
// ############################################################
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, 
                                        WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    
    switch(msg){
        case WM_CLOSE:{
            running = false;
            break;
        }
        case WM_KEYDOWN:
            if(wParam == 'A') {std::cout<<"yooooooo you pressed A"<<std::endl;}
            //note: you HAVE to use std::endl for this to appear in the console
            // might have something to do with flushing the buffer
            // it definitely has to do with flushing. Code below confirms it.
            else{
                char output = wParam;
                std::cout<<output<<std::flush;
            }
            break;
        default:{
            // let window handle default input for now
            //invoke the default proc address for every other msg
            result = DefWindowProcA(window, msg, wParam, lParam);
        }
    }
    //"now you know how sdl2 or glfw open up windows"
    return result;
}

bool platform_create_window(int width, int height, char* title){
    HINSTANCE instance = GetModuleHandleA(0); //why use the A suffix?
    // the a stands for ansi which is the encoding...

    char classID[] = {"window"}; //could be any string
    WNDCLASSA wc = {};
    wc.hInstance = instance; 
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = classID; //this is NOT the title, just a unique identifier
    wc.lpfnWndProc = windows_window_callback; //Callback for input into the window
        // lpfnwndproc "long function pointer to a windows procedure address"
        //all of those are events that will be sent to this function
    if(!RegisterClassA(&wc)){ //if this succeeds, then we will have registered our class
        return false;
    }

    // WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    int dwStyle =   WS_OVERLAPPEDWINDOW; //combines the most common options into one

    window = CreateWindowExA(0, classID, title, 
         dwStyle, // a style to the window; tells it what we wnt to show
         100,     //these next 2 params tell windows where to position the window
         100,
         width,
         height,        // btw, NULL is the windows null pointer
         NULL,    //the parent
         NULL,    //the menu
         instance, //INSTANCE
         NULL);    //lparam
    //check if the window is null
    if(window == NULL){
        return false;
    }
    ShowWindow(window, SW_SHOW);

    return true;
}

void platform_update_window(){
    MSG msg;
    //we queue up messages, and we need to make sure we empty that q
    //we peek one message, we fill in the data, then we dispatch it
    // which is then handled by the callback
    while(PeekMessageA(&msg, window,
    0, //filter, we don't want to filter anything so we leave at 0
    0, //max filter, we leave that at 0 too
    PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg); //calls the callback when creating the window
    } //pm_REMOVE, whenever we peek a msg, we remove it from the q
}
#endif

int main(){
    platform_create_window(1200, 700, "Kevin Engine");
    while(running){
        //update
        platform_update_window();//every loop iteration, we update the window,& we check for input
    }
    return 0;
}