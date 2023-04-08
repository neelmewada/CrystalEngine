#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{
    
    class SceneOutlinerViewInterface
    {
    public:
        
        virtual void OnSceneOpened(Scene* scene) = 0;
    };

    //using SceneOutlinerViewBus = CE::MessageBus<SceneOutlinerViewInterface>;
    CE_EVENT_BUS(CRYSTALEDITOR_API, SceneOutlinerViewBus, SceneOutlinerViewInterface);

} // namespace CE::Editor
