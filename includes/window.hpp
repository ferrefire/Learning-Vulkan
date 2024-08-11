#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

class Window
{
	private:
		unsigned int width = 800;
		unsigned int height = 600;

	public:
		Window();
		~Window();

        GLFWwindow *data = nullptr;

        void Create();
		void Destroy();
		void Close();

		bool IsOpen();
};

#endif