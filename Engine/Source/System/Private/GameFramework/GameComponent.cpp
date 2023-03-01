
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

    void GameComponent::AddUpdateListener(IObjectUpdateListener<GameComponent>* listener)
    {
        updateListeners.Add(listener);
    }

    void GameComponent::RemoveUpdateListener(IObjectUpdateListener<GameComponent>* listener)
    {
        updateListeners.Remove(listener);
    }

    void GameComponent::OnComponentValuesUpdated()
    {
        for (auto listener : updateListeners)
        {
            listener->OnObjectUpdated(this);
        }
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, GameComponent)
