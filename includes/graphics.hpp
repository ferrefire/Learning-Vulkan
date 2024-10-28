#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "mesh.hpp"
#include "window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Graphics
{
    private:
		Device &device;
		Window &window;

	public:
		Graphics(Device &device, Window &window);
		~Graphics();

		VkInstance instance = nullptr;

		void CreateInstance();
		void Create();

		void DestroyInstance();
		void Destroy();

		void DrawFrame();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, 
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};