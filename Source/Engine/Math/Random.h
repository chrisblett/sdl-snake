#pragma once

#include <random>

class Random
{
public:
	// Initialise the random engine
	static void Init();

	// Produces and returns a random int in the range [min, max]
	static int GetInt(int min, int max);

private:
	static void Seed();

	static std::mt19937 s_rng;
};
