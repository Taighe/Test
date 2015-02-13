#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;

out vec4 fragNormal;
out vec4 fragPosition;

uniform mat4 projectionView;
uniform float timer;

void main()
{
	fragPosition = position;
	vec4 p = position;
	p.y = sin(position.y + timer);
	fragNormal = normal;
	gl_Position = projectionView * position;
}