#include "CoreMinimal.h"

namespace CE
{
	Gradient::Gradient()
	{

	}

	Gradient::Gradient(std::initializer_list<Key> list) : keys(list)
	{

	}

	Color Gradient::Evaluate(f32 position) const
	{
		if (keys.GetSize() == 0)
			return Color();
		if (keys.GetSize() == 1)
			return keys[0].value;

		for (int i = 0; i < keys.GetSize(); i++)
		{
			if (i > 0 && keys[i - 1].position <= position && position < keys[i].position)
			{
				f32 left = keys[i - 1].position;
				f32 right = keys[i].position;
				return Color::Lerp(keys[i - 1].value, keys[i].value, Math::Clamp01((position - left) / (right - left)));
			}
		}

		return position < 0 ? keys[0].value : keys.GetLast().value;
	}

} // namespace CE



