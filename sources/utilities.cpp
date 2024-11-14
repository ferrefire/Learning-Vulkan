#include "utilities.hpp"

#include "manager.hpp"
#include "time.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

std::string Utilities::FileToString(const char *path)
{
	std::string fileString;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		file.open(path);

		std::stringstream fileStream;
		fileStream << file.rdbuf();

		file.close();
		fileString = fileStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::FILE_TO_STRING::FILE_NOT_SUCCESFULLY_READ" << " -error: " << e.what() << std::endl;
		Manager::Quit(EXIT_FAILURE);
	}

	return (fileString);
}

std::vector<char> Utilities::FileToBinary(const char *path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file: " + std::string(path));
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

bool Utilities::Contains(const std::string &str, const std::string &find)
{
	return (str.find(find) != std::string::npos);
}

std::string Utilities::GetPath()
{
	std::filesystem::path currentPath = std::filesystem::current_path();

	while (Contains(currentPath.string(), "build"))
	{
		currentPath = currentPath.parent_path();
	}

	std::string path = currentPath.string();

	return path;
}

float Utilities::SignedFloor(float x)
{
    if (x > 0) return floor(x);
    else return ceil(x);
}

int Utilities::Fits(float part, float whole)
{
    float times = whole / part;
    int result = SignedFloor(times);
	
    return result;
}

void Utilities::PrintVec(glm::vec3 vec)
{
	std::cout << vec.x << " | " << vec.y << " | " << vec.z << std::endl;
}

glm::vec3 Utilities::RotateVec(glm::vec3 vec, float angle, glm::vec3 axis)
{
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation = glm::rotate(rotation, glm::radians(angle), axis);
	vec = rotation * glm::vec4(vec, 1.0f);
	return (vec);
}

glm::vec2 Utilities::Normalize(const glm::vec2 &vec)
{
	float unit = abs(vec.x) + abs(vec.y);

	if (unit == 0) return (glm::vec2(0));

	glm::vec2 result = glm::vec2(vec.x / unit, vec.y / unit);

	return (result);
}

glm::vec3 Utilities::Normalize(const glm::vec3 &vec)
{
	float unit = abs(vec.x) + abs(vec.y) + abs(vec.z);

	glm::vec3 result = glm::vec3(vec.x / unit, vec.y / unit, vec.z / unit);
	
	return (result);
}

glm::mat4 Utilities::GetRotationMatrix(float angle, glm::vec3 axis)
{
	glm::mat4 rotation = glm::mat4(1.0f);

	rotation = glm::rotate(rotation, glm::radians(angle), axis);

	return (rotation);
}

float Utilities::Random11()
{
	return ((Random01(seed) - 0.5f) * 2.0f);
}

float Utilities::Random11(float newSeed)
{
	return ((Random01(newSeed) - 0.5f) * 2.0f);
}

float Utilities::Random01()
{
	seed = Random01(seed + Time::GetCurrentTime());
	return (Random01(seed));
}

float Utilities::Random01(float newSeed)
{
	size_t seed1 = floatHash(newSeed);
	size_t seed2 = sizetHash(seed1);
	glm::dvec2 vecSeed = glm::dvec2(seed1, seed2);

	return glm::fract(sin(glm::dot(vecSeed, glm::dvec2(12.9898, 78.233))) * 43758.5453123);
}

float Utilities::RandomFloat(float min, float max, float newSeed)
{
	float inter = Random01(newSeed);
	return (glm::mix(min, max, inter));
}

float Utilities::RandomFloat(float min, float max)
{
	float inter = Random01();
	return (glm::mix(min, max, inter));
}

int Utilities::RandomInt(int min, int max, float newSeed)
{
	float inter = Random01(newSeed);
	return (glm::mix(min, max, inter));
}

int Utilities::RandomInt(int min, int max)
{
	float inter = Random01();
	return (glm::mix(min, max, inter));
}

double Utilities::seed = 1;
std::hash<float> Utilities::floatHash;
std::hash<size_t> Utilities::sizetHash;