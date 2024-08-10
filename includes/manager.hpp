#ifndef MANAGER_HPP
#define MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"

class Manager
{
	private:
		static unsigned int width;
		static unsigned int height;

		static Window window;

	public:
		static VkInstance instance;

		static void Start();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();

		static void CreateVulkanInstance();
		static void CreateVulkanSurface();

		static void Frame();

		static Window &GetWindow();
};

#endif