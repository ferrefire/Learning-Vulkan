#pragma once

#include "mesh.hpp"
#include "buffer.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"
#include "texture.hpp"
#include "shape.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct BranchConfiguration
{
	float seed = 32.0f;
	int resolution = 24;
	int splitCount = 3;
	glm::vec3 base = glm::vec3(0);
	glm::vec3 offset = glm::vec3(0);
	glm::vec3 angles = glm::vec3(0);
	glm::vec2 scale = glm::vec2(1);
	float minSize = 0.1f;
	bool main = false;

	float GetAngle(glm::vec2 dir);
	Shape Generate();
};

struct TreeCountData
{
	uint32_t lod0Count;
	uint32_t lod1Count;
};

struct TreeData
{
	uint32_t posxz;
	uint32_t posyroty;
	uint32_t scaxcoly;
};

struct TreeRenderData
{
	//uint32_t posxz;
	//uint32_t posyroty;
	//uint32_t scaxcoly;
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 rotscacol;
};

struct TreeVariables
{
	uint32_t treeBase;
	uint32_t treeCount;

	uint32_t treeLod0RenderBase;
	uint32_t treeLod0RenderCount;
	uint32_t treeLod1RenderBase;
	uint32_t treeLod1RenderCount;
	uint32_t treeTotalRenderBase;
	uint32_t treeTotalRenderCount;

	float spacing;
	float spacingMult;
};

class Trees
{
	private:
		

	public:
		static uint32_t treeBase;
		static uint32_t treeCount;

		static uint32_t treeLod0RenderBase;
		static uint32_t treeLod0RenderCount;
		static uint32_t treeLod1RenderBase;
		static uint32_t treeLod1RenderCount;
		static uint32_t treeTotalRenderBase;
		static uint32_t treeTotalRenderCount;

		static std::vector<TreeCountData> treeRenderCounts;

		static Mesh treeLod0Mesh;
		static Mesh treeLod1Mesh;

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
		static std::vector<Buffer> variableBuffers;

		static TreeVariables treeVariables;

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

		static void GenerateTrunkMesh(Mesh &mesh, BranchConfiguration config);
		//static Shape GenerateBranchShape(BranchConfiguration branchConfig);
};