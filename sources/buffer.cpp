#include "buffer.hpp"

#include "manager.hpp"

#include <stdexcept>
#include <cstring>

BufferConfiguration Buffer::StagingBuffer()
{
	BufferConfiguration configuration;
	configuration.mapped = true;
	configuration.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	return (configuration);
}

BufferConfiguration Buffer::VertexBuffer()
{
	BufferConfiguration configuration;
	configuration.mapped = false;
	configuration.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	return (configuration);
}

BufferConfiguration Buffer::IndexBuffer()
{
	BufferConfiguration configuration;
	configuration.mapped = false;
	configuration.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	return (configuration);
}

Buffer::Buffer() : device{Manager::currentDevice}
{

}

Buffer::Buffer(Device &device) : device{device}
{

}

Buffer::~Buffer()
{
	Destroy();
}

void Buffer::Create(BufferConfiguration &configuration)
{
	CreateBuffer(configuration);
}

void Buffer::Create(void *data, BufferConfiguration &configuration)
{
	Buffer stagingBuffer;
	stagingBuffer.CreateStagingBuffer(data, configuration.size);

	Create(configuration);
	stagingBuffer.CopyTo(buffer, configuration.size);
	stagingBuffer.Destroy();
}

void Buffer::CreateBuffer(BufferConfiguration &configuration)
{
	if (buffer || memory) throw std::runtime_error("cannot create buffer because it already exists");

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = configuration.size;
	bufferInfo.usage = configuration.usage;
	bufferInfo.sharingMode = configuration.sharingMode;

	if (vkCreateBuffer(device.logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, configuration.memoryProperties);

	if (vkAllocateMemory(device.logicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory");
	}

	vkBindBufferMemory(device.logicalDevice, buffer, memory, 0);

	if (configuration.mapped) vkMapMemory(device.logicalDevice, memory, 0, configuration.size, 0, &mappedBuffer);
}

void Buffer::CreateStagingBuffer(void *data, VkDeviceSize size)
{
	BufferConfiguration stagingConfig = StagingBuffer();
	stagingConfig.size = size;
	Create(stagingConfig);

	//vkMapMemory(device.logicalDevice, stagingBuffer.memory, 0, stagingConfig.size, 0, &stagingData);
	memcpy(mappedBuffer, data, (size_t)size);
	vkUnmapMemory(device.logicalDevice, memory);
	mappedBuffer = nullptr;
}

void Buffer::Destroy()
{
	DestroyBuffer();
}

void Buffer::DestroyBuffer()
{
	if (!device.logicalDevice) return;

	if (buffer)
	{
		vkDestroyBuffer(device.logicalDevice, buffer, nullptr);
		buffer = nullptr;
	}

	if (mappedBuffer)
	{
		vkUnmapMemory(device.logicalDevice, memory);
		mappedBuffer = nullptr;
	}
	
	if (memory)
	{
		vkFreeMemory(device.logicalDevice, memory, nullptr);
		memory = nullptr;
	}
}

void Buffer::CopyTo(VkBuffer targetBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = device.BeginGraphicsCommand();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, buffer, targetBuffer, 1, &copyRegion);

	device.EndGraphicsCommand(commandBuffer);
}

void Buffer::CopyTo(VkImage targetImage, ImageConfiguration &configuration)
{
	VkCommandBuffer commandBuffer = device.BeginGraphicsCommand();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = configuration.aspect;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = configuration.arrayLayers;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {configuration.width, configuration.height, 1};

	vkCmdCopyBufferToImage(commandBuffer, buffer, targetImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	device.EndGraphicsCommand(commandBuffer);
}