#pragma once

#include "texture.hpp"
#include "pipeline.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Shadow
{
	private:


	public:
		static VkRenderPass shadowPass;
		static VkFramebuffer shadowFrameBuffer;
		static Texture shadowTexture;

		static void CreateShadowResources();
		static void CreateShadowPass();

		static void DestroyShadowResources();
		static void DestroyShadowPass();
};