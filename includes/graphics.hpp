#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Graphics
{
    private:

    public:
        Graphics();
        ~Graphics();

        struct QueueFamilies
        {
            uint32_t graphicsFamily = 0;
            bool graphicsFamilyFound = false;

            uint32_t presentationFamily = 0;
            bool presentationFamilyFound = false;

            bool Complete()
            {
                return (graphicsFamilyFound && presentationFamilyFound);
            }
        };

        VkInstance instance = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice device = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentationQueue = nullptr;
        VkSurfaceKHR surface = nullptr;

        void CreateInstance();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSurface();

        void DestroyInstance();
        void DestroyDevice();
        void DestroySurface();
        void Destroy();

        QueueFamilies FindQueueFamilies(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);
};

#endif