#pragma once

#include "CoreTypes.h"
#include "Containers/String.h"

#include "ObjectMacros.h"
#include "RTTI.h"
#include "Variant.h"
#include "Class.h"
#include "Enum.h"
#include "Field.h"

#include "ConfigParser.h"

namespace CE
{

    class CORE_API Component;

    class CORE_API Object
    {
        CE_CLASS(Object);

    public:
        Object(UUID uuid = UUID());
        Object(CE::Name name, UUID uuid = UUID());

        virtual ~Object();

        // - Getters & Setters -
        CE_INLINE CE::Name GetName() const
        {
            return name;
        }
        
        CE_INLINE UUID GetUuid() const
        {
            return uuid;
        }

    private:
        CE::Name name;
        CE::UUID uuid;
        
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
        CE_FIELD(uuid)
    ),
    CE_FUNCTION_LIST()
)
