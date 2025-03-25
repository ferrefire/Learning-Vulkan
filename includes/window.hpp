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
		//VkRenderPass shadowPass = nullptr;

		VkSwapchainKHR swapChain = nullptr;
		std::vector<Texture> swapChainTextures;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		//VkFramebuffer shadowFrameBuffer = nullptr;

		Texture depthTexture;
		Texture colorTexture;
		Texture multiSampleTexture;
		//Texture shadowTexture;

		bool framebufferResized = false;
		bool isResizeable = true;
		bool mouseVisible = false;
		bool mouseLocked = true;
		bool recreatingSwapchain = false;

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
		void CreateMultiSampleResources();
		//void CreateShadowResources();
		void CreateRenderPass();
		//void CreateShadowPass();

		void DestroyResources();
		void DestroySurface(VkInstance instance);
		void DestroySwapChain();
		void DestroyImageViews();
		void DestroyFramebuffers();
		void DestroyDepthResources();
		void DestroyColorResources();
		void DestroyMultiSampleResources();
		//void DestroyShadowResources();
		void DestroyRenderPass();
		//void DestroyShadowPass();

		bool IsOpen();
		static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		void RecreateSwapChain();
		void SetMouseVisibility(bool visible);
		void SetMouseLocked(bool locked);
};