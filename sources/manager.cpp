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

	vkDestroyInstance(instance, nullptr);

	window.Destroy();
	glfwTerminate();

	exit(exitCode);
}

void Manager::InitializeGLFW()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//window = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);
	window.Create();
}

void Manager::InitializeVulkan()
{
	CreateVulkanInstance();
}

void Manager::CreateVulkanInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Limitless";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
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

Window Manager::window;
unsigned int Manager::width = 800;
unsigned int Manager::height = 600;

VkInstance Manager::instance;