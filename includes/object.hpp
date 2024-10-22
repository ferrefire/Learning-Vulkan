#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "buffer.hpp"
//#include "descriptor.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

struct UniformBufferObject
{
	alignas(16) glm::mat4 model = glm::mat4(1);
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
};

class Object
{
    private:
        glm::vec3 position = glm::vec3(0);
        glm::vec3 rotation = glm::vec3(0);
        glm::vec3 scale = glm::vec3(1);

        glm::mat4 translation = glm::mat4(1);

    public:
		Mesh *mesh = nullptr;
		Pipeline *pipeline = nullptr;

		Object();
        Object(Mesh *mesh, Pipeline *pipeline);
        ~Object();

		UniformBufferObject ubo;
		std::vector<Buffer> uniformBuffers;

		//Descriptor descriptor;

		void Create();
		void CreateUniformBuffers();
		//void CreateDescriptor();

		void Destroy();
		void DestroyUniformBuffers();
		//void DestroyDescriptor();

		void UpdateUniformBuffer(uint32_t currentImage);

		void Move(glm::vec3 amount);
        void Rotate(glm::vec3 amount);
        void Resize(glm::vec3 amount);

        glm::mat4 Translation();
};
