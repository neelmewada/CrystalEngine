#include "CoreMinimal.h"

namespace CE
{
	Gradient::Gradient()
	{

	}

	Gradient::Gradient(std::initializer_list<Key> list, f32 degrees) : keys(list), degrees(degrees)
	{

	}

	Gradient::Gradient(const Array<Key>& list, f32 degrees) : keys(list), degrees(degrees)
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
		keys.Add({ color, position });
	}

	void Gradient::RemoveKeyAt(u32 index)
	{
		keys.RemoveAt(index);
	}

	SIZE_T Gradient::GetHash() const
	{
		if (keys.IsEmpty())
			return 0;

		SIZE_T hash = CE::GetHash(degrees);

		for (int i = 0; i < keys.GetSize(); ++i)
		{
			CombineHash(hash, keys[i]);
		}

		return hash;
	}

} // namespace CE



