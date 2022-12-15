#pragma once

#include "Component/Component.h"

namespace CE
{

    class SYSTEM_API GameComponent : public Component
    {
        CE_CLASS(GameComponent, Component);

    public:

        GameComponent(CE::Name name);
        virtual ~GameComponent();

        virtual void Tick(f32 deltaTime) override {}
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, GameComponent, 
    CE_SUPER(CE::Component),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
