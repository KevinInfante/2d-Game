#include"platform.h"
#include "KevinEngine_lib.h"

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

void* platform_load_gl_function(char* funName){
    //this is a windows function, it loads opengl32.dll from windows library 
    //it returns a proc address
    PROC proc = wglGetProcAddress(funName); //formerly "glCreateProgram"
    if(!proc){
        //if the above function to get the proc address doesn't work, we try to load it directly
        // from the windows system
        //"if you have a graphics card that supports opengl, which everyone should have, you wil
        // have an opengl32.dll file on your system, which is the library we're loading here"
        static HMODULE openglDLL = LoadLibraryA("opengl32.dll"); 
        proc = GetProcAddress(openglDLL, funName);
        if(!proc){
        SM_ASSERT(false, "Failed to load gl function %s", "glCreateProgram");
        return nullptr; //to indicate that we did not find the function
        }
    }
    
    return (void*)proc; //returning a void pointer to the proc address
}