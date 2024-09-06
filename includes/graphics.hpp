#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Graphics
{
    private:

    public:
        Graphics();
        ~Graphics();

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

		VkInstance instance = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice device = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentationQueue = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
        VkPhysicalDeviceProperties properties;
		VkRenderPass renderPass = nullptr;
		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;
		VkCommandPool commandPool = nullptr;
		VkCommandBuffer commandBuffer = nullptr;
		VkSemaphore imageAvailableSemaphore = nullptr;
		VkSemaphore renderFinishedSemaphore = nullptr;
		VkFence inFlightFence = nullptr;

		const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		void CreateInstance();
        void CreateLogicalDevice();
        void CreateSurface();
        void CreateSwapChain();
		void CreateImageViews();
		void CreateGraphicsPipeline();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateCommandBuffer();
		void CreateSyncObjects();
		void Create();

		void DestroyInstance();
        void DestroyDevice();
        void DestroySurface();
        void DestroySwapChain();
		void DestroyImageViews();
		void DestroyGraphicsPipeline();
		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyCommandPool();
		void DestroySyncObjects();
		void Destroy();

		void PickPhysicalDevice();
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void DrawFrame();

        QueueFamilies FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		VkShaderModule CreateShaderModule(const std::vector<char> &code);
};

#endif