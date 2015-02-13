#version 410

in vec4 fragNormal;
in vec4 fragPosition;
out vec4 fragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 materialColor;
uniform vec3 ambientLight;
uniform vec3 eyePos;
uniform float specularPower;

void main()
{
	vec3 ambient = materialColor * ambientLight;
	
	vec3 N = normalize(fragNormal.xyz);
	vec3 L = normalize(lightDir);

	float d = max(0.0f, dot(N, -L));
	vec3 diffuse = vec3(d) * lightColor * materialColor;

	vec3 E = normalize(eyePos - fragPosition.xyz);
	vec3 R = reflect(L, N);
	float s = max(0, dot(R, E));
	s = pow(s, specularPower);
	vec3 specular = vec3(s) * lightColor * materialColor;

	//vec3 dir = vec3(0,1,0.1);

	//vec3 N = normalize(fragNormal.xyz);
	//float d = dot(N, dir);
	//d = max(d, 0);

	//vec3 finalDiffuse = vec3(d);
	
	//float d = max(0, dot(normalize(fragNormal.xyz), vec3(0,1,0)));
	
	fragColor = vec4(ambient + diffuse + specular, 1); 	
}