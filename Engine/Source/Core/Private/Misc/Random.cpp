#include "CoreMinimal.h"

#include <random>

namespace CE
{
	static std::random_device device; // obtain a random number from hardware
	static std::mt19937 generator(device()); // seed the generator

	int Random::Range(int min, int max)
	{
		std::uniform_int_distribution<> dist{ min, max };
		return dist(generator);
	}

	s64 Random::Range(s64 min, s64 max)
	{
		std::uniform_int_distribution<s64> dist{ min, max };
		return dist(generator);
	}

	float Random::Range(float min, float max)
	{
		std::uniform_real_distribution<float> dist{ min, max };
		return dist(generator);
	}
	

} // namespace CE
