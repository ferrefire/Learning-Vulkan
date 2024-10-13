#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "pipeline.hpp"
#include "mesh.hpp"
#include "window.hpp"

#include <vector>

class Graphics
{
    private:
		Device &device;
		//Pipeline &pipeline;
		Window &window;
		//std::vector<Pipeline> &pipelines;

		const int MAX_FRAMES_IN_FLIGHT = 2;
		//uint32_t currentFrame = 0;

	public:
		//Graphics(Device &device, Window &window, Pipeline &pipeline);
		Graphics(Device &device, Window &window);
		~Graphics();

		VkInstance instance = nullptr;

		void CreateInstance();
		void Create();

		void DestroyInstance();
		void Destroy();

		void DrawFrame();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};