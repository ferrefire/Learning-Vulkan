#include "manager.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "input.hpp"

void Manager::Start()
{
	try
	{
		InitializeGLFW();
		InitializeVulkan();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		Quit(EXIT_FAILURE);
	}
}

void Manager::Clean()
{
	
}

void Manager::Quit(int exitCode)
{
	Clean();

	graphics.Destroy();

	if (glfwInitialized)
    {
        window.Destroy();
        glfwTerminate();
    }

	exit(exitCode);
}

void Manager::InitializeGLFW()
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize glfw library");
    }

    glfwInitialized = true;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window.Create();
}

void Manager::InitializeVulkan()
{
	graphics.Create();

	vulkanInitialized = true;

	std::cout << device.properties.deviceName << std::endl;
}

void Manager::Frame()
{
	if (Input::GetKey(GLFW_KEY_ESCAPE).pressed)
	{
		window.Close();
	}
}

Window &Manager::GetWindow()
{
	return (window);
}

Graphics &Manager::GetGraphics()
{
	return (graphics);
}

bool Manager::glfwInitialized = false;
bool Manager::vulkanInitialized = false;

Device Manager::device;
Window Manager::window{device};
Camera Manager::camera{window};
std::vector<Pipeline> Manager::pipelines(1, Pipeline{device, camera});
Graphics Manager::graphics{device, window, pipelines};

Device &Manager::currentDevice = Manager::device;
Window &Manager::currentWindow = Manager::window;
Camera &Manager::currentCamera = Manager::camera;
Graphics &Manager::currentGraphics = Manager::graphics;