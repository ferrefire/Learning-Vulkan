#include "mesh.hpp"

#include "manager.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

/*
VkVertexInputBindingDescription Vertex::GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, coordinate);

	return attributeDescriptions;
}
*/

Mesh *Mesh::Cube()
{
	return (cube);
}

void Mesh::CreateDefaults()
{
	cube->coordinate = true;
	cube->shape.SetShape(CUBE);
	cube->RecalculateVertices();
	cube->Create();
}

Mesh::Mesh() : name{"New mesh"}
{

}

Mesh::Mesh(std::string name) : name{name}
{

}

Mesh::~Mesh()
{
	Destroy();
}

void Mesh::Create()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Mesh::CreateVertexBuffer()
{
	if (vertexBuffer) throw std::runtime_error(name + ": cannot create vertex buffer because it already exists");
	if (verticesData.size() == 0) throw std::runtime_error(name + ": cannot create vertex buffer because there are no vertices");

	VkDeviceSize bufferSize = sizeof(verticesData[0]) * verticesData.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	Manager::currentDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(Manager::currentDevice.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verticesData.data(), (size_t)bufferSize);
	vkUnmapMemory(Manager::currentDevice.logicalDevice, stagingBufferMemory);

	Manager::currentDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	Manager::currentDevice.CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(Manager::currentDevice.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(Manager::currentDevice.logicalDevice, stagingBufferMemory, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	if (indexBuffer) throw std::runtime_error(name + ": cannot create index buffer because it already exists");
	if (indices.size() == 0) throw std::runtime_error(name + ": cannot create index buffer because there are no indices");

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	Manager::currentDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(Manager::currentDevice.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(Manager::currentDevice.logicalDevice, stagingBufferMemory);

	Manager::currentDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	Manager::currentDevice.CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(Manager::currentDevice.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(Manager::currentDevice.logicalDevice, stagingBufferMemory, nullptr);
}

void Mesh::Destroy()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
}

void Mesh::DestroyVertexBuffer()
{
	if (vertexBuffer)
	{
		vkDestroyBuffer(Manager::currentDevice.logicalDevice, vertexBuffer, nullptr);
		vertexBuffer = nullptr;
	}

	if (vertexBufferMemory)
	{
		vkFreeMemory(Manager::currentDevice.logicalDevice, vertexBufferMemory, nullptr);
		vertexBufferMemory = nullptr;
	}
}

void Mesh::DestroyIndexBuffer()
{
	if (indexBuffer)
	{
		vkDestroyBuffer(Manager::currentDevice.logicalDevice, indexBuffer, nullptr);
		indexBuffer = nullptr;
	}

	if (indexBufferMemory)
	{
		vkFreeMemory(Manager::currentDevice.logicalDevice, indexBufferMemory, nullptr);
		indexBufferMemory = nullptr;
	}
}

void Mesh::RecalculateVertices()
{
	vertices.clear();
	vertices.resize(glm::max(shape.positions.size(), shape.coordinates.size()));

	for (int i = 0; i < vertices.size(); i++)
	{
		if (i < shape.positions.size()) vertices[i].position = shape.positions[i];
		if (i < shape.coordinates.size()) vertices[i].coordinate = shape.coordinates[i];
	}

	VertexInfo info = MeshVertexInfo(); //Check if can be removed
	verticesData.clear();
	
	for (Vertex vertex : vertices)
	{
		if (position)
		{
			verticesData.push_back(vertex.position.x);
			verticesData.push_back(vertex.position.y);
			verticesData.push_back(vertex.position.z);
		}
		if (coordinate)
		{
			verticesData.push_back(vertex.coordinate.x);
			verticesData.push_back(vertex.coordinate.y);
		}
	}

	indices.clear();
	indices.resize(shape.indices.size());

	for (int i = 0; i < indices.size(); i++)
	{
		indices[i] = shape.indices[i];
	}
}

void Mesh::Bind(VkCommandBuffer commandBuffer)
{
	if (!vertexBuffer || !indexBuffer)
	{
		std::cout << name + ": error: cannot bind mesh because not all buffers exist" << std::endl;
		return ;
	}

	//if (bound == this) return ;
	//bound = this;

	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
}

VertexInfo Mesh::MeshVertexInfo()
{
	return (GetVertexInfo(position, coordinate, normal, color));
}

VertexInfo Mesh::GetVertexInfo(bool position, bool coordinate, bool normal, bool color)
{
	VertexInfo vertexInfo{};

	vertexInfo.bindingCount = 1;
	vertexInfo.attributeCount = position + coordinate + normal + color;
	vertexInfo.attributeDescriptions.resize(vertexInfo.attributeCount);
	
	vertexInfo.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInfo.bindingDescription.binding = 0;
	vertexInfo.bindingDescription.stride = 0;

	int index = 0;
	if (position)
	{
		vertexInfo.attributeDescriptions[index].binding = 0;
		vertexInfo.attributeDescriptions[index].location = index;
		vertexInfo.attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInfo.attributeDescriptions[index].offset = vertexInfo.bindingDescription.stride;
		vertexInfo.bindingDescription.stride += sizeof(glm::vec3);
		index++;
	}
	if (coordinate)
	{
		vertexInfo.attributeDescriptions[index].binding = 0;
		vertexInfo.attributeDescriptions[index].location = index;
		vertexInfo.attributeDescriptions[index].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInfo.attributeDescriptions[index].offset = vertexInfo.bindingDescription.stride;
		vertexInfo.bindingDescription.stride += sizeof(glm::vec2);
		index++;
	}
	if (normal)
	{
		vertexInfo.attributeDescriptions[index].binding = 0;
		vertexInfo.attributeDescriptions[index].location = index;
		vertexInfo.attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInfo.attributeDescriptions[index].offset = vertexInfo.bindingDescription.stride;
		vertexInfo.bindingDescription.stride += sizeof(glm::vec3);
		index++;
	}
	if (color)
	{
		vertexInfo.attributeDescriptions[index].binding = 0;
		vertexInfo.attributeDescriptions[index].location = index;
		vertexInfo.attributeDescriptions[index].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInfo.attributeDescriptions[index].offset = vertexInfo.bindingDescription.stride;
		vertexInfo.bindingDescription.stride += sizeof(glm::vec4);
		index++;
	}

	vertexInfo.floatCount = vertexInfo.bindingDescription.stride / 4;

	return (vertexInfo);
}

Mesh *Mesh::cube = Manager::NewMesh();
Mesh *Mesh::bound = nullptr;