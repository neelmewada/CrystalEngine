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
        GameComponent();
        GameComponent(CE::Name name);
        virtual ~GameComponent();

        void Activate() override;
        void Deactivate() override;
        
        void Init() override {}

        void Tick(f32 deltaTime) override {}

        CE_INLINE GameObject* GetOwner() const { return owner; }

        CE_SIGNAL(OnComponentValuesUpdated);

        GameComponent* AddSubComponent(ClassType* componentClass);
        GameComponent* AddSubComponent(TypeId typeId);

        void RemoveSubComponent(ClassType* componentClass);
        void RemoveSubComponent(TypeId componentTypeId);

        GameComponent* AddSubComponent(GameComponent* subComponent);
        void RemoveSubComponent(GameComponent* subComponent);

        template<typename TComponent> requires std::is_base_of<GameComponent, TComponent>::value
        TComponent* AddSubComponent()
        {
            return (TComponent*)AddSubComponent(TComponent::Type());
        }

        template<typename TComponent> requires std::is_base_of<GameComponent, TComponent>::value
        void RemoveSubComponent()
        {
            RemoveSubComponent(TComponent::Type());
        }

    protected:
        void OnParentComponentRemoved();

        FIELD(Hidden)
        CE::GameObject* owner = nullptr;

        FIELD(Hidden)
    	Array<GameComponent*> subComponents{};

        FIELD(Hidden)
    	GameComponent* parent = nullptr;

        friend class CE::GameObject;
    };
    
} // namespace CE

#include "GameComponent.rtti.h"

