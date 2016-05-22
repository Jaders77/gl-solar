#include "TextureManager.h"
#include <iostream>
#include <gl\GLU.h>

TextureManager::TextureManager()
	: mGlTexIndex(0)
{
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif
	glGenTextures(15, mGlTextures);
	GLenum lGlError(glGetError());
	if (GL_NO_ERROR != lGlError) {
		std::cerr << "Draw skybox error: " << gluErrorString(lGlError) << std::endl;
	}
}

TextureManager::~TextureManager()
{
	glDeleteTextures(15, mGlTextures);
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
}

bool TextureManager::loadTexture(const char* aFilename, GLuint& aTextureId)
{
	bool lResult = true;
	FREE_IMAGE_FORMAT lFiFormat = lFiFormat = FreeImage_GetFileType(aFilename, 0);
	if (lFiFormat == FIF_UNKNOWN) {
		lFiFormat = FreeImage_GetFIFFromFilename(aFilename);
	}
	lResult = lFiFormat != FIF_UNKNOWN;
	if (lResult)
	{
		lResult = TRUE == FreeImage_FIFSupportsReading(lFiFormat);
	}
	if (lResult)
	{
		FIBITMAP* lFiBitmap = FreeImage_Load(lFiFormat, aFilename);
		if (lResult)
		{
			BYTE* lFiBits = FreeImage_GetBits(lFiBitmap);
			unsigned int lFiWidth = FreeImage_GetWidth(lFiBitmap);
			unsigned int lFiHeight = FreeImage_GetHeight(lFiBitmap);
			lResult = (0 != lFiBits) && (lFiWidth > 0) && (lFiHeight > 0);
			if (lResult) {
				FREE_IMAGE_COLOR_TYPE lFiColorType = FreeImage_GetColorType(lFiBitmap);
				GLenum lGlError = GL_NO_ERROR;
				switch (lFiColorType) {
				case FIC_RGB:
					aTextureId = mGlTextures[mGlTexIndex++];
					glBindTexture(GL_TEXTURE_2D, aTextureId);
					//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					// Artefact on texture border with mipmaping
					//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, lFiWidth, lFiHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, lFiBits);
					//glGenerateMipmap(GL_TEXTURE_2D);
					break;
				case FIC_RGBALPHA:
					aTextureId = mGlTextures[mGlTexIndex++];
					glBindTexture(GL_TEXTURE_2D, aTextureId);
					//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, lFiWidth, lFiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, lFiBits);
					glGenerateMipmap(GL_TEXTURE_2D);
					break;
				default:
					lResult = false;
				}

			}
			FreeImage_Unload(lFiBitmap);
		}
	}
	GLenum lGlError(glGetError());
	if (GL_NO_ERROR != lGlError) {
		std::cerr << "Draw skybox error: " << gluErrorString(lGlError) << std::endl;
		lResult = false;
	}
	return lResult;
}

bool TextureManager::loadTextureSkyBox(const char* aFilenameLeft, const char* aFilenameFront, const char* aFilenameRight, const char* aFilenameTop, const char* aFilenameBottom, const char* aFilenameRear, GLuint& aTextureId)
{
	aTextureId = mGlTextures[mGlTexIndex++];
	glBindTexture(GL_TEXTURE_CUBE_MAP, aTextureId);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	bool lResult = loadTextureSkyBoxInternal(aFilenameLeft, aTextureId, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	lResult &= loadTextureSkyBoxInternal(aFilenameFront, aTextureId, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	lResult &= loadTextureSkyBoxInternal(aFilenameRight, aTextureId, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	lResult &= loadTextureSkyBoxInternal(aFilenameTop, aTextureId, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	lResult &= loadTextureSkyBoxInternal(aFilenameBottom, aTextureId, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	lResult &= loadTextureSkyBoxInternal(aFilenameRear, aTextureId, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	GLenum lGlError(glGetError());
	if (GL_NO_ERROR != lGlError) {
		std::cerr << "Draw skybox error: " << gluErrorString(lGlError) << std::endl;
		lResult = false;
	}
	return lResult;
}

bool TextureManager::loadTextureSkyBoxInternal(const char* aFilename, GLuint& aTextureId, GLenum aMapPos)
{
	bool lResult = true;
	FREE_IMAGE_FORMAT lFiFormat = lFiFormat = FreeImage_GetFileType(aFilename, 0);
	if (lFiFormat == FIF_UNKNOWN) {
		lFiFormat = FreeImage_GetFIFFromFilename(aFilename);
	}
	lResult = lFiFormat != FIF_UNKNOWN;
	if (lResult)
	{
		lResult = TRUE == FreeImage_FIFSupportsReading(lFiFormat);
	}
	if (lResult)
	{
		FIBITMAP* lFiBitmap = FreeImage_Load(lFiFormat, aFilename);
		if (lResult)
		{
			BYTE* lFiBits = FreeImage_GetBits(lFiBitmap);
			unsigned int lFiWidth = FreeImage_GetWidth(lFiBitmap);
			unsigned int lFiHeight = FreeImage_GetHeight(lFiBitmap);
			lResult = (0 != lFiBits) && (lFiWidth > 0) && (lFiHeight > 0);
			if (lResult) {
				FREE_IMAGE_COLOR_TYPE lFiColorType = FreeImage_GetColorType(lFiBitmap);
				GLenum lGlError = GL_NO_ERROR;
				switch (lFiColorType) {
				case FIC_RGB:
					glTexImage2D(aMapPos, 0, GL_RGB8, lFiWidth, lFiHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, lFiBits);
					break;
				case FIC_RGBALPHA:
					glTexImage2D(aMapPos, 0, GL_RGBA8, lFiWidth, lFiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, lFiBits);
					break;
				default:
					lResult = false;
				}

			}
			FreeImage_Unload(lFiBitmap);
		}
	}
	return lResult;
}
