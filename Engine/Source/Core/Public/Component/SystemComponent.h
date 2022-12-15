#pragma once

#include "Component.h"

namespace CE
{

    class CORE_API SystemObject;

    class CORE_API SystemComponent : public Component
    {
        CE_CLASS(SystemComponent, Component);

    public:

        virtual void Init() override;

        virtual void Activate() override;
        virtual void Deactivate() override;

        virtual void Tick(f32 deltaTime) override;

        virtual bool IsCompatibleWith(SystemObject* systemObject) { return true; }
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, SystemComponent,
    CE_SUPER(CE::Component),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
