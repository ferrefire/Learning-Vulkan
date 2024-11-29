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

		static int cascadeCount;
		static VkRenderPass shadowCascadePass;
		static std::vector<VkFramebuffer> shadowCascadeFrameBuffers;
		static std::vector<Texture> shadowCascadeTextures;
		static std::vector<glm::mat4> shadowCascadeViews;
		static std::vector<glm::mat4> shadowCascadeProjections;
		static std::vector<glm::mat4> shadowCascadeTransformations;
		static std::vector<float> shadowCascadeDistances;
		static std::vector<int> shadowCascadeResolutions;

		static VkRenderPass shadowTrapezoidPass;
		static VkFramebuffer shadowLod0FrameBuffer;
		static VkFramebuffer shadowLod1FrameBuffer;
		static Texture shadowLod0Texture;
		static Texture shadowLod1Texture;

		static glm::mat4 shadowLod0View;
		static glm::mat4 shadowLod0Projection;
		static glm::mat4 shadowLod0Transformation;
		static glm::mat4 shadowLod1View;
		static glm::mat4 shadowLod1Projection;
		static glm::mat4 shadowLod1Transformation;

		static int shadowLod0Resolution;
		static float shadowLod0Distance;
		static int shadowLod1Resolution;
		static float shadowLod1Distance;

		static bool trapezoidal;

		static void Create();
		static void CreateShadowResources();
		static void CreateTrapezoidResources();
		static void CreateCascadeResources();
		static void CreateShadowPass();
		static void CreateTrapezoidPass();
		static void CreateCascadePass();

		static void Destroy();
		static void DestroyShadowResources();
		static void DestroyShadowPass();

		static glm::mat4 GetTrapezoidView(int lod, float dis);
		//static glm::mat4 GetTrapezoidView(int lod, float dis);
		static void SetCascadeViews();
		static glm::mat4 GetTrapezoidProjection(int lod);
		//static glm::mat4 GetTrapezoidProjection(int lod);
		static void SetCascadeProjections();
		static glm::mat4 GetTrapezoidTransformation(int lod);
		static void SetCascadeTransformations();

		static glm::mat4 CreateBoundedProjection(int lod, float near, float far, float depthMult);
		static glm::vec2 ComputeQ(const Line &centerLine, const Line &topLine, float delta, int lod, float far);
};