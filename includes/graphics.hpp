#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

const int MAX_FRAMES_IN_FLIGHT = 2;

class Graphics
{
    private:
		uint32_t currentFrame = 0;

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
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;
		VkCommandPool commandPool = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkDescriptorSet> descriptorSets;
		bool framebufferResized = false;

		const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void *> uniformBuffersMapped;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;

		void CreateInstance();
        void CreateLogicalDevice();
        void CreateSurface();
        void CreateSwapChain();
		void CreateImageViews();
		void CreateGraphicsPipeline();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void CreateDescriptorSetLayout();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateTextureImage();
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
		void DestroyDescriptorSetLayout();
		void DestroyUniformBuffers();
		void DestroyDescriptorPool();
		void Destroy();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void PickPhysicalDevice();
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecreateSwapChain();
		void DrawFrame();
		void UpdateUniformBuffer(uint32_t currentImage);
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

		QueueFamilies FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		VkShaderModule CreateShaderModule(const std::vector<char> &code);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif