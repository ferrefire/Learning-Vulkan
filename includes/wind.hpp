#pragma once

#include "texture.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Wind
{
    private:

    public:
        static int resolution;

        static Texture windTexture;

        static Pipeline computePipeline;
        
        static Descriptor computeDescriptor;

        static void Create();
        static void CreateTextures();
        static void CreatePipelines();
        static void CreateDescriptors();

        static void Destroy();
        static void DestroyTextures();
        static void DestroyPipelines();
        static void DestroyDescriptors();

        static void RecordComputeCommands(VkCommandBuffer commandBuffer);
        static void ComputeWind(VkCommandBuffer commandBuffer);
};