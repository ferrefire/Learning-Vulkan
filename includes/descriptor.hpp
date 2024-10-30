#pragma once

#include "device.hpp"
#include "buffer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct DescriptorConfiguration
{
	VkDescriptorType type;
	VkShaderStageFlags stages;
	uint32_t count = 1;

	std::vector<VkDescriptorBufferInfo> buffersInfo;

	VkDescriptorImageInfo imageInfo;
};

class Descriptor
{
	private:
		Device &device;

	public:
		Descriptor();
		Descriptor(Device &device);
		~Descriptor();

		bool perFrame = true;
		std::vector<DescriptorConfiguration> descriptorConfigs;

		//VkDescriptorSetLayout descriptorSetLayout = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;

		void Create(std::vector<DescriptorConfiguration> configs, VkDescriptorSetLayout descriptorSetLayout);
		//void CreateDescriptorSetLayout(std::vector<DescriptorConfiguration> &configuration);
		void CreateDescriptorPool();
		void CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout);

		void Destroy();
		//void DestroyDescriptorSetLayout();
		void DestroyDescriptorPool();

		void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipelineBindPoint bindPoint, uint32_t index);
		void Update();
};
