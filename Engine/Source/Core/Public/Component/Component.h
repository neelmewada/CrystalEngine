#pragma once

#include "Types/CoreTypeDefs.h"

#include "Object/Object.h"

namespace CE
{
    
    class CORE_API Component : public Object
    {
        CE_CLASS(Component, CE::Object)
    public:

        Component(CE::Name name);
        virtual ~Component();
        
        virtual void Init() = 0;
        
        virtual void Activate() { isActive = true; }
        virtual void Deactivate() { isActive = false; }
        
        virtual void Tick(f32 deltaTime) = 0;

        inline bool IsActive()
        {
            return isActive;
        }

    private:
        bool isActive = false;
    };

} // namespace CE


CE_RTTI_CLASS(CORE_API, CE, Component,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(

    ),
    CE_FUNCTION_LIST(
        CE_FUNCTION(Init)
        CE_FUNCTION(Activate)
        CE_FUNCTION(Deactivate)
        CE_FUNCTION(Tick)
    )
)
