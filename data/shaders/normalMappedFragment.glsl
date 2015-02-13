#version 410

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 fragTangent;
in vec3 fragBiTangent;
in vec2 fragTexCoord;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform vec3 eyePos;
uniform float specularPower;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D specularTex;

uniform mat4 projectionView;

void main()
{
	mat3 TBN = mat3(normalize(fragTangent), normalize(fragBiTangent), normalize(fragNormal));

	vec3 sampledNormal = texture(normalTex, fragTexCoord).xyz;
	vec3 adjustedNormal = sampledNormal * 2 - 1;
	
	vec3 materialColor = texture(diffuseTex, fragTexCoord).xyz;
	
	vec3 ambient = materialColor * ambientLight;
	
	vec3 N = normalize(TBN * adjustedNormal);
	vec3 L = normalize(lightDir);

	float d = max(0.0f, dot(N, -L));
	vec3 diffuse = vec3(d) * lightColor * materialColor;

	vec3 E = normalize(eyePos - fragPosition.xyz);
	vec3 R = reflect(L, N);
	float s = max(0, dot(R, E));
	s = pow(s, specularPower);

	vec3 materialSpecular = texture(specularTex, fragTexCoord).xyz;
	vec3 specular = vec3(s) * lightColor * materialColor;
	
	fragColor = vec4(ambient + diffuse + specular, 1); 	
}