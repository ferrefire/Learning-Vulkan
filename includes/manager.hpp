#pragma once

#include "window.hpp"
#include "graphics.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "camera.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Manager
{
	private:
        static bool glfwInitialized;
        static bool vulkanInitialized;

		static Window window;
		static Camera camera;
		static Device device;
		//static Pipeline pipeline;
		static std::vector<Pipeline> pipelines;
		static Graphics graphics;

	public:
		static Window &currentWindow;
		static Camera &currentCamera;
		static Device &currentDevice;
		static Graphics &currentGraphics;

		static void Start();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();

		static void Frame();

		static Window &GetWindow();
		static Graphics &GetGraphics();
};