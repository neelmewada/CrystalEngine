#pragma once

#include "Object/Object.h"
#include "Object/SystemObject.h"

namespace CE
{

    class SYSTEM_API Engine : public SystemObject
    {
        CE_CLASS(Engine, SystemObject);

    private:
        Engine();
        virtual ~Engine();

    public:

        virtual void Startup();
        virtual void Shutdown();

        virtual void Tick(f32 deltaTime);

        virtual void OnSystemComponentAdded(SystemComponent* component) override;

        friend class SystemModule;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, Engine,
    CE_SUPER(CE::SystemObject),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

