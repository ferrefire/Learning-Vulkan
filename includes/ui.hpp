#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class UI
{
	private:
		
	public:
		static ImGuiContext* context;
		static ImGuiIO *io;

		static void Create();
		static void CreateContext();

		static void Destroy();
		static void DestroyContext();

		static void Frame();
		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
};