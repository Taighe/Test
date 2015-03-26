#version 410

layout(location = 0) in vec4 Position;

uniform mat4 lightMatrix;

void main() 
{
	gl_Position = lightMatrix * Position;
}