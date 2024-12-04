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
		//throw std::runtime_error("validation layers requested, but not available");
		std::cout << "validation layers requested, but not available" << std::endl;
		Manager::settings.validationLayersActive = false;
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
	if (Manager::settings.trees) Trees::RecordGraphicsCommands(commandBuffer);
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
	//if (Shadow::trapezoidal) RenderTrapezoidShadows(commandBuffer, imageIndex);
	RenderCascadeShadows(commandBuffer, imageIndex);
}

/*void Graphics::RenderTrapezoidShadows(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkRenderPassBeginInfo lod0RenderPassInfo{};
	lod0RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	lod0RenderPassInfo.renderPass = Shadow::shadowTrapezoidPass;
	lod0RenderPassInfo.framebuffer = Shadow::shadowLod0FrameBuffer;
	lod0RenderPassInfo.renderArea.offset = {0, 0};
	lod0RenderPassInfo.renderArea.extent.width = Shadow::shadowLod0Resolution;
	lod0RenderPassInfo.renderArea.extent.height = Shadow::shadowLod0Resolution;

	std::vector<VkClearValue> clearValues(1);
	clearValues[0].depthStencil = {1.0f, 0};

	lod0RenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	lod0RenderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &lod0RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport lod0Viewport{};
	lod0Viewport.x = 0.0f;
	lod0Viewport.y = 0.0f;
	lod0Viewport.width = Shadow::shadowLod0Resolution;
	lod0Viewport.height = Shadow::shadowLod0Resolution;
	lod0Viewport.minDepth = 0.0f;
	lod0Viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &lod0Viewport);

	VkRect2D lod0Scissor{};
	lod0Scissor.offset = {0, 0};
	lod0Scissor.extent.width = Shadow::shadowLod0Resolution;
	lod0Scissor.extent.height = Shadow::shadowLod0Resolution;
	vkCmdSetScissor(commandBuffer, 0, 1, &lod0Scissor);

	Grass::RecordShadowCommands(commandBuffer, 0);
	//Trees::RecordShadowCommands(commandBuffer, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (Manager::settings.trees)
	{
		VkRenderPassBeginInfo lod1RenderPassInfo{};
		lod1RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		lod1RenderPassInfo.renderPass = Shadow::shadowTrapezoidPass;
		lod1RenderPassInfo.framebuffer = Shadow::shadowLod1FrameBuffer;
		lod1RenderPassInfo.renderArea.offset = {0, 0};
		lod1RenderPassInfo.renderArea.extent.width = Shadow::shadowLod1Resolution;
		lod1RenderPassInfo.renderArea.extent.height = Shadow::shadowLod1Resolution;

		lod1RenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		lod1RenderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &lod1RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport lod1Viewport{};
		lod1Viewport.x = 0.0f;
		lod1Viewport.y = 0.0f;
		lod1Viewport.width = Shadow::shadowLod1Resolution;
		lod1Viewport.height = Shadow::shadowLod1Resolution;
		lod1Viewport.minDepth = 0.0f;
		lod1Viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &lod1Viewport);

		VkRect2D lod1Scissor{};
		lod1Scissor.offset = {0, 0};
		lod1Scissor.extent.width = Shadow::shadowLod1Resolution;
		lod1Scissor.extent.height = Shadow::shadowLod1Resolution;
		vkCmdSetScissor(commandBuffer, 0, 1, &lod1Scissor);

		Trees::RecordShadowCommands(commandBuffer, 1);
		//Grass::RecordShadowCommands(commandBuffer, 1);

		vkCmdEndRenderPass(commandBuffer);
	}
}*/

void Graphics::RenderCascadeShadows(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	for (int i = 0; i < Shadow::cascadeCount; i++)
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

		Grass::RecordShadowCommands(commandBuffer, i);
		Trees::RecordShadowCommands(commandBuffer, i);

		vkCmdEndRenderPass(commandBuffer);
	}
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

void Graphics::RecordGraphicsCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer");
	}

	if (Manager::settings.shadows) RenderShadows(commandBuffer, imageIndex);
	RenderGraphics(commandBuffer, imageIndex);
	if (Manager::settings.occlussionCulling) RenderCulling(commandBuffer, imageIndex);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
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
	Trees::RecordComputeCommands(commandBuffer);
	Grass::RecordComputeCommands(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
	}
}

void Graphics::Frame()
{
	//if (window.recreatingSwapchain) return;
	vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame], VK_TRUE, uint64_t(1000000000));
	Manager::UpdateShaderVariables();

	ComputeFrame();
	DrawFrame();
	
	Manager::currentFrame = (Manager::currentFrame + 1) % Manager::settings.maxFramesInFlight;
}

