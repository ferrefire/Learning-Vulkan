#include "graphics.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include "terrain.hpp"
#include "grass.hpp"
#include "texture.hpp"
#include "shadow.hpp"
#include "culling.hpp"
#include "trees.hpp"

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

void Graphics::RenderGraphics(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = window.renderPass;
	renderPassInfo.framebuffer = window.swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = window.swapChainExtent;

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = {{1.0f, 1.0f, 1.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(window.swapChainExtent.width);
	viewport.height = static_cast<float>(window.swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = window.swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	Terrain::RecordGraphicsCommands(commandBuffer);
	Trees::RecordGraphicsCommands(commandBuffer);
	//std::cout << "trees render" << std::endl;
	Grass::RecordGraphicsCommands(commandBuffer);

	if (Manager::settings.screenQuad)
	{
		Manager::screenQuad.pipeline->BindGraphics(commandBuffer);
		Manager::globalDescriptor.Bind(commandBuffer, Manager::screenQuad.pipeline->graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
		Manager::screenQuadDescriptor.Bind(commandBuffer, Manager::screenQuad.pipeline->graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
		Manager::screenQuad.mesh->Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(Manager::screenQuad.mesh->indices.size()), 1, 0, 0, 0);
	}

	vkCmdEndRenderPass(commandBuffer);
}

void Graphics::RenderShadows(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = Shadow::shadowPass;
	renderPassInfo.framebuffer = Shadow::shadowFrameBuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent.width = Shadow::shadowResolution;
	renderPassInfo.renderArea.extent.height = Shadow::shadowResolution;

	std::vector<VkClearValue> clearValues(1);
	clearValues[0].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = Shadow::shadowResolution;
	viewport.height = Shadow::shadowResolution;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent.width = Shadow::shadowResolution;
	scissor.extent.height = Shadow::shadowResolution;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	//Terrain::RecordCommands(commandBuffer, true);
	Grass::RecordShadowCommands(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);
}

void Graphics::RenderCulling(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = Culling::cullPass;
	renderPassInfo.framebuffer = Culling::cullFrameBuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent.width = Culling::cullResolutionWidth;
	renderPassInfo.renderArea.extent.height = Culling::cullResolutionHeight;

	std::vector<VkClearValue> clearValues(1);
	clearValues[0].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = Culling::cullResolutionWidth;
	viewport.height = Culling::cullResolutionHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent.width = Culling::cullResolutionWidth;
	scissor.extent.height = Culling::cullResolutionHeight;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	Terrain::RecordCullCommands(commandBuffer);
	//Grass::RecordCullingCommands(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);
}

void Graphics::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer");
	}

	RenderShadows(commandBuffer, imageIndex);
	if (Manager::settings.occlussionCulling) RenderCulling(commandBuffer, imageIndex);
	RenderGraphics(commandBuffer, imageIndex);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
	}
}

void Graphics::DrawFrame()
{
	Manager::UpdateShaderVariables();
	Terrain::PostFrame();
	Trees::PostFrame();
	Grass::PostFrame();

	vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.logicalDevice, window.swapChain, UINT64_MAX, device.imageAvailableSemaphores[Manager::currentFrame], VK_NULL_HANDLE, &imageIndex);

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

	//VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &device.imageAvailableSemaphores[Manager::currentFrame];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.graphicsCommandBuffers[Manager::currentFrame];

	//VkSemaphore signalSemaphores[] = {device.renderFinishedSemaphores[Manager::currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &device.renderFinishedSemaphores[Manager::currentFrame];

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, device.inFlightFences[Manager::currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &device.renderFinishedSemaphores[Manager::currentFrame];

	//VkSwapchainKHR swapChains[] = {window.swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &window.swapChain;
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

	Culling::Create();
	Shadow::Create();

	//Manager::Create();
	Manager::CreateShaderVariableBuffers();
	Manager::CreateDescriptorSetLayout();

	Texture::CreateDefaults();
	Mesh::CreateDefaults();

	Terrain::Create();
	Manager::CreateDescriptor();
	Grass::Create();
	Trees::Create();

	if (Manager::settings.screenQuad)
	{
		Manager::screenQuad.mesh = Manager::NewMesh();
		Manager::screenQuad.mesh->shape.positionsOnly = true;
		Manager::screenQuad.mesh->shape.SetShape(QUAD);
		Manager::screenQuad.mesh->RecalculateVertices();
		Manager::screenQuad.mesh->Create();

		Manager::screenQuad.pipeline = Manager::NewPipeline();
		PipelineConfiguration pipelineConfig = Pipeline::DefaultConfiguration();

		std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
		descriptorLayoutConfig[0].type = IMAGE_SAMPLER;
		descriptorLayoutConfig[0].stages = FRAGMENT_STAGE;

		VertexInfo vertexInfo = Manager::screenQuad.mesh->MeshVertexInfo();

		Manager::screenQuad.pipeline->CreateGraphicsPipeline("screenQuad", descriptorLayoutConfig, pipelineConfig, vertexInfo);

		std::vector<DescriptorConfiguration> descriptorConfig(1);
		descriptorConfig[0].type = IMAGE_SAMPLER;
		descriptorConfig[0].stages = FRAGMENT_STAGE;
		descriptorConfig[0].imageInfo.imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[0].imageInfo.imageView = Culling::cullTexture.imageView;
		descriptorConfig[0].imageInfo.sampler = Culling::cullTexture.sampler;

		Manager::screenQuadDescriptor.Create(descriptorConfig, Manager::screenQuad.pipeline->objectDescriptorSetLayout);
	}

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

	window.DestroyResources();

	Culling::Destroy();

	Shadow::Destroy();

	Terrain::Destroy();
	Grass::Destroy();
	Trees::Destroy();
	Manager::screenQuadDescriptor.Destroy();
	Manager::Clean();

	device.DestroyLogicalDevice();
	window.DestroySurface(instance);
    DestroyInstance();
}