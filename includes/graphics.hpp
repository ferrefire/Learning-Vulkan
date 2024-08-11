#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

class Graphics
{
    private:
        

    public:
        Graphics();
        ~Graphics();

        VkInstance instance = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice device = nullptr;

        void CreateInstance();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        //void CreateVulkanSurface();

        void DestroyInstance();
        void DestroyDevice();
        void Destroy();
};

#endif