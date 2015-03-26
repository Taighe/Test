#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;

out vec4 vPosition;
out vec4 vNormal;

uniform mat4 view;
uniform mat4 projectionView;

void main()
{
	vPosition = view * vec4(Position.xyz, 1);
	vNormal = normalize(view * vec4(Normal.xyz, 0));

	gl_Position = projectionView * vec4(Position.xyz, 1);
}