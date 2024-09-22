#include "mesh.hpp"

#include <stdexcept>
#include <cstring>

#include "manager.hpp"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	Destroy();
}

void Mesh::CreateVertexBuffer()
{
	if (vertexBuffer) throw std::runtime_error("cannot create vertex buffer because it already exists");

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	graphics->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(graphics->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(graphics->device, stagingBufferMemory);

	graphics->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	graphics->CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(graphics->device, stagingBuffer, nullptr);
	vkFreeMemory(graphics->device, stagingBufferMemory, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	if (indexBuffer) throw std::runtime_error("cannot create index buffer because it already exists");

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	graphics->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(graphics->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(graphics->device, stagingBufferMemory);

	graphics->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	graphics->CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(graphics->device, stagingBuffer, nullptr);
	vkFreeMemory(graphics->device, stagingBufferMemory, nullptr);
}

VkVertexInputBindingDescription Mesh::Vertex::GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Mesh::Vertex::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

	return attributeDescriptions;
}

void Mesh::DestroyVertexBuffer()
{
	if (vertexBuffer)
	{
		vkDestroyBuffer(graphics->device, vertexBuffer, nullptr);
		vertexBuffer = nullptr;
	}

	if (vertexBufferMemory)
	{
		vkFreeMemory(graphics->device, vertexBufferMemory, nullptr);
		vertexBufferMemory = nullptr;
	}
}

void Mesh::DestroyIndexBuffer()
{
	if (indexBuffer)
	{
		vkDestroyBuffer(graphics->device, indexBuffer, nullptr);
		indexBuffer = nullptr;
	}

	if (indexBufferMemory)
	{
		vkFreeMemory(graphics->device, indexBufferMemory, nullptr);
		indexBufferMemory = nullptr;
	}
}

void Mesh::Destroy()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
}

Graphics *Mesh::graphics = nullptr;