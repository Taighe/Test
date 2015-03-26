#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;

out vec4 vNormal;
out vec4 vShadowCoord;

uniform mat4 projectionView;
uniform mat4 LightMatrix;

void main()
{
	vNormal = Normal;
	gl_Position = projectionView * Position;

	vShadowCoord = LightMatrix * Position;
}