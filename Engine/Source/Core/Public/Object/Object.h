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

