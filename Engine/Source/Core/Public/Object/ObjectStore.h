#pragma once

#include "Object/Object.h"

namespace CE
{

    template<typename T> requires std::is_pointer<T>::value
    class ObjectStore
    {
    public:
        

    private:
        CE::Array<T*> objects;
    };
    
} // namespace CE
