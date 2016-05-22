#pragma once
#include <string>
#include "glad/glad.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	static GLuint createShader(GLenum aShaderType, const std::string& aFilename);
	static bool linkProgram(GLuint aGlProgram);

private:
	static std::string readFile(const std::string& aFilename);
};
