#version 330 core
in vec4 aColor;
out vec4 FragColor;


void main()
{

	//FragColor = texture(myTexture, TexCoords) + (diff * 0.2);
    FragColor = aColor;
}