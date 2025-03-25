#include "window.hpp"

#include "manager.hpp"
#include "input.hpp"

#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iostream>

Window::Window(Device &device) : device{device} , depthTexture{device}, colorTexture{device}
{
	
}

Window::~Window()
{
	Close();
}

void Window::Create()
{
	if (data) return ;

	glfwWindowHint(GLFW_RESIZABLE, isResizeable);

	//if (Manager::settings.fullscreen)
	//{
	//	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//	width = mode->width;
	//	height = mode->height;
	//}

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = Manager::settings.fullscreen ? mode->width : mode->width / 2;
	height = Manager::settings.fullscreen ? mode->height : mode->height / 2;

	data = glfwCreateWindow(width, height, "Vulkan window", Manager::settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    
    if (!data)
    {
        throw std::runtime_error("failed to create window");
    }

	glfwSetFramebufferSizeCallback(data, Window::framebufferResizeCallback);
	SetMouseVisibility(mouseVisible);
	glfwSetCursorPosCallback(data, Input::mouse_callback);
	glfwSetScrollCallback(data, Input::scroll_callback);
}

void Window::Destroy()
{
	if (!data) return ;

	glfwDestroyWindow(data);
	data = nullptr;
}

void Window::Close()
{
	if (!data) return ;

	glfwSetWindowShouldClose(data, true);
}

bool Window::IsOpen()
{
	if (!data) return (false);

	return (!glfwWindowShouldClose(data));
}

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
	Manager::currentWindow.framebufferResized = true;
}

void Window::CreateResources()
{
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateColorResources();
	CreateDepthResources();
	CreateMultiSampleResources();
	CreateFramebuffers();
}

void Window::CreateSurface(VkInstance instance)
{
	if (surface) throw std::runtime_error("cannot create surface because it already exists");

    if (glfwCreateWindowSurface(instance, data, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface");
    }
}

void Window::CreateSwapChain()
{
	if (swapChain) throw std::runtime_error("cannot create swap chain because it already exists");

    SwapChainSupportDetails swapChainSupport = device.swapChainSupportDetails;

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + Manager::settings.extraFrames;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

	//Manager::settings.maxFramesInFlight = imageCount;
	std::cout << "max frames in flight: " << imageCount << std::endl;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilies indices = device.queueFamilies;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentationFamily};

    if (indices.graphicsFamily != indices.presentationFamily)
    {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
    else
    {
		std::cout << "no concurrent image sharing" << std::endl;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain");
    }

	vkGetSwapchainImagesKHR(device.logicalDevice, swapChain, &imageCount, nullptr);
	std::vector<VkImage> swapChainImages;
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device.logicalDevice, swapChain, &imageCount, swapChainImages.data());
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
	width = extent.width;
	height = extent.height;

	swapChainTextures.resize(swapChainImages.size());
	for (int i = 0; i < swapChainImages.size(); i++)
	{
		swapChainTextures[i].image = swapChainImages[i];
	}
}

VkSurfaceFormatKHR Window::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        //if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_PASS_THROUGH_EXT)
        {
			std::cout << "Correct surface format found" << std::endl;
            return availableFormat;
        }
    }

	std::cout << "No correct surface format found, using other format" << std::endl;
    return (availableFormats[0]);
}

VkPresentModeKHR Window::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
	// return (VK_PRESENT_MODE_FIFO_RELAXED_KHR);
	// return (VK_PRESENT_MODE_IMMEDIATE_KHR);
	// return (VK_PRESENT_MODE_MAILBOX_KHR);

	bool mailboxAvailable = false;
	bool fifoRelaxedAvailable = false;

	for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) mailboxAvailable = true;
		if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) fifoRelaxedAvailable = true;
	}

	if (mailboxAvailable) std::cout << "Mailbox present mode found" << std::endl;
	if (fifoRelaxedAvailable) std::cout << "Fifo relaxed present mode found" << std::endl;

	if (mailboxAvailable && Manager::settings.uncappedFPS) return (VK_PRESENT_MODE_MAILBOX_KHR);
	if (fifoRelaxedAvailable && !Manager::settings.uncappedFPS) return (VK_PRESENT_MODE_FIFO_RELAXED_KHR);

	if (Manager::settings.uncappedFPS)
	{
		std::cout << "Resorting to standard immediate present mode" << std::endl;
		return (VK_PRESENT_MODE_IMMEDIATE_KHR);
	}

	std::cout << "Resorting to standard fifo present mode" << std::endl;
	return (VK_PRESENT_MODE_FIFO_KHR);
}

