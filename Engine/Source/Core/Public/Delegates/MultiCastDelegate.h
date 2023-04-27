#pragma once

#include "MultiCastDelegateBase.h"

namespace CE
{
    
    template<class T>
    class MultiCastDelegate : public Internal::MultiCastDelegateBase<T>
    {
    public:
        MultiCastDelegate() : Internal::MultiCastDelegateBase<T>()
        {}


    };

} // namespace CE
