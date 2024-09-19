#include "textures.hpp"

#include <stdexcept>
#include <iostream>

stbi_uc *Textures::LoadTexture(const std::string path, int &texWidth, int &texHeight, int &texChannels)
{
	stbi_uc *pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image");
	}

	return (pixels);
}

void Textures::FreePixels(stbi_uc *pixels)
{
	stbi_image_free(pixels);
}