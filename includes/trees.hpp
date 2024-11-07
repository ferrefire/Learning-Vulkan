#pragma once

#include "mesh.hpp"
#include "buffer.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"
#include "texture.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct TreeData
{
	uint32_t posxz;
	uint32_t posyroty;
	uint32_t scaxcoly;
};

struct TreeRenderData
{
	//uint32_t posxz;
	//uint32_t normxz;
	//uint32_t posynormy;
	//uint32_t rot;
	//uint32_t scaxcoly;

	uint32_t posxz;
	uint32_t posyroty;
	uint32_t scaxcoly;
};

class Trees
{
	private:
		

	public:
		static uint32_t treeBase;
		static uint32_t treeCount;

		static uint32_t treeRenderBase;
		static uint32_t treeRenderCount;
		static std::vector<uint32_t> treeRenderCounts;

		static Mesh treeMesh;

		static Pipeline graphicsPipeline;
		static Pipeline shadowPipeline;
		static Pipeline cullPipeline;
		static Pipeline computeSetupPipeline;
		static Pipeline computeRenderPipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor shadowDescriptor;
		static Descriptor cullDescriptor;
		static Descriptor computeSetupDescriptor;
		static Descriptor computeRenderDescriptor;

		static Buffer dataBuffer;
		static std::vector<Buffer> renderBuffers;
		static std::vector<Buffer> countBuffers;

		static void Create();
		static void CreateMeshes();
		static void CreateGraphicsPipeline();
		static void CreateShadowPipeline();
		static void CreateCullPipeline();
		static void CreateComputeSetupPipeline();
		static void CreateComputeRenderPipeline();
		static void CreateTextures();
		static void CreateBuffers();
		static void CreateGraphicsDescriptor();
		static void CreateShadowDescriptor();
		static void CreateCullDescriptor();
		static void CreateComputeSetupDescriptor();
		static void CreateComputeRenderDescriptor();

		static void Destroy();
		static void DestroyMeshes();
		static void DestroyPipelines();
		static void DestroyTextures();
		static void DestroyBuffers();
		static void DestroyDescriptors();

		static void Start();
		static void Frame();
		static void PostFrame();
		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
		static void RecordShadowCommands(VkCommandBuffer commandBuffer);
		static void RecordCullCommands(VkCommandBuffer commandBuffer);
		static void RenderTrees(VkCommandBuffer commandBuffer);
		static void RenderShadows(VkCommandBuffer commandBuffer);
		static void RenderCulling(VkCommandBuffer commandBuffer);
		static void ComputeTreeSetup();
		static void ComputeTreeRender();
};