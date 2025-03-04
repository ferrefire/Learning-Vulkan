#include "manager.hpp"
#include "time.hpp"
#include "input.hpp"
#include "utilities.hpp"
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
		else if (std::string(argv[i]).compare("vl") == 0)
		{
			Manager::settings.validationLayersActive = false;
		}
		else if (std::string(argv[i]).compare("uc") == 0)
		{
			Manager::settings.uncappedFPS = true;
		}
		else if (std::string(argv[i]).compare("pf") == 0)
		{
			Manager::settings.performanceMode = true;
		}
		else if (std::string(argv[i]).compare(0, 4, "cin=") == 0)
		{
			std::string arg = argv[i];
			Manager::cinematic.Load((Utilities::GetPath() + "/cinematics/" + (argv[i] + arg.find('=') + 1) + ".txt").c_str());
			Manager::cinematic.speed = 1;
		}
		else if (std::string(argv[i]).compare(0, 5, "qcin=") == 0)
		{
			std::string arg = argv[i];
			Manager::cinematic.Load((Utilities::GetPath() + "/cinematics/" + (argv[i] + arg.find('=') + 1) + ".txt").c_str());
			Manager::cinematic.speed = 4;
		}
		else if (std::string(argv[i]).compare(0, 7, "newcin=") == 0)
		{
			std::string arg = argv[i];
			Manager::cinematic.name = (Utilities::GetPath() + "/cinematics/" + (argv[i] + arg.find('=') + 1) + ".txt");
			Manager::cinematic.creating = true;
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

		Manager::currentGraphics.Frame();

		Manager::PostFrame();

		glfwPollEvents();

		//Manager::objects[1]->Rotate(glm::vec3(100.0f * Time::deltaTime, 50.0f * Time::deltaTime, 25.0f * Time::deltaTime));
	}

	Manager::currentDevice.WaitForIdle();

	Manager::Quit(EXIT_SUCCESS);
}