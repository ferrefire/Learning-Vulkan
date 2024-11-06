#include "shadow.hpp"

#include "manager.hpp"

#include <stdexcept>

void Shadow::CreateShadowResources()
{
	if (shadowTexture.image != nullptr || shadowTexture.imageMemory != nullptr || shadowTexture.imageView != nullptr)
		throw std::runtime_error("cannot create shadow resources because they already exist");

	ImageConfiguration imageConfig;
	// imageConfig.width = swapChainExtent.width;
	// imageConfig.height = swapChainExtent.height;
	imageConfig.width = 1024;
	imageConfig.height = 1024;
	imageConfig.format = Manager::currentDevice.FindDepthFormat();
	// imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	// imageConfig.sampleCount = device.MaxSampleCount();

	SamplerConfiguration samplerConfig;

	shadowTexture.CreateImage(imageConfig, samplerConfig);

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = shadowPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &shadowTexture.imageView;
	framebufferInfo.width = 1024;
	framebufferInfo.height = 1024;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &framebufferInfo, nullptr, &shadowFrameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow framebuffer");
	}
}

void Shadow::CreateShadowPass()
{
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = Manager::currentDevice.FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 0;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentDescription> attachments(1);
	attachments[0] = depthAttachment;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	// renderPassInfo.dependencyCount = 1;
	// renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(Manager::currentDevice.logicalDevice, &renderPassInfo, nullptr, &shadowPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow pass");
	}
}

void Shadow::DestroyShadowResources()
{
	if (shadowFrameBuffer)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, shadowFrameBuffer, nullptr);
		shadowFrameBuffer = nullptr;
	}

	shadowTexture.Destroy();
}

void Shadow::DestroyShadowPass()
{
	if (!shadowPass) return;

	vkDestroyRenderPass(Manager::currentDevice.logicalDevice, shadowPass, nullptr);
	shadowPass = nullptr;
}

VkRenderPass Shadow::shadowPass = nullptr;
VkFramebuffer Shadow::shadowFrameBuffer = nullptr;
Texture Shadow::shadowTexture;