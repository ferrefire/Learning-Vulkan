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

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
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
        Pipeline(Device &device, Camera &camera);
        ~Pipeline();

		UniformBufferObject ubo;
        Mesh mesh;
        Texture texture;

		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		VkPipelineLayout graphicsPipelineLayout = nullptr;
		VkPipeline graphicsPipeline = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void *> uniformBuffersMapped;

        void CreateGraphicsPipeline(std::string vertexShader, std::string fragmentShader, VkRenderPass renderPass);
        void CreateDescriptorSetLayout();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        VkShaderModule CreateShaderModule(const std::vector<char> &code);
        void UpdateUniformBuffer(uint32_t currentImage);

        void DestroyGraphicsPipeline();
        void DestroyDescriptorSetLayout();
        void DestroyUniformBuffers();
        void DestroyDescriptorPool();
};