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

		void Frame();
		void DrawFrame();
		void ComputeFrame();

		void RecordComputeCommands(VkCommandBuffer commandBuffer);
		void RecordGraphicsCommands();
		void RenderGraphics(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecordCullCommands();
		void RenderCulling(VkCommandBuffer commandBuffer);
		void RecordShadowCommands();
		void RenderShadows(VkCommandBuffer commandBuffer);
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, 
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};