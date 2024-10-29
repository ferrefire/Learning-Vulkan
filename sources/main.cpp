#include "manager.hpp"
#include "time.hpp"
#include "input.hpp"
//#include "mesh.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

void Setup(int argc, char **argv)
{
	stbi_set_flip_vertically_on_load(true);

	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]).compare("wf") == 0)
		{
			Manager::settings.wireframe = true;
		}
		else if (std::string(argv[i]).compare("fs") == 0)
		{
			Manager::settings.fullscreen = true;
		}
		else if (std::string(argv[i]).compare("dc") == 0)
		{
			Manager::settings.discrete = false;
		}
		else if (std::string(argv[i]).compare("ms") == 0)
		{
			Manager::settings.mssa = true;
		}
	}
}

int main(int argc, char **argv)
{
	Setup(argc, argv);

	Manager::Setup();
	Manager::Start();

	while (Manager::currentWindow.IsOpen())
	{
		Time::Frame();
		Input::Frame();
		Manager::Frame();

		glfwPollEvents();

		Manager::currentGraphics.DrawFrame();

		Manager::PostFrame();

		//Manager::objects[1]->Rotate(glm::vec3(100.0f * Time::deltaTime, 50.0f * Time::deltaTime, 25.0f * Time::deltaTime));
	}

	Manager::currentDevice.WaitForIdle();

	Manager::Quit(EXIT_SUCCESS);
}