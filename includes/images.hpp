#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "device.hpp"
#include "pipeline.hpp"

#include <string>

class Images
{
    private:
        

    public:
        static stbi_uc *LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels);
		static void FreePixels(stbi_uc *pixels);

        static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory, Device &device);
        static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, Device &device);

        static void CreateTextureImage(Pipeline &pipeline, Device &device);
        static void CreateTextureImageView(Pipeline &pipeline, Device &device);
        static void CreateTextureSampler(Pipeline &pipeline, Device &device);

        static void DestroyTextureImage(Pipeline &pipeline, Device &device);
		static void DestroyTextureImageView(Pipeline &pipeline, Device &device);
		static void DestroyTextureSampler(Pipeline &pipeline, Device &device);
};
