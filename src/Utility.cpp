#include "Utility.h"
#include "gl_core_4_4.h"
#include <cstdio>


bool loadShaders(char* a_vertexFile, char* a_fragmentFile, unsigned int* a_result)
{
	bool succeeded = false;

	FILE* vertexFile = fopen(a_vertexFile, "r");
	FILE* fragmentFile = fopen(a_fragmentFile, "r");

	if (vertexFile == 0 || fragmentFile == 0)
	{

	}
	else
	{
		fseek(vertexFile, 0, SEEK_END);
		int vertexFileLength = ftell(vertexFile);
		fseek(vertexFile, 0, SEEK_SET);

		fseek(fragmentFile, 0, SEEK_END);
		int fragmentFileLength = ftell(fragmentFile);
		fseek(fragmentFile, 0, SEEK_SET);

		char* vsSource = new char[vertexFileLength];
		char* fsSource = new char[fragmentFileLength];
	}

	fclose(vertexFile);
	fclose(fragmentFile);

	return succeeded;
}