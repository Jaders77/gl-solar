#pragma once
#include "Planet.h"
#include <vector>
#include "glm/glm.hpp"
#include "TextureManager.h"

class Space
{
public:
	Space();
	~Space();

	void addPlanetarySystem(const CPlanetPtr& aPlanetarySystem);
	CPlanetPtr findPlanet(const std::string& aName);
	void computePosition(double aTime);
	void draw(const glm::mat4& aProjectionMat, const glm::mat4& aLookMat);

private:
	void buildShaders();
	void buildSkybox();
	void buildSolarSystem();
	void drawSky(const glm::mat4& aProjectionMat, const glm::mat4& aLookMat);

private:
	std::vector<CPlanetPtr> mPlanetarySystems;
	TextureManager mTextureManager;
	GLuint mGlSkyboxProgram;
	GLuint mGlPlanetProgram;
	GLuint mGlOrbitProgram;
	GLuint mTextureSkyBox;
};

