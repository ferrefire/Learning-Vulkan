#pragma once

#include "mesh.hpp"
#include "buffer.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct GrassData
{
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 rotation;
	alignas(16) glm::vec3 normal;
};

class Grass
{
	private:


	public:
		static int grassCount;
		static int grassTotalCount;
		static int grassRenderCount;

		static Mesh grassMesh;

		static Pipeline graphicsPipeline;
		static Pipeline computePipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor computeDescriptor;

		static std::vector<Buffer> dataBuffers;

		static void Create();
		static void CreateMeshes();
		static void CreateGraphicsPipeline();
		static void CreateComputePipeline();
		static void CreateBuffers();
		static void CreateGraphicsDescriptor();
		static void CreateComputeDescriptor();

		static void Destroy();
		static void DestroyMeshes();
		static void DestroyPipelines();
		static void DestroyBuffers();
		static void DestroyDescriptors();

		static void Start();
		static void Frame();
		static void RecordCommandBuffer(VkCommandBuffer commandBuffer);
		static void RenderGrass(VkCommandBuffer commandBuffer);
		static void ComputeGrass();
};
