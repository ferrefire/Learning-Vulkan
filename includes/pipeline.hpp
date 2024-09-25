#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "mesh.hpp"
#include "texture.hpp"

#include <vector>
#include <string>

class Pipeline
{
    private:
        Device &device;

        const int MAX_FRAMES_IN_FLIGHT = 2;

    public:
        Pipeline(Device &device);
        ~Pipeline();

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

        //VkImage textureImage = nullptr;
		//VkDeviceMemory textureImageMemory = nullptr;
		//VkImageView textureImageView = nullptr;
		//VkSampler textureSampler = nullptr;

        void CreateGraphicsPipeline(std::string vertexShader, std::string fragmentShader, VkRenderPass renderPass);
        void CreateDescriptorSetLayout();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        VkShaderModule CreateShaderModule(const std::vector<char> &code);
        void UpdateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);

        void DestroyGraphicsPipeline();
        void DestroyDescriptorSetLayout();
        void DestroyUniformBuffers();
        void DestroyDescriptorPool();
};