#pragma once

#include "device.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <string>

#define IMAGE_2D VK_IMAGE_TYPE_2D

#define R8G8B8A8 VK_FORMAT_R8G8B8A8_SRGB

struct ImageConfiguration
{
    VkImageType type = IMAGE_2D;
    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    VkFormat format = R8G8B8A8;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
};

struct SamplerConfiguration
{
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode repeatMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    VkBool32 anisotrophic = VK_TRUE;
    VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    VkBool32 unnormalizedCoordinates = VK_FALSE;
    VkBool32 compare = VK_FALSE;
    VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
    float maxLod = VK_LOD_CLAMP_NONE;
};

class Texture
{
    private:
        Device &device;

    public:
        Texture();
        Texture(Device &device);
        ~Texture();

        VkImage image = nullptr;
		VkDeviceMemory imageMemory = nullptr;
		VkImageView imageView = nullptr;
		VkSampler sampler = nullptr;

        void CreateTexture(std::string name);
        void CreateTextureImage(std::string name);
        void CreateImage(ImageConfiguration &configuration, bool view);
        void CreateImageView(ImageConfiguration &configuration);
        void CreateSampler(SamplerConfiguration &configuration);
        void CreateMipmaps(ImageConfiguration &configuration);

        void Destroy();
        void DestroyImage();
        void DestroyImageView();
		void DestroySampler();

        static stbi_uc *LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels);
        void TransitionImageLayout(ImageConfiguration &configuration, VkImageLayout newLayout);
};
