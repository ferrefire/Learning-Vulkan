#include "graphics.hpp"

#include <stdexcept>
#include <vector>
#include <set>

#include "manager.hpp"

Graphics::Graphics()
{

}

Graphics::~Graphics()
{
    Destroy();
}

void Graphics::CreateInstance()
{
    if (instance) return ;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Limitless";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance");
    }
}

Graphics::QueueFamilies Graphics::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilies result;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (!result.graphicsFamilyFound && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            result.graphicsFamily = i;
            result.graphicsFamilyFound = true;
        }

        if (!result.presentationFamilyFound)
        {
            VkBool32 presentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);

            if (presentationSupport)
            {
                result.presentationFamily = i;
                result.presentationFamilyFound = true;
            }
        }

        if (result.Complete()) return (result);

        i++;
    }

    //throw std::runtime_error("failed to find a suitable queue family");
    return (result);
}

bool Graphics::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilies queueFamilies = FindQueueFamilies(device);

    return (queueFamilies.Complete());
}

void Graphics::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find a GPU with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto &device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            physicalDevice = device;
            break;
        }
    }

    if (!physicalDevice)
    {
        throw std::runtime_error("failed to find a suitable GPU");
    }
}

void Graphics::CreateLogicalDevice()
{
    QueueFamilies queueFamilies = FindQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {queueFamilies.graphicsFamily, queueFamilies.presentationFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(device, queueFamilies.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueFamilies.presentationFamily, 0, &presentationQueue);
}

void Graphics::CreateSurface()
{
    if (glfwCreateWindowSurface(instance, Manager::GetWindow().data, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface");
    }
}

void Graphics::DestroyInstance()
{
    if (!instance) return ;

    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
}

void Graphics::DestroyDevice()
{
    if (!device) return;

    vkDestroyDevice(device, nullptr);
    device = nullptr;
}

void Graphics::DestroySurface()
{
    if (!surface) return;

    vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = nullptr;
}

void Graphics::Destroy()
{
    DestroyDevice();
    DestroySurface();
    DestroyInstance();
}