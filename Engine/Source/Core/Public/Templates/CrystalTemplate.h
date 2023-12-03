#pragma once

#include <algorithm>

#include "RemoveReference.h"

#include "SharedPtr.h"
#include "UniquePtr.h"

namespace CE
{
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
