#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Space.h"

double gZoomDistance = .01;

const char* gPanetName[] = {
	"Sun",
	"Mercury",
	"Venus",
	"Earth",
	"Moon",
	"Mars",

	//"Phobos",
	//"Deimos",
	//"Ceres",

	"Jupiter",
	"Saturn",
	"Uranus",
	"Neptune"
	//"Pluto"
};
size_t gPlanet = 4;

static void GlfwErrorCallback(int aError, const char* aDescription)
{
	std::cerr << aDescription << std::endl;
}

static void GlfwKeyCallback(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aMods)
{
	if (aAction == GLFW_PRESS) {
		switch (aKey) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(aWindow, GL_TRUE);
			break;
		case GLFW_KEY_1:
			gPlanet = 0;
			break;
		case GLFW_KEY_2:
			gPlanet = 1;
			break;
		case GLFW_KEY_3:
			gPlanet = 2;
			break;
		case GLFW_KEY_4:
			gPlanet = 3;
			break;
		case GLFW_KEY_5:
			gPlanet = 4;
			break;
		case GLFW_KEY_6:
			gPlanet = 5;
			break;
		case GLFW_KEY_7:
			gPlanet = 6;
			break;
		case GLFW_KEY_8:
			gPlanet = 7;
			break;
		case GLFW_KEY_9:
			gPlanet = 8;
			break;
		case GLFW_KEY_0:
			gPlanet = 9;
			break;
		}
	}
}

void GlfwScrollCallback(GLFWwindow* aWindow, double aXOffset, double aYOffset)
{
	gZoomDistance = std::min(150., std::max(0.0001, gZoomDistance * std::exp(aYOffset / 10.)));
}

int main(int argc, char** argv)
{
	GLFWwindow* lGlfwWindow;
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	lGlfwWindow = glfwCreateWindow(1920, 1080, argv[0], NULL, NULL);
	if (!lGlfwWindow) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(lGlfwWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
	glfwSetKeyCallback(lGlfwWindow, GlfwKeyCallback);
	glfwSetScrollCallback(lGlfwWindow, GlfwScrollCallback);
	glfwSetInputMode(lGlfwWindow, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glm::dvec2 lMouseOld;
	glm::vec2 lPosition(0.f, 0.f);
	Space lSpace;
	double TimeStart = glfwGetTime();
	while (!glfwWindowShouldClose(lGlfwWindow)) {
		int Width, Height;
		glfwGetFramebufferSize(lGlfwWindow, &Width, &Height);
		float Ratio = Width / (float)Height;
		glViewport(0, 0, Width, Height);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 lProjectionMat = glm::perspective(glm::radians(60.f), Ratio, 0.00001f, 400.f);

		// Compute position
		double lTime = glfwGetTime() - TimeStart;
		lSpace.computePosition(lTime * .01);

		// Compute mouse action
		if (glfwGetMouseButton(lGlfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			glm::dvec2 lMouse;
			glfwGetCursorPos(lGlfwWindow, &lMouse.x, &lMouse.y);
			if (lMouseOld.x == 0.) {
				lMouseOld = lMouse;
			}
			lPosition += (lMouse - lMouseOld);
			lMouseOld = lMouse;
		}
		else {
			lMouseOld = glm::dvec2();
		}

		// Selected planet
		CPlanetPtr lSelectedPlanetPtr = lSpace.findPlanet(gPanetName[gPlanet]);
		glm::mat4 lPlanetMat(1.0);
		if (lSelectedPlanetPtr) {
			lSelectedPlanetPtr->getPlanetPosition(lPlanetMat);
		}

		// Compute scene center position
		glm::vec3 lPlanetPosition = lPlanetMat * glm::vec4(0.f, 0.f, 0.f, 1.f);
		glm::vec3 lEyeUp = glm::normalize(lPlanetMat * glm::vec4(0.f, 1.f, 0.f, 0.f));

		// Compute camera position
		lPlanetMat = glm::rotate(lPlanetMat, glm::radians(lPosition.x), glm::vec3(0.0f, 1.0f, 0.0f));
		lPlanetMat = glm::rotate(lPlanetMat, glm::radians(lPosition.y), glm::vec3(0.0f, 0.0f, 1.0f));
		lPlanetMat = glm::translate(lPlanetMat, glm::vec3((float)gZoomDistance, 0.f, 0.f));
		glm::vec3 lEye = lPlanetMat * glm::vec4(0.f, 0.f, 0.f, 1.f);

		// Render
		glm::mat4 lLookMat = glm::lookAt(lEye, lPlanetPosition, lEyeUp);
		glEnable(GL_DEPTH_TEST);
		lSpace.draw(lProjectionMat, lLookMat);
		glDisable(GL_DEPTH_TEST);


		glfwSwapBuffers(lGlfwWindow);
		glfwPollEvents();
	}
	glfwDestroyWindow(lGlfwWindow);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	return 0;
}
