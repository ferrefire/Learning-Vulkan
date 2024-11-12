#include "utilities.hpp"

#include "manager.hpp"

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