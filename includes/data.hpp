#pragma once

#include "pipeline.hpp"
#include "descriptor.hpp"
#include "buffer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct ComputeData
{
    float viewHeight = 0;
    float frustumIntersect = 0;
    float frustumIntersectAverage = 0;
};


class Data
{
    private:
        

    public:
        static Pipeline computePipeline;
        static Descriptor computeDescriptor;
        static std::vector<Buffer> computeBuffers;

        static std::vector<ComputeData> computeData;

        static void Create();
        static void CreatePipelines();
        static void CreateDescriptors();
        static void CreateBuffers();

        static void Destroy();
        static void DestroyPipelines();
        static void DestroyDescriptors();
        static void DestroyBuffers();

        static void Start();
        static void SetData();
        static ComputeData GetData();
};