#include "manager.hpp"
#include "time.hpp"
#include "input.hpp"
//#include "mesh.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>

int main()
{
	Manager::Start();

	while (Manager::currentWindow.IsOpen())
	{
		Time::Frame();
		Input::Frame();
		Manager::Frame();

		glfwPollEvents();

		Manager::currentGraphics.DrawFrame();
	}

	Manager::currentDevice.WaitForIdle();

	Manager::Quit(EXIT_SUCCESS);
}