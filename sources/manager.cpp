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

	if (vulkanInitialized) 
    {
        graphics.Destroy();
    }

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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window.Create();
}

void Manager::InitializeVulkan()
{
	graphics.CreateInstance();

    vulkanInitialized = true;

    graphics.CreateSurface();
    graphics.PickPhysicalDevice();
    graphics.CreateLogicalDevice();
    graphics.CreateSwapChain();

	std::cout << graphics.properties.deviceName << std::endl;
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

bool Manager::glfwInitialized = false;
bool Manager::vulkanInitialized = false;

Window Manager::window;
Graphics Manager::graphics;