#pragma once

#include <cstdint>

class Random
{
	private:
		uint32_t seed = 1;

		double Interpolater();

	public:
		Random();
		Random(uint32_t seed);
		~Random();

		static int Int(int seed);
		static int Int(int seed, int min, int max);

		//static float Float(float seed, float min, float max);

		void SetSeed(uint32_t seed);
		int Next(int min, int max);
};