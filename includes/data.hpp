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

struct GeneralData
{
    float viewHeight = 0;
};

struct HeightData
{
	glm::vec4 position;
};

struct DataRequest
{
	glm::vec3 position = glm::vec3(0);
	float *source = nullptr;
	void (*func)(int);
	int index;
};

class Data
{
    private:
        

    public:
        static Pipeline computePipeline;
        static Descriptor computeDescriptor;
		static Buffer generalBuffer;
		static Buffer heightBuffer;

		static std::vector<GeneralData> generalData;
		static std::vector<HeightData> heightData;
		static std::vector<DataRequest> requestData;

		static int requestCount;

		static void Create();
        static void CreatePipelines();
		static void CreateBuffers();
        static void CreateDescriptors();

        static void Destroy();
        static void DestroyPipelines();
		static void DestroyBuffers();
        static void DestroyDescriptors();

        static void Start();
        static void RecordComputeCommands(VkCommandBuffer commandBuffer);
        static void ComputeGeneralData(VkCommandBuffer commandBuffer);
        static void SetData();
        static GeneralData GetGeneralData();
		static void RequestData(glm::vec3 position, float *source);
		static void RequestData(glm::vec3 position, float *source, void (*func)(int), int index);
};