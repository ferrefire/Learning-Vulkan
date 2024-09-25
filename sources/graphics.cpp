#include "graphics.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include "images.hpp"

#include <stdexcept>
#include <set>
#include <iostream>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <cstring>

Graphics::Graphics(Device &device, Window &window, Pipeline &pipeline) : device{device}, window{window}, pipeline{pipeline}
{

}

Graphics::~Graphics()
{
    Destroy();
}

void Graphics::CreateInstance()
{
    if (instance) throw std::runtime_error("cannot create instance because it already exists");

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

/*
void Graphics::CreateCommandPool()
{
	if (commandPool) throw std::runtime_error("cannot create command pool because it already exists");

	//QueueFamilies queueFamilyIndices = FindQueueFamilies(physicalDevice);
	QueueFamilies queueFamilyIndices = device.queueFamilies;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if (vkCreateCommandPool(device.logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool");
	}
}

void Graphics::CreateCommandBuffers()
{
	if (commandBuffers.size() != 0) throw std::runtime_error("cannot create command buffers because they already exists");

	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

	if (vkAllocateCommandBuffers(device.logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers");
	}
}
*/

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
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

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

	VkBuffer vertexBuffers[] = {pipeline.mesh.vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, pipeline.mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipelineLayout, 0, 1, &pipeline.descriptorSets[currentFrame], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pipeline.mesh.indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer");
	}
}

void Graphics::DrawFrame()
{
	vkWaitForFences(device.logicalDevice, 1, &device.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.logicalDevice, window.swapChain, UINT64_MAX, device.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized)
	{
		window.framebufferResized = false;
		window.RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to acquire swap chain image");
	}

	vkResetFences(device.logicalDevice, 1, &device.inFlightFences[currentFrame]);

	vkResetCommandBuffer(device.commandBuffers[currentFrame], 0);
	RecordCommandBuffer(device.commandBuffers[currentFrame], imageIndex);

	pipeline.UpdateUniformBuffer(currentFrame, window.swapChainExtent);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {device.imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = {device.renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, device.inFlightFences[currentFrame]) != VK_SUCCESS)
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

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

/*
void Graphics::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

void Graphics::CreateTextureImage()
{
	if (pipeline.textureImage != nullptr || pipeline.textureImageMemory != nullptr)
		throw std::runtime_error("cannot create texture image because it already exists");

	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	stbi_uc *pixels = Textures::LoadTexture(Utilities::GetPath() + "/textures/texture.jpg", &texWidth, &texHeight, &texChannels);
	imageSize = texWidth * texHeight * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(device.logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device.logicalDevice, stagingBufferMemory);

	Textures::FreePixels(pixels);

	Images::CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
		VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pipeline.textureImage, pipeline.textureImageMemory, device);

	TransitionImageLayout(pipeline.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(stagingBuffer, pipeline.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	TransitionImageLayout(pipeline.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(device.logicalDevice, stagingBufferMemory, nullptr);
}

VkCommandBuffer Graphics::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device.logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Graphics::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device.graphicsQueue);

	vkFreeCommandBuffers(device.logicalDevice, commandPool, 1, &commandBuffer);
}

void Graphics::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage ,0, 0, nullptr, 0, nullptr, 1, &barrier);

	EndSingleTimeCommands(commandBuffer);
}

void Graphics::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {width, height, 1};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	EndSingleTimeCommands(commandBuffer);
}

void Graphics::CreateTextureImageView()
{
	if (pipeline.textureImageView != nullptr) throw std::runtime_error("cannot create texture image view because it already exists");

	pipeline.textureImageView = Images::CreateImageView(pipeline.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, device);
}

void Graphics::CreateTextureSampler()
{
	if (pipeline.textureSampler != nullptr) throw std::runtime_error("cannot create texture sampler because it already exists");

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device.physicalDevice, &properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device.logicalDevice, &samplerInfo, nullptr, &pipeline.textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler");
	}
}

bool Graphics::HasStencilComponent(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);
}
*/

void Graphics::Create()
{
	CreateInstance();

	window.CreateSurface(instance);

	device.Create(instance, window.surface);

	window.CreateSwapChain();
	window.CreateImageViews();
	window.CreateRenderPass();

	pipeline.CreateDescriptorSetLayout();
	pipeline.CreateGraphicsPipeline("simple", "simple2", window.renderPass);

	window.CreateDepthResources();
	window.CreateFramebuffers();

	device.CreateCommandPool();

	Images::CreateTextureImage(pipeline, device);
	Images::CreateTextureImageView(pipeline, device);
	Images::CreateTextureSampler(pipeline, device);

	//CreateCommandPool();
	//CreateTextureImage();
	//CreateTextureImageView();
	//CreateTextureSampler();

	pipeline.mesh.CreateVertexBuffer();
	pipeline.mesh.CreateIndexBuffer();

	pipeline.CreateUniformBuffers();
	pipeline.CreateDescriptorPool();
	pipeline.CreateDescriptorSets();

	device.CreateCommandBuffers();

	//CreateCommandBuffers();

	device.CreateSyncObjects();
}

void Graphics::DestroyInstance()
{
    if (!instance) return ;

    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
}

/*
void Graphics::DestroyCommandPool()
{
	if (!commandPool) return;

	vkDestroyCommandPool(device.logicalDevice, commandPool, nullptr);
	commandPool = nullptr;
}

void Graphics::DestroyTextureImage()
{
	if (pipeline.textureImage != nullptr)
	{
		vkDestroyImage(device.logicalDevice, pipeline.textureImage, nullptr);
		pipeline.textureImage = nullptr;
	}
	if (pipeline.textureImageMemory != nullptr)
	{
		vkFreeMemory(device.logicalDevice, pipeline.textureImageMemory, nullptr);
		pipeline.textureImageMemory = nullptr;
	}
}

void Graphics::DestroyTextureImageView()
{
	if (!pipeline.textureImageView) return;

	vkDestroyImageView(device.logicalDevice, pipeline.textureImageView, nullptr);
	pipeline.textureImageView = nullptr;
}

void Graphics::DestroyTextureSampler()
{
	if (!pipeline.textureSampler) return;

	vkDestroySampler(device.logicalDevice, pipeline.textureSampler, nullptr);
	pipeline.textureSampler = nullptr;
}
*/

void Graphics::Destroy()
{
	device.DestroySyncObjects();

	device.DestroyCommandPool();

	//DestroyCommandPool();

	window.DestroyFramebuffers();

	pipeline.DestroyGraphicsPipeline();

	window.DestroyRenderPass();
	window.DestroyDepthResources();
	window.DestroyImageViews();
	window.DestroySwapChain();

	Images::DestroyTextureSampler(pipeline, device);
	Images::DestroyTextureImageView(pipeline, device);
	Images::DestroyTextureImage(pipeline, device);

	//DestroyTextureSampler();
	//DestroyTextureImageView();
	//DestroyTextureImage();

	pipeline.DestroyUniformBuffers();
	pipeline.DestroyDescriptorPool();
	pipeline.DestroyDescriptorSetLayout();

	pipeline.mesh.Destroy();

	device.DestroyLogicalDevice();

	window.DestroySurface(instance);

    DestroyInstance();
}