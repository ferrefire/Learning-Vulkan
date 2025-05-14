#include "culling.hpp"

#include "manager.hpp"

#include <stdexcept>

void Culling::Create()
{
	cullResolutionWidth = int(Manager::currentWindow.width * 0.25);
	cullResolutionHeight = int(Manager::currentWindow.height * 0.25);

	cullProjection = glm::perspective(glm::radians(Manager::camera.FOV), (float)cullResolutionWidth / 
		(float)cullResolutionHeight, Manager::camera.near, Manager::camera.far);
	cullProjection[1][1] *= -1;

	CreateCullPass();
	CreateCullResources();
}

void Culling::CreateCullResources()
{
	if (cullTextures.size() != 0)
		throw std::runtime_error("cannot create cull resources because they already exist");

	//cullResolutionWidth = Manager::currentWindow.width;
	//cullResolutionHeight = Manager::currentWindow.height;

	ImageConfiguration imageConfig;
	// imageConfig.width = swapChainExtent.width;
	// imageConfig.height = swapChainExtent.height;
	imageConfig.width = cullResolutionWidth;
	imageConfig.height = cullResolutionHeight;
	imageConfig.format = Manager::currentDevice.FindDepthFormat();
	imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageConfig.transitionLayout = LAYOUT_READ_ONLY;

	SamplerConfiguration samplerConfig;
	samplerConfig.repeatMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerConfig.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerConfig.anisotrophic = VK_FALSE;

	cullTextures.resize(Manager::settings.maxFramesInFlight);
	for (Texture &texture : cullTextures)
	{
		texture.CreateImage(imageConfig, samplerConfig);
		// cullTexture.TransitionImageLayout(imageConfig);
	}

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = cullPass;
	framebufferInfo.attachmentCount = 1;
	//framebufferInfo.pAttachments = &cullTexture.imageView;
	framebufferInfo.pAttachments = &cullTextures[0].imageView;
	framebufferInfo.width = cullResolutionWidth;
	framebufferInfo.height = cullResolutionHeight;
	framebufferInfo.layers = 1;

	cullFrameBuffers.resize(Manager::settings.maxFramesInFlight);
	int i = 0;
	for (VkFramebuffer &frameBuffer : cullFrameBuffers)
	{
		//framebufferInfo.pAttachments = &cullTextures[i].imageView;
		if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &framebufferInfo, nullptr, &frameBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create cull framebuffer");
		}
		i++;
	}
}

void Culling::CreateCullPass()
{
	//cullResolutionWidth = Manager::currentWindow.width;
	//cullResolutionHeight = Manager::currentWindow.height;

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

	if (vkCreateRenderPass(Manager::currentDevice.logicalDevice, &renderPassInfo, nullptr, &cullPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create cull pass");
	}
}

void Culling::Destroy()
{
	DestroyCullPass();
	DestroyCullResources();
}

void Culling::DestroyCullResources()
{
	//if (cullFrameBuffer)
	//{
	//	vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, cullFrameBuffer, nullptr);
	//	cullFrameBuffer = nullptr;
	//}

	// cullTexture.Destroy();

	for (VkFramebuffer &frameBuffer : cullFrameBuffers)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, frameBuffer, nullptr);
	}
	cullFrameBuffers.clear();

	for (Texture &texture : cullTextures)
	{
		texture.Destroy();
	}
	cullTextures.clear();
}

void Culling::DestroyCullPass()
{
	if (!cullPass)
		return;

	vkDestroyRenderPass(Manager::currentDevice.logicalDevice, cullPass, nullptr);
	cullPass = nullptr;
}

VkRenderPass Culling::cullPass = nullptr;
//VkFramebuffer Culling::cullFrameBuffer = nullptr;
//Texture Culling::cullTexture;
std::vector<VkFramebuffer> Culling::cullFrameBuffers;
std::vector<Texture> Culling::cullTextures;

int Culling::cullResolutionWidth = 960;
int Culling::cullResolutionHeight = 540;
float Culling::cullDistance = 20;

glm::mat4 Culling::cullProjection = glm::mat4(1);