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
	bool leaves = false;
	bool main = false;
	int iteration = 0;
	int maxIteration = 7;
	//float seed = 103.84f;
	//float seed = 50.0f;
	float seed = 8.35f;
	int lod = 0;

	int resolution = 24;
	int splitCount = 3;
	float splitChance = 0;
	float fallChance = 0;
	int blendRange = 2;
	float minSize = 0.1f;
	float angleRandomness = 0.25f;
	float reach = 2.0f;
	//float thickness = 0.6f;
	float thickness = 0.6f;
	float length = 0.75f;
	float branchThickness = 1.25f;
	
	glm::vec2 sturdiness = glm::vec2(0);
	glm::vec2 steepness = glm::vec2(-10.0f, 45.0f);
	glm::vec3 base = glm::vec3(0);
	glm::vec3 offset = glm::vec3(0);
	glm::vec3 angles = glm::vec3(0);
	glm::vec2 scale = glm::vec2(1);
	glm::vec2 mainScale = glm::vec2(1);

	float GetAngle(glm::vec2 dir);
	Shape Generate();
};

struct TreeCountData
{
	uint32_t lod0RenderCount;
	uint32_t lod1RenderCount;
	uint32_t lod2RenderCount;
	uint32_t lod3RenderCount;
	uint32_t lod4RenderCount;
	uint32_t lod5RenderCount;

	uint32_t lod0ShadowCount;
	uint32_t lod1ShadowCount;
	uint32_t lod2ShadowCount;
	uint32_t lod3ShadowCount;
	uint32_t lod4ShadowCount;
	uint32_t lod5ShadowCount;
};

struct TreeData
{
	uint32_t posxz;
	uint32_t posyroty;
	uint32_t scaxcoly;
	uint32_t life;
};

struct TreeRenderData
{
	uint32_t posxz;
	uint32_t posyroty;
	uint32_t scaxcoly;
};

struct TreeVariables
{
	alignas(4) uint32_t treeBase;
	alignas(4) uint32_t treeCount;

	alignas(4) uint32_t treeLod0RenderBase;
	alignas(4) uint32_t treeLod0RenderCount;
	alignas(4) uint32_t treeLod1RenderBase;
	alignas(4) uint32_t treeLod1RenderCount;
	alignas(4) uint32_t treeLod2RenderBase;
	alignas(4) uint32_t treeLod2RenderCount;
	alignas(4) uint32_t treeLod3RenderBase;
	alignas(4) uint32_t treeLod3RenderCount;
	alignas(4) uint32_t treeLod4RenderBase;
	alignas(4) uint32_t treeLod4RenderCount;
	alignas(4) uint32_t treeLod5RenderBase;
	alignas(4) uint32_t treeLod5RenderCount;
	alignas(4) uint32_t treeTotalRenderBase;
	alignas(4) uint32_t treeTotalRenderCount;

	alignas(4) float spacing;
	alignas(4) float spacingMult;

	alignas(4) float leafSizeMultiplier;
	alignas(4) float leafBlendDistance;
	alignas(4) float leafBlendStart;
	alignas(4) float leafBlendMult;
	alignas(4) float leafFadeDistance;
	alignas(4) float leafFadeStart;
	alignas(4) float leafFadeMult;

	alignas(16) glm::vec4 leafCounts[6];
};

struct TrunkVariables
{
	//glm::vec4 trunkColor = glm::vec4(48.0f, 32.0f, 16.0f, 255.0f) / 255.0f;
	float trunkAmbient = 0.025f;
	alignas(16) glm::vec4 trunkLodColor = glm::vec4(36.0f, 30.0f, 22.0f, 255.0f) / 255.0f;
	glm::vec4 trunkTint = glm::vec4(255.0f, 217.0f, 169.0f, 255.0f) / 255.0f;
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
		static uint32_t treeLod2RenderBase;
		static uint32_t treeLod2RenderCount;
		static uint32_t treeLod3RenderBase;
		static uint32_t treeLod3RenderCount;
		static uint32_t treeLod4RenderBase;
		static uint32_t treeLod4RenderCount;
		static uint32_t treeLod5RenderBase;
		static uint32_t treeLod5RenderCount;
		static uint32_t treeTotalRenderBase;
		static uint32_t treeTotalRenderCount;

		static std::vector<TreeCountData> treeRenderCounts;

		static std::vector<glm::vec4> leafPositions;
		//static std::vector<glm::vec4> leafPositions0;
		//static std::vector<glm::vec4> leafPositions1;

		static Mesh treeLod0Mesh;
		static Mesh treeLod1Mesh;
		static Mesh treeLod2Mesh;
		static Mesh treeLod3Mesh;
		static Mesh treeLod4Mesh;
		static Mesh treeLod5Mesh;

		static Pipeline graphicsPipeline;
		static Pipeline graphicsLodPipeline;
		static Pipeline shadowPipeline;
		static Pipeline cullPipeline;
		static Pipeline capturePipeline;
		static Pipeline computeSetupPipeline;
		static Pipeline computeRenderPipeline;

		//static Texture diffuseTexture;
		static std::vector<Texture> treeTextures;

		static Descriptor graphicsDescriptor;
		static Descriptor shadowDescriptor;
		static Descriptor cullDescriptor;
		static Descriptor computeSetupDescriptor;
		static Descriptor computeRenderDescriptor;

		static Buffer dataBuffer;
		static Buffer leafPositionsBuffer;
		static std::vector<Buffer> renderBuffers;
		static std::vector<Buffer> shadowBuffers;
		static std::vector<Buffer> countBuffers;
		//static std::vector<Buffer> variableBuffers;
		static Buffer variableBuffer;
		static Buffer trunkBuffer;

		static TreeVariables treeVariables;
		static uint32_t totalLeafCount;

		static TrunkVariables trunkVariables;

		static bool treesComputed;
		static bool shouldUpdateLodTree;

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
		static void RecordComputeCommands(VkCommandBuffer commandBuffer);
		static void RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade);
		static void RecordCullCommands(VkCommandBuffer commandBuffer);
		static void RecordCaptureCommands(VkCommandBuffer commandBuffer);
		static void RenderTrees(VkCommandBuffer commandBuffer);
		static void RenderShadows(VkCommandBuffer commandBuffer, int cascade);
		static void RenderCulling(VkCommandBuffer commandBuffer);
		static void ComputeTreeSetup();
		static void ComputeTreeRender(VkCommandBuffer commandBuffer, bool capture = false);
		static void SetData();
		static void CaptureTree();
		static void UpdateTreeVariables();

		static void GenerateTrunkMesh(Mesh &mesh, BranchConfiguration config);
		//static Shape GenerateBranchShape(BranchConfiguration branchConfig);
};