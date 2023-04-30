#pragma once

#include "Object/Object.h"

namespace CE
{

    class SystemComponent;

    class CORE_API SystemObject : public Object
    {
        CE_CLASS(SystemObject, Object)
    public:
        SystemObject();

        ~SystemObject();

        virtual bool AddSystemComponent(SystemComponent* systemComponent);

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

        virtual void OnSystemComponentAdded(SystemComponent* component) {}

        virtual void OnSystemComponentRemoved(SystemComponent* component) {}

    protected:
        CE::Array<SystemComponent*> components{};
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, SystemObject,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(

    ),
    CE_FUNCTION_LIST()
)

