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
#define IMAGE_SAMPLER VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
#define IMAGE_STORAGE VK_DESCRIPTOR_TYPE_STORAGE_IMAGE

#define VERTEX_STAGE VK_SHADER_STAGE_VERTEX_BIT
#define FRAGMENT_STAGE VK_SHADER_STAGE_FRAGMENT_BIT
#define ALL_STAGE VK_SHADER_STAGE_ALL_GRAPHICS
#define COMPUTE_STAGE VK_SHADER_STAGE_COMPUTE_BIT

#define IMAGE_READ_ONLY VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

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
};

struct DescriptorLayoutConfiguration
{
    VkDescriptorType type;
    VkShaderStageFlags stages;
};

class Pipeline
{
    private:
        Device &device;
		Camera &camera;

    public:
        static PipelineConfiguration DefaultConfiguration();

        Pipeline(Device &device, Camera &camera);
        ~Pipeline();

		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		//VkDescriptorPool descriptorPool = nullptr;
		//std::vector<VkDescriptorSet> descriptorSets;

		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;

		VkPipelineLayout computePipelineLayout = nullptr;
		VkPipeline computePipeline = nullptr;

		//void Create(VertexInfo vertexInfo);
		//void Create(std::string shader, PipelineConfiguration pipelineConfig, std::vector<DescriptorConfiguration> descriptorConfig, VertexInfo vertexInfo);
		void Create(std::string shader, std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig, PipelineConfiguration &pipelineConfig, VertexInfo &vertexInfo);
		//void CreateCompute(std::string shader, std::vector<DescriptorConfiguration> descriptorConfig);
		//void CreateCompute(std::string shader);
		void CreateGraphicsPipeline(std::string vertexShader, std::string fragmentShader, VertexInfo &vertexInfo, PipelineConfiguration &configuration);
		//void CreateComputePipeline(std::string computeShader);
		void CreateDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig);
		//void CreateDescriptorPool(std::vector<DescriptorConfiguration> &configuration);
        //void CreateDescriptorSets(std::vector<DescriptorConfiguration> &configuration);
        VkShaderModule CreateShaderModule(const std::vector<char> &code);
        void Bind(VkCommandBuffer commandBuffer, Window &window);

        void Destroy();
        void DestroyGraphicsPipeline();
        void DestroyDescriptorSetLayout();
        //void DestroyDescriptorPool();
};