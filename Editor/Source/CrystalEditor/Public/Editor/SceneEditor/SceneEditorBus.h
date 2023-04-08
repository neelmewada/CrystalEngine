#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API SceneEditorRequests
    {
    public:
        
        virtual void OpenEmptyScene() = 0;

        virtual void CreateEmptyGameObject() = 0;
        
        virtual void OpenScene(String sceneAssetPath) = 0;
        
    };

    CE_EVENT_BUS(CRYSTALEDITOR_API, SceneEditorBus, SceneEditorRequests);
    
} // namespace CE::Editor
