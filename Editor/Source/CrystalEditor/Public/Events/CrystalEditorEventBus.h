#pragma once

#include "CoreTypes.h"
#include "IO/Path.h"

#include "Events/EditorSystemEventBus.h"

namespace CE::Editor
{

    /// Editor events specific to Crystal Editor
    class CrystalEditorEvents : public EditorSystemEvents
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Multiple;


        virtual void OnWelcomeScreenTimeout() {}

        virtual void OnCreateProject(IO::Path projectDirectory, String projectName) {}

        virtual void OnOpenProject(IO::Path projectPath) {}
    };

    using CrystalEditorEventBus = MessageBus<CrystalEditorEvents>;
    
} // namespace CE::Editor

