#include "ShaderManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/GLU.h>

ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
}

GLuint ShaderManager::createShader(GLenum aShaderType, const std::string& aFilename)
{
	std::string lSrcStr = readFile(aFilename);
	const GLchar* lpSrc = static_cast<const GLchar*>(lSrcStr.c_str());

	GLuint lShaderHandle = glCreateShader(aShaderType);
	glShaderSource(lShaderHandle, 1, &lpSrc, nullptr);
	glCompileShader(lShaderHandle);

	GLint lLogLength;
	glGetShaderiv(lShaderHandle, GL_INFO_LOG_LENGTH, &lLogLength);

	if (lLogLength > 0) {
		std::vector<GLchar> lLogStr(lLogLength);

		glGetShaderInfoLog(lShaderHandle, lLogLength, nullptr, &lLogStr[0]);

		std::cerr << "Shader '" << aFilename << "' compilation warnings:" << std::endl;
		std::cerr << lLogStr.data() << std::endl;
	}
	GLenum lGlError(glGetError());
	if (GL_NO_ERROR != lGlError) {
		std::cerr << "Compil shader error: " << gluErrorString(lGlError) << std::endl;
		lShaderHandle = GL_NONE;
	}
	return lShaderHandle;
}

bool ShaderManager::linkProgram(GLuint aGlProgram)
{
	glLinkProgram(aGlProgram);

	GLint lLogLength;
	glGetProgramiv(aGlProgram, GL_INFO_LOG_LENGTH, &lLogLength);

	if (lLogLength > 0) {
		std::vector<GLchar> lLogStr(lLogLength);

		glGetProgramInfoLog(aGlProgram, lLogLength, nullptr, &lLogStr[0]);

		std::cerr << "Link shader warnings: " << std::endl;
		std::cerr << lLogStr.data() << std::endl;
	}
	GLenum lGlError(glGetError());
	bool lResult = GL_NO_ERROR == lGlError;
	if (!lResult) {
		std::cerr << "Link shader error: " << gluErrorString(lGlError) << std::endl;
	}
	return lResult;
}

std::string ShaderManager::readFile(const std::string& aFilename) {
	std::stringstream lResult;
	std::ifstream lFileStream(aFilename);

	if (!lFileStream.is_open()) {
		std::cerr << "Could not read file '" << aFilename << "'. File does not exist." << std::endl;
		return "";
	}

	lResult << lFileStream.rdbuf();

	lFileStream.close();
	return lResult.str();
}
