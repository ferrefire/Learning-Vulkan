#include "texture.hpp"

#include "manager.hpp"
#include "utilities.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

Texture *Texture::Statue()
{
	return (statue);
}

void Texture::CreateDefaults()
{
	statue->CreateTexture("texture.jpg");
}

ImageConfiguration Texture::ImageStorage(uint32_t width, uint32_t height)
{
	ImageConfiguration imageConfig;
	imageConfig.width = width;
	imageConfig.height = height;
	imageConfig.format = R16;
	//imageConfig.format = R8G8B8A8;
	imageConfig.layout = LAYOUT_GENERAL;
	imageConfig.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	imageConfig.createMipmaps = false;

	return (imageConfig);
}

Texture::Texture() : device{Manager::currentDevice}
{

}

Texture::Texture(Device &device) : device{device}
{

}

Texture::~Texture()
{
    Destroy();
}

void Texture::CreateTexture(std::string name)
{
	SamplerConfiguration samplerConfig;

    CreateTextureImage(name, samplerConfig);
}

void Texture::CreateTexture(std::string name, SamplerConfiguration &samplerConfig)
{
	CreateTextureImage(name, samplerConfig);
}

void Texture::Destroy()
{
    DestroySampler();
    DestroyImageView();
    DestroyImage();
}

stbi_uc *Texture::LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels)
{
	stbi_uc *pixels = stbi_load(path.c_str(), texWidth, texHeight, texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image");
	}

	return (pixels);
}

