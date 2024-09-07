#include "window.hpp"

#include <stdexcept>

#include "manager.hpp"

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

	glfwWindowHint(GLFW_RESIZABLE, isResizeable);
	data = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);
    
    if (!data)
    {
        throw std::runtime_error("failed to create window");
    }

	glfwSetFramebufferSizeCallback(data, Window::framebufferResizeCallback);
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

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
	Manager::currentGraphics->framebufferResized = true;
}