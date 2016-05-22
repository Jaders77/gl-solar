#include "Planet.h"
#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <gl\GLU.h>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

//#define REALISTIC

#ifdef REALISTIC
const float gGeneralScale = 0.00000001f;
#else
const float gGeneralScale = 0.000001f;
//const float gGeneralScale = 0.0000001f;
#endif

Planet::Planet(const std::string& aName, float aRadius, float aOrbitalDistance, float aOrbitalEccentricity, float aOrbitalTilt, float aOrbitalPeriod, float aTilt, float aRotationPeriod, GLuint aTexture)
	: mName(aName)
#ifdef REALISTIC
	, mRadius(aRadius * gGeneralScale)
	, mOrbitalDistanceE(aOrbitalDistance * aOrbitalEccentricity * gGeneralScale)
	, mOrbitalDistanceW(aOrbitalDistance * gGeneralScale)
	, mOrbitalDistanceH(aOrbitalDistance * gGeneralScale * std::sqrt(1.f - aOrbitalEccentricity*aOrbitalEccentricity))
#else
	, mRadius(std::log10(aRadius * gGeneralScale + 1) + 0.001f)
	, mOrbitalDistanceE(std::log10(aOrbitalDistance * gGeneralScale * aOrbitalEccentricity + 1))
	, mOrbitalDistanceW(std::log10(aOrbitalDistance * gGeneralScale + 1))
	, mOrbitalDistanceH(std::log10(aOrbitalDistance * gGeneralScale * std::sqrt(1.f - aOrbitalEccentricity*aOrbitalEccentricity) + 1))
#endif
	, mOrbitalTilt(aOrbitalTilt)
	, mOrbitalPeriod((aOrbitalPeriod == 0) ? 0.f : glm::two_pi<float>() / aOrbitalPeriod)
	, mTilt(aTilt)
	, mRotationPeriod((aRotationPeriod == 0) ? 0.f : glm::two_pi<float>() / aRotationPeriod)
	, mTexture(aTexture)
	, mVaoSphere(GL_NONE)
	, mSphereTriangleNum(0)
	, mOrbitalCenter()
	, mPlanetCenter()
	, mSatellites()
{
	buildSphere(mRadius, 30, 30);
}

Planet::~Planet()
{
	glDeleteVertexArrays(1, &mVaoSphere);
}

const std::string& Planet::getName()
{
	return mName;
}

void Planet::addSatellite(const CPlanetPtr& aSatellite)
{
	mSatellites.push_back(aSatellite);
}

CPlanetPtr Planet::findPlanet(const std::string& aName)
{
	for (auto& lSatellitePtr : mSatellites) {
		if (aName == lSatellitePtr->getName())
		{
			return lSatellitePtr;
		}
		CPlanetPtr lCPlanetPtr = lSatellitePtr->findPlanet(aName);
		if (lCPlanetPtr) {
			return lCPlanetPtr;
		}
	}
	return nullptr;
}

void Planet::computePosition(double aTime, glutil::MatrixStack aMatrix)
{
	glutil::PushStack lStack(aMatrix);
	aMatrix.RotateZ(glm::radians(mOrbitalTilt));
	aMatrix.Translate(mOrbitalDistanceE, 0.f, 0.f);
	mOrbitalCenter = aMatrix;

	float lOrbitalAngle = static_cast<float>(aTime) * mOrbitalPeriod;
	aMatrix.Translate(glm::cos(lOrbitalAngle) * mOrbitalDistanceW, 0.f, glm::sin(lOrbitalAngle) * mOrbitalDistanceH);
	aMatrix.RotateZ(glm::radians(mTilt));

	for (auto& lSatellitePtr : mSatellites) {
		lSatellitePtr->computePosition(aTime, aMatrix);
	}

	aMatrix.RotateY(static_cast<float>(aTime) * mRotationPeriod);
	mPlanetCenter = aMatrix;
}

void Planet::getPlanetPosition(glm::mat4& aPosition)
{
	aPosition = mPlanetCenter.Top();
}

