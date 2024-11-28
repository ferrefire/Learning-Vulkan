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

//struct ComputeData
//{
//    float viewHeight = 0;
//    float frustumIntersect = 0;
//    float frustumIntersectSecond = 0;
//    float frustumIntersectAverage = 0;
//};

struct IntersectData
{
    glm::vec3 position = glm::vec3(0);
    uint32_t active = 0;
};

class Data
{
    private:
        

    public:
        static Pipeline computePipeline;
        static Descriptor computeDescriptor;
        //static std::vector<Buffer> computeBuffers;
        static Buffer intersectBuffer;

        //static std::vector<ComputeData> computeData;
        static std::vector<IntersectData> intersectData;

        static int intersectCount;
        static int activeIntersectCount;

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
        static int AddIntersect(glm::vec3 position);
        //static ComputeData GetComputeData();
        static IntersectData GetIntersectData(int index);
};