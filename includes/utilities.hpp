#pragma once

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
};