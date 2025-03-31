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
#include "data.hpp"
#include "leaves.hpp"
#include "sky.hpp"
#include "capture.hpp"
#include "water.hpp"
#include "wind.hpp"
#include "ui.hpp"
#include "buildings.hpp"

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

//#define TIMEOUT UINT64_MAX
#define TIMEOUT 1000000000

#ifndef GRASS_ENABLED
#define GRASS_ENABLED true
#endif

#ifndef TREES_ENABLED
#define TREES_ENABLED true
#endif

#ifndef QUAD_TREES_ENABLED
#define QUAD_TREES_ENABLED false
#endif

#ifndef LEAVES_ENABLED
#define LEAVES_ENABLED true
#endif

#ifndef SHADOWS_ENABLED
#define SHADOWS_ENABLED true
#endif

#ifndef WATER_ENABLED
#define WATER_ENABLED true
#endif

#ifndef WIND_ENABLED
#define WIND_ENABLED true
#endif

#ifndef UI_ENABLED
#define UI_ENABLED true
#endif

#ifndef BUILDINGS_ENABLED
#define BUILDINGS_ENABLED true
#endif

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
		//throw std::runtime_error("validation layers requested, but not available");
		std::cout << "validation layers requested, but not available" << std::endl;
		Manager::settings.validationLayersActive = false;
	}
	
	std::cout << "Validation layers: " << (Manager::settings.validationLayersActive ? "enabled" : "disabled") << std::endl;

	VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Limitless";
	//appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	appInfo.pEngineName = "No Engine";
	//appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	//appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.apiVersion = VK_API_VERSION_1_3;

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

