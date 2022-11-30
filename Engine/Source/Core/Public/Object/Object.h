#pragma once

#include "Containers/String.h"

#include "ObjectMacros.h"
#include "RTTI.h"
#include "Class.h"
#include "Enum.h"

namespace CE
{
    
    class CORE_API Object
    {
    public:
        Object();
        Object(CE::Name name);

        ~Object();

        // - Getters & Setters -
        inline CE::Name GetName()
        {
            return Name;
        }

        // Virtual methods

        //virtual ClassType* GetType() = 0;

    private:
        CE::Name Name;
    };

} // namespace CE

