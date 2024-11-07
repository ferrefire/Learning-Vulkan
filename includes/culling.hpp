#pragma once

#include "texture.hpp"
#include "pipeline.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Culling
{
	private:
		

	public:
		static VkRenderPass cullPass;
		static VkFramebuffer cullFrameBuffer;
		static Texture cullTexture;

		//static glm::mat4 cullView;
		static glm::mat4 cullProjection;

		static int cullResolutionWidth;
		static int cullResolutionHeight;
		static float cullDistance;

		static void Create();
		static void CreateCullResources();
		static void CreateCullPass();

		static void Destroy();
		static void DestroyCullResources();
		static void DestroyCullPass();
};