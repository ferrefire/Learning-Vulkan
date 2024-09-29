#include "texture.hpp"

#include "utilities.hpp"
#include "images.hpp"

#include <stdexcept>
#include <cstring>

Texture::Texture()
{

}

Texture::~Texture()
{
    Destroy();
}

void Texture::Create(std::string name, Device *device)
{
    this->device = device;
    CreateImage(name);
    CreateImageView();
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
	device->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(device->logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device->logicalDevice, stagingBufferMemory);

	stbi_image_free(pixels);

	Images::CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
		VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory, *device);

	device->TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	device->CopyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	device->TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(device->logicalDevice, stagingBufferMemory, nullptr);
}

void Texture::CreateImageView()
{
    if (imageView != nullptr) throw std::runtime_error("cannot create texture image view because it already exists");

	imageView = Images::CreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, *device);
}

void Texture::CreateSampler()
{
    if (sampler != nullptr) throw std::runtime_error("cannot create texture sampler because it already exists");

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	//VkPhysicalDeviceProperties properties{};
	//vkGetPhysicalDeviceProperties(device->physicalDevice, &properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = device->properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device->logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler");
	}
}

void Texture::DestroyImage()
{
    if (image != nullptr)
	{
		vkDestroyImage(device->logicalDevice, image, nullptr);
		image = nullptr;
	}
	if (imageMemory != nullptr)
	{
		vkFreeMemory(device->logicalDevice, imageMemory, nullptr);
		imageMemory = nullptr;
	}
}

void Texture::DestroyImageView()
{
    if (!imageView) return;

	vkDestroyImageView(device->logicalDevice, imageView, nullptr);
	imageView = nullptr;
}

void Texture::DestroySampler()
{
    if (!sampler) return;

	vkDestroySampler(device->logicalDevice, sampler, nullptr);
	sampler = nullptr;
}
