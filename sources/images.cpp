#include "images.hpp"

#include "utilities.hpp"

#include <stdexcept>

/*
stbi_uc *Images::LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels)
{
	std::string currentPath = Utilities::GetPath();

	stbi_uc *pixels = stbi_load((currentPath + "/textures/texture.jpg").c_str(), texWidth, texHeight, texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image");
	}

	return (pixels);
}

void Images::FreePixels(stbi_uc *pixels)
{
	stbi_image_free(pixels);
}
*/

void Images::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
	VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory, Device &device)
{
    VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device.logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.logicalDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device.logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory");
	}

	vkBindImageMemory(device.logicalDevice, image, imageMemory, 0);
}

VkImageView Images::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, Device &device)
{
    VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device.logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image view");
	}

	return imageView;
}

/*
void Images::CreateTextureImage(Pipeline &pipeline, Device &device)
{
    if (pipeline.textureImage != nullptr || pipeline.textureImageMemory != nullptr)
		throw std::runtime_error("cannot create texture image because it already exists");

	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	stbi_uc *pixels = LoadTexture(Utilities::GetPath() + "/textures/texture.jpg", &texWidth, &texHeight, &texChannels);
	imageSize = texWidth * texHeight * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(device.logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device.logicalDevice, stagingBufferMemory);

	FreePixels(pixels);

	CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
		VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pipeline.textureImage, pipeline.textureImageMemory, device);

	device.TransitionImageLayout(pipeline.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	device.CopyBufferToImage(stagingBuffer, pipeline.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	device.TransitionImageLayout(pipeline.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(device.logicalDevice, stagingBufferMemory, nullptr);
}

void Images::CreateTextureImageView(Pipeline &pipeline, Device &device)
{
    if (pipeline.textureImageView != nullptr) throw std::runtime_error("cannot create texture image view because it already exists");

	pipeline.textureImageView = Images::CreateImageView(pipeline.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, device);
}

void Images::CreateTextureSampler(Pipeline &pipeline, Device &device)
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

void Images::DestroyTextureImage(Pipeline &pipeline, Device &device)
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

void Images::DestroyTextureImageView(Pipeline &pipeline, Device &device)
{
    if (!pipeline.textureImageView) return;

	vkDestroyImageView(device.logicalDevice, pipeline.textureImageView, nullptr);
	pipeline.textureImageView = nullptr;
}

void Images::DestroyTextureSampler(Pipeline &pipeline, Device &device)
{
    if (!pipeline.textureSampler) return;

	vkDestroySampler(device.logicalDevice, pipeline.textureSampler, nullptr);
	pipeline.textureSampler = nullptr;
}
*/
