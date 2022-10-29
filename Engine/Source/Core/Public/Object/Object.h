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

    private:
        String Name;
    };

} // namespace CE

