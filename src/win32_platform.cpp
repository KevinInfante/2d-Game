#include"platform.h"
#include "KevinEngine_lib.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include<windows.h> 
#include"wglext.h"
#include"glcorearb.h" //for GL_TRUE

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
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

    //Fake window is required for initializing openGL, this is how it is done behind the
    // scene in sdl2, glfw, etc.
    { //creating a local scope for this purpose
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
        if(window == NULL){  SM_ASSERT(false, "failed to create fake window");
            return false;
        }
        //we need to create a face device context
        HDC fakeDC = GetDC(window); //this is a windows function
        if(!fakeDC){
            SM_ASSERT(false, "Failed to get HDC");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        // double buffering means that we get one image from the gpu; it gives us one 
        // thing to draw on, we draw our image, then we give it back, then the gpu
        // has a second image for us, which we take while the first image is being 
        // drawn, we draw on the second image, and it continuously interleaves the
        // two images
        //"double buffering involves two buffers: one for rendering and one for display."
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;
        
        //choose pixel format by supplying our fake device context
        int pixelFormat = ChoosePixelFormat(fakeDC, &pfd);
        if(!pixelFormat){ //(optional) error checking
            SM_ASSERT(false, "Failed to choose pixel format");
            return false;
        }

        //setting the pixel format that we have just chosen on the device context
        //also changes the pixel format of the window
        if(!SetPixelFormat(fakeDC, pixelFormat, &pfd)){
            SM_ASSERT(false, "Failed to set pixel format");
            return false;
        }

        // Create handle to fake OpenGL rendering context
        HGLRC fakeRC = wglCreateContext(fakeDC);
        if(!fakeRC){
            SM_ASSERT(false, "Failed to Create render context");
            return false;
        }
        //using that rendering context we can call windowGL make current, which means
        //that all subsequent OpenGL calls made by the thread are drawn on the device
        //identified by HDC
        if(!wglMakeCurrent(fakeDC, fakeRC)){
            SM_ASSERT(false, "Failed to make current");
            return false;
        }
        //"this is where we can finally load in the functions that we need, to set on
        //our window"
        
        //we need to set our pixel format, and we need to set attributes
        //need to include wgext.h for this
        wglChoosePixelFormatARB =
         (PFNWGLCHOOSEPIXELFORMATARBPROC)platform_load_gl_function("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = 
         (PFNWGLCREATECONTEXTATTRIBSARBPROC)platform_load_gl_function("wglCreateContextAttribsARB");
        
        //check if we've gotten a proc address back from both
        if(!wglCreateContextAttribsARB || !wglChoosePixelFormatARB){
            SM_ASSERT(false, "Failed to load openGL functions");
            return false;
        }

        //if everything worked then we basically destroy everthing we just created and recreate 
        //that again, bc we can't use this rendering context, bc we have to set attributes on the
        //window, before we actually create a rendering context. And in order to do that [set 
        //attributes] we need those 2 functions [PFNW...]
        //"we need those 2 functions to set stuff on the window, but in order to do that, we need
        //to create a window first, delete, and then create the window using those functions"
        //"that's usually what GLAD (GLUT?) does for you, behind your back"

        //clean up the take stuff
        wglMakeCurrent(fakeDC, 0); //call this function with 0
        wglDeleteContext(fakeRC);
        ReleaseDC(window, fakeDC);

        //can't reuse the same (Device) context
        //because we already called setpixelformat
        DestroyWindow(window);
    }

    //Actual OpenGL initialization
    {
        //add in border size of the window
        {
            RECT borderRect = {};
            AdjustWindowRectEx(&borderRect, dwStyle, 0, 0);
            //^setting menu and extended style params to 0

            //then it gives us a rectangle,& we want to increase our width and height by that rect
            width += borderRect.right - borderRect.left;
            height += borderRect.bottom - borderRect.top;
        }
        //copy the create window function from above
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
        if(window == NULL){
            SM_ASSERT(false, "Failed to create Windows window");
            return false;
        }
        //also copy and pased the part where we got the device context from the window
        HDC dc = GetDC(window); //this is a windows function
        if(!dc){
            SM_ASSERT(false, "Failed to get DC");
            return false;
        }
        //const and pixel attributes array
        const int pixelAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE, //we'd like to draw to the window
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,//we'd like to upport opengl
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,//we'd like to double buffer
            WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,//we'd like to set swap meth. to copy
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, //pixel type: rgba
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32, //pixel formats, like how many bits set for each pixel
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            0 //terminate with 0, otherwise, opengl will throw an error
        };

        UINT numPixelFormats;
        int pixelFormat = 0;
        if(!wglChoosePixelFormatARB(dc, pixelAttribs,
                                    0, //Float List
                                    1, //Max Formats
                                    &pixelFormat,
                                    &numPixelFormats)){
            SM_ASSERT(0, "Failed to wglChoosePixelFormatARB");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd = {0};
        //"we have already set the attributes above, we can now get the pixel format
        //out of our device"
        DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        //setting the pixel format again, like before
        if(!SetPixelFormat(dc, pixelFormat, &pfd)){
            SM_ASSERT(0, "Failed to SetPixelFormat");
            return false; //he had return true here, but i think that was a mistake
        }

        const int contextAttribs[] = { //definiing certain things ab our opengl version
            //before we actually create our rendering context
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4, //4 and 3 for version 4.3
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0 //Terminates the array
        };

        HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
        if(!rc){
            SM_ASSERT(0, "failed to create render context for opengl");
            return false;
        }
        //make the rc the primary rendering context on our device context
        if(!wglMakeCurrent(dc, rc)){
            SM_ASSERT(0, "failed to wglMakeCurrent");
            return false;
        }
        //if all of that works, we have successfully initialized openGL on windows
        // "that is everything that GLUT,GLFW, or SDL, this is what they do"
        //usually they extract it out into one function
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