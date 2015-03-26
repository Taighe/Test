#version 410

out vec3 LightOutput;

// position in view-space

uniform vec3 lightPosView;
uniform vec3 lightDiffuse;
uniform float lightRadius;

uniform sampler2D posTexture;
uniform sampler2D normTexture;

void main() 
{
	//calculate uvs using fragCoord
	
	vec2 texCoord = gl_FragCoord.xy / textureSize(posTexture, 0).xy;

	vec3 normal = ( texture(normTexture, texCoord).xyz );
	normal *= 2;
	normal -= 1;

	vec3 position = texture(posTexture, texCoord).xyz;
	
	vec3 toLight = lightPosView - position;

	float d = max(0, dot(normal, normalize(toLight)));

	//linear falloff
	float fallOff = 1 - min(1, length(toLight) / lightRadius);
	
	LightOutput = lightDiffuse * d * fallOff;
}