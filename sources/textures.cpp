#include "textures.hpp"

#include <stdexcept>
#include <iostream>

#include "utilities.hpp"
#include <string>

stbi_uc *Textures::LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels)
{
	std::string currentPath = Utilities::GetPath();

	stbi_uc *pixels = stbi_load((currentPath + "/textures/texture.jpg").c_str(), texWidth, texHeight, texChannels, STBI_rgb_alpha);

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