#ifndef _UTILITY_H
#define _UTILITY_H

typedef unsigned int GLuint;

bool loadShader(char* a_vertexFile, GLuint* a_result);

bool loadShaders(char* a_vertexFile, char* a_fragmentFile, GLuint* a_result);
bool loadShaders(char* a_vertexFile, char* a_fragmentFile, char* a_geometryFile, GLuint* a_result);

#endif