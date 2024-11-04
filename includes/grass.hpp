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
};

class Grass
{
	private:


	public:
		static uint32_t grassBase;
		static uint32_t grassCount;

		static uint32_t grassLodBase;
		static uint32_t grassLodCount;

		static uint32_t grassTotalBase;
		static uint32_t grassTotalCount;

		static Mesh grassMesh;
		static Mesh grassLodMesh;

		static Pipeline graphicsPipeline;
		static Pipeline computePipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor computeDescriptor;

		static std::vector<Buffer> dataBuffers;
		static std::vector<Buffer> lodDataBuffers;
		static std::vector<Buffer> countBuffers;
		static std::vector<Buffer> lodCountBuffers;
		static std::vector<Buffer> variableBuffers;

		static GrassVariables grassVariables;

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
		static void PostFrame();
		static void RecordCommands(VkCommandBuffer commandBuffer);
		static void RenderGrass(VkCommandBuffer commandBuffer);
		static void ComputeGrass();
};
