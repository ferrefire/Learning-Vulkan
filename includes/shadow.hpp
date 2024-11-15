#pragma once

#include "texture.hpp"
#include "pipeline.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Shadow
{
	private:


	public:
		static VkRenderPass shadowPass;
		static VkFramebuffer shadowLod0FrameBuffer;
		static VkFramebuffer shadowLod1FrameBuffer;
		static Texture shadowLod0Texture;
		static Texture shadowLod1Texture;

		static glm::mat4 shadowLod0View;
		static glm::mat4 shadowLod0Projection;
		static glm::mat4 shadowLod1View;
		static glm::mat4 shadowLod1Projection;

		static int shadowLod0Resolution;
		static float shadowLod0Distance;
		static int shadowLod1Resolution;
		static float shadowLod1Distance;

		static void Create();
		static void CreateShadowResources();
		static void CreateShadowPass();

		static void Destroy();
		static void DestroyShadowResources();
		static void DestroyShadowPass();

		static glm::mat4 GetShadowView(int lod);
		static glm::mat4 GetShadowProjection(int lod);
};