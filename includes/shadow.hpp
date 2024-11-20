#pragma once

#include "texture.hpp"
#include "pipeline.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct Point2D
{
	float x, y;

	bool operator<(const Point2D &other) const
	{
		return x < other.x || (x == other.x && y < other.y);
	}
};

struct Line
{
	glm::vec2 point;
	glm::vec2 normal;
};

struct Trapezoid
{
	glm::vec2 topLeft, topRight, bottomLeft, bottomRight;
};

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
		static glm::mat4 shadowLod1Transformation;

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
		static glm::mat4 GetShadowTransformation(int lod);

		static glm::mat4 CreateBoundedProjection(const glm::mat4 &shadowView, float farDis);
		static glm::vec2 ComputeQ(const Line &centerLine, const Line &topLine, float delta);
};