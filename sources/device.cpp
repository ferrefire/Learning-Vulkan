#include "device.hpp"

#include "manager.hpp"

#include <stdexcept>
#include <vector>
#include <set>
#include <iostream>

Device::Device()
{

}

Device::~Device()
{

}

void Device::Create(VkInstance instance, VkSurfaceKHR surface)
{
    PickPhysicalDevice(instance, surface);
    CreateLogicalDevice(surface);
}

void Device::DestroyLogicalDevice()
{
    if (!logicalDevice) return;

    vkDestroyDevice(logicalDevice, nullptr);
    logicalDevice = nullptr;
}

void Device::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
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
        if (IsDeviceSuitable(device, surface))
        {
            physicalDevice = device; 
            break;
        }
    }

    if (!physicalDevice)
    {
        throw std::runtime_error("failed to find a suitable GPU");
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
}

bool Device::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilies tempQueueFamilies = FindQueueFamilies(device, surface);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	bool isDiscrete = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return (tempQueueFamilies.Complete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy &&
		(!Manager::settings.discrete || isDiscrete));
}

QueueFamilies Device::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilies result;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> tempQueueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, tempQueueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : tempQueueFamilies)
    {
		if (!result.graphicsFamilyFound && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
            result.graphicsFamily = i;
            result.graphicsFamilyFound = true;
        }
		else if (!result.seperateComputeFamilyFound && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
		{
			result.computeFamily = i;
			result.computeFamilyFound = true;
			result.seperateComputeFamilyFound = true;
		}

		if (!result.computeFamilyFound && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
		{
			result.computeFamily = i;
			result.computeFamilyFound = true;
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
	
	if (result.Valid()) return (result);

	throw std::runtime_error("failed to find required queue families");

	return (result);
}

bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails Device::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return (details);
}

void Device::CreateLogicalDevice(VkSurfaceKHR surface)
{
    if (logicalDevice) throw std::runtime_error("cannot create logical device because it already exists");

    queueFamilies = FindQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {queueFamilies.graphicsFamily, queueFamilies.computeFamily, queueFamilies.presentationFamily};

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
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.tessellationShader = VK_TRUE;
	if (Manager::settings.wireframe)
	{
		deviceFeatures.fillModeNonSolid = VK_TRUE;
		//deviceFeatures.wideLines = VK_TRUE;
	}

	VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(logicalDevice, queueFamilies.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilies.computeFamily, 0, &computeQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilies.presentationFamily, 0, &presentationQueue);

	//queueFamilies = FindQueueFamilies(physicalDevice, surface);
    swapChainSupportDetails = QuerySwapChainSupport(physicalDevice, surface);

	if (queueFamilies.Complete()) std::cout << "Seperate compute family found" << std::endl;
}

void Device::CreateSyncObjects()
{
    if (imageAvailableSemaphores.size() != 0 || renderFinishedSemaphores.size() != 0 || inFlightFences.size() != 0) 
		throw std::runtime_error("cannot create sync objects because they already exists");

	imageAvailableSemaphores.resize(Manager::settings.maxFramesInFlight);
	renderFinishedSemaphores.resize(Manager::settings.maxFramesInFlight);
	inFlightFences.resize(Manager::settings.maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < Manager::settings.maxFramesInFlight; i++)
	{
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame");
		}
	}
}

void Device::DestroySyncObjects()
{
    for (VkSemaphore &semaphore : imageAvailableSemaphores)
	{
		vkDestroySemaphore(logicalDevice, semaphore, nullptr);
	}
	imageAvailableSemaphores.clear();

	for (VkSemaphore &semaphore : renderFinishedSemaphores)
	{
		vkDestroySemaphore(logicalDevice, semaphore, nullptr);
	}
	renderFinishedSemaphores.clear();

	for (VkFence &fence : inFlightFences)
	{
		vkDestroyFence(logicalDevice, fence, nullptr);
	}
	inFlightFences.clear();
}

void Device::WaitForIdle()
{
    vkDeviceWaitIdle(logicalDevice);
}

uint32_t Device::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type");
}

VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format");
}

VkFormat Device::FindDepthFormat()
{
	return (FindSupportedFormat(
		{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));
}

void Device::CreateCommandPools()
{
	if (graphicsCommandPool) throw std::runtime_error("cannot create graphics command pool because it already exists");

	VkCommandPoolCreateInfo graphicsPoolInfo{};
	graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	graphicsPoolInfo.queueFamilyIndex = queueFamilies.graphicsFamily;

	if (vkCreateCommandPool(logicalDevice, &graphicsPoolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool");
	}

	if (computeCommandPool) throw std::runtime_error("cannot create compute command pool because it already exists");

	VkCommandPoolCreateInfo computePoolInfo{};
	computePoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	computePoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	computePoolInfo.queueFamilyIndex = queueFamilies.computeFamily;

	if (vkCreateCommandPool(logicalDevice, &computePoolInfo, nullptr, &computeCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool");
	}
}

void Device::CreateCommandBuffers()
{
	if (graphicsCommandBuffers.size() != 0) throw std::runtime_error("cannot create graphics command buffers because they already exists");

	graphicsCommandBuffers.resize(Manager::settings.maxFramesInFlight);

	VkCommandBufferAllocateInfo graphicsAllocInfo{};
	graphicsAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	graphicsAllocInfo.commandPool = graphicsCommandPool;
	graphicsAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	graphicsAllocInfo.commandBufferCount = (uint32_t)Manager::settings.maxFramesInFlight;

	if (vkAllocateCommandBuffers(logicalDevice, &graphicsAllocInfo, graphicsCommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers");
	}

	if (computeCommandBuffers.size() != 0) throw std::runtime_error("cannot create compute command buffers because they already exists");

	computeCommandBuffers.resize(1);

	VkCommandBufferAllocateInfo computeAllocInfo{};
	computeAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	computeAllocInfo.commandPool = computeCommandPool;
	computeAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	computeAllocInfo.commandBufferCount = (uint32_t)computeCommandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice, &computeAllocInfo, computeCommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers");
	}
}

void Device::DestroyCommandPools()
{
	if (graphicsCommandPool)
	{
		vkDestroyCommandPool(logicalDevice, graphicsCommandPool, nullptr);
		graphicsCommandPool = nullptr;
	}

	if (computeCommandPool)
	{
		vkDestroyCommandPool(logicalDevice, computeCommandPool, nullptr);
		computeCommandPool = nullptr;
	}
}

VkCommandBuffer Device::BeginGraphicsCommand()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	return commandBuffer;
}

VkCommandBuffer Device::BeginComputeCommand()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = computeCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	return commandBuffer;
}

void Device::EndGraphicsCommand(VkCommandBuffer commandBuffer)
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(logicalDevice, graphicsCommandPool, 1, &commandBuffer);
}

void Device::EndComputeCommand(VkCommandBuffer commandBuffer)
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(computeQueue);

	vkFreeCommandBuffers(logicalDevice, computeCommandPool, 1, &commandBuffer);
}

VkSampleCountFlagBits Device::MaxSampleCount() 
{
	if (!Manager::settings.mssa) return (VK_SAMPLE_COUNT_1_BIT);

	VkSampleCountFlagBits maxDevice = MaxDeviceSampleCount();

	if (Manager::settings.maxSamples < maxDevice) return (Manager::settings.maxSamples);

	return (maxDevice);
}

VkSampleCountFlagBits Device::MaxDeviceSampleCount() 
{
	VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

bool Device::HasStencilComponent(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);
}