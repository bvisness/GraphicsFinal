#include "VisnessGL.h"
#include <stdio.h>
#include <iostream>

static char*
readShaderSource(const char* shaderFile)
{
	FILE* fp;

#ifdef __APPLE__
	fp = fopen(shaderFile, "rt");
#else
	errno_t err;
	err = fopen_s(&fp, shaderFile, "rt");
	if (err != 0) { return NULL; }
#endif

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	size = fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

GLuint InitComputeShader(const char* shaderFile) {
	GLchar* source = readShaderSource(shaderFile);
	if (source == NULL) {
		std::cerr << "Failed to read " << shaderFile << std::endl;
		exit(EXIT_FAILURE);
	}

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	if (shader == 0) {
		std::cerr << "Failed to create compute shader (glCreateShader returned 0)" << std::endl;
		GLenum err = glGetError();
		std::cerr << "Error code: " << err << std::endl;
		std::cerr << "GL_INVALID_ENUM: " << GL_INVALID_ENUM << std::endl;
		printf("OpenGL version: %s\n", glGetString(GL_VERSION));
		exit(EXIT_FAILURE);
	}

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		std::cerr << shaderFile << " failed to compile:" << std::endl;

		GLint logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;

		delete[] logMsg;
		exit(EXIT_FAILURE);
	}

	delete[] source;

	GLuint program = glCreateProgram();
	glAttachShader(program, shader);

	/* link  and error check */
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		std::cerr << "Shader program failed to link" << std::endl;

		GLint logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;

		delete[] logMsg;
		exit(EXIT_FAILURE);
	}

	/* use program object */
	glUseProgram(program);

	return program;
}
