#pragma once

namespace CE
{

	class CORE_API Random
	{
		CE_STATIC_CLASS(Random)
	public:

		// Both inclusive: [min, max]
		static int Range(int min = INT_MIN, int max = INT_MAX);

		// Both inclusive: [min, max]
		static s64 Range(s64 min = LLONG_MIN, s64 max = LLONG_MAX);

		static float Range(float min = 0, float max = 1);
	};
    
} // namespace CE
