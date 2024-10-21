#pragma once

#include "shape.hpp"
#include "buffer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>
#include <string>

struct Vertex
{
	glm::vec3 position;
	glm::vec2 coordinate;
	glm::vec3 normal;
	glm::vec4 color;
};

struct VertexInfo
{
	uint32_t bindingCount;
	uint32_t attributeCount;
	uint32_t floatCount;
	VkVertexInputBindingDescription bindingDescription;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
};

class Mesh
{
	private:
		static Mesh *cube;
		static Mesh *bound;

	public:
		static void CreateDefaults();
		static Mesh *Cube();

		Mesh();
		Mesh(std::string name);
		~Mesh();

		std::string name;

		bool position = true;
		bool coordinate = false;
		bool normal = false;
		bool color = false;

		Shape shape;

		std::vector<Vertex> vertices;
		std::vector<float> verticesData;
		std::vector<uint16_t> indices;

		//VkBuffer vertexBuffer = nullptr;
		//VkDeviceMemory vertexBufferMemory = nullptr;
		Buffer vertexBuffer;
		//VkBuffer indexBuffer = nullptr;
		//VkDeviceMemory indexBufferMemory = nullptr;
		Buffer indexBuffer;

		void Create();
		void CreateVertexBuffer();
		void CreateIndexBuffer();

		void Destroy();
		void DestroyVertexBuffer();
		void DestroyIndexBuffer();

		void RecalculateVertices();

		void Bind(VkCommandBuffer commandBuffer);

		VertexInfo MeshVertexInfo();
		static VertexInfo GetVertexInfo(bool position = false, bool coordinate = false, bool normal = false, bool color = false);
};