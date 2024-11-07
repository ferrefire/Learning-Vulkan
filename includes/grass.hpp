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

struct GrassData
{
	//alignas(16) glm::vec3 position;
	//alignas(16) glm::vec3 rotation;
	//alignas(16) glm::vec3 normal;

	//glm::vec3 position;
	//glm::vec3 rotation;
	//glm::vec3 normal;

	uint32_t posxz;
	uint32_t normxz;
	uint32_t posynormy;
	uint32_t rot;
	uint32_t scaxcoly;
};

struct GrassVariables
{
	alignas(4) uint32_t grassBase = 0;
	alignas(4) float grassBaseMult = 0;
	alignas(4) uint32_t grassCount = 0;

	alignas(4) uint32_t grassLodBase = 0;
	alignas(4) float grassLodBaseMult = 0;
	alignas(4) uint32_t grassLodCount = 0;

	alignas(4) uint32_t grassTotalBase = 0;
	alignas(4) float grassTotalBaseMult = 0;
	alignas(4) uint32_t grassTotalCount = 0;

	alignas(4) float spacing = 0.125;
	alignas(4) float spacingMult = 8;

	alignas(4) float windStrength = 0.225;
	alignas(4) float windFrequency = 2.75;

	alignas(16) glm::vec3 flooredViewPosition = glm::vec3(0);
};

class Grass
{
	private:


	public:
		static uint32_t grassBase;
		static uint32_t grassCount;
		static std::vector<uint32_t> grassRenderCounts;

		static uint32_t grassLodBase;
		static uint32_t grassLodCount;
		static std::vector<uint32_t> grassLodRenderCounts;

		static uint32_t grassTotalBase;
		static uint32_t grassTotalCount;

		static Mesh grassMesh;
		static Mesh grassLodMesh;

		static Pipeline graphicsPipeline;
		static Pipeline shadowPipeline;
		static Pipeline computePipeline;
		static Pipeline clumpingComputePipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor shadowDescriptor;
		static Descriptor computeDescriptor;
		static Descriptor clumpingComputeDescriptor;

		static std::vector<Buffer> dataBuffers;
		static std::vector<Buffer> lodDataBuffers;
		static std::vector<Buffer> countBuffers;
		static std::vector<Buffer> lodCountBuffers;
		static std::vector<Buffer> variableBuffers;

		static Texture clumpingTexture;

		static GrassVariables grassVariables;

		static void Create();
		static void CreateMeshes();
		static void CreateGraphicsPipeline();
		static void CreateShadowPipeline();
		static void CreateComputePipelines();
		static void CreateTextures();
		static void CreateBuffers();
		static void CreateGraphicsDescriptor();
		static void CreateShadowDescriptor();
		static void CreateComputeDescriptors();

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
		static void RenderGrass(VkCommandBuffer commandBuffer);
		static void RenderShadows(VkCommandBuffer commandBuffer);
		static void ComputeGrass();
		static void ComputeClumping();
};
