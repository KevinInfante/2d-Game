// ############################################################
//                        PLATFORM GLOBALS
// ############################################################
#include<iostream>

// ############################################################
//                        PLATFORM FUNCTIONS
// ############################################################

bool platform_create_window(int width, int height, char* title);

static bool running = true;

// ############################################################
//                        WINDOWS PLATFORM 
// ############################################################
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include<windows.h>
#endif

// ############################################################
//                        windows GLOBALS
// ############################################################

// ############################################################
//                        PLATFORM IMPLEMENTATION
// ############################################################
bool platform_create_window(int width, int height, char* title){
    HINSTANCE instance = GetModuleHandleA(0); //why use the A suffix?
    // the a stands for ansi which is the encoding...

    char classID[] = {"window"}; //could be any string
    WNDCLASSA wc = {};
    wc.hInstance = instance; 
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = classID; //this is NOT the title, just a unique identifier
    wc.lpfnWndProc = DefWindowProcA; // "long function pointer to a windows proc address"
     //"this function is the callback for input, e.g. whenever you move the mouse,press keys, etc."
        //all of those are events that will be sent to this function
        //for now we use the default fnc, wince we're just opening a window
    if(!RegisterClassA(&wc)){ //if this succeeds, then we will have registered our class
        return false;
    }

    // WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    int dwStyle =   WS_OVERLAPPEDWINDOW; //combines the most common options into one

    HWND window = CreateWindowExA(0, classID, title, 
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

int main(){
    platform_create_window(1200, 700, "Kevin Engine");
    while(running){
        //update
    }
    return 0;
}