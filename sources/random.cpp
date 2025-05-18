#include "random.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cstdlib>

Random::Random()
{

}

Random::Random(uint32_t seed) : seed{seed}
{

}

Random::~Random()
{

}

int Random::Int(int seed)
{
	srand(seed);
	return (rand());
}

int Random::Int(int seed, int min, int max)
{
	Random temp(seed);

	for (int i = 0; i < 10; i++)
	{
		temp.Next(min, max);
	}

	return (temp.Next(min, max));
}

double Random::Interpolater()
{
	srand(this->seed);
	this->seed = rand();
	
	return (double(this->seed) / double(RAND_MAX));
}

void Random::SetSeed(uint32_t seed)
{
	this->seed = seed;
}

int Random::Next(int min, int max)
{
	return (glm::mix(min, max, this->Interpolater()));
}