#pragma once

#include "shape.hpp"
#include "vertex.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>
#include <string>

class Mesh
{
	private:
		static Mesh cube;

	public:
		static Mesh *Cube();

		Mesh();
		Mesh(std::string name);
		~Mesh();

		std::string name;

		Shape shape;

		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		VkBuffer vertexBuffer = nullptr;
		VkDeviceMemory vertexBufferMemory = nullptr;
		VkBuffer indexBuffer = nullptr;
		VkDeviceMemory indexBufferMemory = nullptr;

		void Create();
		void CreateVertexBuffer();
		void CreateIndexBuffer();

		void Destroy();
		void DestroyVertexBuffer();
		void DestroyIndexBuffer();

		void RecalculateVertices();

		void Bind(VkCommandBuffer commandBuffer);
};