#include "graphics.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include "terrain.hpp"
#include "texture.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <set>
#include <iostream>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <cstring>

Graphics::Graphics(Device &device, Window &window) : device{device}, window{window}
{

}

Graphics::~Graphics()
{
    Destroy();
}

void Graphics::CreateInstance()
{
    if (instance) throw std::runtime_error("cannot create instance because it already exists");

	if (Manager::settings.validationLayersActive && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available");
	}
	
	std::cout << "Validation layers: " << (Manager::settings.validationLayersActive ? "enabled" : "disabled") << std::endl;

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

    //uint32_t glfwExtensionCount = 0;
    //const char **glfwExtensions;
    //glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //createInfo.enabledExtensionCount = glfwExtensionCount;
    //createInfo.ppEnabledExtensionNames = glfwExtensions;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (Manager::settings.validationLayersActive)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(Manager::settings.validationLayers.size());
		createInfo.ppEnabledLayerNames = Manager::settings.validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance");
    }
}

void Graphics::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = window.renderPass;
	renderPassInfo.framebuffer = window.swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = window.swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{1.0f, 1.0f, 1.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//Manager::UpdateShaderVariables();

	Terrain::RecordCommands(commandBuffer);

	//for (Object *object : Manager::objects)
	//{
	//	object->pipeline->Bind(commandBuffer, Manager::currentWindow);
	//	//object->pipeline->UpdateUniformBuffer(object->Translation(), Manager::currentFrame);
	//	object->UpdateUniformBuffer(Manager::currentFrame);
	//	object->mesh->Bind(commandBuffer);
	//	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object->mesh->indices.size()), 1, 0, 0, 0);
	//}

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
	}
}

void Graphics::DrawFrame()
{
	vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.logicalDevice, window.swapChain, UINT64_MAX, device.imageAvailableSemaphores[Manager::currentFrame], VK_NULL_HANDLE, &imageIndex);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized)
	//{
	//	window.framebufferResized = false;
	//	window.RecreateSwapChain();
	//	return;
	//}
	//else if (result != VK_SUCCESS)
	//{
	//	throw std::runtime_error("failed to acquire swap chain image");
	//}

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		window.RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image");
	}

	vkResetFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame]);

	vkResetCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame], 0);
	RecordCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.graphicsCommandBuffers[Manager::currentFrame];

	VkSemaphore signalSemaphores[] = {device.renderFinishedSemaphores[Manager::currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, device.inFlightFences[Manager::currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {window.swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(device.presentationQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized)
	{
		window.framebufferResized = false;
		window.RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	Manager::currentFrame = (Manager::currentFrame + 1) % Manager::settings.maxFramesInFlight;
}

bool Graphics::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : Manager::settings.validationLayers)
	{
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> Graphics::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    //if (Manager::settings.validationLayersActive) 
	//{
    //    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    //}

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Graphics::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void Graphics::Create()
{
	CreateInstance();

	window.CreateSurface(instance);

	device.Create(instance, window.surface);

	window.CreateResources();

	device.CreateCommandPools();
	device.CreateCommandBuffers();
	device.CreateSyncObjects();

	Manager::Create();

	Texture::CreateDefaults();
	Mesh::CreateDefaults();

	Terrain::Create();

	/*
	Object *obj1 = Manager::NewObject();
	obj1->mesh = Mesh::Cube();
	obj1->CreateUniformBuffers();
	obj1->pipeline = Manager::NewPipeline();
	obj1->pipeline->Create(&obj1->uniformBuffers);
	obj1->Resize(glm::vec3(2));
	obj1->Move(glm::vec3(0, 10, -5));

	Object *obj2 = Manager::NewObject();
	obj2->mesh = Mesh::Cube();
	obj2->CreateUniformBuffers();
	obj2->pipeline = Manager::NewPipeline();
	obj2->pipeline->Create(&obj2->uniformBuffers);
	obj2->Resize(glm::vec3(3));
	obj2->Move(glm::vec3(0, 15, -10));
	*/
}

void Graphics::DestroyInstance()
{
    if (!instance) return ;

    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
}

void Graphics::Destroy()
{
	device.DestroySyncObjects();
	device.DestroyCommandPools();

	//window.DestroyFramebuffers();
	//window.DestroyRenderPass();
	//window.DestroyDepthResources();
	//window.DestroyColorResources();
	//window.DestroyImageViews();
	//window.DestroySwapChain();

	window.DestroyResources();
	window.DestroySurface(instance);

	Terrain::Destroy();

	//Manager::DestroyPipelines();
	//Manager::DestroyTextures();
	//Manager::DestroyMeshes();
	//Manager::DestroyObjects();

	Manager::Clean();

	device.DestroyLogicalDevice();
	window.DestroySurface(instance);
    DestroyInstance();
}