void Graphics::PresentFrame(uint32_t imageIndex)
{
	VkSemaphore presentWaitSemaphores[] = {device.renderFinishedSemaphores[Manager::currentFrame]};

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = presentWaitSemaphores;

	VkSwapchainKHR swapChains[] = {window.swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(device.presentationQueue, &presentInfo);
}

void Graphics::RecordGraphicsCommands(uint32_t imageIndex)
{
	vkResetCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame], 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame], &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer");

	RenderGraphics(device.graphicsCommandBuffers[Manager::currentFrame], imageIndex);

	if (vkEndCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer");

	VkSubmitInfo submitInfo{};
	VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame], device.shadowSemaphores[Manager::currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};
	// VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame]};
	// VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	if (Terrain::HeightMapsGenerated() && SHADOWS_ENABLED) submitInfo.waitSemaphoreCount = 2;
	else submitInfo.waitSemaphoreCount = 1;

	//submitInfo.waitSemaphoreCount = 2;
	//submitInfo.waitSemaphoreCount = 1;

	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.graphicsCommandBuffers[Manager::currentFrame];

	VkSemaphore signalSemaphores[] = {device.renderFinishedSemaphores[Manager::currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, device.inFlightFences[Manager::currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("failed to submit graphics command buffer");
}

void Graphics::RenderGraphics(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = window.renderPass;
	renderPassInfo.framebuffer = window.swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = window.swapChainExtent;

	std::vector<VkClearValue> clearValues(3);
	clearValues[0].color = {{1.0f, 1.0f, 1.0f, 1.0f}};
	clearValues[1].color = {{1.0f, 1.0f, 1.0f, 1.0f}};
	clearValues[2].depthStencil = {1.0f, 0};

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

	if (BUILDINGS_ENABLED) Buildings::RecordGraphicsCommands(commandBuffer);

	if (TREES_ENABLED) Trees::RecordGraphicsCommands(commandBuffer);

	if (LEAVES_ENABLED) Leaves::RecordGraphicsCommands(commandBuffer);

	if (GRASS_ENABLED) Grass::RecordGraphicsCommands(commandBuffer);

	if (Manager::settings.screenQuad && Terrain::HeightMapsGenerated())
	{
		Manager::screenQuad.pipeline->BindGraphics(commandBuffer);
		Manager::globalDescriptor.Bind(commandBuffer, Manager::screenQuad.pipeline->graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
		Manager::screenQuadDescriptor.Bind(commandBuffer, Manager::screenQuad.pipeline->graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
		Manager::screenQuad.mesh->Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(Manager::screenQuad.mesh->indices.size()), 1, 0, 0, 0);
	}

	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

	Sky::RecordCommands(commandBuffer);

	if (WATER_ENABLED) Water::RecordGraphicsCommands(commandBuffer);

	if (UI_ENABLED) UI::RecordGraphicsCommands(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);
}

void Graphics::RecordCullCommands()
{
	vkResetCommandBuffer(device.cullCommandBuffers[Manager::currentFrame], 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(device.cullCommandBuffers[Manager::currentFrame], &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer");

	//Time::StartTimer();
	RenderCulling(device.cullCommandBuffers[Manager::currentFrame]);
	//Time::StopTimer("cull pass");

	if (vkEndCommandBuffer(device.cullCommandBuffers[Manager::currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.cullCommandBuffers[Manager::currentFrame];
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = &device.cullSemaphores[Manager::currentFrame];
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("failed to submit cull command buffer");
}

void Graphics::RenderCulling(VkCommandBuffer commandBuffer)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = Culling::cullPass;
	renderPassInfo.framebuffer = Culling::cullFrameBuffers[Manager::currentFrame];
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

	START_TIMER(cullTime);
	Terrain::RecordCullCommands(commandBuffer);
	STOP_TIMER(cullTime, false);
	// Grass::RecordCullingCommands(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);
}

void Graphics::RecordShadowCommands()
{
	vkResetCommandBuffer(device.shadowCommandBuffers[Manager::currentFrame], 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(device.shadowCommandBuffers[Manager::currentFrame], &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer");

	RenderShadows(device.shadowCommandBuffers[Manager::currentFrame]);

	if (vkEndCommandBuffer(device.shadowCommandBuffers[Manager::currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.shadowCommandBuffers[Manager::currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &device.shadowSemaphores[Manager::currentFrame];
	//submitInfo.signalSemaphoreCount = 0;
	//submitInfo.pSignalSemaphores = nullptr;

	//if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, device.inFlightFences[Manager::currentFrame]) != VK_SUCCESS)
	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("failed to submit shadow command buffer");
}

void Graphics::RenderShadows(VkCommandBuffer commandBuffer)
{
	int cascadeRange = Shadow::cascadeCount;
	if (Capture::capturing) cascadeRange = 1;

	for (int i = 0; i < cascadeRange; i++)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = Shadow::shadowCascadePass;
		renderPassInfo.framebuffer = Shadow::shadowCascadeFrameBuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent.width = Shadow::shadowCascadeResolutions[i];
		renderPassInfo.renderArea.extent.height = Shadow::shadowCascadeResolutions[i];

		std::vector<VkClearValue> clearValues(1);
		clearValues[0].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = Shadow::shadowCascadeResolutions[i];
		viewport.height = Shadow::shadowCascadeResolutions[i];
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent.width = Shadow::shadowCascadeResolutions[i];
		scissor.extent.height = Shadow::shadowCascadeResolutions[i];
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		if (LEAVES_ENABLED) Leaves::RecordShadowCommands(commandBuffer, i);

		if (TREES_ENABLED) Trees::RecordShadowCommands(commandBuffer, i);

		if (GRASS_ENABLED) Grass::RecordShadowCommands(commandBuffer, i);

		vkCmdEndRenderPass(commandBuffer);
	}
}

void Graphics::RecordComputeCommands(VkCommandBuffer commandBuffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer");
	}

	//Terrain::RecordComputeCommands(commandBuffer);
	if (TREES_ENABLED) Trees::RecordComputeCommands(commandBuffer);
	if (GRASS_ENABLED) Grass::RecordComputeCommands(commandBuffer);
	Data::RecordComputeCommands(commandBuffer);
	if (WIND_ENABLED) Wind::RecordComputeCommands(commandBuffer);

	//START_TIMER(dataTime); 
	//Data::RecordComputeCommands(commandBuffer); //removeee
	//STOP_TIMER(dataTime, false);

	//Sky::ComputeView(commandBuffer);
	//Sky::ComputeAerial(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
	}
}

void Graphics::Frame()
{
	vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame], VK_TRUE, TIMEOUT);
	vkResetFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame]);

	Manager::PreFrame();

	Manager::UpdateShaderVariables();

	if (Terrain::HeightMapsGenerated())
	{
		RecordCullCommands();

		ComputeFrame();

		if (SHADOWS_ENABLED) RecordShadowCommands();
	}

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.logicalDevice, window.swapChain, TIMEOUT, 
		device.imageAvailableSemaphores[Manager::currentFrame], VK_NULL_HANDLE, &imageIndex);

	RecordGraphicsCommands(imageIndex);

	PresentFrame(imageIndex);
	
	Manager::currentFrame = (Manager::currentFrame + 1) % Manager::settings.maxFramesInFlight;
}

void Graphics::ComputeFrame()
{
	Terrain::PostFrame();

	vkResetFences(device.logicalDevice, 1, &device.computeFences[Manager::currentFrame]);
	vkResetCommandBuffer(device.computeCommandBuffers[Manager::currentFrame], 0);
	RecordComputeCommands(device.computeCommandBuffers[Manager::currentFrame]);

	VkSubmitInfo submitInfo{};
	VkSemaphore waitSemaphores[] = {device.cullSemaphores[Manager::currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//submitInfo.waitSemaphoreCount = 1;
	//submitInfo.pWaitSemaphores = waitSemaphores;
	//submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.computeCommandBuffers[Manager::currentFrame];

	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	if (vkQueueSubmit(device.computeQueue, 1, &submitInfo, device.computeFences[Manager::currentFrame]) != VK_SUCCESS)
	//if (vkQueueSubmit(device.computeQueue, 1, &submitInfo, nullptr) != VK_SUCCESS)
	{
		std::cout << "error!!!!!!!!!!!!" << std::endl;
		throw std::runtime_error("failed to submit compute command buffer");
	}

	vkWaitForFences(device.logicalDevice, 1, &device.computeFences[Manager::currentFrame], VK_TRUE, TIMEOUT);

	Trees::SetData();
	Grass::SetData();
	Data::SetData();
}

void Graphics::DrawFrame() 
{
	//vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame], VK_TRUE, TIMEOUT);
	//Manager::UpdateShaderVariables();

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.logicalDevice, window.swapChain, TIMEOUT, device.imageAvailableSemaphores[Manager::currentFrame], VK_NULL_HANDLE, &imageIndex);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR)
	//{
	//	window.RecreateSwapChain();
	//	return;
	//}
	//else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	//{
	//	throw std::runtime_error("failed to acquire swap chain image");
	//}

	//vkResetFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame]);
	vkResetCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame], 0);
	//RecordGraphicsCommands(device.graphicsCommandBuffers[Manager::currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame], device.shadowSemaphores[Manager::currentFrame]};
	//VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame], device.computeFinishedSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	//VkCommandBuffer commandBuffers[] = {device.shadowCommandBuffers[Manager::currentFrame], device.graphicsCommandBuffers[Manager::currentFrame]};
	submitInfo.waitSemaphoreCount = 2;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.graphicsCommandBuffers[Manager::currentFrame];
	//submitInfo.commandBufferCount = 2;
	//submitInfo.pCommandBuffers = commandBuffers;

	VkSemaphore signalSemaphores[] = {device.renderFinishedSemaphores[Manager::currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, device.inFlightFences[Manager::currentFrame]) != VK_SUCCESS)
	{
		std::cout << "error!!!!!!!!!!!!" << std::endl;
		throw std::runtime_error("failed to submit draw command buffer");
	}

	VkSemaphore presentWaitSemaphores[] = {device.renderFinishedSemaphores[Manager::currentFrame]};

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = presentWaitSemaphores;

	VkSwapchainKHR swapChains[] = {window.swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(device.presentationQueue, &presentInfo);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized)
	//{
	//	std::cout << "doing this" << std::endl;
	//	window.framebufferResized = false;
	//	window.RecreateSwapChain();
	//}
	//else if (result != VK_SUCCESS)
	//{
	//	throw std::runtime_error("failed to present swap chain image!");
	//}
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
	double timer;

	//Time::StartTimer(timer);
	CreateInstance();
	//Time::StopTimer(timer, "instance");

	//Time::StartTimer(timer);
	window.CreateSurface(instance);
	device.Create(instance, window.surface);
	window.CreateResources();

	device.CreateCommandPools();
	device.CreateCommandBuffers();
	device.CreateSyncObjects();
	device.CreateDescriptorPool();
	//Time::StopTimer(timer, "window and device");

	std::cout << "default stuff created" << std::endl;

	//if (Manager::settings.performanceMode)
	//{
	//	Grass::grassBase = 256;
	//	Grass::grassLodBase = 1536;
	//	Trees::treeLod0RenderBase = 8;
	//	Trees::treeLod1RenderBase = 8;
	//	Trees::treeLod2RenderBase = 8;
	//	Trees::treeLod3RenderBase = 32;
	//	Shadow::shadowCascadeResolutions[0] = ceil(Shadow::shadowCascadeResolutions[0] * 0.5f);
	//	Shadow::shadowCascadeResolutions[1] = ceil(Shadow::shadowCascadeResolutions[1] * 0.5f);
	//	Shadow::shadowCascadeResolutions[2] = ceil(Shadow::shadowCascadeResolutions[2] * 0.5f);
	//	Shadow::shadowCascadeResolutions[3] = ceil(Shadow::shadowCascadeResolutions[3] * 0.5f);
	//	Shadow::shadowCascadeDistances[0] *= 0.75;
	//	Shadow::shadowCascadeDistances[1] *= 0.75;
	//	Shadow::shadowCascadeDistances[2] *= 0.75;
	//	Shadow::shadowCascadeDistances[3] *= 0.75;
	//}

	//UI::Create();

	Culling::Create();
	Shadow::Create();
	//Capture::Create();

	Manager::CreateShaderVariableBuffers();
	Manager::CreateDescriptorSetLayout();

	Texture::CreateDefaults();
	Mesh::CreateDefaults();

	Terrain::Create();
	Sky::Create();
	Wind::Create();

	Manager::CreateDescriptor();

	Grass::Create();
	Trees::Create();
	Leaves::Create();
	Water::Create();
	Data::Create();
	Buildings::Create();

	if (Manager::settings.screenQuad)
	{
		Manager::screenQuad.mesh = Manager::NewMesh();
		Manager::screenQuad.mesh->shape.SetShape(QUAD);
		Manager::screenQuad.mesh->RecalculateVertices();
		Manager::screenQuad.mesh->Create();

		Manager::screenQuad.pipeline = Manager::NewPipeline();
		PipelineConfiguration pipelineConfig = Pipeline::DefaultConfiguration();
		pipelineConfig.rasterization.cullMode = VK_CULL_MODE_NONE;

		std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
		descriptorLayoutConfig[0].type = IMAGE_SAMPLER;
		descriptorLayoutConfig[0].stages = FRAGMENT_STAGE;

		VertexInfo vertexInfo = Manager::screenQuad.mesh->MeshVertexInfo();

		Manager::screenQuad.pipeline->CreateGraphicsPipeline("screenQuad", descriptorLayoutConfig, pipelineConfig, vertexInfo);

		std::vector<DescriptorConfiguration> descriptorConfig(1);
		descriptorConfig[0].type = IMAGE_SAMPLER;
		descriptorConfig[0].stages = FRAGMENT_STAGE;
		descriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
		descriptorConfig[0].imageInfo.imageView = Wind::windTexture.imageView;
		descriptorConfig[0].imageInfo.sampler = Wind::windTexture.sampler;

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
	Leaves::Destroy();
	Data::Destroy();
	Sky::Destroy();
	Water::Destroy();
	Wind::Destroy();
	Buildings::Destroy();
	//Capture::Destroy();
	Manager::screenQuadDescriptor.Destroy();
	Manager::Clean();

	//UI::Destroy();

	device.DestroyDescriptorPool();
	device.DestroyLogicalDevice();
	window.DestroySurface(instance);
    DestroyInstance();
}