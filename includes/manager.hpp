#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "window.hpp"
#include "graphics.hpp"

class Manager
{
	private:
        static bool glfwInitialized;
        static bool vulkanInitialized;

		static Window window;
        static Graphics graphics;

	public:
		static void Start();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();

		static void Frame();

		static Window &GetWindow();
};

#endif