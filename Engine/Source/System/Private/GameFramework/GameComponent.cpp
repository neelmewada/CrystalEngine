
#include "GameFramework/GameComponent.h"

namespace CE
{

    GameComponent::GameComponent(CE::Name name)
        : Component(name)
    {
        
    }

    GameComponent::~GameComponent()
    {
        if (owner != nullptr)
        {
            owner->RemoveComponent(this);
        }
    }

    void GameComponent::Activate()
    {
        Super::Activate();
    }

    void GameComponent::Deactivate()
    {
        Super::Deactivate();
    }

} // namespace CE

