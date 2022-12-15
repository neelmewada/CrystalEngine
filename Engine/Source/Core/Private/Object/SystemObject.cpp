
#include "Object/SystemObject.h"

#include "Component/SystemComponent.h"

namespace CE
{
    
    SystemObject::SystemObject() : Object()
    {

    }

    SystemObject::SystemObject(CE::Name name) : Object(name)
    {

    }

    SystemObject::~SystemObject()
    {

    }

    bool SystemObject::AddSystemComponent(SystemComponent* systemComponent)
    {
        if (systemComponent == nullptr)
            return false;
        if (!systemComponent->IsCompatibleWith(this))
            return false;

        if (!components.Exists(systemComponent))
        {
            components.Add(systemComponent);
            systemComponent->Init();
            OnSystemComponentAdded(systemComponent);
            return true;
        }

        // Component already exists
        return true;
    }

    void SystemObject::RemoveSystemComponent(SystemComponent* systemComponent)
    {
        if (systemComponent == nullptr)
            return;

        components.Remove(systemComponent);
    }

} // namespace CE


CE_RTTI_CLASS_IMPL(CORE_API, CE, SystemObject)

