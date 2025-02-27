#pragma once

#include "device.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <string>

#define IMAGE_2D VK_IMAGE_TYPE_2D
#define IMAGE_3D VK_IMAGE_TYPE_3D

#define IMAGE_VIEW_2D VK_IMAGE_VIEW_TYPE_2D
#define IMAGE_VIEW_3D VK_IMAGE_VIEW_TYPE_3D
#define IMAGE_VIEW_2D_ARRAY VK_IMAGE_VIEW_TYPE_2D_ARRAY

#define R8G8B8A8 VK_FORMAT_R8G8B8A8_SRGB
#define R8G8B8 VK_FORMAT_R8G8B8_SRGB
#define R16 VK_FORMAT_R16_UNORM
#define RGB8 VK_FORMAT_R8G8B8A8_UNORM
#define RGB16 VK_FORMAT_R16G16B16A16_UNORM

#define LAYOUT_READ_ONLY VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
#define LAYOUT_TRNSFR_DST VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
#define LAYOUT_GENERAL VK_IMAGE_LAYOUT_GENERAL
#define LAYOUT_UNDEFINED VK_IMAGE_LAYOUT_UNDEFINED

#define CLAMP_TO_EDGE VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
#define REPEAT VK_SAMPLER_ADDRESS_MODE_REPEAT
#define MIRRORED_REPEAT VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT

struct ImageConfiguration
{
    VkImageType type = IMAGE_2D;
    VkImageViewType viewType = IMAGE_VIEW_2D;
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    VkFormat format = R8G8B8A8;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    //VkImageLayout layout = LAYOUT_READ_ONLY;
    VkImageLayout initialLayout = LAYOUT_UNDEFINED;
    VkImageLayout transitionLayout = LAYOUT_READ_ONLY;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	bool createMipmaps = false;
};

struct SamplerConfiguration
{
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode repeatMode = CLAMP_TO_EDGE;
    //VkSamplerAddressMode repeatMode = MIRRORED_REPEAT;
    //VkBool32 anisotrophic = VK_FALSE;
    VkBool32 anisotrophic = VK_FALSE;
    float anisotrophicSampleCount = 0.0;
    VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    VkBool32 unnormalizedCoordinates = VK_FALSE;
    VkBool32 compare = VK_FALSE;
    VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
	float maxLod = 0.0f;
	//float maxLod = VK_LOD_CLAMP_NONE;
};

class Texture
{
    private:
		static Texture *statue;

		Device &device;

    public:
		static void CreateDefaults();
		static Texture *Statue();

		static ImageConfiguration ImageStorage(uint32_t width, uint32_t height);
		static ImageConfiguration ImageStorage(uint32_t width, uint32_t height, uint32_t depth);
		static ImageConfiguration ImageArrayStorage(uint32_t width, uint32_t height, uint32_t layers);

		Texture();
        Texture(Device &device);
        ~Texture();

        VkImage image = nullptr;
		VkDeviceMemory imageMemory = nullptr;
		VkImageView imageView = nullptr;
		VkSampler sampler = nullptr;

		void CreateTexture(std::string name);
		void CreateTexture(std::string name, SamplerConfiguration &samplerConfig);
		void CreateTextureImage(std::string name, SamplerConfiguration &samplerConfig);
		void CreateImage(ImageConfiguration &imageConfig);
		void CreateImage(ImageConfiguration &imageConfig, SamplerConfiguration &samplerConfig);
		void CreateImageView(ImageConfiguration &imageConfig);
		void CreateImageView(ImageConfiguration &imageConfig, SamplerConfiguration &samplerConfig);
		void CreateSampler(SamplerConfiguration &samplerConfig);
		void CreateMipmaps(ImageConfiguration &imageConfig);

		void Destroy();
        void DestroyImage();
        void DestroyImageView();
		void DestroySampler();

        static stbi_uc *LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels);
		//void TransitionImageLayout(ImageConfiguration &imageConfig, VkImageLayout oldLayout, VkImageLayout newLayout);
		void TransitionImageLayout(ImageConfiguration &imageConfig);
		void TransitionImageLayout(VkCommandBuffer commandBuffer, ImageConfiguration &imageConfig);
		void CopyFromImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout layout, ImageConfiguration &config);
		//void TransitionImageLayout(ImageConfiguration &imageConfig, VkImageLayout newLayout);
		//void CopyFrom(VkBuffer buffer, ImageConfiguration &configuration);
};
