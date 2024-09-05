#ifndef MANAGER_HPP
#define MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "graphics.hpp"
#include "pipeline.hpp"

class Manager
{
	private:
        static bool glfwInitialized;
        static bool vulkanInitialized;

		static Window window;
        static Graphics graphics;
		static Pipeline pipeline;

	public:
		static void Start();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();

		static void Frame();

		static Window &GetWindow();
		static Graphics &GetGraphics();
};

#endif