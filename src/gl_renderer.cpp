# include "gl_renderer.h" //to get access to the open gl functions
#include "input.h" //he didn't have this
#define GL_GLEXT_PROTOTYPES 

// #############################################################################
//                           OpenGL Structs
// #############################################################################

//we will need a global context for opengl to hold data, e.g. access to program IDs,
// and access to texture IDs
struct GLContext{
    GLuint programID;
};

// #############################################################################
//                           OpenGL Globals
// #############################################################################
static GLContext glContext;
// the opengl renderer will only have access to this, in addition to the main.cpp file

// #############################################################################
//                           OpenGL Functions
// #############################################################################

/*
short but simple explanation of how opengl works:
when we use opengl, a program runs on the gpu. This program takes in data from the
CPU, and then transforms that data, in a way to display an image to the screen.
The way that works is, we have what's called an input assembler, which basically
takes in the data from the CPU, which are the vertices. The input assember supplies
these vertices to the vertex shader- that is a program (like any C/C++ program)
that has a main function, and is then being invoked for each of these vertices.
The job of the vertex shader is to transform these vertices into the correct
coordinate space. We then have optional steps like tesselation and the geometry
shader, which can help with detail generation. In our engine we are going to skip
these two steps and go straight to the rasterization and the fragment shader. You
can think of these like a pixel grid. (He gives an example to explain here).
The fragment shader is also a program like the vertex shader. The vertex shader
runs for every vertex, and the gragment shader runs for every fragment (which is
basically like a pixel). Keep in mind, having way more fragments than vertices
will impact performance.
*/
//we need the vertex shader and the fragment shader 

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                        GLsizei length, const GLchar* message, const void* user){
    if(severity == GL_DEBUG_SEVERITY_LOW ||
       severity == GL_DEBUG_SEVERITY_MEDIUM ||
       severity == GL_DEBUG_SEVERITY_HIGH) {
        SM_ASSERT(false, "OpenGL Error: %s", message);
       }
    else{
        SM_TRACE((char*)message);
    }
}

bool gl_init(BumpAllocator* transientStorage){
    load_gl_functions();

    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    //time to actually create shaders
    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER); //these are just IDs/handles
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER); 

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);

    if(!vertShader || !fragShader){
        SM_ASSERT(false, "Failed to load shaders");
        return false;
    }

    glShaderSource(vertShaderID, 1, &vertShader, 0);
    glShaderSource(fragShaderID, 1, &fragShader, 0);

    glCompileShader(vertShaderID); //compiles the text files into actual code
    glCompileShader(fragShaderID);

    //test if vertex shader compiled successfully
    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Failed to compile Vertex Shaders %s", shaderLog);
        }
    }
    //test if fragment shader compiled successfully
    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fragShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Failed to compile Fragment Shaders %s", shaderLog);
        }
    }

    //now that we have our shaders, we can finally create our program
    glContext.programID = glCreateProgram(); //this function returns an int
    glAttachShader(glContext.programID, vertShaderID);
    glAttachShader(glContext.programID, fragShaderID);
    glLinkProgram(glContext.programID);
    //after this is done, we can delete the shaders bc they are now part of the program,
    //and since we don't need them anymore, we should clean them up
    glDetachShader(glContext.programID, vertShaderID);
    glDetachShader(glContext.programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    //he said he doesn't like using vertex arrays, but regardless,
    //this has to be done in openGL, otherwise doesn't seem to work
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); 

    // Depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    //he suggests using an application called RenderDoc, which is used to inspect 
    //graphics applications, then use the app to determine what's going wrong
    glUseProgram(glContext.programID);
    return true;
}
// this transient storage is a block of memory we'll allocate when we start the
// program, and then every frame, (every run thru the update loop), we'll reset
// the memory so that other modules in the game can reuse the memory

void gl_render(){
    //this func is where we supply a rgba
    glClearColor(119.0f / 255.0f, 33.0f / 255.0f, 111.0f / 255.0f, 1.0f);
    glClearDepth(0.0f);
    //the above 2 functions are saying "these are the value I want to clear with", and then
    // we are actually invoking the clear by saying we're clearing color and depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //opengl doesn't know which viewport to render to yet
    glViewport(0, 0, input.screenSizeX, input.screenSizeY); //position and size

    glDrawArrays(GL_TRIANGLES, 0, 6);
    // this should draw us a quad
}