VkExtent2D Window::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(data, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void Window::CreateImageViews()
{
	//if (swapChainImageViews.size() != 0) throw std::runtime_error("cannot create image views because they already exists");
	for (Texture &texture : swapChainTextures)
	{
		if (texture.imageView != nullptr) throw std::runtime_error("cannot create image views because they already exist");
	}

	//swapChainImageViews.resize(swapChainImages.size());
	//for (size_t i = 0; i < swapChainImages.size(); i++)
	//{
	//	swapChainImageViews[i] = Images::CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, device);
	//}

	ImageConfiguration configuration;
	configuration.format = swapChainImageFormat;
	//configuration.sampleCount = VK_SAMPLE_COUNT_1_BIT; //check

	for (Texture &texture : swapChainTextures)
	{
		texture.CreateImageView(configuration);
	}
}

void Window::CreateFramebuffers()
{
	if (swapChainFramebuffers.size() != 0) throw std::runtime_error("cannot create framebuffers because they already exists");

	//swapChainFramebuffers.resize(swapChainImageViews.size());
	swapChainFramebuffers.resize(swapChainTextures.size());

	for (size_t i = 0; i < swapChainTextures.size(); i++)
	{
		std::vector<VkImageView> attachments;

		if (!Manager::settings.msaa)
		{
			attachments.resize(3);
			attachments[0] = swapChainTextures[i].imageView;
			attachments[1] = colorTexture.imageView;
			attachments[2] = depthTexture.imageView; 
		}
		else
		{
			attachments.resize(4);
			attachments[3] = swapChainTextures[i].imageView;
			attachments[1] = colorTexture.imageView;
			attachments[2] = depthTexture.imageView; 
			attachments[0] = multiSampleTexture.imageView;
		}
		//std::array<VkImageView, 2> attachments = {swapChainImageViews[i], depthImageView};
		//std::array<VkImageView, 3> attachments = {colorTexture.imageView, depthTexture.imageView, swapChainTextures[i].imageView};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device.logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer");
		}
	}
}

void Window::CreateDepthResources()
{
	if (depthTexture.image != nullptr || depthTexture.imageMemory != nullptr || depthTexture.imageView != nullptr)
		throw std::runtime_error("cannot create depth resources because they already exist");

	ImageConfiguration imageConfig;
	imageConfig.width = swapChainExtent.width;
	imageConfig.height = swapChainExtent.height;
	imageConfig.format = device.FindDepthFormat();
	//imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	//imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	//imageConfig.sampleCount = device.MaxSampleCount();
	imageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;

	SamplerConfiguration samplerConfig;

	depthTexture.CreateImage(imageConfig, samplerConfig);
}

void Window::CreateColorResources()
{
	if (colorTexture.image != nullptr || colorTexture.imageMemory != nullptr || colorTexture.imageView != nullptr)
		throw std::runtime_error("cannot create color resources because they already exist");

	ImageConfiguration imageConfig;
	imageConfig.width = swapChainExtent.width;
	imageConfig.height = swapChainExtent.height;
	imageConfig.format = swapChainImageFormat;
	imageConfig.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	//imageConfig.sampleCount = device.MaxSampleCount();
	imageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;
	//imageConfig.format = RGB8;

	SamplerConfiguration samplerConfig;

	colorTexture.CreateImage(imageConfig, samplerConfig);
}

