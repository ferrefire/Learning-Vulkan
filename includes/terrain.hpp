#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "descriptor.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Terrain
{
    private:
        

    public:
        static Mesh mesh;
		static Pipeline graphicsPipeline;
		static Descriptor graphicsDescriptor;
		static Pipeline computePipeline;
		static Descriptor computeDescriptor;
		static Object object;
		static Texture grassTexture;
		static Texture heightMapTexture;

		static void Create();
        static void CreateTextures();
		static void CreateMeshes();
		static void CreateObjects();
		static void CreateGraphicsPipeline();
		static void CreateGraphicsDescriptor();
		static void CreateComputePipeline();
		static void CreateComputeDescriptor();

		static void Destroy();
        static void DestroyTextures();
		static void DestroyMeshes();
		static void DestroyObjects();
		static void DestroyPipelines();
		static void DestroyDescriptors();

		static void Start();
		static void RecordCommands(VkCommandBuffer commandBuffer);
};
