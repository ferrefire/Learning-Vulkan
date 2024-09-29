#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"

#include <vector>

class Window
{
	private:
		Device &device;

	public:
		Window(Device &device);
		~Window();

		unsigned int width = 800;
		unsigned int height = 600;

		GLFWwindow *data = nullptr;

		VkSurfaceKHR surface = nullptr;
		VkRenderPass renderPass = nullptr;

        VkSwapchainKHR swapChain = nullptr;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkImage depthImage = nullptr;
		VkDeviceMemory depthImageMemory = nullptr;
		VkImageView depthImageView = nullptr;

		bool framebufferResized = false;
		bool isResizeable = true;
		bool mouseVisible = false;

		void Create();
		void Destroy();
		void Close();

		void CreateSurface(VkInstance instance);
		void CreateSwapChain();
		void CreateImageViews();
		void CreateFramebuffers();
		void CreateDepthResources();
		void CreateRenderPass();

		void DestroySurface(VkInstance instance);
		void DestroySwapChain();
		void DestroyImageViews();
		void DestroyFramebuffers();
		void DestroyDepthResources();
		void DestroyRenderPass();

		bool IsOpen();
		static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		void RecreateSwapChain();
		void SetMouseVisibility(bool visible);
};