void Planet::draw(const glm::mat4& aProjectionMat, const glm::mat4& aLookMat, GLuint aGlPlanetProgram, GLuint aGlOrbitProgram)
{
	glUseProgram(aGlOrbitProgram);
	GLint lProjectionUni = glGetUniformLocation(aGlOrbitProgram, "proj_matrix");
	GLint lLookUni = glGetUniformLocation(aGlOrbitProgram, "look_matrix");
	GLint lOrbitUni = glGetUniformLocation(aGlOrbitProgram, "orbit_matrix");
	glUniformMatrix4fv(lProjectionUni, 1, GL_FALSE, glm::value_ptr(aProjectionMat));
	glUniformMatrix4fv(lLookUni, 1, GL_FALSE, glm::value_ptr(aLookMat));
	glUniformMatrix4fv(lOrbitUni, 1, GL_FALSE, glm::value_ptr(mOrbitalCenter.Top()));
	glBegin(GL_LINE_STRIP);
	for (float lOrbitalAngle = 0.f; lOrbitalAngle < glm::two_pi<float>(); lOrbitalAngle += 0.05f) {
		glVertex3f(glm::cos(lOrbitalAngle) * mOrbitalDistanceW, 0.f, glm::sin(lOrbitalAngle) * mOrbitalDistanceH);
	}
	glVertex3f(mOrbitalDistanceW, 0.f, 0.f);
	glEnd();
	glUseProgram(GL_NONE);

	glUseProgram(aGlPlanetProgram);
	lProjectionUni = glGetUniformLocation(aGlPlanetProgram, "proj_matrix");
	lLookUni = glGetUniformLocation(aGlPlanetProgram, "look_matrix");
	GLint lPlanetUni = glGetUniformLocation(aGlPlanetProgram, "planet_matrix");
	glUniformMatrix4fv(lProjectionUni, 1, GL_FALSE, glm::value_ptr(aProjectionMat));
	glUniformMatrix4fv(lLookUni, 1, GL_FALSE, glm::value_ptr(aLookMat));
	glUniformMatrix4fv(lPlanetUni, 1, GL_FALSE, glm::value_ptr(mPlanetCenter.Top()));
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glBindVertexArray(mVaoSphere);

	glDrawElements(GL_TRIANGLES, mSphereTriangleNum, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(GL_NONE);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(GL_NONE);

	GLenum lGlError(glGetError());
	if (GL_NO_ERROR != lGlError) {
		std::cerr << "Draw skybox error: " << gluErrorString(lGlError) << std::endl;
	}

	for (auto& lSatellite : mSatellites) {
		lSatellite->draw(aProjectionMat, aLookMat, aGlPlanetProgram, aGlOrbitProgram);
	}
}

void Planet::buildSphere(float aRadius, int aSlices, int aStacks)
{
	std::vector<glm::vec3> lPositions;
	std::vector<GLuint> lIndicies;

	for (int lStack = 0; lStack < (aStacks + 1); ++lStack)
	{
		float lPhi = lStack * glm::pi<float>() / static_cast<float>(aStacks - 1);
		float lPhiCos = cos(lPhi);
		float lPhiSin = sin(lPhi);
		for (int lSlice = 0; lSlice < (aSlices + 1); ++lSlice)
		{
			float lTheta = lSlice * glm::two_pi<float>() / static_cast<float>(aSlices);
			lPositions.push_back(glm::vec3(cos(lTheta) * lPhiSin, lPhiCos, sin(lTheta) * lPhiSin) * aRadius);
		}
	}
	for (int lQuad = 0; lQuad < aSlices * aStacks; ++lQuad)
	{
		lIndicies.push_back(lQuad);
		lIndicies.push_back(lQuad + aSlices + 1);
		lIndicies.push_back(lQuad + aSlices);

		lIndicies.push_back(lQuad + aSlices + 1);
		lIndicies.push_back(lQuad);
		lIndicies.push_back(lQuad + 1);
	}
	glGenVertexArrays(1, &mVaoSphere);
	glBindVertexArray(mVaoSphere);

	GLuint lVbos[2];
	glGenBuffers(2, lVbos);

	glBindBuffer(GL_ARRAY_BUFFER, lVbos[0]);
	glBufferData(GL_ARRAY_BUFFER, lPositions.size() * sizeof(glm::vec3), lPositions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lVbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lIndicies.size() * sizeof(GLuint), lIndicies.data(), GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	mSphereTriangleNum = lIndicies.size();
}
