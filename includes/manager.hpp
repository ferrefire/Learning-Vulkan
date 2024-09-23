#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "graphics.hpp"
#include "device.hpp"
#include "pipeline.hpp"

class Manager
{
	private:
        static bool glfwInitialized;
        static bool vulkanInitialized;

		static Window window;
		static Device device;
		static Pipeline pipeline;
        static Graphics graphics;

	public:
		static Window *currentWindow;
		static Device *currentDevice;
		static Graphics *currentGraphics;

		static void Start();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();

		static void Frame();

		static Window &GetWindow();
		static Graphics &GetGraphics();
};