void Texture::CreateTextureImage(std::string name, SamplerConfiguration &samplerConfig)
{
    if (image != nullptr || imageMemory != nullptr)
		throw std::runtime_error("cannot create texture image because it already exists");

	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	stbi_uc *pixels = Texture::LoadTexture(Utilities::GetPath() + "/textures/" + name, &texWidth, &texHeight, &texChannels);
	imageSize = texWidth * texHeight * 4; //corrupt image error probably

	ImageConfiguration imageConfig;
	imageConfig.width = texWidth;
	imageConfig.height = texHeight;
	imageConfig.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageConfig.layout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageConfig.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	Buffer stagingBuffer;
	stagingBuffer.CreateStagingBuffer(pixels, imageSize);

	stbi_image_free(pixels);

	CreateImage(imageConfig);

	TransitionImageLayout(imageConfig, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	stagingBuffer.CopyTo(image, imageConfig);
	stagingBuffer.Destroy();

	if (imageConfig.createMipmaps) CreateMipmaps(imageConfig);

	CreateImageView(imageConfig, samplerConfig);
}

void Texture::CreateImage(ImageConfiguration &imageConfig)
{
    if (image != nullptr || imageMemory != nullptr)
		throw std::runtime_error("cannot create texture image because it already exists");

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = imageConfig.type;
	imageInfo.extent.width = imageConfig.width;
	imageInfo.extent.height = imageConfig.height;
	imageInfo.extent.depth = imageConfig.depth;
	imageInfo.mipLevels = imageConfig.mipLevels;
	imageInfo.arrayLayers = imageConfig.arrayLayers;
	imageInfo.format = imageConfig.format;
	imageInfo.tiling = imageConfig.tiling;
	imageInfo.initialLayout = imageConfig.layout;
	imageInfo.usage = imageConfig.usage;
	imageInfo.samples = imageConfig.sampleCount;
	imageInfo.sharingMode = imageConfig.sharingMode;

	if (vkCreateImage(device.logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.logicalDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, imageConfig.memoryProperties);

	if (vkAllocateMemory(device.logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory");
	}

	vkBindImageMemory(device.logicalDevice, image, imageMemory, 0);
}

void Texture::CreateImage(ImageConfiguration &imageConfig, SamplerConfiguration &samplerConfig)
{
	CreateImage(imageConfig);
	CreateImageView(imageConfig, samplerConfig);
}

void Texture::CreateImageView(ImageConfiguration &imageConfig)
{
	SamplerConfiguration samplerConfig;
	CreateImageView(imageConfig, samplerConfig);
}

void Texture::CreateImageView(ImageConfiguration &imageConfig, SamplerConfiguration &samplerConfig)
{
    if (imageView != nullptr) throw std::runtime_error("cannot create texture image view because it already exists");

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = imageConfig.viewType;
	viewInfo.format = imageConfig.format;
	viewInfo.subresourceRange.aspectMask = imageConfig.aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = imageConfig.mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = imageConfig.arrayLayers;

	if (vkCreateImageView(device.logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image view");
	}

	//imageView = Images::CreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, *device);

	//SamplerConfiguration samplerConfiguration;
	CreateSampler(samplerConfig);
}

/*
void Texture::CreateSampler()
{
    if (sampler != nullptr) throw std::runtime_error("cannot create texture sampler because it already exists");

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device.logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler");
	}
}
*/

void Texture::CreateSampler(SamplerConfiguration &samplerConfig)
{
    if (sampler != nullptr) throw std::runtime_error("cannot create texture sampler because it already exists");

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = samplerConfig.magFilter;
	samplerInfo.minFilter = samplerConfig.minFilter;
	samplerInfo.addressModeU = samplerConfig.repeatMode;
	samplerInfo.addressModeV = samplerConfig.repeatMode;
	samplerInfo.addressModeW = samplerConfig.repeatMode;
	samplerInfo.anisotropyEnable = samplerConfig.anisotrophic;
	samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = samplerConfig.borderColor;
	samplerInfo.unnormalizedCoordinates = samplerConfig.unnormalizedCoordinates;
	samplerInfo.compareEnable = samplerConfig.compare;
	samplerInfo.compareOp = samplerConfig.compareOp;
	samplerInfo.mipmapMode = samplerConfig.mipmapMode;
	samplerInfo.mipLodBias = samplerConfig.mipLodBias;
	samplerInfo.minLod = samplerConfig.minLod;
	samplerInfo.maxLod = samplerConfig.maxLod;

	if (vkCreateSampler(device.logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler");
	}
}

void Texture::CreateMipmaps(ImageConfiguration &imageConfig)
{
	VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(device.physicalDevice, imageConfig.format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) 
	{
    	throw std::runtime_error("texture image format does not support linear blitting");
	}

	VkCommandBuffer commandBuffer = device.BeginGraphicsCommand();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = imageConfig.aspect;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = imageConfig.width;
	int32_t mipHeight = imageConfig.height;

	for (uint32_t i = 1; i < imageConfig.mipLevels; i++) 
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = imageConfig.aspect;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = imageConfig.aspect;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
    	if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = imageConfig.mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    device.EndGraphicsCommand(commandBuffer);

	imageConfig.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void Texture::TransitionImageLayout(ImageConfiguration &imageConfig, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = device.BeginGraphicsCommand();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = imageConfig.layout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = imageConfig.aspect;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = imageConfig.mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = imageConfig.arrayLayers;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
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

	device.EndGraphicsCommand(commandBuffer);

	imageConfig.layout = newLayout;
}

/*
void Texture::CopyFrom(VkBuffer buffer, ImageConfiguration &configuration)
{
	VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = configuration.aspect;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = configuration.arrayLayers;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {configuration.width, configuration.height, 1};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	device.EndSingleTimeCommands(commandBuffer);
}
*/

void Texture::DestroyImage()
{
	//if (!device.logicalDevice)
	//{
	//	std::cout << ": error: cannot destroy image because device does not exist" << std::endl;
	//	return;
	//}
    if (image != nullptr && device.logicalDevice != nullptr)
	{
		vkDestroyImage(device.logicalDevice, image, nullptr);
		image = nullptr;
	}
	if (imageMemory != nullptr && device.logicalDevice != nullptr)
	{
		vkFreeMemory(device.logicalDevice, imageMemory, nullptr);
		imageMemory = nullptr;
	}
}

void Texture::DestroyImageView()
{
    if (!imageView) return;
	if (!device.logicalDevice)
	{
		std::cout << ": error: cannot destroy image view because device does not exist" << std::endl;
		return;
	}

	vkDestroyImageView(device.logicalDevice, imageView, nullptr);
	imageView = nullptr;
}

void Texture::DestroySampler()
{
    if (!sampler) return;
	if (!device.logicalDevice)
	{
		std::cout << ": error: cannot destroy image sampler because device does not exist" << std::endl;
		return;
	}

	vkDestroySampler(device.logicalDevice, sampler, nullptr);
	sampler = nullptr;
}

Texture *Texture::statue = Manager::NewTexture();
