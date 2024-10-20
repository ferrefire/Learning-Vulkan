#pragma once

#include "device.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "camera.hpp"

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

#define VERTEX_STAGE VK_SHADER_STAGE_VERTEX_BIT
#define FRAGMENT_STAGE VK_SHADER_STAGE_FRAGMENT_BIT
#define ALL_STAGE VK_SHADER_STAGE_ALL_GRAPHICS

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

struct DescriptorConfiguration
{
    VkDescriptorType type;
    VkShaderStageFlags stages;

    VkDescriptorBufferInfo bufferInfo;
    VkDescriptorImageInfo imageInfo;
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 model = glm::mat4(1);
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
};

class Pipeline
{
    private:
        Device &device;
		Camera &camera;

        const int MAX_FRAMES_IN_FLIGHT = 2;

    public:
        static PipelineConfiguration DefaultConfiguration();

        Pipeline(Device &device, Camera &camera);
        ~Pipeline();

		UniformBufferObject ubo;
        Texture texture;

		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void *> uniformBuffersMapped;

        void Create();
        void Create(std::string shader, PipelineConfiguration pipelineConfig, std::vector<DescriptorConfiguration> descriptorConfig, VertexInfo vertexInfo);
        void CreateGraphicsPipeline(std::string vertexShader, std::string fragmentShader, VertexInfo &vertexInfo, PipelineConfiguration &configuration);
        void CreateDescriptorSetLayout();
        void CreateDescriptorSetLayout(std::vector<DescriptorConfiguration> &configuration);
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorPool(std::vector<DescriptorConfiguration> &configuration);
        void CreateDescriptorSets();
        void CreateDescriptorSets(std::vector<DescriptorConfiguration> &configuration);
        VkShaderModule CreateShaderModule(const std::vector<char> &code);
        void UpdateUniformBuffer(glm::mat4 translation, uint32_t currentImage);
        void Bind(VkCommandBuffer commandBuffer, Window &window);

        void Destroy();
        void DestroyGraphicsPipeline();
        void DestroyDescriptorSetLayout();
        void DestroyUniformBuffers();
        void DestroyDescriptorPool();
};