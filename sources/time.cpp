#include "time.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <chrono>
#include <ctime>
#include <string>

#include "manager.hpp"

void Time::Frame()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

	frameCountThisTick++;
	frameTimeThisTick += deltaTime;

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

		frameCountLastTick = frameCountThisTick;
		frameTimeLastTick = frameTimeThisTick;
		frameCountThisTick = 0;
		frameTimeThisTick = 0;
	}

	if (newSubTick) newSubTick = false;
	if (currentFrame - timeLastSubTick > 0.25)
	{
		newSubTick = true;
		timeLastSubTick = currentFrame;
	}

	if (newFrameTick) newFrameTick = false;
	if (currentFrame - timeLastFrameTick > 0.01666666)
	{
		newFrameTick = true;
		timeLastFrameTick = currentFrame;
	}

	Time::CalculateFPS();
}

unsigned int Time::GetTime()
{
	std::time_t result = std::time(nullptr);
	return result;
}

void Time::CalculateFPS()
{
	if (Time::newSubTick)
	{
		currentFPS = int(frameCountLastTick / frameTimeLastTick);
		if (!Manager::settings.fullscreen)
		{
			glfwSetWindowTitle(Manager::currentWindow.data, std::to_string(currentFPS).c_str());
		}
	}
}

float Time::deltaTime = 0;
float Time::currentFrame = 0;
float Time::lastFrame = 0;

double Time::timeLastSecond = 0;
double Time::timeLastTick = 0;
double Time::timeLastSubTick = 0;
double Time::timeLastFrameTick = 0;

int Time::frameCountThisTick = 0;
float Time::frameTimeThisTick = 0;
int Time::frameCountLastTick = 0;
float Time::frameTimeLastTick = 0;

bool Time::newSecond = false;
bool Time::newTick = false;
bool Time::newSubTick = false;
bool Time::newFrameTick = false;

int Time::currentFPS = 0;