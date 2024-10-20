#include "texture.hpp"

#include "manager.hpp"
#include "utilities.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

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

void Texture::Create(std::string name)
{
    //this->device = device;
    CreateImage(name);
    //CreateImageView();
    CreateSampler();
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

void Texture::CreateImage(std::string name)
{
    if (image != nullptr || imageMemory != nullptr)
		throw std::runtime_error("cannot create texture image because it already exists");

	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	stbi_uc *pixels = Texture::LoadTexture(Utilities::GetPath() + "/textures/" + name, &texWidth, &texHeight, &texChannels);
	imageSize = texWidth * texHeight * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(device.logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device.logicalDevice, stagingBufferMemory);

	stbi_image_free(pixels);

	ImageConfiguration configuration;
	configuration.width = texWidth;
	configuration.height = texHeight;
	configuration.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	configuration.layout = VK_IMAGE_LAYOUT_UNDEFINED;

	CreateImage(configuration, false);

	//Images::CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
	//	VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory, *device);

	device.TransitionImageLayout(image, configuration.format, configuration.layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	configuration.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	device.CopyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	device.TransitionImageLayout(image, configuration.format, configuration.layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	configuration.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(device.logicalDevice, stagingBufferMemory, nullptr);

	CreateImageView(configuration);
}

void Texture::CreateImage(ImageConfiguration &configuration, bool view)
{
    if (image != nullptr || imageMemory != nullptr)
		throw std::runtime_error("cannot create texture image because it already exists");

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = configuration.type;
	imageInfo.extent.width = configuration.width;
	imageInfo.extent.height = configuration.height;
	imageInfo.extent.depth = configuration.depth;
	imageInfo.mipLevels = configuration.mipLevels;
	imageInfo.arrayLayers = configuration.arrayLayers;
	imageInfo.format = configuration.format;
	imageInfo.tiling = configuration.tiling;
	imageInfo.initialLayout = configuration.layout;
	imageInfo.usage = configuration.usage;
	imageInfo.samples = configuration.sampleCount;
	imageInfo.sharingMode = configuration.sharingMode;

	if (vkCreateImage(device.logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.logicalDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, configuration.memoryProperties);

	if (vkAllocateMemory(device.logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory");
	}

	vkBindImageMemory(device.logicalDevice, image, imageMemory, 0);

	if (view) CreateImageView(configuration);
}

void Texture::CreateImageView(ImageConfiguration &configuration)
{
    if (imageView != nullptr) throw std::runtime_error("cannot create texture image view because it already exists");

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = configuration.viewType;
	viewInfo.format = configuration.format;
	viewInfo.subresourceRange.aspectMask = configuration.aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device.logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image view");
	}

	//imageView = Images::CreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, *device);
}

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

void Texture::CreateSampler(SamplerConfiguration &configuration)
{
    if (sampler != nullptr) throw std::runtime_error("cannot create texture sampler because it already exists");

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = configuration.magFilter;
	samplerInfo.minFilter = configuration.minFilter;
	samplerInfo.addressModeU = configuration.repeatMode;
	samplerInfo.addressModeV = configuration.repeatMode;
	samplerInfo.addressModeW = configuration.repeatMode;
	samplerInfo.anisotropyEnable = configuration.anisotrophic;
	samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = configuration.borderColor;
	samplerInfo.unnormalizedCoordinates = configuration.unnormalizedCoordinates;
	samplerInfo.compareEnable = configuration.compare;
	samplerInfo.compareOp = configuration.compareOp;
	samplerInfo.mipmapMode = configuration.mipmapMode;
	samplerInfo.mipLodBias = configuration.mipLodBias;
	samplerInfo.minLod = configuration.minLod;
	samplerInfo.maxLod = configuration.maxLod;

	if (vkCreateSampler(device.logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler");
	}
}

void Texture::DestroyImage()
{
	if (!&device)
	{
		std::cout << ": error: cannot destroy image because device does not exist" << std::endl;
		return;
	}
    if (image != nullptr)
	{
		vkDestroyImage(device.logicalDevice, image, nullptr);
		image = nullptr;
	}
	if (imageMemory != nullptr)
	{
		vkFreeMemory(device.logicalDevice, imageMemory, nullptr);
		imageMemory = nullptr;
	}
}

void Texture::DestroyImageView()
{
    if (!imageView) return;
	if (!&device)
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
	if (!&device)
	{
		std::cout << ": error: cannot destroy image sampler because device does not exist" << std::endl;
		return;
	}

	vkDestroySampler(device.logicalDevice, sampler, nullptr);
	sampler = nullptr;
}
