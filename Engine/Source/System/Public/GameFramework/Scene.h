#pragma once

#include "Object/Object.h"
#include "Object/ObjectStore.h"

namespace CE
{

    class SYSTEM_API Scene : public Object
    {
        CE_CLASS(Scene)

    public:
        Scene();
        Scene(Name name);
        virtual ~Scene();

    protected:
        ObjectStore<CE::Object*> objects{};
    };
    
} // namespace CE


CE_RTTI_CLASS(SYSTEM_API, CE, Scene,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(objects)
    ),
    CE_FUNCTION_LIST()
)
