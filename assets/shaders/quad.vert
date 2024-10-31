#version 430 core

void main(){
    // generating vertices on the GPU
    // mostly because we have a 2d engine
    // in opengl, the y axis is flipped from what you'd expect,
        //going up is positive, going down is negative

    //opengl coordinates
    // -1/1                 1/1
    // -1/-1                1/-1
    vec2 vertices[6] =
    { //I'm not sure why we do some vertices twice
        //top left
        vec2(-0.5, 0.5),  //0.5 to be half the size of the screen
        //bottom left
        vec2(-0.5, -0.5),
        //top right 
        vec2(0.5,0.5),
        //top right 
        vec2(0.5,0.5),
        //bottom left
        vec2(-0.5, -0.5),
        //bottom right
        vec2(0.5,-0.5)
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
    //the first 1.0 is the z axis, the second the perspective
}