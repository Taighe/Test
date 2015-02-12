#version 410

uniform vec3 kA = vec3(1,0,0);
uniform vec3 kD = vec3(1,0,0);
uniform vec3 kS = vec3(1,0,0);

uniform vec3 iA = vec3(0.25f,0.25f,0.25f);
uniform vec3 iD = vec3(1,1,1);
uniform vec3 iS = vec3(1,1,1);
uniform float iSpecPower = 32.0f;

in vec3 N; //normalised surface normal from mesh
in vec3 P;

uniform vec3 camPos; // world-space camera position
uniform vec3 L;

void main() 
{
	vec3 ambient = kA * iA;
	
	float NdL = max(0.0f, dot(N, -L));
	vec3 Diffuse = KD * iD * NdL;
	float R = reflect(L, N);
	vec3 E = normalize(camPos)
};