#pragma once

#include "ObjectMacros.h"
#include "Types.h"
#include "Containers/String.h"

namespace CE
{
    
    class CORE_API Object
    {
    public:
        Object();
        Object(String name);

        // - Getters & Setters -
        inline String GetName()
        {
            return Name;
        }

        // Virtual methods

        //virtual ClassType* GetType() = 0;

    private:
        String Name;
    };

} // namespace CE

