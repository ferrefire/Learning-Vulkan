#include "time.hpp"

#include "GLFW/glfw3.h"

#include <chrono>
#include <ctime>

void Time::Frame()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

	if (newSecond) newSecond = false;
	if (currentFrame - timeLastSecond > 1.0)
	{
		newSecond = true;
		timeLastSecond = currentFrame;
	}

	if (newTick) newTick = false;
	if (currentFrame - timeLastTick > 0.1)
	{
		newTick = true;
		timeLastTick = currentFrame;
	}

	if (newSubTick) newSubTick = false;
	if (currentFrame - timeLastSubTick > 0.25)
	{
		newSubTick = true;
		timeLastSubTick = currentFrame;
	}

	framesSinceLastFrameTick++;
	if (newFrameTick) newFrameTick = false;
	if (currentFrame - timeLastFrameTick > 0.01666666)
	{
		newFrameTick = true;
		timeLastFrameTick = currentFrame;
		framesSinceLastFrameTick = 0;
	}
}

unsigned int Time::GetTime()
{
	std::time_t result = std::time(nullptr);
	return result;
}

float Time::deltaTime = 0;
float Time::currentFrame = 0;
float Time::lastFrame = 0;
double Time::timeLastSecond = 0;
double Time::timeLastTick = 0;
double Time::timeLastSubTick = 0;
double Time::timeLastFrameTick = 0;
int Time::framesSinceLastFrameTick = 0;

bool Time::newSecond = false;
bool Time::newTick = false;
bool Time::newSubTick = false;
bool Time::newFrameTick = false;