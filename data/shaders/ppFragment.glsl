#version 410
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D target;
uniform float timer;

// just sample the target and return the colour
vec4 Simple() 
{
	return texture(target, TexCoord);
}

// simple box blur
vec4 BoxBlur() 
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	
	// 9-tap box kernel
	vec4 colour = texture(target, TexCoord);
	
	colour += texture(target, TexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, TexCoord + vec2(-texel.x, 0));
	colour += texture(target, TexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, TexCoord + vec2(0, texel.y));
	colour += texture(target, TexCoord + vec2(0, -texel.y));
	colour += texture(target, TexCoord + vec2(texel.x, texel.y));
	colour += texture(target, TexCoord + vec2(texel.x, 0));
	colour += texture(target, TexCoord + vec2(texel.x, -texel.y));
	
	return colour / 9;
}

vec4 Distort() 
{
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(TexCoord, mid);
	vec2 normalizedCoord = normalize(TexCoord - mid);
	
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) + timer * 0.02f;
	vec2 newCoord = mid + bias * normalizedCoord;
	
	return texture(target, newCoord);
}

void main()
{
	FragColor =  BoxBlur() - (Distort());
}