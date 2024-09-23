#pragma once

#include <stb_image.h>

#include <string>

class Textures
{
	private:
		

	public:
		static stbi_uc *LoadTexture(const std::string path, int *texWidth, int *texHeight, int *texChannels);
		static void FreePixels(stbi_uc *pixels);

};