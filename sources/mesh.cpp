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

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(graphics->device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertex buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(graphics->device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = graphics->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(graphics->device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory");
	}

	vkBindBufferMemory(graphics->device, vertexBuffer, vertexBufferMemory, 0);

	void *data;
	vkMapMemory(graphics->device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(graphics->device, vertexBufferMemory);
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
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);
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

void Mesh::Destroy()
{
	DestroyVertexBuffer();
}

Graphics *Mesh::graphics = nullptr;