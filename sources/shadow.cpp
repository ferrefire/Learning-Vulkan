#include "shadow.hpp"

#include "manager.hpp"

#include <stdexcept>

void Shadow::Create()
{
	GetShadowProjection(0);
	GetShadowProjection(1);
	CreateShadowPass();
	CreateShadowResources();
}

void Shadow::CreateShadowResources()
{
	//if (shadowTexture.image != nullptr || shadowTexture.imageMemory != nullptr || shadowTexture.imageView != nullptr)
	//	throw std::runtime_error("cannot create shadow resources because they already exist");

	ImageConfiguration imageLod0Config;
	imageLod0Config.width = shadowLod0Resolution;
	imageLod0Config.height = shadowLod0Resolution;
	imageLod0Config.format = Manager::currentDevice.FindDepthFormat();
	imageLod0Config.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageLod0Config.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageLod0Config.transitionLayout = LAYOUT_READ_ONLY;

	ImageConfiguration imageLod1Config;
	imageLod1Config.width = shadowLod1Resolution;
	imageLod1Config.height = shadowLod1Resolution;
	imageLod1Config.format = Manager::currentDevice.FindDepthFormat();
	imageLod1Config.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageLod1Config.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageLod1Config.transitionLayout = LAYOUT_READ_ONLY;

	SamplerConfiguration samplerConfig;
	samplerConfig.repeatMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerConfig.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	samplerConfig.anisotrophic = VK_FALSE;

	shadowLod0Texture.CreateImage(imageLod0Config, samplerConfig);
	shadowLod1Texture.CreateImage(imageLod1Config, samplerConfig);
	shadowLod0Texture.TransitionImageLayout(imageLod0Config);
	shadowLod1Texture.TransitionImageLayout(imageLod1Config);

	VkFramebufferCreateInfo lod0FramebufferInfo{};
	lod0FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	lod0FramebufferInfo.renderPass = shadowPass;
	lod0FramebufferInfo.attachmentCount = 1;
	lod0FramebufferInfo.pAttachments = &shadowLod0Texture.imageView;
	lod0FramebufferInfo.width = shadowLod0Resolution;
	lod0FramebufferInfo.height = shadowLod0Resolution;
	lod0FramebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &lod0FramebufferInfo, nullptr, &shadowLod0FrameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow framebuffer");
	}

	VkFramebufferCreateInfo lod1FramebufferInfo{};
	lod1FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	lod1FramebufferInfo.renderPass = shadowPass;
	lod1FramebufferInfo.attachmentCount = 1;
	lod1FramebufferInfo.pAttachments = &shadowLod1Texture.imageView;
	lod1FramebufferInfo.width = shadowLod1Resolution;
	lod1FramebufferInfo.height = shadowLod1Resolution;
	lod1FramebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &lod1FramebufferInfo, nullptr, &shadowLod1FrameBuffer) != VK_SUCCESS)
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
	if (shadowLod0FrameBuffer)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, shadowLod0FrameBuffer, nullptr);
		shadowLod0FrameBuffer = nullptr;
	}
	if (shadowLod1FrameBuffer)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, shadowLod1FrameBuffer, nullptr);
		shadowLod1FrameBuffer = nullptr;
	}

	shadowLod0Texture.Destroy();
	shadowLod1Texture.Destroy();
}

void Shadow::DestroyShadowPass()
{
	if (!shadowPass) return;

	vkDestroyRenderPass(Manager::currentDevice.logicalDevice, shadowPass, nullptr);
	shadowPass = nullptr;
}

glm::mat4 Shadow::GetShadowView(int lod)
{
	glm::vec3 direction = Manager::shaderVariables.lightDirection;

	glm::vec3 focus = Manager::camera.Position();

	glm::vec3 front = glm::normalize(-direction);
	glm::vec3 side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(side, front));

	if (lod == 0)
	{
		glm::vec3 position = focus + direction * shadowLod0Distance;
		shadowLod0View = glm::lookAt(position, position + front, up);
		return (shadowLod0View);
	}
	else
	{
		glm::vec3 position = focus + direction * shadowLod1Distance;
		shadowLod1View = glm::lookAt(position, position + front, up);
		return (shadowLod1View);
	}
}

glm::mat4 Shadow::GetShadowProjection(int lod)
{
	if (lod == 0)
	{
		//shadowLod0Projection = Manager::camera.CreateBoundedOrtho(GetShadowView(0));
		//shadowLod0Projection[1][1] *= -1;
		//return (shadowLod0Projection);

		shadowLod0Projection = glm::ortho(-shadowLod0Distance, shadowLod0Distance, -shadowLod0Distance, shadowLod0Distance, 1.0f, shadowLod0Distance * 2.0f);
		shadowLod0Projection[1][1] *= -1;
		return (shadowLod0Projection);
	}
	else
	{
		//shadowLod1Projection = Manager::camera.CreateBoundedOrtho(GetShadowView(1));
		//shadowLod1Projection[1][1] *= -1;
		//return (shadowLod1Projection);

		shadowLod1Projection = glm::ortho(-shadowLod1Distance, shadowLod1Distance, -shadowLod1Distance, shadowLod1Distance, 1.0f, shadowLod1Distance * 2.0f);
		shadowLod1Projection[1][1] *= -1;
		return (shadowLod1Projection);
	}
}

VkRenderPass Shadow::shadowPass = nullptr;
VkFramebuffer Shadow::shadowLod0FrameBuffer = nullptr;
VkFramebuffer Shadow::shadowLod1FrameBuffer = nullptr;
Texture Shadow::shadowLod0Texture;
Texture Shadow::shadowLod1Texture;

glm::mat4 Shadow::shadowLod0View = glm::mat4(1);
glm::mat4 Shadow::shadowLod0Projection = glm::mat4(1);
glm::mat4 Shadow::shadowLod1View = glm::mat4(1);
glm::mat4 Shadow::shadowLod1Projection = glm::mat4(1);

int Shadow::shadowLod0Resolution = 4096;
float Shadow::shadowLod0Distance = 20;
int Shadow::shadowLod1Resolution = 4096 * 2;
float Shadow::shadowLod1Distance = 250;