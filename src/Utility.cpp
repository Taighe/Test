#include "Utility.h"
#include "gl_core_4_4.h"
#include <cstdio>


bool loadShaders(char* a_vertexFile, char* a_fragmentFile, GLuint* a_result)
{
	bool succeeded = false;

	FILE* vertexFile = fopen(a_vertexFile, "r");
	FILE* fragmentFile = fopen(a_fragmentFile, "r");

	if (vertexFile == 0 || fragmentFile == 0)
	{
		fclose(vertexFile);
		fclose(fragmentFile);
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

		vertexFileLength = fread(vsSource, 1, vertexFileLength, vertexFile);
		fragmentFileLength = fread(fsSource, 1, fragmentFileLength, fragmentFile);

		int success = GL_FALSE;
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexShader, 1, &vsSource, &vertexFileLength);
		glCompileShader(vertexShader);
		glShaderSource(fragmentShader, 1, &fsSource, &fragmentFileLength);
		glCompileShader(fragmentShader);

		*a_result = glCreateProgram();

		glAttachShader(*a_result, vertexShader);
		glAttachShader(*a_result, fragmentShader);
		glBindAttribLocation(*a_result, 0, "position");
		glBindAttribLocation(*a_result, 1, "TexCoord");
		glLinkProgram(*a_result);
		glGetProgramiv(*a_result, GL_LINK_STATUS, &success);

		succeeded = true;

		if (success == GL_FALSE)
		{
			int length = 0;
			char* infoLog = new char[length];
			glGetProgramiv(*a_result, GL_INFO_LOG_LENGTH, &length);
			glGetProgramInfoLog(*a_result, length, 0, infoLog);
			printf("ERROR: Failed to link shader program!\n");
			printf("%s\n", infoLog);
			delete[] infoLog;
			succeeded = false;
		}

		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		fclose(vertexFile);
		fclose(fragmentFile);
	}


	return succeeded;
}