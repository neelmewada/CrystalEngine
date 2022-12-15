
#include "GameFramework/GameComponent.h"

namespace CE
{

    GameComponent::GameComponent(CE::Name name)
        : Component(name)
    {

    }

    GameComponent::~GameComponent()
    {

    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, GameComponent)
