#include "shadow.hpp"

#include "manager.hpp"

#include <stdexcept>

void Shadow::Create()
{
	GetShadowProjection();
	CreateShadowPass();
	CreateShadowResources();
}

void Shadow::CreateShadowResources()
{
	if (shadowTexture.image != nullptr || shadowTexture.imageMemory != nullptr || shadowTexture.imageView != nullptr)
		throw std::runtime_error("cannot create shadow resources because they already exist");

	ImageConfiguration imageConfig;
	// imageConfig.width = swapChainExtent.width;
	// imageConfig.height = swapChainExtent.height;
	imageConfig.width = shadowResolution;
	imageConfig.height = shadowResolution;
	imageConfig.format = Manager::currentDevice.FindDepthFormat();
	// imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	// imageConfig.sampleCount = device.MaxSampleCount();

	SamplerConfiguration samplerConfig;
	samplerConfig.repeatMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerConfig.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	samplerConfig.anisotrophic = VK_FALSE;

	shadowTexture.CreateImage(imageConfig, samplerConfig);

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = shadowPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &shadowTexture.imageView;
	framebufferInfo.width = shadowResolution;
	framebufferInfo.height = shadowResolution;
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

void Shadow::Destroy()
{
	DestroyShadowPass();
	DestroyShadowResources();
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

glm::mat4 Shadow::GetShadowView()
{
	glm::vec3 direction = Manager::shaderVariables.lightDirection;

	//glm::vec3 cameraFront = Manager::camera.Front();
	//cameraFront = (cameraFront + 1.0f) * 0.5f;
	//cameraFront.y = 0;

	//glm::vec3 focus = (Manager::camera.Position() + Manager::camera.Front() * 10.0f);
	glm::vec3 focus = Manager::camera.Position();
	//glm::vec3 position = focus + glm::vec3(0, shadowDistance * 2, 0) + glm::vec3(cameraFront.x, 0, cameraFront.z) * shadowDistance;
	glm::vec3 position = focus + direction * shadowDistance * 1.0f;
	//position += cameraFront * shadowDistance * 2.0f;

	glm::vec3 front = glm::normalize(-direction);
	glm::vec3 side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(side, front));
	shadowView = glm::lookAt(position, position + front, up);

	//shadowView = glm::lookAt(position, Manager::camera.Position(), glm::vec3(0, 1, 0));

	return (shadowView);
}

glm::mat4 Shadow::GetShadowProjection()
{
	//shadowProjection = glm::perspective(glm::radians(45.0f), 1024.0f / 1024.0f, 0.1f, 25000.0f);

	shadowProjection = glm::ortho(-shadowDistance, shadowDistance, -shadowDistance, shadowDistance, 1.0f, shadowDistance * 2.0f);

	shadowProjection[1][1] *= -1;

	return (shadowProjection);
}

VkRenderPass Shadow::shadowPass = nullptr;
VkFramebuffer Shadow::shadowFrameBuffer = nullptr;
Texture Shadow::shadowTexture;

glm::mat4 Shadow::shadowView = glm::mat4(1);
glm::mat4 Shadow::shadowProjection = glm::mat4(1);

int Shadow::shadowResolution = 4096;
float Shadow::shadowDistance = 20;