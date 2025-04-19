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

		void SetSeed(uint32_t seed);
		int Next(int min, int max);
};