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
		static double seed;
		static std::hash<float> floatHash;
		static std::hash<size_t> sizetHash;

		static std::string FileToString(const char *path);
		static std::vector<char> FileToBinary(const char *path);
		static bool Contains(const std::string &str, const std::string &find);
		static std::string GetPath();
		static float SignedFloor(float x);
		static int Fits(float part, float whole);
		static void PrintVec(glm::vec3 vec);
		static glm::vec3 RotateVec(glm::vec3 vec, float angle, glm::vec3 axis);
		static glm::vec2 Normalize(const glm::vec2 &vec);
		static glm::vec3 Normalize(const glm::vec3 &vec);
		static glm::mat4 GetRotationMatrix(float angle, glm::vec3 axis);
		static float Random01();
		static float Random01(float seed);
		static float Random11();
		static float Random11(float seed);
		static float RandomFloat(float min, float max, float seed);
		static float RandomFloat(float min, float max);
		static int RandomInt(int min, int max, float seed);
		static int RandomInt(int min, int max);
};