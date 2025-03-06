#include "capture.hpp"

#include "manager.hpp"

#include <stdexcept>

void Capture::Create()
{
    captureProjection = glm::perspective(glm::radians(Manager::camera.FOV), (float)captureWidth / (float)captureHeight, Manager::camera.near, Manager::camera.far);
	captureProjection[1][1] *= -1;

    captureTextures.resize(captureCount);
    captureFramebuffers.resize(captureCount);

    CreateRenderPass();
    CreateTextures();
    CreateFramebuffers();
}

void Capture::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
	colorAttachment.format = RGB8;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = LAYOUT_READ_ONLY;

    VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
	depthAttachment.format = Manager::currentDevice.FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<VkAttachmentDescription> attachments(2);
	attachments[0] = colorAttachment;
	attachments[1] = depthAttachment;

    VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(Manager::currentDevice.logicalDevice, &renderPassInfo, nullptr, &capturePass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create capture pass");
	}
}

void Capture::CreateTextures()
{
    SamplerConfiguration colorSamplerConfig;
    ImageConfiguration colorConfig;
	colorConfig.width = captureWidth;
	colorConfig.height = captureHeight;
	colorConfig.format = RGB8;
	colorConfig.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	colorConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	colorConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;
    colorConfig.transitionLayout = LAYOUT_READ_ONLY;

	colorTexture.CreateImage(colorConfig, colorSamplerConfig);
    colorTexture.TransitionImageLayout(colorConfig);

    SamplerConfiguration depthSamplerConfig;
    ImageConfiguration depthConfig;
	depthConfig.width = captureWidth;
	depthConfig.height = captureHeight;
	depthConfig.format = Manager::currentDevice.FindDepthFormat();
	depthConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;

	depthTexture.CreateImage(depthConfig, depthSamplerConfig);

    for (Texture &texture : captureTextures)
    {
	    texture.CreateImage(colorConfig, colorSamplerConfig);
        texture.TransitionImageLayout(colorConfig);
    }

    /*SamplerConfiguration captureSamplerConfig;
    ImageConfiguration captureConfig = Texture::ImageStorage(captureWidth, captureHeight);
    captureConfig.format = RGB8;

	captureTexture.CreateImage(captureConfig, captureSamplerConfig);
	captureTexture.TransitionImageLayout(captureConfig);*/
}

void Capture::CreateFramebuffers()
{
    for (int i = 0; i < captureFramebuffers.size(); i++)
    {
        std::vector<VkImageView> attachments(2);
        attachments[0] = captureTextures[i].imageView;
        attachments[1] = depthTexture.imageView;

        VkFramebufferCreateInfo captureFramebufferInfo{};
	    captureFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	    captureFramebufferInfo.renderPass = capturePass;
	    captureFramebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	    captureFramebufferInfo.pAttachments = attachments.data();
	    captureFramebufferInfo.width = captureWidth;
	    captureFramebufferInfo.height = captureHeight;
	    captureFramebufferInfo.layers = 1;

	    if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &captureFramebufferInfo, nullptr, &captureFramebuffers[i]) != VK_SUCCESS)
	    {
	    	throw std::runtime_error("failed to create capture framebuffer");
	    }
    }
}

void Capture::Destroy()
{
    DestroyRenderPass();
    DestroyTextures();
    DestroyFramebuffers();
}

void Capture::DestroyRenderPass()
{
    if (capturePass)
	{
		vkDestroyRenderPass(Manager::currentDevice.logicalDevice, capturePass, nullptr);
		capturePass = nullptr;
	}
}

void Capture::DestroyTextures()
{
    colorTexture.Destroy();
    depthTexture.Destroy();
    //captureTexture.Destroy();

    for (Texture &texture : captureTextures)
    {
        texture.Destroy();
    }
    captureTextures.clear();
}

void Capture::DestroyFramebuffers()
{
    //if (captureFramebuffer)
	//{
	//	vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, captureFramebuffer, nullptr);
	//	captureFramebuffer = nullptr;
	//}

    for (VkFramebuffer framebuffer : captureFramebuffers)
    {
        vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, framebuffer, nullptr);
    }
    captureFramebuffers.clear();
}

void Capture::StartCapturing(int iteration)
{
    if (captureCommandBuffer != nullptr || iteration >= captureFramebuffers.size()) return;

    captureCommandBuffer = Manager::currentDevice.BeginGraphicsCommand();

    VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = capturePass;
	renderPassInfo.framebuffer = captureFramebuffers[iteration];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent.width = captureWidth;
	renderPassInfo.renderArea.extent.height = captureHeight;

    std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = {{1.0f, 1.0f, 1.0f, 0.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(captureCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(captureWidth);
	viewport.height = static_cast<float>(captureHeight);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(captureCommandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent.width = captureWidth;
	scissor.extent.height = captureHeight;
	vkCmdSetScissor(captureCommandBuffer, 0, 1, &scissor);
}

void Capture::StopCapturing()
{
    if (captureCommandBuffer == nullptr) return;

    vkCmdEndRenderPass(captureCommandBuffer);

    Manager::currentDevice.EndGraphicsCommand(captureCommandBuffer);
    captureCommandBuffer = nullptr;
}

uint32_t Capture::captureWidth = 32;
uint32_t Capture::captureHeight = 32;
glm::mat4 Capture::captureProjection = glm::mat4(1.0);

VkRenderPass Capture::capturePass = nullptr;

Texture Capture::colorTexture;
Texture Capture::depthTexture;
std::vector<Texture> Capture::captureTextures;
std::vector<VkFramebuffer> Capture::captureFramebuffers;

VkCommandBuffer Capture::captureCommandBuffer = nullptr;

int Capture::captureCount = 8;
bool Capture::capturing = false;

//Pipeline Capture::graphicsPipeline{Manager::currentDevice, Manager::camera};