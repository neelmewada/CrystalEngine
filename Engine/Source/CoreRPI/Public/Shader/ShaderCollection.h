#pragma once

namespace CE::RPI
{
	class Shader;

	class CORERPI_API ShaderCollection final
	{
	public:

		struct Item
		{
		public:
			Name shaderTag{};
			RPI::Shader* shader = nullptr;
			bool enabled = true;
			RHI::DrawListTag drawListOverride{};
		};

		ShaderCollection();

		~ShaderCollection();

		using iterator = Array<Item>::Iterator;
		using const_iterator = Array<Item>::ConstIterator;

		auto begin() { return items.begin(); }
		auto end() { return items.end(); }

		auto cbegin() const { return items.begin(); }
		auto cend() const { return items.end(); }

		SIZE_T GetSize() const { return items.GetSize(); }

		bool IsEmpty() const { return items.IsEmpty(); }

		void Clear() { items.Clear(); }

		bool HasItem(RHI::DrawListTag drawListTag);

		RPI::Shader* GetShader(RHI::DrawListTag drawListTag);

		const Item& At(SIZE_T index) const
		{
			return items[index];
		}

		Item& At(SIZE_T index)
		{
			return items[index];
		}

		void Add(const Item& item)
		{
			items.Add(item);
		}

		void RemoveAt(SIZE_T index)
		{
			items.RemoveAt(index);
		}

		const Item& operator[](SIZE_T index) const
		{
			return items[index];
		}

		Item& operator[](SIZE_T index)
		{
			return items[index];
		}

	private:

		Array<Item> items{};
	};

} // namespace CE::RPI
