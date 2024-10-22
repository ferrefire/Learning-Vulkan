#include "window.hpp"

#include "manager.hpp"
#include "input.hpp"

#include <stdexcept>
#include <algorithm>

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

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

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
        {
            return availableFormat;
        }
    }

    return (availableFormats[0]);
}

VkPresentModeKHR Window::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
	for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

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
		//std::array<VkImageView, 2> attachments = {swapChainImageViews[i], depthImageView};
		std::array<VkImageView, 3> attachments = {colorTexture.imageView, depthTexture.imageView, swapChainTextures[i].imageView};

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
	imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageConfig.sampleCount = device.MaxSampleCount();

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
	imageConfig.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageConfig.sampleCount = device.MaxSampleCount();

	SamplerConfiguration samplerConfig;

	colorTexture.CreateImage(imageConfig, samplerConfig);
}

void Window::CreateRenderPass()
{
	if (renderPass) throw std::runtime_error("cannot create render pass because it already exists");

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = device.MaxSampleCount();
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = device.FindDepthFormat();
	depthAttachment.samples = device.MaxSampleCount();
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	//dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}

void Window::RecreateSwapChain()
{
	int tempWidth = 0, tempHeight = 0;
	glfwGetFramebufferSize(data, &tempWidth, &tempHeight);
	while (tempWidth == 0 || tempHeight == 0)
	{
		glfwGetFramebufferSize(data, &tempWidth, &tempHeight);
		glfwWaitEvents();
	}

	device.WaitForIdle();

	DestroyDepthResources();
	DestroyColorResources();
	DestroyFramebuffers();
	DestroyImageViews();
	DestroySwapChain();

	device.queueFamilies = device.FindQueueFamilies(device.physicalDevice, surface);
	device.swapChainSupportDetails = device.QuerySwapChainSupport(device.physicalDevice, surface);

	CreateSwapChain();
	CreateImageViews();
	CreateColorResources();
	CreateDepthResources();
	CreateFramebuffers();
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

    for (Texture &texture : swapChainTextures)
	{
		texture.image = nullptr;
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

	for (Texture &texture : swapChainTextures)
	{
		texture.DestroyImageView();
	}
}

void Window::DestroyRenderPass()
{
	if (!renderPass) return;

	vkDestroyRenderPass(device.logicalDevice, renderPass, nullptr);
	renderPass = nullptr;
}

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

void Window::SetMouseVisibility(bool visible)
{
	mouseVisible = visible;
	glfwSetInputMode(data, GLFW_CURSOR, mouseVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