void Window::CreateMultiSampleResources()
{
	if (!Manager::settings.msaa) return;

	if (multiSampleTexture.image != nullptr || multiSampleTexture.imageMemory != nullptr || multiSampleTexture.imageView != nullptr)
		throw std::runtime_error("cannot create color resources because they already exist");

	ImageConfiguration imageConfig;
	imageConfig.width = swapChainExtent.width;
	imageConfig.height = swapChainExtent.height;
	imageConfig.format = swapChainImageFormat;
	//imageConfig.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	imageConfig.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	imageConfig.sampleCount = device.MaxSampleCount();
	//imageConfig.format = RGB8;
	//imageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;

	SamplerConfiguration samplerConfig;

	multiSampleTexture.CreateImage(imageConfig, samplerConfig);
}

/*
void Window::CreateShadowResources()
{
	if (shadowTexture.image != nullptr || shadowTexture.imageMemory != nullptr || shadowTexture.imageView != nullptr)
		throw std::runtime_error("cannot create shadow resources because they already exist");

	ImageConfiguration imageConfig;
	//imageConfig.width = swapChainExtent.width;
	//imageConfig.height = swapChainExtent.height;
	imageConfig.width = 1024;
	imageConfig.height = 1024;
	imageConfig.format = device.FindDepthFormat();
	//imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	//imageConfig.sampleCount = device.MaxSampleCount();

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

	if (vkCreateFramebuffer(device.logicalDevice, &framebufferInfo, nullptr, &shadowFrameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow framebuffer");
	}
}
*/

