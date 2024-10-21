#pragma once

#include "device.hpp"
#include "texture.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct BufferConfiguration
{
	bool mapped = false;
	VkDeviceSize size = 0;
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};

class Buffer
{
	private:
		Device &device;

	public:
		static BufferConfiguration StagingBuffer();
		static BufferConfiguration VertexBuffer();
		static BufferConfiguration IndexBuffer();

		Buffer();
		Buffer(Device &device);
		~Buffer();

		VkBuffer buffer = nullptr;
		VkDeviceMemory memory = nullptr;
		void * mappedBuffer = nullptr;

		void Create(BufferConfiguration &configuration);
		void Create(void *data, BufferConfiguration &configuration);
		void CreateBuffer(BufferConfiguration &configuration);
		void CreateStagingBuffer(void *data, VkDeviceSize size);

		void Destroy();
		void DestroyBuffer();

		void CopyTo(VkBuffer targetBuffer, VkDeviceSize size);
		void CopyTo(VkImage targetImage, ImageConfiguration &configuration);
};
