#pragma once

namespace CE
{

    template<typename T>
    struct RemoveReference
    {
        typedef T Type;
    };

    template<typename T>
    struct RemoveReference<T&>
    {
        typedef T Type;
    };

    template<typename T>
    struct RemoveReference<T&&>
    {
        typedef T Type;
    };
    
} // namespace CE
