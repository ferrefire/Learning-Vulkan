#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
	private:
		unsigned int width = 800;
		unsigned int height = 600;

	public:
		GLFWwindow *data = nullptr;

		Window();
		~Window();

		void Create();
		void Destroy();
		void Close();

		bool IsOpen();
};

#endif