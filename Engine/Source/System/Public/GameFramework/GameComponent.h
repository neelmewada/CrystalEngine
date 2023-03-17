#pragma once

#include "Component/Component.h"

namespace CE
{
    class GameObject;

    class SYSTEM_API GameComponent : public Component
    {
        CE_CLASS(GameComponent, Component)
    public:
        
        GameComponent(CE::Name name);
        virtual ~GameComponent();
        
        virtual void Init() override {}

        virtual void Tick(f32 deltaTime) override {}

        CE_INLINE GameObject* GetOwner() const { return owner; }

        CE_SIGNAL(OnComponentValuesUpdated);
        
    protected:
        CE::GameObject* owner = nullptr;

        friend class CE::GameObject;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, GameComponent, 
    CE_SUPER(CE::Component),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(AllowMultiple = false),
    CE_FIELD_LIST(
        CE_FIELD(owner, Hidden)
    ),
    CE_FUNCTION_LIST(
        // Signals
        CE_FUNCTION(OnComponentValuesUpdated, Signal)
    )
)


