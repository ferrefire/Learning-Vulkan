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
		static VkFramebuffer shadowFrameBuffer;
		static Texture shadowTexture;

		static glm::mat4 shadowView;
		static glm::mat4 shadowProjection;

		static int shadowResolution;
		static float shadowDistance;

		static void Create();
		static void CreateShadowResources();
		static void CreateShadowPass();

		static void Destroy();
		static void DestroyShadowResources();
		static void DestroyShadowPass();

		static glm::mat4 GetShadowView();
		static glm::mat4 GetShadowProjection();
};