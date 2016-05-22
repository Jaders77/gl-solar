#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glad/glad.h>
#include "MatrixStack.h"

typedef std::shared_ptr<class Planet> CPlanetPtr;

class Planet
{
public:
	Planet(const std::string& aName, float aRadius, float aOrbitalDistance, float aOrbitalEccentricity, float aOrbitalTilt, float aOrbitalPeriod, float aTilt, float aRotationPeriod, GLuint aTexture);
	~Planet();

	const std::string& getName();
	void addSatellite(const CPlanetPtr & aSatellite);
	CPlanetPtr findPlanet(const std::string& aName);
	void computePosition(double aTime, glutil::MatrixStack aMatrix);
	void getPlanetPosition(glm::mat4& aPosition);
	void draw(const glm::mat4& aProjectionMat, const glm::mat4& aLookMat, GLuint aGlPlanetProgram, GLuint aGlOrbitProgram);

private:
	void buildSphere(float aRadius, int aSlices, int aStacks);

private:
	std::string mName;
	float mRadius;
	float mOrbitalDistanceE;
	float mOrbitalDistanceW;
	float mOrbitalDistanceH;
	float mOrbitalTilt;
	float mOrbitalPeriod;
	float mTilt;
	float mRotationPeriod;
	GLuint mTexture;
	GLuint mVaoSphere;
	GLsizei mSphereTriangleNum;
	glutil::MatrixStack mOrbitalCenter;
	glutil::MatrixStack mPlanetCenter;
	std::vector<CPlanetPtr> mSatellites;
};

