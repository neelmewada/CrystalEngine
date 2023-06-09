#include "CoreMinimal.h"

namespace CE
{
	Gradient::Gradient()
	{

	}

	Gradient::Gradient(std::initializer_list<Key> list, Direction dir) : keys(list), direction(dir)
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

	void Gradient::Clear()
	{
		keys.Clear();
	}

	void Gradient::AddKey(const Key& key)
	{
		keys.Add(key);
	}

	void Gradient::AddKey(f32 position, const Color& color)
	{
		keys.Add({ position, color });
	}

	void Gradient::RemoveKeyAt(u32 index)
	{
		keys.RemoveAt(index);
	}

} // namespace CE



