#ifndef MESH_HPP
#define MESH_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <array>

#include "graphics.hpp"

class Mesh
{
	private:


	public:
		Mesh();
		~Mesh();

		struct Vertex
		{
			glm::vec2 pos;
			glm::vec3 color;

			static VkVertexInputBindingDescription GetBindingDescription();
			static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
		};

		const std::vector<Vertex> vertices = {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

		static Graphics *graphics;

		VkBuffer vertexBuffer = nullptr;
		VkDeviceMemory vertexBufferMemory = nullptr;

		void CreateVertexBuffer();

		void DestroyVertexBuffer();
		void Destroy();
};

#endif