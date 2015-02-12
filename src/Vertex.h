#ifndef VERTEX_H
#define VERTEX_H

#include "glm_header.h"

struct Vertex
{
	vec4 position;
	vec4 color;
};

struct TextureVertex
{
	vec4 position;
	vec2 texCoord;
};

#endif