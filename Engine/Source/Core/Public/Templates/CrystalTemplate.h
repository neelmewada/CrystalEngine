#pragma once

#include <algorithm>

#include "RemoveReference.h"

namespace CE
{

    template<bool TValue>
    struct TBoolConst
    {
        static constexpr bool Value = TValue;
    };
	
    struct TFalseType : TBoolConst<false> {};

    struct TTrueType : TBoolConst<true> {};

    template<typename T>
    FORCE_INLINE RemoveReference<T>::Type&& MoveTemp(T&& value)
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
