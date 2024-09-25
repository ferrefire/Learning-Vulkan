#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "device.hpp"

#include <string>

class Texture
{
    private:
        Device *device;

    public:
        Texture();
        ~Texture();

        VkImage image = nullptr;
		VkDeviceMemory imageMemory = nullptr;
		VkImageView imageView = nullptr;
		VkSampler sampler = nullptr;

        void Create(std::string name, Device *device);
        void CreateImage(std::string name);
        void CreateImageView();
        void CreateSampler();

        void Destroy();
        void DestroyImage();
        void DestroyImageView();
		void DestroySampler();

        static stbi_uc *LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels);
};
