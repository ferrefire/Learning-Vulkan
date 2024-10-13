#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "object.hpp"

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
        static Pipeline pipeline;
        static Object object;

        static void Create();
        static void CreateMesh();
        static void CreatePipeline();

        static void Destroy();
        static void DestroyMesh();
        static void DestroyPipeline();

        static void RecordCommands(VkCommandBuffer commandBuffer);
};
