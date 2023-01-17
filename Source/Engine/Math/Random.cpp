#include "Random.h"
#include "../Util.h"

#include <random>

std::mt19937 Random::s_rng;

void Random::Init()
{
	Seed();
	Util::DebugPrint("Random engine initialised\n");
}

void Random::Seed()
{
	s_rng.seed(std::random_device{}());
}

int Random::GetInt(int min, int max)
{
	std::uniform_int_distribution<> dist(min, max);
	return dist(s_rng);
}