void Graphics::ComputeFrame()
{
	Terrain::PostFrame();
	//Trees::PostFrame();
	//Grass::PostFrame();
	//// Data::SetData();
	//return;

	//vkWaitForFences(device.logicalDevice, 1, &device.computeFences[Manager::currentFrame], VK_TRUE, UINT64_MAX);
	//Manager::UpdateShaderVariables();

	//vkResetFences(device.logicalDevice, 1, &device.computeFences[Manager::currentFrame]);
	vkResetFences(device.logicalDevice, 1, &device.computeFences[0]);
	vkResetCommandBuffer(device.computeCommandBuffers[0], 0);
	RecordComputeCommands(device.computeCommandBuffers[0]);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame]};
	//VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	//submitInfo.waitSemaphoreCount = 1;
	//submitInfo.pWaitSemaphores = waitSemaphores;
	//submitInfo.pWaitDstStageMask = waitStages;
	//submitInfo.commandBufferCount = 1;
	//submitInfo.pCommandBuffers = &device.graphicsCommandBuffers[Manager::currentFrame];

	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.computeCommandBuffers[0];

	//VkSemaphore signalSemaphores[] = {device.computeFinishedSemaphore};
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = signalSemaphores;

	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	if (vkQueueSubmit(device.computeQueue, 1, &submitInfo, device.computeFences[0]) != VK_SUCCESS)
	//if (vkQueueSubmit(device.computeQueue, 1, &submitInfo, nullptr) != VK_SUCCESS)
	{
		std::cout << "error!!!!!!!!!!!!" << std::endl;
		throw std::runtime_error("failed to submit compute command buffer");
	}

	vkWaitForFences(device.logicalDevice, 1, &device.computeFences[0], VK_TRUE, uint64_t(1000000000));
	Trees::SetData();
	Grass::SetData();
}

void Graphics::DrawFrame() 
{
	vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame], VK_TRUE, uint64_t(1000000000));
	//Manager::UpdateShaderVariables();

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.logicalDevice, window.swapChain, uint64_t(1000000000), device.imageAvailableSemaphores[Manager::currentFrame], VK_NULL_HANDLE, &imageIndex);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR)
	//{
	//	window.RecreateSwapChain();
	//	return;
	//}
	//else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	//{
	//	throw std::runtime_error("failed to acquire swap chain image");
	//}

	vkResetFences(device.logicalDevice, 1, &device.inFlightFences[Manager::currentFrame]);
	vkResetCommandBuffer(device.graphicsCommandBuffers[Manager::currentFrame], 0);
	RecordGraphicsCommands(device.graphicsCommandBuffers[Manager::currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame]};
	//VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[Manager::currentFrame], device.computeFinishedSemaphore};
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
	CreateInstance();

	window.CreateSurface(instance);
	device.Create(instance, window.surface);
	window.CreateResources();

	device.CreateCommandPools();
	device.CreateCommandBuffers();
	device.CreateSyncObjects();



	std::cout << "default stuff created" << std::endl;

	if (Manager::settings.performanceMode)
	{
		Grass::grassBase = 256;
		Grass::grassLodBase = 1536;
		Trees::treeLod0RenderBase = 8;
		Trees::treeLod1RenderBase = 8;
		Trees::treeLod2RenderBase = 8;
		Trees::treeLod3RenderBase = 32;
		Shadow::shadowCascadeResolutions[0] = ceil(Shadow::shadowCascadeResolutions[0] * 0.5f);
		Shadow::shadowCascadeResolutions[1] = ceil(Shadow::shadowCascadeResolutions[1] * 0.5f);
		Shadow::shadowCascadeResolutions[2] = ceil(Shadow::shadowCascadeResolutions[2] * 0.5f);
		Shadow::shadowCascadeResolutions[3] = ceil(Shadow::shadowCascadeResolutions[3] * 0.5f);
		Shadow::shadowCascadeDistances[0] *= 0.75;
		Shadow::shadowCascadeDistances[1] *= 0.75;
		Shadow::shadowCascadeDistances[2] *= 0.75;
		Shadow::shadowCascadeDistances[3] *= 0.75;
	}

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
	if (Manager::settings.trees) Trees::Create();
	//Data::Create();

	if (Manager::settings.screenQuad)
	{
		Manager::screenQuad.mesh = Manager::NewMesh();
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
		descriptorConfig[0].imageInfo.imageView = Shadow::shadowCascadeTextures[3].imageView;
		descriptorConfig[0].imageInfo.sampler = Shadow::shadowCascadeTextures[3].sampler;

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
	Data::Destroy();
	Manager::screenQuadDescriptor.Destroy();
	Manager::Clean();

	device.DestroyLogicalDevice();
	window.DestroySurface(instance);
    DestroyInstance();
}