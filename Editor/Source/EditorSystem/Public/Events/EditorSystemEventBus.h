#pragma once

#include "CoreTypes.h"
#include "IO/Path.h"

#include "Messaging/MessageBus.h"

namespace CE::Editor
{

    class EditorSystemEvents : public IBusInterface
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Multiple;


        virtual void OnWelcomeScreenTimeout() {}

        virtual void CreateProject(IO::Path projectDirectory, String projectName) {}

    };

    using EditorSystemEventBus = MessageBus<EditorSystemEvents>;
    
} // namespace CE::Editor

