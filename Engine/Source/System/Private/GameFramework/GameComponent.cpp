
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

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, GameComponent)
