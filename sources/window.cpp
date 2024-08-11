#include "window.hpp"

#include <stdexcept>

Window::Window()
{

}

Window::~Window()
{
	Close();
}

void Window::Create()
{
	if (data) return ;

	data = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);
    
    if (!data)
    {
        throw std::runtime_error("failed to create window");
    }
}

void Window::Destroy()
{
	if (!data) return ;

	glfwDestroyWindow(data);
	data = nullptr;
}

void Window::Close()
{
	if (!data) return ;

	glfwSetWindowShouldClose(data, true);
}

bool Window::IsOpen()
{
	if (!data) return (false);

	return (!glfwWindowShouldClose(data));
}