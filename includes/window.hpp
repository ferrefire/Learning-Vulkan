#pragma once

#include "device.hpp"
#include "texture.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Window
{
	private:
		Device &device;

	public:
		Window(Device &device);
		~Window();

		uint32_t width = 800;
		uint32_t height = 600;

		GLFWwindow *data = nullptr;

		VkSurfaceKHR surface = nullptr;
		VkRenderPass renderPass = nullptr;

        VkSwapchainKHR swapChain = nullptr;
		//std::vector<VkImage> swapChainImages;
		//std::vector<VkImageView> swapChainImageViews;
		std::vector<Texture> swapChainTextures;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		//VkImage depthImage = nullptr;
		//VkDeviceMemory depthImageMemory = nullptr;
		//VkImageView depthImageView = nullptr;

		Texture depthTexture;
		Texture colorTexture;

		bool framebufferResized = false;
		bool isResizeable = true;
		bool mouseVisible = false;

		void Create();
		void Destroy();
		void Close();

		void CreateResources();
		void CreateSurface(VkInstance instance);
		void CreateSwapChain();
		void CreateImageViews();
		void CreateFramebuffers();
		void CreateColorResources();
		void CreateDepthResources();
		void CreateRenderPass();

		void DestroyResources();
		void DestroySurface(VkInstance instance);
		void DestroySwapChain();
		void DestroyImageViews();
		void DestroyFramebuffers();
		void DestroyDepthResources();
		void DestroyColorResources();
		void DestroyRenderPass();

		bool IsOpen();
		static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		void RecreateSwapChain();
		void SetMouseVisibility(bool visible);
};