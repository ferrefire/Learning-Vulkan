#ifndef SHADER_HPP
#define SHADER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class Shader
{
	private:

	public:
		Shader();
		~Shader();

		VkShaderModule CreateShaderModule(const std::vector<char> &code);

		void Create(std::string vertexPath, std::string fragmentPath);
		void Destroy();
};

#endif