#pragma once

#include "mesh.hpp"
#include "texture.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Water
{
    private:
        
    public:
        static Mesh waterMesh;

        static std::vector<Texture> normalTextures;

        static Pipeline graphicsPipeline;

        static Descriptor graphicsDescriptor;

        static void Create();
        static void CreateMeshes();
        static void CreateTextures();
        static void CreatePipelines();
        static void CreateDescriptors();

        static void Destroy();
        static void DestroyMeshes();
        static void DestroyTextures();
        static void DestroyPipelines();
        static void DestroyDescriptors();

		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
		static void RenderWater(VkCommandBuffer commandBuffer);
};