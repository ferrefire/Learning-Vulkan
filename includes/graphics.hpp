#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"

#include <vector>

const int MAX_FRAMES_IN_FLIGHT = 2;

class Graphics
{
    private:
		uint32_t currentFrame = 0;

	public:
        Graphics();
        ~Graphics();

		/*
        struct QueueFamilies
        {
            uint32_t graphicsFamily = 0;
            bool graphicsFamilyFound = false;

            uint32_t presentationFamily = 0;
            bool presentationFamilyFound = false;

            bool Complete()
            {
                return (graphicsFamilyFound && presentationFamilyFound);
            }
        };

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		*/

		VkInstance instance = nullptr;

		Device device;

        //VkPhysicalDevice physicalDevice = nullptr;
        //VkDevice device = nullptr;
        //VkQueue graphicsQueue = nullptr;
        //VkQueue presentationQueue = nullptr;
		//VkPhysicalDeviceProperties properties;
		//const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		//std::vector<VkSemaphore> imageAvailableSemaphores;
		//std::vector<VkSemaphore> renderFinishedSemaphores;
		//std::vector<VkFence> inFlightFences;
		
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
		VkRenderPass renderPass = nullptr;
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;
		bool framebufferResized = false;
		VkCommandPool commandPool = nullptr;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void *> uniformBuffersMapped;

		VkImage textureImage = nullptr;
		VkDeviceMemory textureImageMemory = nullptr;
		VkImageView textureImageView = nullptr;
		VkSampler textureSampler = nullptr;

		VkImage depthImage = nullptr;
		VkDeviceMemory depthImageMemory = nullptr;
		VkImageView depthImageView = nullptr;

		void CreateInstance();

        //void CreateLogicalDevice();
		//void CreateSyncObjects();

		void CreateCommandPool();
		void CreateCommandBuffers();
        void CreateSurface();
        void CreateSwapChain();
		void CreateImageViews();
		void CreateGraphicsPipeline();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateDescriptorSetLayout();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateDepthResources();
		void Create();

		void DestroyInstance();

        //void DestroyDevice();
		//void DestroySyncObjects();

        void DestroySurface();
        void DestroySwapChain();
		void DestroyImageViews();
		void DestroyGraphicsPipeline();
		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyCommandPool();
		void DestroyDescriptorSetLayout();
		void DestroyUniformBuffers();
		void DestroyDescriptorPool();
		void DestroyTextureImage();
		void DestroyTextureImageView();
		void DestroyTextureSampler();
		void DestroyDepthResources();
		void Destroy();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		//void PickPhysicalDevice();
		//bool IsDeviceSuitable(VkPhysicalDevice device);
		//QueueFamilies FindQueueFamilies(VkPhysicalDevice device);
		//bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		//SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecreateSwapChain();
		void DrawFrame();
		void UpdateUniformBuffer(uint32_t currentImage);
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
		VkShaderModule CreateShaderModule(const std::vector<char> &code);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};