#pragma once

#include "Object/Object.h"

namespace CE
{

    class CORE_API ObjectManager
    {
    public:
        ObjectManager() = delete;
        ~ObjectManager() = delete;

        static void RegisterObject(Object* object);
        static void DeregisterObject(Object* object);

        static bool IsObjectRegistered(Name name);
        static bool IsObjectRegistered(Object* object);

    };
    
} // namespace CE
