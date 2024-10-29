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
	float mapScale = 1.0f;
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
		static Texture heightMapLod1Texture;
		static Texture heightMapLod0Texture;

		static HeightMapVariables heightMapVariables;
		static Buffer heightMapVariablesBuffer;

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
		static void RecordCommands(VkCommandBuffer commandBuffer);
		static void ComputeHeightMap();
};