void Window::CreateRenderPass()
{
	if (renderPass) throw std::runtime_error("cannot create render pass because it already exists");

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	//colorAttachment.format = RGB8;
	colorAttachment.samples = device.MaxSampleCount();
	//colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 1;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = device.FindDepthFormat();
	depthAttachment.samples = device.MaxSampleCount();
	//depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 2;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkAttachmentDescription swapchainAttachment{};
	swapchainAttachment.format = swapChainImageFormat;
	swapchainAttachment.samples = device.MaxSampleCount();
	swapchainAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	swapchainAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	swapchainAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	swapchainAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	swapchainAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	swapchainAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	if (Manager::settings.msaa) swapchainAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference swapchainAttachmentRef{};
	swapchainAttachmentRef.attachment = 0;
	swapchainAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription multiSampleAttachment{};
	multiSampleAttachment.format = swapChainImageFormat;
	multiSampleAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	multiSampleAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	multiSampleAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	multiSampleAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	multiSampleAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	multiSampleAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	multiSampleAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference multiSampleAttachmentRef{};
	multiSampleAttachmentRef.attachment = 3;
	multiSampleAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference inputReferences[2]{};
	inputReferences[0].attachment = 1;
	inputReferences[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	inputReferences[1].attachment = 2;
	inputReferences[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkSubpassDescription postProcessingPass{};
	postProcessingPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	postProcessingPass.colorAttachmentCount = 1;
	postProcessingPass.pColorAttachments = &swapchainAttachmentRef;
	postProcessingPass.inputAttachmentCount = 2;
	postProcessingPass.pInputAttachments = inputReferences;
	if (Manager::settings.msaa) postProcessingPass.pResolveAttachments = &multiSampleAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = 0;
	dependency.dstSubpass = 1;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	if (Manager::settings.msaa) dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	if (Manager::settings.msaa) dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[] = {swapchainAttachment, colorAttachment, depthAttachment, multiSampleAttachment};
	VkSubpassDescription subpasses[] = {subpass, postProcessingPass};

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 3;
	if (Manager::settings.msaa) renderPassInfo.attachmentCount = 4;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 2;
	renderPassInfo.pSubpasses = subpasses;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}

/*
void Window::CreateShadowPass()
{
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = device.FindDepthFormat();
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
	//renderPassInfo.dependencyCount = 1;
	//renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &shadowPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow pass");
	}
}
*/

void Window::RecreateSwapChain()
{
	std::cout << "recreating swap chain" << std::endl;

	recreatingSwapchain = true;

	int tempWidth = 0, tempHeight = 0;
	glfwGetFramebufferSize(data, &tempWidth, &tempHeight);
	while (tempWidth == 0 || tempHeight == 0)
	{
		glfwGetFramebufferSize(data, &tempWidth, &tempHeight);
		glfwWaitEvents();
	}

	//vkWaitForFences(Manager::currentDevice.logicalDevice, Manager::settings.maxFramesInFlight, Manager::currentDevice.inFlightFences.data(), VK_TRUE, UINT64_MAX);
	//device.WaitForIdle();
	vkQueueWaitIdle(device.graphicsQueue);

	DestroyDepthResources();
	DestroyColorResources();
	DestroyMultiSampleResources();
	DestroyFramebuffers();
	DestroyImageViews();
	DestroySwapChain();

	device.queueFamilies = device.FindQueueFamilies(device.physicalDevice, surface);
	device.swapChainSupportDetails = device.QuerySwapChainSupport(device.physicalDevice, surface);

	CreateSwapChain();
	CreateImageViews();
	CreateColorResources();
	CreateDepthResources();
	CreateMultiSampleResources();
	CreateFramebuffers();

	recreatingSwapchain = false;
}

void Window::DestroyResources()
{
	DestroyFramebuffers();
	DestroyRenderPass();
	DestroyDepthResources();
	DestroyColorResources();
	DestroyMultiSampleResources();
	DestroyImageViews();
	DestroySwapChain();
}

void Window::DestroySurface(VkInstance instance)
{
    if (!surface) return;

    vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = nullptr;
}

void Window::DestroySwapChain()
{
    if (swapChain != nullptr)
	{
		vkDestroySwapchainKHR(device.logicalDevice, swapChain, nullptr);
    	swapChain = nullptr;
	}

    //for (Texture &texture : swapChainTextures)
	//{
	//	texture.image = nullptr;
	//}
	//swapChainTextures.clear();

	for (Texture &texture : swapChainTextures)
	{
		texture.image = nullptr;
		texture.Destroy();
	}
	swapChainTextures.clear();
}

void Window::DestroyImageViews()
{
	//for (VkImageView imageView : swapChainImageViews)
	//{
	//	vkDestroyImageView(device.logicalDevice, imageView, nullptr);
	//}
	//swapChainImageViews.clear();

	//for (Texture &texture : swapChainTextures)
	//{
	//	texture.DestroyImageView();
	//}
}

void Window::DestroyRenderPass()
{
	if (!renderPass) return;

	vkDestroyRenderPass(device.logicalDevice, renderPass, nullptr);
	renderPass = nullptr;
}

/*
void Window::DestroyShadowPass()
{
	if (!shadowPass) return;

	vkDestroyRenderPass(device.logicalDevice, shadowPass, nullptr);
	shadowPass = nullptr;
}
*/

void Window::DestroyFramebuffers()
{
	for (auto framebuffer : swapChainFramebuffers)
	{
		vkDestroyFramebuffer(device.logicalDevice, framebuffer, nullptr);
	}

	swapChainFramebuffers.clear();
}

void Window::DestroyDepthResources()
{
	//if (depthImageView)
	//{
	//	vkDestroyImageView(device.logicalDevice, depthImageView, nullptr);
	//	depthImageView = nullptr;
	//}
	//if (depthImage)
	//{
	//	vkDestroyImage(device.logicalDevice, depthImage, nullptr);
	//	depthImage = nullptr;
	//}
	//if (depthImageMemory)
	//{
	//	vkFreeMemory(device.logicalDevice, depthImageMemory, nullptr);
	//	depthImageMemory = nullptr;
	//}

	depthTexture.Destroy();
}

void Window::DestroyColorResources()
{
	colorTexture.Destroy();
}

void Window::DestroyMultiSampleResources()
{
	if (!Manager::settings.msaa) return;

	multiSampleTexture.Destroy();
}

/*
void Window::DestroyShadowResources()
{
	if (shadowFrameBuffer) vkDestroyFramebuffer(device.logicalDevice, shadowFrameBuffer, nullptr);

	shadowTexture.Destroy();
}
*/

void Window::SetMouseVisibility(bool visible)
{
	mouseVisible = visible;
	glfwSetInputMode(data, GLFW_CURSOR, mouseVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Window::SetMouseLocked(bool locked)
{
	mouseLocked = locked;
	glfwSetInputMode(data, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_NORMAL : GLFW_CENTER_CURSOR);
}
