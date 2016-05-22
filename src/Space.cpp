#include "Space.h"
#include <iostream>
#include <gl\GLU.h>
#include "ShaderManager.h"

Space::Space()
	: mPlanetarySystems()
	, mTextureManager()
	, mGlSkyboxProgram(glCreateProgram())
	, mGlPlanetProgram(glCreateProgram())
	, mGlOrbitProgram(glCreateProgram())
	, mTextureSkyBox(GL_NONE)
{
	buildShaders();
	buildSolarSystem();
}


Space::~Space()
{
	glDeleteProgram(mGlOrbitProgram);
	glDeleteProgram(mGlPlanetProgram);
	glDeleteProgram(mGlSkyboxProgram);
}

void Space::addPlanetarySystem(const CPlanetPtr& aPlanetarySystem)
{
	mPlanetarySystems.push_back(aPlanetarySystem);
}

CPlanetPtr Space::findPlanet(const std::string& aName)
{
	for (auto& lPlanetarySystemsPtr : mPlanetarySystems) {
		if (aName == lPlanetarySystemsPtr->getName())
		{
			return lPlanetarySystemsPtr;
		}
		CPlanetPtr lCPlanetPtr = lPlanetarySystemsPtr->findPlanet(aName);
		if (lCPlanetPtr) {
			return lCPlanetPtr;
		}
	}
	return nullptr;
}

void Space::computePosition(double aTime)
{
	glutil::MatrixStack lMatrix;
	for (auto& lPlanetarySystems : mPlanetarySystems) {
		lPlanetarySystems->computePosition(aTime, lMatrix);
	}
}

void Space::draw(const glm::mat4& aProjectionMat, const glm::mat4& aLookMat)
{
	drawSky(aProjectionMat, aLookMat);
	for (auto& lPlanetarySystems : mPlanetarySystems) {
		lPlanetarySystems->draw(aProjectionMat, aLookMat, mGlPlanetProgram, mGlOrbitProgram);
	}
}

void Space::buildShaders()
{
	GLuint lSkyboxVertexShader = ShaderManager::createShader(GL_VERTEX_SHADER, "shader/skybox_vertex.glsl");
	GLuint lSkyboxFragmentShader = ShaderManager::createShader(GL_FRAGMENT_SHADER, "shader/skybox_fragment.glsl");
	glAttachShader(mGlSkyboxProgram, lSkyboxVertexShader);
	glAttachShader(mGlSkyboxProgram, lSkyboxFragmentShader);
	ShaderManager::linkProgram(mGlSkyboxProgram);
	glDeleteShader(lSkyboxVertexShader);
	glDeleteShader(lSkyboxFragmentShader);

	GLuint lPlanetVertexShader = ShaderManager::createShader(GL_VERTEX_SHADER, "shader/planet_vertex.glsl");
	GLuint lPlanetFragmentShader = ShaderManager::createShader(GL_FRAGMENT_SHADER, "shader/planet_fragment.glsl");
	glAttachShader(mGlPlanetProgram, lPlanetVertexShader);
	glAttachShader(mGlPlanetProgram, lPlanetFragmentShader);
	ShaderManager::linkProgram(mGlPlanetProgram);
	glDeleteShader(lPlanetVertexShader);
	glDeleteShader(lPlanetFragmentShader);

	GLuint lOrbitVertexShader = ShaderManager::createShader(GL_VERTEX_SHADER, "shader/orbit_vertex.glsl");
	GLuint lOrbitFragmentShader = ShaderManager::createShader(GL_FRAGMENT_SHADER, "shader/orbit_fragment.glsl");
	glAttachShader(mGlOrbitProgram, lOrbitVertexShader);
	glAttachShader(mGlOrbitProgram, lOrbitFragmentShader);
	ShaderManager::linkProgram(mGlOrbitProgram);
	glDeleteShader(lOrbitVertexShader);
	glDeleteShader(lOrbitFragmentShader);
}

