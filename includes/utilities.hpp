#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

class Utilities
{
	private:
		/* data */

	public:
		static std::string FileToString(const char *path);
		static std::vector<char> FileToBinary(const char *path);
		static bool Contains(const std::string &str, const std::string &find);
		static std::string GetPath();
		static float SignedFloor(float x);
		static int Fits(float part, float whole);
		static void PrintVec(glm::vec3 vec);
};