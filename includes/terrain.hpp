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
	alignas(4) float mapScale = 1.0f;
	alignas(8) glm::vec2 mapOffset;

	alignas(8) glm::vec2 terrainOffset;
	alignas(8) glm::vec2 terrainLod0Offset;
	alignas(8) glm::vec2 terrainLod1Offset;
};

class Terrain
{
    private:
        

    public:
        static Mesh mesh;

		static Pipeline graphicsPipeline;
		static Pipeline computePipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor computeDescriptor;

		static Object object;

		static Texture grassTexture;

		static Texture heightMapTexture;
		static Texture heightMapLod0Texture;
		static Texture heightMapLod1Texture;

		static HeightMapVariables heightMapVariables;
		static Buffer heightMapVariablesBuffer;

		static float terrainChunkSize;
		static float terrainLod0Size;
		static float terrainLod1Size;

		static glm::vec2 terrainOffset;
		static glm::vec2 terrainLod0Offset;
		static glm::vec2 terrainLod1Offset;
		static float terrainStep;
		static float terrainLod0Step;
		static float terrainLod1Step;

		static void Create();
        static void CreateTextures();
		static void CreateMeshes();
		static void CreateObjects();
		static void CreateGraphicsPipeline();
		static void CreateGraphicsDescriptor();
		static void CreateComputePipeline();
		static void CreateComputeDescriptor();
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
		static void CheckTerrainOffset();
};
