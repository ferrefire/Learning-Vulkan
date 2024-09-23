#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "pipeline.hpp"

#include <vector>

class Graphics
{
    private:
		Device &device;
		Pipeline &pipeline;

		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;

	public:
        Graphics(Device &device, Pipeline &pipeline);
        ~Graphics();

		VkInstance instance = nullptr;
		
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
		VkRenderPass renderPass = nullptr;

		bool framebufferResized = false;
		VkCommandPool commandPool = nullptr;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkImage depthImage = nullptr;
		VkDeviceMemory depthImageMemory = nullptr;
		VkImageView depthImageView = nullptr;

		void CreateInstance();

		void CreateCommandPool();
		void CreateCommandBuffers();
        void CreateSurface();
        void CreateSwapChain();
		void CreateImageViews();

		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateDepthResources();
		void Create();

		void DestroyInstance();
        void DestroySurface();
        void DestroySwapChain();
		void DestroyImageViews();

		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyCommandPool();
		void DestroyTextureImage();
		void DestroyTextureImageView();
		void DestroyTextureSampler();
		void DestroyDepthResources();
		void Destroy();

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecreateSwapChain();
		void DrawFrame();

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
};