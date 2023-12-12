#pragma once

#include <array>

namespace CE
{
    
	template<typename T, SIZE_T Size>
	class FixedArray
	{
	public:
		static constexpr SIZE_T size = Size;

		FixedArray()
		{

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

		constexpr inline T* GetData() const { return impl.data(); }

		constexpr inline SIZE_T GetSize() const { return impl.size(); }

	private:

		std::array<T, Size> impl{};
	};

} // namespace CE
