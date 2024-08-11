#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <cstdlib>

#include "manager.hpp"
#include "time.hpp"
#include "input.hpp"

int main()
{
	Manager::Start();

	while (Manager::GetWindow().IsOpen())
	{
		Time::Frame();
		Input::Frame();
		Manager::Frame();

		glfwPollEvents();
	}

	Manager::Quit(EXIT_SUCCESS);
}