#pragma once

#include <algorithm>
#include <bitset>

#include "RemoveReference.h"

#include "SharedPtr.h"
#include "UniquePtr.h"

namespace CE
{
	class String;

	template<typename T>
	struct NumericLimits
	{
		constexpr static T Min() noexcept
		{
			return std::numeric_limits<T>::min();
		}

		constexpr static T Max() noexcept
		{
			return std::numeric_limits<T>::max();
		}

		constexpr static T Infinity() noexcept
		{
			return std::numeric_limits<T>::infinity();
		}
	};

	template<SIZE_T Bits = 32>
	class BitSet
	{
	public:
		inline void Set(SIZE_T pos, bool value = true)
		{
			impl.set(pos, value);
		}

		/// @brief Flips all bits
		inline void Flip()
		{
			impl.flip();
		}

		inline bool Test(SIZE_T pos) const
		{
			return impl.test(pos);
		}

		inline SIZE_T Count() const
		{
			return impl.count();
		}

		inline bool Any() const
		{
			return impl.any();
		}

		inline bool All() const
		{
			return impl.all();
		}

		inline bool None() const
		{
			return impl.none();
		}

		inline u32 ToU32() const
		{
			return impl.to_ulong();
		}

		inline u64 ToU64() const
		{
			return impl.to_ullong();
		}

		String ToString() const;

	private:

		std::bitset<Bits> impl{};
	};

    template<bool TValue>
    struct TBoolConst
    {
        static constexpr bool Value = TValue;
    };
	
    struct TFalseType : TBoolConst<false> {};

    struct TTrueType : TBoolConst<true> {};

    template<typename T>
    FORCE_INLINE typename RemoveReference<T>::Type&& MoveTemp(T&& value)
    {
        return static_cast<typename RemoveReference<T>::Type&&>(value);
    }

    template<typename T>
    void Swap(T& a, T& b)
    {
        T aTemp = MoveTemp(a);
        a = MoveTemp(b);
        b = MoveTemp(aTemp);
    }

} // namespace CE
