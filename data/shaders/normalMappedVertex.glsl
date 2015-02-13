#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec4 tangent;
layout(location=3) in vec2 texCoord;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragTangent;
out vec3 fragBiTangent;
out vec2 fragTexCoord;

uniform mat4 projectionView;
uniform float timer;

void main()
{
	fragPosition = position.xyz;
	fragNormal = normal.xyz;
	fragTangent = tangent.xyz;
	fragBiTangent = cross(normal.xyz, tangent.xyz);
	fragTexCoord = texCoord;

	gl_Position = projectionView * position;
}