#pragma once

#include "Object/Object.h"

#include "Component/SystemComponent.h"

namespace CE
{

    class CORE_API SystemObject : public Object
    {
        CE_CLASS(SystemObject, Object);

    public:
        SystemObject();
        SystemObject(CE::Name name);

        ~SystemObject();

        bool AddSystemComponent(SystemComponent* systemComponent);

        template<typename T> requires std::is_base_of<SystemComponent, T>::value
        T* AddSystemComponent()
        {
            T* instance = new T;
            if (AddSystemComponent(instance))
            {
                return instance;
            }
            return nullptr;
        }

        void RemoveSystemComponent(SystemComponent* systemComponent);
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, SystemObject,
    CE_SUPER(CE::Object),
    CE_ATTRIBS(),
    CE_FIELD_LIST(

    ),
    CE_FUNCTION_LIST()
)

