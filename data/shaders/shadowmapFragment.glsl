#version 410

in vec4 vNormal;
in vec4 vShadowCoord;

out vec4 FragColor;

uniform vec3 lightDir;
uniform sampler2D shadowMap;
uniform float shadowBias;

void main()
{
	float d = max(0, dot(normalize(vNormal.xyz), lightDir));
	if(texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - 0.1f)
	{
		d = 0;
	}
	
	FragColor = vec4(d,d,d,1); 
}