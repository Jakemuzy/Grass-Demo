#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 aColor;

uniform mat4 model; //transformations
uniform mat4 view;  //camera
uniform mat4 projection;    //Ortho or perspective camera

uniform float red;
uniform float blue;



void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    aColor = vec4(red, aPos[1]/8.0f, blue, 1.0f);
}