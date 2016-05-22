#pragma once

#include <glad/glad.h>
#include "FreeImage.h"

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	bool loadTexture(const char* aFilename, GLuint& aTextureId);
	bool loadTextureSkyBox(const char* aFilenameLeft, const char* aFilenameFront, const char* aFilenameRight, const char* aFilenameTop, const char* aFilenameBottom, const char* aFilenameRear, GLuint& aTextureId);

private:
	bool loadTextureSkyBoxInternal(const char* aFilename, GLuint& aTextureId, GLenum aMapPos);

private:
	size_t mGlTexIndex = 0;
	GLuint mGlTextures[15];
};
