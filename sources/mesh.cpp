#include "mesh.hpp"

#include "manager.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

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

void Mesh::Create(VkCommandBuffer commandBuffer)
{
	CreateVertexBuffer(commandBuffer);
	CreateIndexBuffer(commandBuffer);
	this->created = true;
}

void Mesh::CreateVertexBuffer(VkCommandBuffer commandBuffer)
{
	if (vertexBuffer.buffer || vertexBuffer.memory) throw std::runtime_error(name + ": cannot create vertex buffer because it already exists");
	if (verticesData.size() == 0) throw std::runtime_error(name + ": cannot create vertex buffer because there are no vertices");

	VkDeviceSize bufferSize = sizeof(verticesData[0]) * verticesData.size();

	BufferConfiguration configuration = Buffer::VertexBuffer();
	configuration.size = bufferSize;

	vertexBuffer.Create(verticesData.data(), configuration, commandBuffer);
}

void Mesh::CreateIndexBuffer(VkCommandBuffer commandBuffer)
{
	if (indexBuffer.buffer || indexBuffer.memory) throw std::runtime_error(name + ": cannot create index buffer because it already exists");
	if (indices.size() == 0) throw std::runtime_error(name + ": cannot create index buffer because there are no indices");

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	BufferConfiguration configuration = Buffer::IndexBuffer();
	configuration.size = bufferSize;

	indexBuffer.Create(indices.data(), configuration, commandBuffer);
}

void Mesh::Destroy()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
	this->created = false;
}

void Mesh::DestroyAtRuntime()
{
	vkWaitForFences(Manager::currentDevice.logicalDevice, Manager::settings.maxFramesInFlight, Manager::currentDevice.inFlightFences.data(), VK_TRUE, UINT64_MAX);

	Destroy();

	//shape = Shape();
	vertices.clear();
	verticesData.clear();
	indices.clear();
}

void Mesh::DestroyVertexBuffer()
{
	vertexBuffer.Destroy();
}

void Mesh::DestroyIndexBuffer()
{
	indexBuffer.Destroy();
}

void Mesh::RecalculateVertices()
{
	vertices.clear();
	//vertices.resize(glm::max(glm::max(shape.positions.size(), shape.coordinates.size()), shape.normals.size()));
	vertices.resize(shape.positions.size());

	for (int i = 0; i < vertices.size(); i++)
	{
		if (i < shape.positions.size()) vertices[i].position = shape.positions[i];
		if (i < shape.coordinates.size()) vertices[i].coordinate = shape.coordinates[i];
		if (i < shape.normals.size()) vertices[i].normal = shape.normals[i];
		if (i < shape.colors.size()) vertices[i].color = shape.colors[i];
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
		if (normal)
		{
			verticesData.push_back(vertex.normal.x);
			verticesData.push_back(vertex.normal.y);
			verticesData.push_back(vertex.normal.z);
		}
		if (color)
		{
			verticesData.push_back(vertex.color.x);
			verticesData.push_back(vertex.color.y);
			verticesData.push_back(vertex.color.z);
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
	if (!vertexBuffer.buffer || !indexBuffer.buffer)
	{
		std::cout << name + ": error: cannot bind mesh because not all buffers exist" << std::endl;
		return ;
	}

	//VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
	VkDeviceSize offsets[] = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, INDEX_TYPE);
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
		vertexInfo.attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInfo.attributeDescriptions[index].offset = vertexInfo.bindingDescription.stride;
		vertexInfo.bindingDescription.stride += sizeof(glm::vec3);
		index++;
	}

	vertexInfo.floatCount = vertexInfo.bindingDescription.stride / 4;

	return (vertexInfo);
}

Mesh *Mesh::cube = Manager::NewMesh();