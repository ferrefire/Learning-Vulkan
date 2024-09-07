#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>

#include "manager.hpp"
#include "time.hpp"
#include "input.hpp"
#include "mesh.hpp"

void Setup()
{
	Mesh::graphics = Manager::currentGraphics;
}

int main()
{
	Setup();
	Manager::Start();

	while (Manager::currentWindow->IsOpen())
	{
		Time::Frame();
		Input::Frame();
		Manager::Frame();

		glfwPollEvents();

		Manager::currentGraphics->DrawFrame();
	}

	vkDeviceWaitIdle(Manager::currentGraphics->device);

	Manager::Quit(EXIT_SUCCESS);
}