#pragma once

#include <array>

namespace CE
{
    
	template<typename T, SIZE_T Size>
	class StaticArray
	{
	public:
		static constexpr SIZE_T size = Size;

		StaticArray()
		{}

		inline T& operator[](SIZE_T index)
		{
			return impl[index];
		}

		inline const T& operator[](SIZE_T index) const
		{
			return impl[index];
		}

		inline auto begin()
		{
			return impl.begin();
		}

		inline auto end()
		{
			return impl.end();
		}

		inline const auto begin() const
		{
			return impl.begin();
		}

		inline const auto end() const
		{
			return impl.end();
		}

		constexpr inline bool IsEmpty() const { return false; }

		constexpr inline const T* GetData() const { return impl.data(); }

		constexpr inline T* GetData() { return impl.data(); }

		constexpr inline SIZE_T GetSize() const { return impl.size(); }

		constexpr inline SIZE_T GetCapacity() const { return impl.size(); }

	private:

		std::array<T, Size> impl{};
	};

	template<typename T, SIZE_T Size>
	class FixedArray
	{
	public:

		FixedArray()
		{}

		FixedArray(std::initializer_list<T> list)
		{
			for (const auto& item : list)
			{
				Add(item);
			}
		}

		inline T& operator[](SIZE_T index)
		{
			return impl[index];
		}

		inline const T& operator[](SIZE_T index) const
		{
			return impl[index];
		}

		inline auto begin()
		{
			return impl.begin();
		}

		inline auto end()
		{
			return impl.begin() + count;
		}

		inline const auto begin() const
		{
			return impl.begin();
		}

		inline const auto end() const
		{
			return impl.begin() + count;
		}

		inline bool IsEmpty() const { return count == 0; }

		inline void Clear()
		{
			count = 0;
		}

		inline void Add(const T& item)
		{
			if (count >= GetCapacity())
				return;

			impl[count++] = item;
		}

		inline void Insert(const T& item, u32 index)
		{
			if (count >= GetCapacity())
				return;

			count++;

			for (int i = count - 1; i >= index + 1; i--)
			{
				impl[i] = impl[i - 1];
			}

			impl[index] = item;
		}

		void RemoveAt(u32 index)
		{
			if (index >= count)
				return;

			for (int i = index; i < count - 1; i++)
			{
				impl[i] = impl[i + 1];
			}

			count--;
		}

		void Remove(const T& item)
		{
			for (int i = 0; i < count; i++)
			{
				if (impl[i] == item)
				{
					RemoveAt(i);
					return;
				}
			}
		}

		constexpr inline const T& GetFirst() const { return *impl.begin(); }
		constexpr inline T& GetFirst() { return *impl.begin(); }

		constexpr inline const T& GetLast() const { return *(impl.begin() + count - 1); }
		constexpr inline T& GetLast() { return *(impl.begin() + count - 1); }

		constexpr inline T* GetData() { return impl.data(); }

		inline u32 GetSize() const { return count; }

		constexpr inline u32 GetCapacity() const { return impl.size(); }

	private:

		std::array<T, Size> impl{};
		u32 count = 0;
	};

} // namespace CE
