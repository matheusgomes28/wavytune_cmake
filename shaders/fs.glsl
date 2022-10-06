#version 330 core
in vec3 normal;
in vec4 colour;
out vec4 FragColor;

void main()
{
    FragColor = colour;
}