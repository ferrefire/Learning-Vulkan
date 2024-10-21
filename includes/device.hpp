#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

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

class Device
{
    private:
        const int MAX_FRAMES_IN_FLIGHT = 2;

    public:
        Device();
        ~Device();

        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice logicalDevice = nullptr;
        VkPhysicalDeviceProperties properties;

        VkQueue graphicsQueue = nullptr;
        VkQueue presentationQueue = nullptr;
        QueueFamilies queueFamilies;

        VkCommandPool commandPool = nullptr;
		std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

        SwapChainSupportDetails swapChainSupportDetails;
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        void Create(VkInstance instance, VkSurfaceKHR surface);
        void DestroyLogicalDevice();

        void CreateLogicalDevice(VkSurfaceKHR surface);
        void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

        QueueFamilies FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        void CreateSyncObjects();
        void DestroySyncObjects();
        void WaitForIdle();
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat FindDepthFormat();
        bool HasStencilComponent(VkFormat format);
        void CreateCommandPool();
        void CreateCommandBuffers();
        void DestroyCommandPool();
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        VkSampleCountFlagBits MaxSampleCount();
        //void TransitionImageLayout(VkImage image, ImageConfiguration &configuration, VkImageLayout newLayout);
};