void Space::buildSolarSystem()
{
	mTextureManager.loadTextureSkyBox("res/milkywayleft.png", "res/milkywayfront.png", "res/milkywayright.png", "res/milkywaytop.png", "res/milkywaybottom.png", "res/milkywayback.png", mTextureSkyBox);
	GLuint lTexture = 0;
	if (mTextureManager.loadTexture("res/sun_2k.jpg", lTexture)) {
		CPlanetPtr lSunPtr(std::make_shared<Planet>("Sun", 695508.f, 0.f, 0.f, 0.f, 0.f, 7.25f, -26.f, lTexture));
		addPlanetarySystem(lSunPtr);

		if (mTextureManager.loadTexture("res/mercury_2k.jpg", lTexture)) {
			CPlanetPtr MercuryPtr(std::make_shared<Planet>("Mercury", 2439.7f, 57909227.f, 0.2056f, 7.005f, 87.9693f, 0.0352f, 58.6462f, lTexture));
			lSunPtr->addSatellite(MercuryPtr);
		}

		if (mTextureManager.loadTexture("res/venus_2k.jpg", lTexture)) {
			CPlanetPtr lVenusPtr(std::make_shared<Planet>("Venus", 6051.8f, 108208475.f, 0.0068f, 3.39f, 224.701f, 177.36f, -243.018f, lTexture));
			lSunPtr->addSatellite(lVenusPtr);
		}

		if (mTextureManager.loadTexture("res/earth_daymap_2k.jpg", lTexture)) {
			CPlanetPtr lEarthPtr(std::make_shared<Planet>("Earth", 6371.f, 149598262.f, 0.0167f, 0.f, 365.2569f, 23.4393f, 0.9972f, lTexture));
			lSunPtr->addSatellite(lEarthPtr);
			if (mTextureManager.loadTexture("res/moon_2k.jpg", lTexture)) {
				CPlanetPtr MoonPtr(std::make_shared<Planet>("Moon", 1737.f, 384400.f, 0.0549f, 5.16f, 27.322f, 6.68f, 27.322f, lTexture));
				lEarthPtr->addSatellite(MoonPtr);
			}
		}

		if (mTextureManager.loadTexture("res/mars_2k.jpg", lTexture)) {
			CPlanetPtr lMarsPtr(std::make_shared<Planet>("Mars", 3389.5f, 227943824.f, 0.0934f, 1.8506f, 686.9601f, 25.19f, 1.0259f, lTexture));
			lSunPtr->addSatellite(lMarsPtr);
			if (mTextureManager.loadTexture("res/phobos.png", lTexture)) {
				CPlanetPtr lPhobosPtr(std::make_shared<Planet>("Phobos", 11.1f, 9376.f, 0.0151f, 1.075f, 0.3189f, 0.f, 0.f, lTexture));
				lMarsPtr->addSatellite(lPhobosPtr);
			}
			if (mTextureManager.loadTexture("res/deimos.png", lTexture)) {
				CPlanetPtr lDeimosPtr(std::make_shared<Planet>("Deimos", 6.2f, 23458.f, 0.0002f, 1.788f, 1.2624f, 0.f, 0.f, lTexture));
				lMarsPtr->addSatellite(lDeimosPtr);
			}
		}

		if (mTextureManager.loadTexture("res/ceres_2k.jpg", lTexture)) {
			CPlanetPtr lCeresPtr(std::make_shared<Planet>("Ceres", 476.2f, 413690250.f, 0.0797f, 10.59f, 1679.819f, 3.f, 0.378f, lTexture));
			lSunPtr->addSatellite(lCeresPtr);
		}

		//Todo: asteroids

		if (mTextureManager.loadTexture("res/jupiter_2k.jpg", lTexture)) {
			CPlanetPtr lJupiterPtr(std::make_shared<Planet>("Jupiter", 69911.f, 778340821.f, 0.0484f, 1.304f, 4335.3545f, 3.12f, 0.4135f, lTexture));
			lSunPtr->addSatellite(lJupiterPtr);
			// Todo: satellites
		}

		if (mTextureManager.loadTexture("res/saturn_2k.jpg", lTexture)) {
			CPlanetPtr lSaturnPtr(std::make_shared<Planet>("Saturn", 58232.f, 1426666422.f, 0.0538f, 2.4845f, 10757.7365f, 26.73f, 0.444f, lTexture));
			lSunPtr->addSatellite(lSaturnPtr);
			// Todo: satellites
		}

		if (mTextureManager.loadTexture("res/uranus_2k.jpg", lTexture)) {
			CPlanetPtr lUranusPtr(std::make_shared<Planet>("Uranus", 25362.f, 2870658186.f, 0.0472f, 0.7725f, 30687.15f, 97.77f, -0.7183f, lTexture));
			lSunPtr->addSatellite(lUranusPtr);
			// Todo: satellites
		}

		if (mTextureManager.loadTexture("res/neptune_2k.jpg", lTexture)) {
			CPlanetPtr lNeptunePtr(std::make_shared<Planet>("Neptune", 24622.f, 4498396441.f, 0.0086f, 1.7692f, 60224.9036f, 28.3f, 0.6712f, lTexture));
			lSunPtr->addSatellite(lNeptunePtr);
			// Todo: satellites
		}

		if (mTextureManager.loadTexture("res/pluto.png", lTexture)) {
			CPlanetPtr lPlutoPtr(std::make_shared<Planet>("Pluto", 1185.f, 5906440628.f, 0.2488f, 17.1417f, 90553.02f, 122.5f, -6.387f, lTexture));
			lSunPtr->addSatellite(lPlutoPtr);
			// Todo: satellites
		}
	}
}

