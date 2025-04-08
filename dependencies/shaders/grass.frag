#version 330 core
in vec3 aCol;
out vec4 FragColor;
  
void main()
{
    FragColor = vec4(aCol, 1.0);
}