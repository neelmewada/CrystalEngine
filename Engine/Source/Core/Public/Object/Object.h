#pragma once

#include "Containers/String.h"

#include "ObjectMacros.h"
#include "RTTI.h"
#include "Variant.h"
#include "Class.h"
#include "Enum.h"
#include "Field.h"

namespace CE
{

    class CORE_API Component;

    class CORE_API Object
    {
        CE_CLASS(Object);

    public:
        Object();
        Object(CE::Name name);

        virtual ~Object();

        // - Getters & Setters -
        CE_INLINE CE::Name GetName()
        {
            return name;
        }

        // Virtual methods

        //virtual ClassType* GetType() = 0;

    private:
        CE::Name name;
        
    protected:
        //CE::Array<Component*> components{};
    };

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(name)
    ),
    CE_FUNCTION_LIST()
)
