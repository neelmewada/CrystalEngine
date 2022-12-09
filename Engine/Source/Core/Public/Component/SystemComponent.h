#pragma once

#include "Component.h"

namespace CE
{

    class CORE_API SystemObject;

    class CORE_API SystemComponent : public Component
    {
    public:

        virtual void Init() override;

        virtual void Activate() override;
        virtual void Deactivate() override;

        virtual void Tick(f32 deltaTime) override;

        virtual bool IsCompatibleWith(SystemObject* systemObject) { return true; }
    };
    
} // namespace CE
