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
	Manager::currentDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(Manager::currentDevice->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(Manager::currentDevice->logicalDevice, stagingBufferMemory);

	Manager::currentDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	Manager::currentGraphics->CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(Manager::currentDevice->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(Manager::currentDevice->logicalDevice, stagingBufferMemory, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	if (indexBuffer) throw std::runtime_error("cannot create index buffer because it already exists");

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	Manager::currentDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(Manager::currentDevice->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(Manager::currentDevice->logicalDevice, stagingBufferMemory);

	Manager::currentDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	Manager::currentGraphics->CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(Manager::currentDevice->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(Manager::currentDevice->logicalDevice, stagingBufferMemory, nullptr);
}

VkVertexInputBindingDescription Mesh::Vertex::GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Mesh::Vertex::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

	return attributeDescriptions;
}

void Mesh::DestroyVertexBuffer()
{
	if (vertexBuffer)
	{
		vkDestroyBuffer(Manager::currentDevice->logicalDevice, vertexBuffer, nullptr);
		vertexBuffer = nullptr;
	}

	if (vertexBufferMemory)
	{
		vkFreeMemory(Manager::currentDevice->logicalDevice, vertexBufferMemory, nullptr);
		vertexBufferMemory = nullptr;
	}
}

void Mesh::DestroyIndexBuffer()
{
	if (indexBuffer)
	{
		vkDestroyBuffer(Manager::currentDevice->logicalDevice, indexBuffer, nullptr);
		indexBuffer = nullptr;
	}

	if (indexBufferMemory)
	{
		vkFreeMemory(Manager::currentDevice->logicalDevice, indexBufferMemory, nullptr);
		indexBufferMemory = nullptr;
	}
}

void Mesh::Destroy()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
}

void Mesh::RecalculateVertices()
{
	vertices.clear();
	vertices.resize(glm::max(shape.positions.size(), shape.coordinates.size()));

	for (int i = 0; i < vertices.size(); i++)
	{
		if (i < shape.positions.size()) vertices[i].pos = shape.positions[i];
		if (i < shape.coordinates.size()) vertices[i].texCoord = shape.coordinates[i];
	}

	indices.clear();
	indices.resize(shape.indices.size());

	for (int i = 0; i < indices.size(); i++)
	{
		indices[i] = shape.indices[i];
	}
}