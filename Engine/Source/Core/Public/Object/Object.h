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
        CE_CLASS(Object);

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

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(Name)
    )
)