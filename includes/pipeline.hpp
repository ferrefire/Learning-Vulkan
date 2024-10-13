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
        void CreateGraphicsPipeline(std::string vertexShader, std::string fragmentShader, VertexInfo vertexInfo, PipelineConfiguration configuration);
        void CreateDescriptorSetLayout();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        VkShaderModule CreateShaderModule(const std::vector<char> &code);
        void UpdateUniformBuffer(glm::mat4 translation, uint32_t currentImage);
        void Bind(VkCommandBuffer commandBuffer, Window &window);

        void Destroy();
        void DestroyGraphicsPipeline();
        void DestroyDescriptorSetLayout();
        void DestroyUniformBuffers();
        void DestroyDescriptorPool();
};