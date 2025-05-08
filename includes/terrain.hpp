#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "descriptor.hpp"
#include "buffer.hpp"
#include "curve.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TERRAIN_SHADOW_CASCADES 3

struct HeightMapVariables
{
	alignas(8) glm::vec2 terrainOffset;
	alignas(8) glm::vec2 terrainLod0Offset;
	alignas(8) glm::vec2 terrainLod1Offset;

	alignas(4) float terrainTotalSize;
	alignas(4) float terrainTotalSizeMult;

	alignas(4) int terrainChunksLength;
	alignas(4) float terrainChunksLengthMult;

	//alignas(4) float terrainHeight;
	//alignas(4) float terrainHeightMult;
	//alignas(4) float terrainLod0Size;
	//alignas(4) float terrainLod0SizeMult;
	//alignas(4) float terrainLod1Size;
	//alignas(4) float terrainLod1SizeMult;
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

struct ShadowComputeVariables
{
	float distance = 0;
	int resolution =  0;
	float resolutionMultiplier = 0;
	float spacing = 0;
	int lod = 0;
};

struct BlendConfig
{
	float blendDistance;
	float startDistance;
	float distanceIncrease;
	float startScale;
	float scaleIncrease;
	float startNormal;
	float normalIncrease;
	float startAmbient;
	float ambientIncrease;
	int decreaseLod;
	float decreaseAmount;
	alignas(16) glm::vec4 defaultColor;
	alignas(16) glm::vec4 tintColor;
};

struct TerrainTextureVariables
{
	BlendConfig grassConfig;
	BlendConfig rockConfig;
	BlendConfig dirtConfig;
};

class Terrain
{
    private:

	public:
		static Mesh lod0Mesh;
		static Mesh lod1Mesh;
		static Mesh lod2Mesh;

		static Pipeline graphicsPipeline;
		static Pipeline graphicsLodPipeline;
		static Pipeline cullPipeline;
		static Pipeline heightMapComputePipeline;
		static Pipeline heightMapArrayComputePipeline;
		static Pipeline shadowComputePipeline;

		static Descriptor graphicsDescriptor;
		static Descriptor cullDescriptor;
		static Descriptor heightMapComputeDescriptor;
		static Descriptor heightMapArrayComputeDescriptor;
		static Descriptor shadowComputeDescriptor;

		static std::vector<Object> terrainChunks;

		static int grassTextureIndex;
		static int rockTextureIndex;
		static int dirtTextureIndex;

		static std::vector<Texture> grassTextures;
		static std::vector<Texture> rockTextures;
		static std::vector<Texture> dirtTextures;

		static TerrainTextureVariables terrainTextureVariables;
		static std::vector<BlendConfig> grassBlendConfigs;
		static std::vector<BlendConfig> rockBlendConfigs;
		static std::vector<BlendConfig> dirtBlendConfigs;
		static BlendConfig &currentGrassBlendConfig;
		static BlendConfig &currentRockBlendConfig;
		static BlendConfig &currentDirtBlendConfig;

		static int heightMapResolution;
		static int heightMapLod0Resolution;
		static int heightMapLod1Resolution;

		static Texture heightMapArrayTexture;
		static Texture heightMapLod0Texture;
		static Texture heightMapLod1Texture;

		static std::vector<Texture> terrainShadowTextures;

		static HeightMapComputeVariables heightMapComputeVariables;
		static HeightMapArrayComputeVariables heightMapArrayComputeVariables;
		static std::vector<ShadowComputeVariables> shadowComputeVariables;

		static Buffer heightMapComputeVariablesBuffer;
		static Buffer heightMapArrayComputeVariablesBuffer;
		static Buffer shadowComputeVariablesBuffer;
		static Buffer textureVariablesBuffer;
		static Buffer roadsBuffer;

		static float terrainTotalSize;
		static float terrainHeight;
		static float waterHeight;
		static float waterBlendDistance;

		static float terrainMeshSize;
		static float terrainChunkSize;
		static float terrainLod0Size;
		static float terrainLod1Size;

		static int terrainChunkRadius;
		static int terrainChunkLength;
		static int terrainChunkCount;

		static float heightMapScale;
		static int heightMapRadius;
		static int heightMapLength;
		static int heightMapCount;

		static glm::vec3 terrainOffset;
		static glm::vec2 terrainLod0Offset;
		static glm::vec2 terrainLod1Offset;
		static float terrainStep;
		static float terrainLod0Step;
		static float terrainLod1Step;

		static bool updateTerrainShadows;
		static std::vector<glm::vec2> terrainShadowOffsets;

		static uint32_t currentBoundHeightMap;

		static int heightMapArrayLayersGenerated;

		static Curve lowLandsCurve;

		static uint16_t roadsData[1000000];

		static void Create();
        static void CreateTextures();
		static void CreateMeshes();
		static void CreateObjects();
		static void CreateGraphicsPipeline();
		static void CreateGraphicsDescriptor();
		static void CreateCullPipeline();
		static void CreateCullDescriptor();
		static void CreateComputePipelines();
		static void CreateComputeDescriptors();
		static void CreateShadowPipeline();
		static void CreateShadowDescriptor();
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
		static void PostFrame();
		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
		static void RecordComputeCommands(VkCommandBuffer commandBuffer);
		static void RecordCullCommands(VkCommandBuffer commandBuffer);
		static void ComputeHeightMap(VkCommandBuffer commandBuffer, uint32_t lod);
		static void ComputeHeightMapArray(VkCommandBuffer commandBuffer, uint32_t index);
		static void ComputeShadows(uint32_t index);
		static void CheckTerrainOffset(VkCommandBuffer commandBuffer);
		static void CheckTerrainShadowOffset();
		static void RenderTerrain(VkCommandBuffer commandBuffer);
		static void RenderCulling(VkCommandBuffer commandBuffer);
		static bool InView(const glm::vec3 &position, float tolerance, const glm::mat4 &projection, const glm::mat4 &view, bool lod);
		static bool ChunkInView(glm::vec3 position, float tolerance, glm::mat4 projection, glm::mat4 view, bool main, bool lod);
		static bool HeightMapsGenerated();
		static void EditTextureIndex();
		static void UpdateTextureVariables();
};
