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

struct LeafData
{
	uint32_t posxz;
	uint32_t posyroty;
	uint32_t scalxrotx;
	uint32_t colx;
	//glm::vec3 position;
};

class Leaves
{
	private:
		

	public:
		static Mesh leafMeshLod0;
		static Mesh leafMeshLod1;
		static Mesh leafMeshLod2;

		static Pipeline graphicsPipeline;
		static Pipeline shadowPipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor shadowDescriptor;

		static std::vector<Buffer> dataBuffers;

		static void Create();
		static void CreateMeshes();
		static void CreateGraphicsPipeline();
		static void CreateShadowPipeline();
		static void CreateBuffers();
		static void CreateGraphicsDescriptor();
		static void CreateShadowDescriptor();

		static void Destroy();
		static void DestroyMeshes();
		static void DestroyPipelines();
		static void DestroyBuffers();
		static void DestroyDescriptors();

		static void Start();
		static void Frame();
		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
		static void RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade);
		static void RenderLeaves(VkCommandBuffer commandBuffer);
		static void RenderShadows(VkCommandBuffer commandBuffer, int cascade);
};