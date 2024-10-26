#pragma once

#include "../third_party/glcorearb.h"
#include "KevinEngine_lib.h"

// ############################################################
//                     OpenGL Function Pointers
// ############################################################

// "how openGL is loaded by windows or any other platform because
// there is no tutorial out there that I could find that actually
// explains how this works"

//In opengl, you have to declare static function pointers
static PFNGLCREATEPROGRAMPROC glCreateProgram_ptr;
// ^ "the commonly known, opengl creatprogram func"
// stands for: pointer to a function call GL createprogram proc address
//  proc address means this is a pointer to a function that we have to load
//  from a library

//this global function will load all of the opengl funcs that we'll use in 
//our project
void gl_load_function(){ 
    //this is a windows function, it loads opengl32.dll from windows library 
    //it returns a proc address
    PROC proc = wglGetProcAddress("glCreateProgram" ); //glCreateProgram is the fnc we're loading
    if(!proc){
        SM_ASSERT(false, "Failed to load gl function %s", "glCreateProgram");
    }

    //casting proc to PFNGL... and assigning it to glCreateProgram_ptr (it now points to a func)
    glCreateProgram_ptr = (PFNGLCREATEPROGRAMPROC)proc;
}

GLAPI GLuint APIENTRY glCreateProgram(void){
    return glCreateProgram_ptr();
}
// note, it sounds like he is saying "glad" but it might be "glut"
//"this is how GLAD works, GLAD defines this [proc] pointer, then GLAD will check whether it can
// load the pointer (load the opengl function that the pointer points to) then it gives you a 
// wrapper around that pointer that is then called the normal openGL function"