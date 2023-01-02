#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API SceneEditorRequests : public IBusInterface
    {
    public:
        
        virtual void OpenEmptyScene() = 0;
        
        // virtual void OpenScene(String sceneAssetPath) = 0;
        
    };

    using SceneEditorBus = MessageBus<SceneEditorRequests>;
    
} // namespace CE::Editor
