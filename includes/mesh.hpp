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

class Mesh
{
	private:
		

	public:
		Mesh();
		~Mesh();

		/*
		struct VertexPositionCoordinate
		{
			glm::vec3 pos;
			glm::vec2 texCoord;

			static VkVertexInputBindingDescription GetBindingDescription();
			static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
		};

		struct VertexPosition
		{
			glm::vec3 pos;

			static VkVertexInputBindingDescription GetBindingDescription();
			static std::array<VkVertexInputAttributeDescription, 1> GetAttributeDescriptions();
		};
		*/

		Shape shape;

		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		VkBuffer vertexBuffer = nullptr;
		VkDeviceMemory vertexBufferMemory = nullptr;
		VkBuffer indexBuffer = nullptr;
		VkDeviceMemory indexBufferMemory = nullptr;

		void CreateVertexBuffer();
		void CreateIndexBuffer();

		void DestroyVertexBuffer();
		void DestroyIndexBuffer();
		void Destroy();

		void RecalculateVertices();
};