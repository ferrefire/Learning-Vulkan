#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "descriptor.hpp"
#include "buffer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct HeightMapVariables
{
	alignas(8) glm::vec2 terrainOffset;
	alignas(8) glm::vec2 terrainLod0Offset;
	alignas(8) glm::vec2 terrainLod1Offset;
};

struct HeightMapComputeVariables
{
	alignas(4) float mapScale = 1.0f;
	alignas(8) glm::vec2 mapOffset = glm::vec2(0);
};

struct HeightMapArrayComputeVariables
{
	alignas(4) float mapScale = 1.0f;
	alignas(4) uint32_t currentChunkIndex = 0;
	alignas(8) glm::vec2 mapOffset = glm::vec2(0);
};

class Terrain
{
    private:
        

    public:
		static Mesh lod0Mesh;
		static Mesh lod1Mesh;

		static Pipeline graphicsPipeline;
		static Pipeline heightMapComputePipeline;
		static Pipeline heightMapArrayComputePipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor heightMapComputeDescriptor;
		static Descriptor heightMapArrayComputeDescriptor;

		static std::vector<Object> terrainChunks;

		//static Object object;

		static Texture grassDiffuseTexture;
		static Texture rockDiffuseTexture;
		static Texture dirtDiffuseTexture;
		//static Texture grassNormalTexture;
		//static Texture grassSpecularTexture;

		//static Texture heightMapTexture;
		static Texture heightMapArrayTexture;
		static Texture heightMapLod0Texture;
		static Texture heightMapLod1Texture;

		static HeightMapVariables heightMapVariables;
		static std::vector<Buffer> heightMapVariablesBuffers;

		static HeightMapComputeVariables heightMapComputeVariables;
		static Buffer heightMapComputeVariablesBuffer;

		static HeightMapArrayComputeVariables heightMapArrayComputeVariables;
		static Buffer heightMapArrayComputeVariablesBuffer;

		static float terrainTotalSize;
		static float terrainHeight;

		static float terrainChunkSize;
		static float terrainLod0Size;
		static float terrainLod1Size;

		static int terrainChunkRadius;
		static int terrainChunkLength;
		static int terrainChunkCount;

		static int heightMapRadius;
		static int heightMapLength;
		static int heightMapCount;

		static glm::vec2 terrainOffset;
		static glm::vec2 terrainLod0Offset;
		static glm::vec2 terrainLod1Offset;
		static float terrainStep;
		static float terrainLod0Step;
		static float terrainLod1Step;

		static uint32_t currentBoundHeightMap;

		static int heightMapArrayLayersGenerated;

		static void Create();
        static void CreateTextures();
		static void CreateMeshes();
		static void CreateObjects();
		static void CreateGraphicsPipeline();
		static void CreateGraphicsDescriptor();
		static void CreateComputePipelines();
		static void CreateComputeDescriptors();
		static void CreateBuffers();

		static void Destroy();
        static void DestroyTextures();
		static void DestroyMeshes();
		static void DestroyObjects();
		static void DestroyPipelines();
		static void DestroyDescriptors();
		static void DestroyBuffers();

		static void Start();
		static void Frame();
		static void RecordCommands(VkCommandBuffer commandBuffer);
		static void ComputeHeightMap(uint32_t lod);
		static void ComputeHeightMapArray(uint32_t index);
		static void CheckTerrainOffset();
		static void UpdateHeightMapVariables();
		static void RenderTerrain(VkCommandBuffer commandBuffer);
		static bool InView(const glm::vec3 &position, float tolerance, const glm::mat4 &projection, const glm::mat4 &view);
		static bool ChunkInView(glm::vec3 position, float tolerance, glm::mat4 projection, glm::mat4 view, bool main = true);
};
