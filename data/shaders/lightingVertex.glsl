#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;

out vec4 fragNormal;
uniform mat4 projectionView

void main()
{
	fragNormal = normal;
	gl_Position = projectionView * position;
}