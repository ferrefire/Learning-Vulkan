#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "pipeline.hpp"
#include "window.hpp"

#include <vector>

class Graphics
{
    private:
		Device &device;
		Pipeline &pipeline;
		Window &window;

		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;

	public:
        Graphics(Device &device, Window &window, Pipeline &pipeline);
        ~Graphics();

		VkInstance instance = nullptr;

		//VkCommandPool commandPool = nullptr;
		//std::vector<VkCommandBuffer> commandBuffers;

		void CreateInstance();

		//void CreateCommandPool();
		//void CreateCommandBuffers();

		//void CreateTextureImage();
		//void CreateTextureImageView();
		//void CreateTextureSampler();

		void Create();

		void DestroyInstance();

		//void DestroyCommandPool();

		//void DestroyTextureImage();
		//void DestroyTextureImageView();
		//void DestroyTextureSampler();

		void Destroy();

		void DrawFrame();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		//void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		//void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		//VkCommandBuffer BeginSingleTimeCommands();
		//void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		//void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		//bool HasStencilComponent(VkFormat format);
};