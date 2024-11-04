#pragma once

#include "device.hpp"
#include "mesh.hpp"
//#include "texture.hpp"
#include "camera.hpp"
#include "buffer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <vector>
#include <string>

#define UNIFORM_BUFFER VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
#define STORAGE_BUFFER VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
#define IMAGE_SAMPLER VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
#define IMAGE_STORAGE VK_DESCRIPTOR_TYPE_STORAGE_IMAGE

#define VERTEX_STAGE VK_SHADER_STAGE_VERTEX_BIT
#define FRAGMENT_STAGE VK_SHADER_STAGE_FRAGMENT_BIT
#define TESSELATION_CONTROL_STAGE VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
#define TESSELATION_EVALUATION_STAGE VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
#define ALL_GRAPHICS_STAGE VK_SHADER_STAGE_ALL_GRAPHICS
#define COMPUTE_STAGE VK_SHADER_STAGE_COMPUTE_BIT
#define ALL_STAGE VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT

#define IMAGE_READ_ONLY VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

#define GRAPHICS_BIND_POINT VK_PIPELINE_BIND_POINT_GRAPHICS
#define COMPUTE_BIND_POINT VK_PIPELINE_BIND_POINT_COMPUTE

struct PipelineConfiguration
{
    //VkViewport viewport;
    //VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterization;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlending;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
	bool tesselation = false;
	uint32_t pushConstantCount = 0;
	uint32_t pushConstantSize = 0;
	VkShaderStageFlags pushConstantStage = ALL_STAGE;
	bool foliage = false;
};

struct DescriptorLayoutConfiguration
{
    VkDescriptorType type;
    VkShaderStageFlags stages;
	uint32_t count = 1;
};

class Pipeline
{
    private:
        Device &device;
		Camera &camera;

    public:
        static PipelineConfiguration DefaultConfiguration();
		static void CreateDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig, VkDescriptorSetLayout *descriptorSetLayout);

		Pipeline(Device &device, Camera &camera);
        ~Pipeline();

		VkDescriptorSetLayout globalDescriptorSetLayout = nullptr;
		VkDescriptorSetLayout objectDescriptorSetLayout = nullptr;
		//VkDescriptorSetLayout descriptorSetLayout = nullptr;

		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;

		VkPipelineLayout computePipelineLayout = nullptr;
		VkPipeline computePipeline = nullptr;

		void CreateGraphicsPipeline(std::string shader, std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig, PipelineConfiguration &pipelineConfig, VertexInfo &vertexInfo);
		void CreateComputePipeline(std::string shader, std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig);
		// void CreateDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig, VkDescriptorSetLayout *descriptorSetLayout);
		// void CreateGlobalDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig);
		void CreateObjectDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig);
		VkShaderModule CreateShaderModule(const std::vector<char> &code);
		void BindGraphics(VkCommandBuffer commandBuffer, Window &window);
		void BindCompute(VkCommandBuffer commandBuffer);

		void Destroy();
		void DestroyGraphicsPipeline();
		void DestroyComputePipeline();
		void DestroyDescriptorSetLayout();
};