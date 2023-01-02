#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{
    
    class SceneOutlinerViewInterface : public CE::IBusInterface
    {
    public:
        
        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Single;
        
        virtual void OnSceneOpened(Scene* scene) = 0;
    };

    using SceneOutlinerViewBus = CE::MessageBus<SceneOutlinerViewInterface>;

} // namespace CE::Editor
