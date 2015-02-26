#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;
out vec4 fragColor; 
uniform mat4 projectionView; 

void main() 
{ 
	fragColor = color; 
	gl_Position = projectionView * position;
}