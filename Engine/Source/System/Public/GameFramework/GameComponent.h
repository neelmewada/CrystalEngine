#pragma once

#include "Component/Component.h"

namespace CE
{
    class GameObject;

    CLASS(Abstract, AllowMultiple = false)
    class SYSTEM_API GameComponent : public Component
    {
        CE_CLASS(GameComponent, CE::Component)
    public:
        
        GameComponent(CE::Name name);
        virtual ~GameComponent();

        void Activate() override;
        void Deactivate() override;
        
        void Init() override {}

        void Tick(f32 deltaTime) override {}

        CE_INLINE GameObject* GetOwner() const { return owner; }

        CE_SIGNAL(OnComponentValuesUpdated);

    protected:
        FIELD(Hidden)
        CE::GameObject* owner = nullptr;

        friend class CE::GameObject;
    };
    
} // namespace CE

#include "GameComponent.rtti.h"

