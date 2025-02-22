#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#define XZ2XY(vec) glm::vec2(vec.x, vec.z)
#define XY3XZ(vec) glm::vec3(vec.x, 0, vec.y)
#define Y3Y(vec) glm::vec3(0, vec.y, 0)

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
		static float SignedCeil(float x);
		static int Fits(float part, float whole);
		static void PrintVec(glm::vec4 vec);
		static void PrintVec(glm::vec3 vec);
		static void PrintVec(std::string name, glm::vec3 vec);
		static void PrintVec(glm::vec2 vec);
		static void PrintVec(std::string name, glm::vec2 vec);
		static void PrintLine(std::string name, glm::vec2 point, glm::vec2 direction);
		static void PrintPoint(std::string name, glm::vec2 point);
		static void PrintPoint(glm::vec2 point);
		static void PrintPoints(std::string name, const std::vector<glm::vec2> &points);
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
		//static glm::vec2 XZ(glm::vec3 &vec);
};