void Space::drawSky(const glm::mat4& aProjectionMat, const glm::mat4& aLookMat)
{
	GLfloat lSkyboxSize = 100.f;
	glUseProgram(mGlSkyboxProgram);
	GLint lProjectionUni = glGetUniformLocation(mGlSkyboxProgram, "proj_matrix");
	GLint lLookUni = glGetUniformLocation(mGlSkyboxProgram, "look_matrix");
	glUniformMatrix4fv(lProjectionUni, 1, GL_FALSE, glm::value_ptr(aProjectionMat));
	glUniformMatrix4fv(lLookUni, 1, GL_FALSE, glm::value_ptr(aLookMat));
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureSkyBox);
	glBegin(GL_QUADS);
	// back
	glVertex3f(-lSkyboxSize, -lSkyboxSize, lSkyboxSize);
	glVertex3f(lSkyboxSize, -lSkyboxSize, lSkyboxSize);
	glVertex3f(lSkyboxSize, lSkyboxSize, lSkyboxSize);
	glVertex3f(-lSkyboxSize, lSkyboxSize, lSkyboxSize);
	// right
	glVertex3f(lSkyboxSize, lSkyboxSize, lSkyboxSize);
	glVertex3f(lSkyboxSize, lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, -lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, -lSkyboxSize, lSkyboxSize);
	// front
	glVertex3f(lSkyboxSize, lSkyboxSize, -lSkyboxSize);
	glVertex3f(-lSkyboxSize, lSkyboxSize, -lSkyboxSize);
	glVertex3f(-lSkyboxSize, -lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, -lSkyboxSize, -lSkyboxSize);
	// left
	glVertex3f(-lSkyboxSize, -lSkyboxSize, -lSkyboxSize);
	glVertex3f(-lSkyboxSize, -lSkyboxSize, lSkyboxSize);
	glVertex3f(-lSkyboxSize, lSkyboxSize, lSkyboxSize);
	glVertex3f(-lSkyboxSize, lSkyboxSize, -lSkyboxSize);
	// top
	glVertex3f(-lSkyboxSize, lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, lSkyboxSize, lSkyboxSize);
	glVertex3f(-lSkyboxSize, lSkyboxSize, lSkyboxSize);
	// bottom
	glVertex3f(-lSkyboxSize, -lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, -lSkyboxSize, -lSkyboxSize);
	glVertex3f(lSkyboxSize, -lSkyboxSize, lSkyboxSize);
	glVertex3f(-lSkyboxSize, -lSkyboxSize, lSkyboxSize);
	glEnd();

	glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
	glDisable(GL_TEXTURE_CUBE_MAP);
	glUseProgram(GL_NONE);
	GLenum lGlError(glGetError());
	if (GL_NO_ERROR != lGlError) {
		std::cerr << "Draw skybox error: " << gluErrorString(lGlError) << std::endl;
	}
}
