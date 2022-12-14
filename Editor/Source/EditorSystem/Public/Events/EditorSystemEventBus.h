#pragma once

#include "CoreTypes.h"
#include "IO/Path.h"

#include "Messaging/MessageBus.h"

namespace CE::Editor
{

    /// Event bus used by multiple Editor tools to communicate with each other
    class EditorSystemEvents : public IBusInterface
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Multiple;


    };

    using EditorSystemEventBus = MessageBus<EditorSystemEvents>;
    
} // namespace CE::Editor

