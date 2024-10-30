#include "descriptor.hpp"

#include "manager.hpp"

#include <stdexcept>

Descriptor::Descriptor() : device{Manager::currentDevice}
{

}

Descriptor::Descriptor(Device &device) : device{device}
{

}

Descriptor::~Descriptor()
{
	Destroy();
}

void Descriptor::Create(std::vector<DescriptorConfiguration> configs, VkDescriptorSetLayout descriptorSetLayout)
{
	descriptorConfigs = std::vector<DescriptorConfiguration>(configs);

	//CreateDescriptorSetLayout(configuration);
	CreateDescriptorPool();
	CreateDescriptorSets(descriptorSetLayout);
}

/*
void Descriptor::CreateDescriptorSetLayout(std::vector<DescriptorConfiguration> &configuration)
{
	if (descriptorSetLayout)
		throw std::runtime_error("cannot create descriptor set layout because it already exists");

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.resize(configuration.size());

	int index = 0;
	for (DescriptorConfiguration &config : configuration)
	{
		bindings[index].binding = index;
		bindings[index].descriptorCount = 1;
		bindings[index].descriptorType = config.type;
		bindings[index].stageFlags = config.stages;
		bindings[index].pImmutableSamplers = nullptr;
		index++;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device.logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout");
	}
}
*/

void Descriptor::CreateDescriptorPool()
{
	if (descriptorPool) throw std::runtime_error("cannot create descriptor pool because it already exists");

	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.resize(descriptorConfigs.size());

	uint32_t count = perFrame ? static_cast<uint32_t>(Manager::settings.maxFramesInFlight) : 1;

	int index = 0;
	for (DescriptorConfiguration &config : descriptorConfigs)
	{
		poolSizes[index].type = config.type;
		poolSizes[index].descriptorCount = count * config.count;
		index++;
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = count;

	if (vkCreateDescriptorPool(device.logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool");
	}
}

void Descriptor::CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout)
{
	if (descriptorSets.size() != 0) throw std::runtime_error("cannot create descriptor sets because they already exist");

	uint32_t count = perFrame ? static_cast<uint32_t>(Manager::settings.maxFramesInFlight) : 1;

	std::vector<VkDescriptorSetLayout> layouts(count, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = count;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(count);
	if (vkAllocateDescriptorSets(device.logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	Update();
}

void Descriptor::Destroy()
{
	DestroyDescriptorPool();
	//DestroyDescriptorSetLayout();
}

/*
void Descriptor::DestroyDescriptorSetLayout()
{
	if (!descriptorSetLayout)
		return;

	vkDestroyDescriptorSetLayout(device.logicalDevice, descriptorSetLayout, nullptr);
	descriptorSetLayout = nullptr;
}
*/

void Descriptor::DestroyDescriptorPool()
{
	if (!descriptorPool) return;

	vkDestroyDescriptorPool(device.logicalDevice, descriptorPool, nullptr);
	descriptorPool = nullptr;
}

void Descriptor::Bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipelineBindPoint bindPoint, uint32_t index)
{
	uint32_t setIndex = perFrame ? Manager::currentFrame : 0;

	vkCmdBindDescriptorSets(commandBuffer, bindPoint, pipelineLayout, index, 1, &descriptorSets[setIndex], 0, nullptr);
}

void Descriptor::Update()
{
	uint32_t count = perFrame ? static_cast<uint32_t>(Manager::settings.maxFramesInFlight) : 1;

	for (size_t i = 0; i < count; i++)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites{};
		descriptorWrites.resize(descriptorConfigs.size());

		int index = 0;
		for (DescriptorConfiguration &config : descriptorConfigs)
		{
			if (config.type == UNIFORM_BUFFER && config.count > 1)
			{
				descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[index].dstSet = descriptorSets[i];
				descriptorWrites[index].dstBinding = index;
				descriptorWrites[index].dstArrayElement = 0;
				descriptorWrites[index].descriptorType = config.type;
				descriptorWrites[index].descriptorCount = config.count;
				descriptorWrites[index].pBufferInfo = &config.buffersInfo[i * config.count];
			}
			else if (config.type == UNIFORM_BUFFER)
			{
				int maxIndex = glm::min(i, config.buffersInfo.size() - 1);

				descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[index].dstSet = descriptorSets[i];
				descriptorWrites[index].dstBinding = index;
				descriptorWrites[index].dstArrayElement = 0;
				descriptorWrites[index].descriptorType = config.type;
				descriptorWrites[index].descriptorCount = 1;
				descriptorWrites[index].pBufferInfo = &config.buffersInfo[maxIndex];
			}
			else if (config.type == IMAGE_SAMPLER || config.type == IMAGE_STORAGE)
			{
				descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[index].dstSet = descriptorSets[i];
				descriptorWrites[index].dstBinding = index;
				descriptorWrites[index].dstArrayElement = 0;
				descriptorWrites[index].descriptorType = config.type;
				descriptorWrites[index].descriptorCount = 1;
				descriptorWrites[index].pImageInfo = &config.imageInfo;
			}

			index++;
		}

		vkUpdateDescriptorSets(device.logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
